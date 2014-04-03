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


#ifndef __MISC_CMM_H
#define __MISC_CMM_H


#define IS_ENABLE_MISC_TIMER(_pAd)							((((_pAd)->ulConfiguration & 0x00000001) == 0x00000001))
#ifdef DOT11_N_SUPPORT
#ifdef DOT11N_DRAFT3
#define IS_ENABLE_40TO20_BY_TIMER(_pAd) 					((((_pAd)->ulConfiguration & 0x00000003) == 0x00000003))
#endif // DOT11N_DRAFT3 //
#define IS_ENABLE_TXWI_AMPDU_SIZE_BY_TIMER(_pAd)			((((_pAd)->ulConfiguration & 0x00000005) == 0x00000005))
#define IS_ENABLE_TXWI_AMPDU_DENSITY_BY_TIMER(_pAd)		((((_pAd)->ulConfiguration & 0x00000009) == 0x00000009))

#define IS_ENABLE_RATE_ADAPTIVE_BY_TIMER(_pAd) 			((((_pAd)->ulConfiguration & 0x00000011) == 0x00000011))
#define IS_ENABLE_REJECT_ORE_BA_BY_TIMER(_pAd) 			((((_pAd)->ulConfiguration & 0x00000021) == 0x00000021))
#endif // DOT11_N_SUPPORT //
#define IS_ENABLE_TX_POWER_DOWN_BY_TIMER(_pAd) 			((((_pAd)->ulConfiguration & 0x00000041) == 0x00000041))
#define IS_ENABLE_LNA_MID_GAIN_DOWN_BY_TIMER(_pAd) 		((((_pAd)->ulConfiguration & 0x00000081) == 0x00000081))

#define IS_ENABLE_WIFI_ACTIVE_PULL_LOW_BY_FORCE(_pAd)	((((_pAd)->ulConfiguration & 0x00000400) == 0x00000400))
#ifdef RTMP_PCI_SUPPORT
#define IS_ENABLE_SINGLE_CRYSTAL_SHARING_BY_FORCE(_pAd)	((((_pAd)->ulConfiguration & 0x00000800) == 0x00000800))
#endif // RTMP_PCI_SUPPORT //

#define IS_ENABLE_WM_RATE_UP_BY_TIMER(_pAd)			        ((((_pAd)->ulBTConfiguration & 0x00001001) == 0x00001001))
#define IS_ENABLE_WM_WIFI_ACTIVE_PULL_HIGH_BY_TIMER(_pAd)	((((_pAd)->ulBTConfiguration & 0x00002001) == 0x00002001))
#define IS_ENABLE_WM_STATUS_CHECKING(_pAd)	                ((((_pAd)->ulConfiguration & 0x00004000) == 0x00004000))

#ifdef DOT11_N_SUPPORT
#define GET_PARAMETER_OF_AMPDU_SIZE(_pAd)				((((_pAd)->ulConfiguration & 0xC0000000) >> 30))
#define GET_PARAMETER_OF_AMPDU_DENSITY(_pAd)			((((_pAd)->ulConfiguration & 0x30000000) >> 28))
#define GET_PARAMETER_OF_MCS_THRESHOLD(_pAd)				((((_pAd)->ulConfiguration & 0x0C000000) >> 26))
#ifdef DOT11N_DRAFT3
#define GET_PARAMETER_OF_TXRX_THR_THRESHOLD(_pAd)		((((_pAd)->ulConfiguration & 0x03000000) >> 24))
#endif // DOT11N_DRAFT3 //
#endif // DOT11_N_SUPPORT //

#define WM_COEX_SET_FLAG(_pAd, _F)       ((_pAd)->coexFlags |= (_F))
#define WM_COEX_CLEAR_FLAG(_pAd, _F)     ((_pAd)->coexFlags &= ~(_F))
#define WM_COEX_TEST_FLAG(_pAd, _F)      (((_pAd)->coexFlags & (_F)) != 0)

#define fWM_CHECK_RADIO				0x00001000
#define fWM_RADIO_ON					0x00002000
#define fWM_RESTORE_R65				0x00004000
#define fWM_RESTORE_MAC_SYS_CTRL		0x00008000
#define fWM_GPIO4_ACTIVE_HIGH		0x00010000

/* BUSY_0: no busy, BUSY_1 to BUSY_5: more busy */
typedef enum _BUSY_DEGREE {
	BUSY_0 = 0, 
	BUSY_1,		
	BUSY_2,		
	BUSY_3,		  
	BUSY_4,		
	BUSY_5		
	
} BUSY_DEGREE;


typedef enum{
	CHK_BY_CNT = 1,
	CHK_BY_GPIO = 2,
}WM_CHK_METHOD;

#define DFT_WM_CHK_METHOD		CHK_BY_CNT
#endif /* __MISC_CMM_H */

