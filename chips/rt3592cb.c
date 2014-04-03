/*
 *************************************************************************
 * Ralink Tech Inc.
 * 5F., No.36, Taiyuan St., Jhubei City,
 * Hsinchu County 302,
 * Taiwan, R.O.C.
 *
 * (c) Copyright 2002-2010, Ralink Technology, Inc.
 *
 * This program is free software; you can redistribute it and/or modify  *
 * it under the terms of the GNU General Public License as published by  *
 * the Free Software Foundation; either version 2 of the License, or     *
 * (at your option) any later version.                                   *
 *                                                                       *
 * This program is distributed in the hope that it will be useful,       *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 * GNU General Public License for more details.                          *
 *                                                                       *
 * You should have received a copy of the GNU General Public License     *
 * along with this program; if not, write to the                         *
 * Free Software Foundation, Inc.,                                       *
 * 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 *                                                                       *
 *************************************************************************/


#include "rt_config.h"


INT WMUpdateRadioStatus(IN RTMP_ADAPTER *pAd)
{
	UINT32 regval, actVal;


	if (pAd->wmChkMethod == CHK_BY_CNT) {
		if (pAd->ulActiveCountPastPeriod == 0) {
			RTMP_IO_READ32(pAd, GPIO_CTRL_CFG, &regval);
			if (IS_RT3592CB(pAd) && WM_COEX_TEST_FLAG(pAd, fWM_CHECK_RADIO)) {
				if (regval & 0x01)
					pAd->ulActiveCountPastPeriod++;
			} else {
				if(regval & 0x10)
					pAd->ulActiveCountPastPeriod++;
			}
		}

		if (pAd->ulActiveCountPastPeriod)
			WM_COEX_SET_FLAG(pAd, fWM_RADIO_ON); //WM on
	}
	else if (pAd->wmChkMethod == CHK_BY_GPIO)
	{
		if (WM_COEX_TEST_FLAG(pAd, fWM_GPIO4_ACTIVE_HIGH))
			actVal = 0x10;
		else
			actVal = 0;
		
		RTMP_IO_FORCE_READ32(pAd, GPIO_CTRL_CFG, &regval);
		if ((regval & 0x10) == actVal)
			WM_COEX_SET_FLAG(pAd, fWM_RADIO_ON); //WM on
		else
			WM_COEX_CLEAR_FLAG(pAd, fWM_RADIO_ON); //WM off
	}


	return 0;
}


void rt3592cb_hw_ctrl(IN RTMP_ADAPTER *pAd)
{
	ULONG				GPIO = 0;
	US_CYC_CNT_STRUC	USCycCnt;
	UCHAR				BBPR3 = 0;

	RTMP_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R3, &BBPR3);
	BBPR3 &= (~0x18);
	BBPR3 &= (~0x03);
	BBPR3 |= 0x01;	
	if ((pAd->CommonCfg.Channel <= 14) && (IS_WM_ON(pAd)))
	{
		RTMP_IO_READ32(pAd, US_CYC_CNT, &USCycCnt.word);
		USCycCnt.field.MiscModeEn = 1;
		RTMP_IO_WRITE32(pAd, US_CYC_CNT, USCycCnt.word);

		/* GPIO0 and GPIO1 cleared as BT Coexistence function */
		RTMP_IO_FORCE_READ32(pAd, GPIO_SWITCH, &GPIO);		
		GPIO = (GPIO & 0xfffffffc);
		RTMP_IO_FORCE_WRITE32(pAd, GPIO_SWITCH, GPIO);

		/* 1x1 at 2.4G, Switch BT to 2.4G, TR0 to 5G */
		RTMPHwCoexSwitch(pAd, COEX_SWTICH_ENABLE);			
	}
	else
	{
		RTMP_IO_READ32(pAd, US_CYC_CNT, &USCycCnt.word);
		USCycCnt.field.MiscModeEn = 0;
		RTMP_IO_WRITE32(pAd, US_CYC_CNT, USCycCnt.word);

		/* GPIO0 and GPIO1 enabled as GPIO function */
		RTMP_IO_FORCE_READ32(pAd, GPIO_SWITCH, &GPIO);		
		GPIO = (GPIO | 0x3);
		RTMP_IO_FORCE_WRITE32(pAd, GPIO_SWITCH, GPIO);

		if(pAd->Antenna.field.RxPath == 2)
			BBPR3 |= (0x8);

		/* 
			2x2 at 2.4G, Switch WM to 5G, TR0 to 2.4G=>COEX_SWTICH_DISABLE
			2x2 at 5G, Switch WM to 2.4G, TR0 to 5G   =>COEX_SWTICH_ENABLE
		*/
		if (pAd->CommonCfg.Channel <= 14)
			RTMPHwCoexSwitch(pAd, COEX_SWTICH_DISABLE);
		else
			RTMPHwCoexSwitch(pAd, COEX_SWTICH_ENABLE);
	}

	RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R3, BBPR3);
#ifdef CONFIG_STA_SUPPORT
	pAd->StaCfg.BBPR3 = BBPR3;
#endif // CONFIG_STA_SUPPORT //
}


void rt3592cb_ant_switch(
	IN RTMP_ADAPTER *pAd,
	IN UINT32 gpioVal)
{
	UINT32 wmOldStatus, wmNewStatus;
	
	wmOldStatus = WM_COEX_TEST_FLAG(pAd, fWM_RADIO_ON);
	WMUpdateRadioStatus(pAd);
	wmNewStatus = WM_COEX_TEST_FLAG(pAd, fWM_RADIO_ON);
	if (wmOldStatus ^ wmNewStatus)
	{

		if (WM_COEX_TEST_FLAG(pAd, fWM_RADIO_ON))
		{
			RTMPHwCoexSwitch(pAd, COEX_SWTICH_ENABLE);
#ifdef CONFIG_STA_SUPPORT
			if ((INFRA_ON(pAd) || ADHOC_ON(pAd)) && (pAd->CommonCfg.Channel <= 14))
			{
				if (INFRA_ON(pAd) )
				{
					MLME_DEAUTH_REQ_STRUCT DeAuthReq;
					MLME_QUEUE_ELEM *pMsgElem;

					pMsgElem = (MLME_QUEUE_ELEM *) kmalloc(sizeof(MLME_QUEUE_ELEM), MEM_ALLOC_FLAG);
					if (pMsgElem)
					{
						NdisZeroMemory(pMsgElem, sizeof(MLME_QUEUE_ELEM));
						COPY_MAC_ADDR(DeAuthReq.Addr, pAd->CommonCfg.Bssid);
						DeAuthReq.Reason = REASON_DEAUTH_STA_LEAVING;
						pMsgElem->MsgLen = sizeof(MLME_DEAUTH_REQ_STRUCT);
						NdisMoveMemory(pMsgElem->Msg, &DeAuthReq, sizeof(MLME_DEAUTH_REQ_STRUCT));
						MlmeDeauthReqAction(pAd, pMsgElem);
						kfree(pMsgElem);
					}
				}
				LinkDown(pAd, FALSE);
				pAd->Mlme.AssocMachine.CurrState = ASSOC_IDLE;
			}
#endif // CONFIG_STA_SUPPORT //
		}
		else
		{
			if ((pAd->CommonCfg.Channel <= 14)
#ifdef CONFIG_STA_SUPPORT
				&& (INFRA_ON(pAd) || ADHOC_ON(pAd)) 
#endif // CONFIG_STA_SUPPORT //
			)
			{
				ULONG	TxPinCfg = 0x00050F0A;
				UCHAR    RfValue;

				RTMPHwCoexSwitch(pAd, COEX_SWTICH_DISABLE);
				RTMP_IO_WRITE32(pAd, TX_PIN_CFG, TxPinCfg);

				RT30xxReadRFRegister(pAd, RF_R01, &RfValue);
				RfValue |= 0x01; // Enable RF block.
				RfValue &= 0x03;	//clear bit[7~2]
				RfValue |= 0xc0;
				RT30xxWriteRFRegister(pAd, RF_R01, RfValue);
			}
		}
	}
}


