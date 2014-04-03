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


#ifndef __MISC_H
#define __MISC_H

#include "misc_cmm.h"

#define IN
#define OUT
#define INOUT

#define CYRSTALL_SHARED			0x01
#define WIFI_2040_SWITCH_THRESHOLD 	BUSY_4
#define HISTORY_RECORD_NUM 		5
#ifdef RTMP_PCI_SUPPORT
#define CHECK_TIME_INTERVAL 		100   //unit : 10ms, must follow timer interval
#define IDLE_STATE_THRESHOLD 	10
#define DETECT_TIMEOUT			10

//
// BT/WiFi Switch for RT3592BC8
//
// BT Enable and work on 2.4G.
//     TR[0] switch to 5G
//     It will only change to 1T1R and only use TR[1] when it link to 2.4G
//     It will only change to 2T2R and use TR[0], TR[1] when it link to 2.4G
//     
//
#define COEX_SWTICH_ENABLE						1  
#define COEX_SWTICH_DISABLE						0
#define COEX_ANT_DIRECT							1
#define COEX_ANT_CROSS							0

#endif // RTMP_PCI_SUPPORT //

#ifdef DOT11N_DRAFT3
BOOLEAN WifiThroughputOverLimit(
	IN	PRTMP_ADAPTER	pAd,
	IN  UCHAR WifiThroughputLimit);
#endif // DOT11N_DRAFT3 //

BUSY_DEGREE CheckBusy(
	IN PLONG History, 
	IN UCHAR HistorySize);

VOID Adjust(
	IN PRTMP_ADAPTER	pAd, 
	IN BOOLEAN			bIssue4020, 
	IN ULONG			NoBusyTimeCount);

VOID TxPowerDown(
	IN PRTMP_ADAPTER	pAd, 
	IN CHAR				Rssi,
	INOUT CHAR			*pDeltaPowerByBbpR1, 
	INOUT CHAR			*pDeltaPwr);

VOID McsDown(
	IN PRTMP_ADAPTER	pAd, 
	IN CHAR				CurrRateIdx, 
	IN PRTMP_TX_RATE_SWITCH	pCurrTxRate, 
	INOUT CHAR			*pUpRateIdx, 
	INOUT CHAR			*pDownRateIdx);

VOID McsDown2(
	IN PRTMP_ADAPTER	pAd, 
	IN UCHAR			MCS3, 
	IN UCHAR			MCS4, 
	IN UCHAR			MCS5, 
	IN UCHAR			MCS6, 
	INOUT UCHAR			*pTxRateIdx);

VOID TxBaSizeDown(
	IN PRTMP_ADAPTER	pAd, 
	INOUT PTXWI_STRUC 	pTxWI);


VOID TxBaDensityDown(
	IN PRTMP_ADAPTER	pAd, 
	INOUT PTXWI_STRUC 	pTxWI);


VOID MiscInit(
	IN PRTMP_ADAPTER pAd);

VOID MiscUserCfgInit(
	IN PRTMP_ADAPTER pAd);


VOID DetectExec(
	IN PVOID SystemSpecific1, 
	IN PVOID FunctionContext, 
	IN PVOID SystemSpecific2, 
	IN PVOID SystemSpecific3);

VOID RTMPHwCoexSwitch(
	IN PRTMP_ADAPTER	pAd,
	IN UCHAR			switch_mode);

VOID RtmpHwCoexAntDiversity(
	IN PRTMP_ADAPTER	pAd,
	IN UCHAR			ant_mode);

void rt3592cb_hw_ctrl(
	IN RTMP_ADAPTER *pAd);

void rt3592cb_ant_switch(
	IN RTMP_ADAPTER *pAd,
	IN UINT32 gpioVal);

INT WMUpdateRadioStatus(
	IN RTMP_ADAPTER *pAd);

#endif /* __MISC_H */

