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
#include "misc.h"

DECLARE_TIMER_FUNCTION(DetectExec);
BUILD_TIMER_FUNCTION(DetectExec);

#ifdef DOT11N_DRAFT3
BOOLEAN WifiThroughputOverLimit(
	IN	PRTMP_ADAPTER	pAd,
	IN  UCHAR WifiThroughputLimit)
{
	BOOLEAN  bIssue4020 = FALSE;
	ULONG tmpReceivedByteCount = 0;
	ULONG tmpTransmittedByteCount = 0;
	static ULONG TxByteCount = 0;
	static ULONG RxByteCount = 0;
	static ULONG TxRxThroughputPerSeconds = 0; //Unit: bytes
	LONG diffTX = 0;
	LONG diffRX = 0;

	bIssue4020 = FALSE;
	
	if (pAd == NULL)
	{
		return FALSE;
	}
	
	if (IS_ENABLE_40TO20_BY_TIMER(pAd))
	{	
		tmpReceivedByteCount = pAd->RalinkCounters.ReceivedByteCount; 
		tmpTransmittedByteCount = pAd->RalinkCounters.TransmittedByteCount;		
					
		if ((TxByteCount != 0) || (RxByteCount != 0 ))
		{				
			diffTX = (LONG)(((tmpTransmittedByteCount - TxByteCount)*5) >> 3);
			diffRX = (LONG)(((tmpReceivedByteCount - RxByteCount)* 5) >> 3);		
						
			if ((diffTX > 0) && (diffRX > 0 ))
			{
				TxRxThroughputPerSeconds = diffTX + diffRX ;//Unit: bytes 
			}
			else if ((diffTX < 0) && (diffRX > 0))
			{
				TxRxThroughputPerSeconds = diffRX;
			}
			else if ((diffTX > 0) && (diffRX < 0))
			{
				TxRxThroughputPerSeconds = diffTX;
			}
			else 
			{
				TxRxThroughputPerSeconds = 0;
			}

			DBGPRINT(RT_DEBUG_INFO,("TxRxThroughputPerSeconds = %ld Bps, %ld KBps, %ldKbps, %ldMbps", 
				TxRxThroughputPerSeconds,
				(TxRxThroughputPerSeconds >> 10),
				(TxRxThroughputPerSeconds >> 7),
				(TxRxThroughputPerSeconds >> 17)));
		}
	
		TxByteCount = tmpTransmittedByteCount;
		RxByteCount = tmpReceivedByteCount;

		DBGPRINT(RT_DEBUG_INFO,("btWifiThr = %d, TxByteCount = %ld, RxByteCount = %ld",
			WifiThroughputLimit,
			TxByteCount,
			RxByteCount));

		if ((TxRxThroughputPerSeconds >> 17) > WifiThroughputLimit)
		{
			bIssue4020 = TRUE;
		}
		else
		{
			bIssue4020 = FALSE;
		}
	}
	
	return bIssue4020;
}
#endif // DOT11N_DRAFT3 //


BUSY_DEGREE CheckBusy(
	IN PLONG History, 
	IN UCHAR HistorySize)
{
	if (History == NULL)
	{
		return BUSY_0;
	}

	DBGPRINT(RT_DEBUG_INFO,(" ---> Check Busy %ld %ld %ld %ld %ld",
		*History,
		*(History+1),
		*(History+2),
		*(History+3),
		*(History+4)));

#ifdef RTMP_PCI_SUPPORT
	if ((*History > 40) || 
		(*(History+1) > 40) || 
		(*(History+2) > 40) ||
		(*(History+3) > 40) ||
		(*(History+4) > 40))
	{
		return BUSY_5; 
	}

	if ((*History > 33) || 
		(*(History+1) > 33) || 
		(*(History+2) > 33) ||
		(*(History+3) > 33) ||
		(*(History+4) > 33))
	{
		return BUSY_5; 
	}
	
	if (((*History >= 20) && 
		(*(History+1) >= 20) && 
		(*(History+2) >= 20) &&
		(*(History+3) >= 20) &&
		(*(History+4) >= 20)))
	{
		return BUSY_4;	
	}

	if ((*History >= 8) && 
		(*(History+1) >= 8) && 
		(*(History+2) >= 8) &&
		(*(History+3) >= 8) &&
		(*(History+4) >= 8))
	{
		return BUSY_3;
	}

	if ((*History >= 10) || 
		(*(History+1) >= 10) || 
		(*(History+2) >= 10) || 
		(*(History+3) >= 10) || 
		(*(History+4) >= 10))
	{
		return BUSY_2;
	}

	if (((*History >= 4) || 
		(*(History+1) >= 4) || 
		(*(History+2) >= 4) || 
		(*(History+3) >= 4) || 
		(*(History+4) >= 4))&&
		(*History >= 2) && 
		(*(History+1) >= 2) && 
		(*(History+2) >= 2) && 
		(*(History+3) >= 2) && 
		(*(History+4) >= 2))	
	{
		return BUSY_2;
	}

	if (((*History >= 1) || 
		(*(History+1) >= 1) || 
		(*(History+2) >= 1) || 
		(*(History+3) >= 1) || 
		(*(History+4) >= 1) ) &&
		((*History >= 0) && 
		(*(History+1) >= 0) && 
		(*(History+2) >= 0) &&
		(*(History+3) >= 0) &&
		(*(History+4) >= 0)))
	{
		return BUSY_1;
	}	
#endif // RTMP_PCI_SUPPORT //

	return BUSY_0;
}

VOID Adjust(
	IN PRTMP_ADAPTER	pAd, 
	IN BOOLEAN			bIssue4020, 
	IN ULONG			NoBusyTimeCount)
{
	CHAR	Rssi;

	Rssi = RTMPMaxRssi(pAd, 
					   pAd->StaCfg.RssiSample.AvgRssi0, 
					   pAd->StaCfg.RssiSample.AvgRssi1, 
					   pAd->StaCfg.RssiSample.AvgRssi2);
	
	DBGPRINT(RT_DEBUG_INFO,("RSSI = %d\n", Rssi));

	if (IS_ENABLE_LNA_MID_GAIN_DOWN_BY_TIMER(pAd))
	{
		UCHAR BbpR65 = 0;
		if (pAd->BusyDegree >= BUSY_1)
		{
			DBGPRINT(RT_DEBUG_INFO, ("Lower LNA Middle Gain at HT20 or HT40\n"));
			pAd->bPermitLnaGainDown = TRUE;

			DBGPRINT(RT_DEBUG_INFO,("RSSI = %d\n", Rssi));

			if (Rssi <= -35)
			{
			/* if RSSI is smaller than -80, then set R65 to High Gain to fix long distance issue */
				if (Rssi <= -80)
				{
					BBP_IO_READ8_BY_REG_ID(pAd, BBP_R65, &BbpR65);

					if (BbpR65 == 0x29)
					{
						DBGPRINT(RT_DEBUG_INFO,("Set R65 to 0x2C from 0x29 (Highest LNA)\n"));
						BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R65, 0x2C); /* Highest LNA Gain */
					}
				}
				else
				{
					BBP_IO_READ8_BY_REG_ID(pAd, BBP_R65, &BbpR65);

					if (BbpR65 == 0x2C)
					{
						DBGPRINT(RT_DEBUG_INFO,("Set R65 to 0x29 from 0x2C (Middle LNA)\n"));
						BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R65, 0x29); /* Middle LNA Gain */
					}
				}
			}
			else
			{
				BBP_IO_READ8_BY_REG_ID(pAd, BBP_R65, &BbpR65);

				if (BbpR65 == 0x29)
				{
					DBGPRINT(RT_DEBUG_INFO,("Set R65 to 0x2C from 0x29 (Highest LNA)\n"));
					BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R65, 0x2C); /* Highest LNA Gain */
				}
			}
		}
		else
		{
			if (NoBusyTimeCount > IDLE_STATE_THRESHOLD)
			{	
			DBGPRINT(RT_DEBUG_INFO, ("Lower LNA Middle Gain at HT20 or HT40 (Highest LNA)\n"));

			pAd->bPermitLnaGainDown = FALSE;
				BBP_IO_READ8_BY_REG_ID(pAd, BBP_R65, &BbpR65);
				if (BbpR65 == 0x29)
				{
					DBGPRINT(RT_DEBUG_INFO,("Set R65 to 0x2C from 0x29 (Highest LNA)\n"));
					BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R65, 0x2C); /* Highest LNA Gain */
				}
			}
			else
			{
				DBGPRINT(RT_DEBUG_INFO, ("Lower LNA Middle Gain at HT20 or HT40\n"));
			}
		}
	}

	if (IS_ENABLE_TX_POWER_DOWN_BY_TIMER(pAd))
	{
		if (pAd->BusyDegree >= BUSY_1)
		{
			DBGPRINT(RT_DEBUG_INFO, ("Lower Tx Power at HT20 or HT40\n"));
				pAd->bPermitTxPowerDown= TRUE;
		}
		else
		{
			if (NoBusyTimeCount > IDLE_STATE_THRESHOLD)
			{
				DBGPRINT(RT_DEBUG_INFO, ("Higher Tx Power at HT20 or HT40\n"));
					pAd->bPermitTxPowerDown= FALSE;
			}
			else
			{
				DBGPRINT(RT_DEBUG_INFO, ("Lower Tx Power at HT20 or HT40\n"));
			}
		}
	}

#ifdef DOT11_N_SUPPORT
	if (IS_ENABLE_TXWI_AMPDU_SIZE_BY_TIMER(pAd))
	{
	/* Fixed long distance issue */
		if ((pAd->BusyDegree >= BUSY_2) && 
		((pAd->CommonCfg.BBPCurrentBW == BW_40) || 
		(pAd->CommonCfg.BBPCurrentBW == BW_20)) && 
		(Rssi <= -80))
		{
			pAd->bPermitTxBaSizeDown = TRUE;
		}
		else
		{
			pAd->bPermitTxBaSizeDown = FALSE;
		}
	}

	if (IS_ENABLE_TXWI_AMPDU_DENSITY_BY_TIMER(pAd))
	{
	/* Fixed long distance issue */
		if ((pAd->BusyDegree >= BUSY_2) && 
		((pAd->CommonCfg.BBPCurrentBW == BW_40) || 
		(pAd->CommonCfg.BBPCurrentBW == BW_20)) && 
		(Rssi <= -80))
		{
			pAd->bPermitTxBaDensityDown = TRUE;
		}
		else
		{
			pAd->bPermitTxBaDensityDown = FALSE;
		}
	}
		
	if (IS_ENABLE_RATE_ADAPTIVE_BY_TIMER(pAd))
	{
		if ((pAd->BusyDegree >= BUSY_2) && 
				(pAd->CommonCfg.BBPCurrentBW == BW_40))
		{
			pAd->bPermitMcsDown = TRUE;
		}
		else
		{
			pAd->bPermitMcsDown = FALSE;
		}
	}

	if (IS_ENABLE_REJECT_ORE_BA_BY_TIMER(pAd))
	{	
	/* Fixed long distance issue */
		if ((pAd->BusyDegree >= BUSY_2) && 
		((pAd->CommonCfg.BBPCurrentBW == BW_40) || 
		(pAd->CommonCfg.BBPCurrentBW == BW_20)) && 
		(Rssi <= -80))
		{
			BASessionTearDownALL(pAd, BSSID_WCID);
			pAd->bPermitRecBaDown = TRUE;
		}
		else
		{
			pAd->bPermitRecBaDown = FALSE;
		}
	}
	
#ifdef DOT11N_DRAFT3
	if (IS_ENABLE_40TO20_BY_TIMER(pAd))
	{
	if (((pAd->BusyDegree >= WIFI_2040_SWITCH_THRESHOLD) && 
			(pAd->BusyDegree != BUSY_5)) && 
			(pAd->CommonCfg.BBPCurrentBW == BW_40) && 
			(OPSTATUS_TEST_FLAG(pAd, fOP_STATUS_SCAN_2040)) &&
			(bIssue4020 == TRUE))
		{
			BSS_2040_COEXIST_IE OldValue;

		DBGPRINT(RT_DEBUG_INFO, ("HT40 --> HT20\n"));
		DBGPRINT(RT_DEBUG_INFO,("ACT - Update2040CoexistFrameAndNotify. BSSCoexist2040 = %x. EventANo = %d. \n", pAd->CommonCfg.BSSCoexist2040.word, pAd->CommonCfg.TriggerEventTab.EventANo));
			OldValue.word = pAd->CommonCfg.BSSCoexist2040.word;
			pAd->CommonCfg.BSSCoexist2040.word = 0;

			//if (pAd->CommonCfg.TriggerEventTab.EventBCountDown > 0)
			pAd->CommonCfg.BSSCoexist2040.field.BSS20WidthReq = 1;

		/*
			Need to check !!!!
			How STA will set Intolerant40 if implementation dependent. Now we don't set this bit first!!!!!
			So Only check BSS20WidthReq change.
		*/
			//if (OldValue.field.BSS20WidthReq != pAd->CommonCfg.BSSCoexist2040.field.BSS20WidthReq)
			{
				Send2040CoexistAction(pAd, BSSID_WCID, TRUE);
			}
		}
	}
#endif // DOT11N_DRAFT3 //
#endif // DOT11_N_SUPPORT //
}

VOID TxPowerDown(
	IN PRTMP_ADAPTER	pAd, 
	IN CHAR				Rssi,
	INOUT CHAR			*pDeltaPowerByBbpR1, 
	INOUT CHAR			*pDeltaPwr)
{
	if ((pAd->bHwCoexInit == TRUE) && 
		IS_ENABLE_TX_POWER_DOWN_BY_TIMER(pAd) &&
		(pAd->bPermitTxPowerDown== TRUE) && 
		(pAd->CommonCfg.TxPowerPercentage == 0xffffffff))
	{
		CHAR DeltaPowerByBbpR1 = *pDeltaPowerByBbpR1;
		CHAR DeltaPwr = *pDeltaPwr;

		DBGPRINT(RT_DEBUG_INFO, (">> ++ DeltaPwr =%d, DeltaPowerByBbpR1= %d, RSSI = %d\n",
			DeltaPwr, DeltaPowerByBbpR1, Rssi));

		if (Rssi <= -80)
		{	
			DBGPRINT(RT_DEBUG_INFO, (">> -1 75%% power dbm\n"));
			DeltaPowerByBbpR1 = 0;
			DeltaPwr -= 1;
		}
		else if ((Rssi <= -50) && (Rssi >= -60))
		{
			DeltaPowerByBbpR1 = -6;
			DeltaPwr -= 1;

			DBGPRINT(RT_DEBUG_INFO, (">> -7 18%% power dbm\n"));
		}
		else if ((Rssi <= -60) && (Rssi >= -70))
		{
			DeltaPowerByBbpR1 = -6;
			DBGPRINT(RT_DEBUG_INFO, (">> -6 25%% power dbm\n"));
		}
		else if ((Rssi <= -70) && (Rssi >= -80)) 
		{
			DBGPRINT(RT_DEBUG_INFO, (">> -3 50%% dbm\n"));
			DeltaPowerByBbpR1 = 0;
			DeltaPwr -= 3;
		}
		else // drop into lowest power
		{
			DeltaPowerByBbpR1 = 0;
			DeltaPowerByBbpR1 -= 12;
		}
		
		*pDeltaPowerByBbpR1 = DeltaPowerByBbpR1;
		*pDeltaPwr = DeltaPwr;
			
		DBGPRINT(RT_DEBUG_INFO, (">> -- DeltaPwr =%d, DeltaPowerByBbpR1= %d, RSSI = %d\n", 
			DeltaPwr, DeltaPowerByBbpR1, Rssi));
	}
}

VOID McsDown(
	IN PRTMP_ADAPTER	pAd, 
	IN CHAR				CurrRateIdx, 
	IN PRTMP_TX_RATE_SWITCH	pCurrTxRate, 
	INOUT CHAR			*pUpRateIdx, 
	INOUT CHAR			*pDownRateIdx)
{
	if ((pAd->bHwCoexInit == TRUE) && 
		IS_ENABLE_RATE_ADAPTIVE_BY_TIMER(pAd) && 
		(pAd->bPermitMcsDown == TRUE))
	{
		UCHAR	btMCSThreshold = 0x00;
		UCHAR	UpRateIdx = *pUpRateIdx;
		UCHAR	DownRateIdx = *pDownRateIdx;

		btMCSThreshold = (UCHAR)(GET_PARAMETER_OF_MCS_THRESHOLD(pAd));
		//0,1,2,3 => MCS= 3, 4, 5, 6
		if (btMCSThreshold <= 0x3)
		{
			btMCSThreshold = btMCSThreshold + 0x3;
		}
		else
		{
			btMCSThreshold = 0x03;
		}
		
		if (CurrRateIdx > 0)
		{
			// Rate is equeal to btMCSThreshold
			if ((pCurrTxRate->CurrMCS == btMCSThreshold)) 
			{
				UpRateIdx = CurrRateIdx;
			}
	
			// Rate be decreased to btMCSThreshold
			else if (pCurrTxRate->CurrMCS > btMCSThreshold)
			{
				UpRateIdx = CurrRateIdx - 1;
			}
			//Rate is under btMCSThreshold
			//else
			//{
			//	UpRateIdx = CurrRateIdx + 1;
			//}
			//DownRateIdx = CurrRateIdx - 1;	
	
			DBGPRINT(RT_DEBUG_INFO,("CurrRateIdx=%d, UpRateIdx=%d, DownRateIdx=%d\n",
				CurrRateIdx, UpRateIdx, DownRateIdx));
		}

		*pUpRateIdx = UpRateIdx;
		*pDownRateIdx = DownRateIdx;
	}
}

VOID McsDown2(
	IN PRTMP_ADAPTER	pAd, 
	IN UCHAR			MCS3, 
	IN UCHAR			MCS4, 
	IN UCHAR			MCS5, 
	IN UCHAR			MCS6, 
	INOUT UCHAR			*pTxRateIdx)
{
	if ((pAd->bHwCoexInit == TRUE) && 
		IS_ENABLE_RATE_ADAPTIVE_BY_TIMER(pAd) && 
		(pAd->bPermitMcsDown == TRUE))
	{
		UCHAR	btMCSThreshold = 0x00;
		UCHAR	TxRateIdx = *pTxRateIdx;

		btMCSThreshold = (UCHAR)(GET_PARAMETER_OF_MCS_THRESHOLD(pAd));
		//0,1,2,3 => MCS= 3, 4, 5, 6
		if (btMCSThreshold <= 0x3)
		{
			btMCSThreshold = btMCSThreshold + 0x3;
		}
		else
		{
			btMCSThreshold = 0x03;
		}
	
		if (btMCSThreshold == 0x03)
		{
			btMCSThreshold = MCS3;
		}
		else if (btMCSThreshold == 0x04)
		{
			btMCSThreshold = MCS4;
		}
		else if (btMCSThreshold == 0x05)
		{
			btMCSThreshold = MCS5;
		}
		else if (btMCSThreshold == 0x06)
		{
			btMCSThreshold = MCS6;
		}
		else
		{
			btMCSThreshold = MCS3;
		}
	
		if (TxRateIdx > btMCSThreshold)
		{
			TxRateIdx = btMCSThreshold; 
		}

		*pTxRateIdx = TxRateIdx;
	}
}

VOID TxBaSizeDown(
	IN PRTMP_ADAPTER	pAd, 
	INOUT PTXWI_STRUC 	pTxWI)
{
	if ((pAd->bHwCoexInit == TRUE) && 
		INFRA_ON(pAd) && 
		(pAd->OpMode == OPMODE_STA) && 
		(pTxWI->BAWinSize != 0) && 
		IS_ENABLE_TXWI_AMPDU_SIZE_BY_TIMER(pAd) && 
		(pAd->bPermitTxBaSizeDown == TRUE))
	{
		UCHAR				btAMPDUSize = 0;

		/* When Bluetooh is busy, we set the BA Size to smaller */
		btAMPDUSize = (UCHAR)(GET_PARAMETER_OF_AMPDU_SIZE(pAd));
		if (btAMPDUSize <= 3)
		{
			//0,1,2,3 => ba size=1, 3, 5, 7
			if (btAMPDUSize == 0)
			{
				pTxWI->BAWinSize = 1;
			}
			else
			{
				pTxWI->BAWinSize = 1 + (btAMPDUSize << 1);
			}
		}
		else 
		{
			pTxWI->BAWinSize = 1;
		}
		
		DBGPRINT(RT_DEBUG_INFO, (">>>>> dynamic BAWinSize = %d, profile AMPDU size=%ld\n", 
		pTxWI->BAWinSize, GET_PARAMETER_OF_AMPDU_SIZE(pAd)));	
	}
}

VOID TxBaDensityDown(
	IN PRTMP_ADAPTER	pAd, 
	INOUT PTXWI_STRUC 	pTxWI)
{
	if ((pAd->bHwCoexInit == TRUE) && 
		INFRA_ON(pAd) && 
		(pAd->OpMode == OPMODE_STA) && 
		IS_ENABLE_TXWI_AMPDU_DENSITY_BY_TIMER(pAd) && 
		(pAd->bPermitTxBaDensityDown == TRUE))
	{
		UCHAR				btAMPDUDensity = 0;

		/* When Bluetooh is busy, we set the BA density to larger */
		btAMPDUDensity = (UCHAR)(GET_PARAMETER_OF_AMPDU_DENSITY(pAd));
		if (btAMPDUDensity <= 3)
		{
			//0,1,2,3 => BA density=0x01, 0x03, 0x05, 0x07 => 0.25u, 1u, 4u, 16u
			if (btAMPDUDensity == 0)
			{
				pTxWI->MpduDensity = 0x01;
			}
			pTxWI->MpduDensity = 1 + (btAMPDUDensity << 1);
		}
		else 
		{
			pTxWI->MpduDensity = 0x07;
		}
		
		DBGPRINT(RT_DEBUG_INFO, (">>>>> dynamic BA density= %d, profile AMPDU density=%ld\n", 
			pTxWI->MpduDensity, GET_PARAMETER_OF_AMPDU_DENSITY(pAd)));
	}
}

VOID MiscInit(
	IN PRTMP_ADAPTER pAd)
{
#ifdef RTMP_PCI_SUPPORT
	if (((pAd->NicConfig3.field.CrystalShared == CYRSTALL_SHARED) || 
		IS_ENABLE_SINGLE_CRYSTAL_SHARING_BY_FORCE(pAd)) &&
		(pAd->StaCfg.PSControl.field.rt30xxPowerMode == 0x3))
	{	
		UCHAR  LinkCtrlSetting = 0;
		USHORT PCIePowerSetting = 0;
		
		PCIePowerSetting = 2;
		DBGPRINT(RT_DEBUG_TRACE, ("before PCIePower Save Level = %d due to single crystall\n", pAd->StaCfg.PSControl.field.rt30xxPowerMode));
		DBGPRINT(RT_DEBUG_TRACE, ("after PCIePower Save Level = 2 due to single crystall\n"));
		
		pAd->PCIePowerSaveLevel = (USHORT)PCIePowerSetting;
		if ((pAd->Rt3xxRalinkLinkCtrl & 0x2) && (pAd->Rt3xxHostLinkCtrl & 0x2))
		{
			LinkCtrlSetting = 1;
		}
		DBGPRINT(RT_DEBUG_TRACE, ("====> rt30xxF LinkCtrlSetting = 0x%x.\n", LinkCtrlSetting));
		AsicSendCommandToMcu(pAd, 0x83, 0xff, (UCHAR)PCIePowerSetting, LinkCtrlSetting);
	}
#endif // RTMP_PCI_SUPPORT //
	if (IS_ENABLE_MISC_TIMER(pAd))
	{
		RTMPInitTimer(pAd, &pAd->Mlme.MiscDetectTimer, GET_TIMER_FUNCTION(DetectExec), pAd, TRUE);
		RTMPSetTimer(&pAd->Mlme.MiscDetectTimer, DETECT_TIMEOUT);
	}

	DBGPRINT(RT_DEBUG_TRACE,("nicConfig2 =0x%04x\n", pAd->NicConfig2.word));
	DBGPRINT(RT_DEBUG_TRACE,("nicConfig3 =0x%04x\n", pAd->NicConfig3.word));
}

VOID MiscUserCfgInit(
	IN PRTMP_ADAPTER pAd)
{
	pAd->cfgCoex15 = TRUE;
	pAd->wmChkMethod = DFT_WM_CHK_METHOD;
#ifdef DOT11_N_SUPPORT
	pAd->bPermitRecBaDown = FALSE;
	pAd->bPermitMcsDown = FALSE;
	pAd->bPermitTxBaSizeDown = FALSE;
	pAd->bPermitTxBaDensityDown = FALSE;
#endif // DOT11_N_SUPPORT //
	pAd->bPermitTxPowerDown = FALSE;
	pAd->bPermitLnaGainDown = FALSE;
#ifdef RTMP_PCI_SUPPORT
	pAd->ulConfiguration = 0x380004DD;
#endif // RTMP_PCI_SUPPORT //
	pAd->ulActiveCountPastPeriod = 0;
	pAd->BusyDegree = BUSY_0;
}


#ifdef RTMP_PCI_SUPPORT
VOID DetectExec(
	IN PVOID SystemSpecific1, 
	IN PVOID FunctionContext, 
	IN PVOID SystemSpecific2, 
	IN PVOID SystemSpecific3)
{
	BOOLEAN 				bIssue4020 = FALSE;
	BOOLEAN 				bPowerSaving = FALSE;
	ULONG					data = 0;
	UCHAR					j = 0;
#ifdef DOT11N_DRAFT3
	UCHAR					btWifiThr = 0;
#endif // DOT11N_DRAFT3 //
	BUSY_DEGREE	BusyDegree = BUSY_0;
	static UCHAR			ulPowerSavingTimeCount = 0; // per 0.01 second, count it.
	static ULONG			TimeCount = 0; // per 0.01 second, count it.
	static ULONG			NoBusyTimeCount = 0; // per second, count it.
	static ULONG			VeryBusyTimeCount = 0;		// per second, count it.
	static ULONG			History[HISTORY_RECORD_NUM] = {0};
	BOOLEAN		bInitialValue = FALSE;
	PRTMP_ADAPTER			pAd = (RTMP_ADAPTER *)FunctionContext;

	
	if (pAd == NULL)
		return;

	do
	{

		if (!((pAd->bHwCoexInit == TRUE) &&
			(IDLE_ON(pAd)) && 
			IS_ENABLE_MISC_TIMER(pAd) && 
			(pAd->CommonCfg.Channel <= 14))
		)
			bInitialValue = FALSE;
		else
		{
			bInitialValue = TRUE;
			//printk("%s(%d):bInitialValue=%d\n", __FUNCTION__, __LINE__, bInitialValue);
			break;
		}

		// TODO: 
		if (IS_RT3592CB(pAd) && ((pAd->CommonCfg.Channel > 14) || (!IS_WM_ON(pAd))))
		{
			bInitialValue = TRUE;
			break;
		}

		TimeCount++;
		
		if ((pAd->bPCIclkOff == TRUE) ||
			RTMP_TEST_PSFLAG(pAd, fRTMP_PS_SET_PCI_CLK_OFF_COMMAND) || 
			RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_IDLE_RADIO_OFF) ||
			OPSTATUS_TEST_FLAG(pAd, fOP_STATUS_DOZE))
		{
			if (pAd->bPCIclkOff == TRUE)
			{
				DBGPRINT(RT_DEBUG_INFO,("pAd->bPCIclkOff == TRUE\n"));
			}
			if (RTMP_TEST_PSFLAG(pAd, fRTMP_PS_SET_PCI_CLK_OFF_COMMAND))
			{
				DBGPRINT(RT_DEBUG_INFO,("fRTMP_PS_SET_PCI_CLK_OFF_COMMAND == TRUE\n"));
			}
			if (RTMP_TEST_PSFLAG(pAd, fRTMP_ADAPTER_IDLE_RADIO_OFF))
			{
				DBGPRINT(RT_DEBUG_INFO,("fRTMP_ADAPTER_IDLE_RADIO_OFF == TRUE\n"));
			}
			if (OPSTATUS_TEST_FLAG(pAd, fOP_STATUS_DOZE))
			{
				DBGPRINT(RT_DEBUG_INFO,("fOP_STATUS_DOZE == TRUE\n"));
			}
			bPowerSaving = TRUE;
			
			ulPowerSavingTimeCount ++;
		}

		if ((RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_RESET_IN_PROGRESS))	||
			(RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_HALT_IN_PROGRESS))	||
			(RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_RADIO_OFF))			||
			(RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_NIC_NOT_EXIST)))
		{
			DBGPRINT(RT_DEBUG_INFO,("error RESET, HALT, RADIO_OFF, NIC_NOT_EXIST\n"));
			if( TimeCount >= CHECK_TIME_INTERVAL) 
			{
				pAd->ulActiveCountPastPeriod=0;
				TimeCount = 0;
				NoBusyTimeCount = 0;

#ifdef DOT11_N_SUPPORT
				pAd->bPermitRecBaDown = FALSE;
				pAd->bPermitMcsDown = FALSE;
				pAd->bPermitTxBaSizeDown = FALSE;
				pAd->bPermitTxBaDensityDown = FALSE;
#endif // DOT11_N_SUPPORT //
				pAd->bPermitTxPowerDown = FALSE;
				pAd->bPermitLnaGainDown = FALSE;
			}
			return;
		}
		
		if (bPowerSaving == FALSE)
		{
			RTMP_IO_READ32(pAd, GPIO_CTRL_CFG, &data);

			if (IS_RT3592CB(pAd) && WM_COEX_TEST_FLAG(pAd, fWM_CHECK_RADIO))
			{
				//
				// RT3592BC8 use GPIO4 for BT hardware on/off.
				// In the case, it reads GPIO#0 instead of GPIO#4.
				//
				if (data & 0x01) // GPIO#0
				{
					pAd->ulActiveCountPastPeriod++;
					WMUpdateRadioStatus(pAd);
				}
			}
			else
			{
				if(data & 0x10) //GPIO#4
				{
					pAd->ulActiveCountPastPeriod++;
				}
			}
		}

		if (TimeCount >= CHECK_TIME_INTERVAL)
		{

			DBGPRINT(RT_DEBUG_INFO,("<--- WATCH TIME\n"));	
			DBGPRINT(RT_DEBUG_INFO,("-->BW=%d, bt active per sec=%ld, No Busy Time Count =%ld, Very Busy Time Count = %ld, PowerSavingTimeCount =%d\n", 
					pAd->CommonCfg.BBPCurrentBW,
					pAd->ulActiveCountPastPeriod,
					NoBusyTimeCount, 
					VeryBusyTimeCount,
					ulPowerSavingTimeCount
					));

			for (j = HISTORY_RECORD_NUM-1; j >= 1; j--)
			{
				History[j]=History[j-1];
			}

			if (ulPowerSavingTimeCount == 0)
			{
				History[0] = pAd->ulActiveCountPastPeriod;
			}
			else if (ulPowerSavingTimeCount < CHECK_TIME_INTERVAL)
			{
				History[0] = (pAd->ulActiveCountPastPeriod * CHECK_TIME_INTERVAL )/(CHECK_TIME_INTERVAL - ulPowerSavingTimeCount);
			}
			else 
			{
				History[0] = 0;
			}
	
			BusyDegree = CheckBusy(&History[0],HISTORY_RECORD_NUM);
			pAd->BusyDegree = BusyDegree;
			
			if (pAd->BusyDegree == BUSY_0)
			{	
				NoBusyTimeCount++;
			}
			else
			{
				NoBusyTimeCount = 0;
			}
		
			if (pAd->BusyDegree >= BUSY_4)
			{	
				VeryBusyTimeCount++;
			}
			else
			{
				VeryBusyTimeCount = 0;
			}
			
#ifdef DOT11N_DRAFT3
			btWifiThr = (UCHAR)(GET_PARAMETER_OF_TXRX_THR_THRESHOLD(pAd));

			//0,1,2,3 => 0, 6, 12, 18
			if ((btWifiThr <= 3) && IS_ENABLE_40TO20_BY_TIMER(pAd))
			{
				bIssue4020 = (WifiThroughputOverLimit(pAd,(btWifiThr*6)) == TRUE) & (VeryBusyTimeCount > 15);
			}
			else 
#endif // DOT11N_DRAFT3 //
			bIssue4020 = FALSE;

			DBGPRINT(RT_DEBUG_INFO, ("-->VeryBusyTimeCount = %ld, bIssue4020 = %d\n", VeryBusyTimeCount, bIssue4020));
			
			pAd->ulActiveCountPastPeriod = 0;
			TimeCount = 0;
			ulPowerSavingTimeCount = 0;
		}
		
		if (TimeCount == 0)
			Adjust(pAd, bIssue4020, NoBusyTimeCount);
	}while(0);

	
	if (bInitialValue)
	{
		UINT8 BbpR65 = 0xff;
		UINT32 MACValue;
		
		pAd->ulActiveCountPastPeriod = 0;
		TimeCount = 0;
		NoBusyTimeCount = 0;
		VeryBusyTimeCount = 0;

#ifdef DOT11_N_SUPPORT
		pAd->bPermitRecBaDown = FALSE;
		pAd->bPermitMcsDown = FALSE;
		pAd->bPermitTxBaSizeDown = FALSE;
		pAd->bPermitTxBaDensityDown = FALSE;
#endif // DOT11_N_SUPPORT //
		pAd->bPermitTxPowerDown = FALSE;
		pAd->bPermitLnaGainDown = FALSE;

		if (WM_COEX_TEST_FLAG(pAd, fWM_RESTORE_R65))
		{
			RTMP_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R65, &BbpR65);
			if (BbpR65 != 0x2c)
			{
				BbpR65 = 0x2c;
				RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R65, BbpR65);
			}
			WM_COEX_CLEAR_FLAG(pAd, fWM_RESTORE_R65);
		}

		if (WM_COEX_TEST_FLAG(pAd, fWM_RESTORE_MAC_SYS_CTRL))
		{
			RTMP_IO_READ32(pAd, MAC_SYS_CTRL , &MACValue);
			if (MACValue & 0x2000)
			{
				MACValue &= (~0x2000);
				RTMP_IO_WRITE32(pAd, MAC_SYS_CTRL , MACValue);
			}
			DBGPRINT(RT_DEBUG_TRACE, ("===> ............. --!! Enable TX halt coexHW\n"));
			WM_COEX_CLEAR_FLAG(pAd, fWM_RESTORE_MAC_SYS_CTRL);
		}
		
	}
}

VOID RTMPHwCoexSwitch(
	IN PRTMP_ADAPTER	pAd,
	IN UCHAR			switch_mode)
{
	ULONG			MacValue;
	UCHAR			WMEnable = 0;
	UCHAR			WMDisable = 0;
	
	
	RTMP_IO_READ32(pAd, LED_CFG, &MacValue);
	if (MacValue & 0x40000000)
	{
		WMEnable = 0x0c;
		WMDisable = 0x03;
	}
	else
	{
		WMEnable = 0x03;
		WMDisable = 0x0c;
	}

	DBGPRINT(RT_DEBUG_TRACE, ("%s():sw=%d,coFlags=0x%lx,LedMode=0x%x,LedCfg=0x%x,WMEnable=0x%x!\n", 
				__FUNCTION__, switch_mode, pAd->coexFlags, pAd->LedCntl.field.LedMode, MacValue, WMEnable));
	
	/*
		Only firmware mode will update LED_CFG (0x102c).
		If it is not firmware mode, we have to update LED_CFG.
	*/
	if ((pAd->LedCntl.field.LedMode == 0) || (pAd->LedCntl.field.LedMode > LED_MODE_SIGNAL_STREGTH))
	{
		MacValue &= 0xf0ffffff;
		if (switch_mode == COEX_SWTICH_ENABLE)
			MacValue |= (WMEnable << 24);
		else
			MacValue |= (WMDisable << 24);
		RTMP_IO_WRITE32(pAd, LED_CFG, MacValue);
	}
	else
	{
		/* firmware control mode, tell firmware to set bit[27:24]. */
		if (switch_mode == COEX_SWTICH_ENABLE)
			AsicSendCommandToMcu(pAd, 0x91, 0xff, WMEnable, 1);
		else 
			AsicSendCommandToMcu(pAd, 0x91, 0xff, WMDisable, 1);
	}
}


VOID RtmpHwCoexAntDiversity(
	IN PRTMP_ADAPTER	pAd,
	IN UCHAR			ant_mode)
{
	ULONG		MACValue, x;

	DBGPRINT(RT_DEBUG_TRACE, ("%s(): ant_mode=%d, coexFlags=0x%lx!\n", 
				__FUNCTION__, ant_mode, pAd->coexFlags));
	
	if (ant_mode == COEX_ANT_DIRECT)
	{
		//direct
		RTMP_IO_READ32(pAd, E2PROM_CSR, &x);
		x &= ~(EESK);
		RTMP_IO_WRITE32(pAd, E2PROM_CSR, x);
		RTMP_IO_READ32(pAd, GPIO_CTRL_CFG, &MACValue);
		MACValue &= ~(0x0808);
		MACValue |= 0x08;
		RTMP_IO_WRITE32(pAd, GPIO_CTRL_CFG, MACValue);
	}
	else
	{
		RTMP_IO_READ32(pAd, E2PROM_CSR, &x);
		x |= (EESK);
		RTMP_IO_WRITE32(pAd, E2PROM_CSR, x);
		RTMP_IO_READ32(pAd, GPIO_CTRL_CFG, &MACValue);
		MACValue &= ~(0x0808);		
		RTMP_IO_WRITE32(pAd, GPIO_CTRL_CFG, MACValue);
	}
}
#endif // RTMP_PCI_SUPPORT //


