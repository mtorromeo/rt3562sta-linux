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


#ifndef __RT35XX_H__
#define __RT35XX_H__

#ifdef RT35xx


#ifdef RTMP_PCI_SUPPORT
#include "chip/mac_pci.h"
#endif

#ifndef RTMP_RF_RW_SUPPORT
#error "For RT3062/3562/3572/3592, you should define the compile flag -DRTMP_RF_RW_SUPPORT"
#endif

#include "chip/rt30xx.h"

extern REG_PAIR   RF3572_RFRegTable[];

//
// Device ID & Vendor ID, these values should match EEPROM value
//
#define NIC3062_PCI_DEVICE_ID	0x3062		// 2T/2R miniCard
#define NIC3562_PCI_DEVICE_ID	0x3562		// 2T/2R miniCard
#define NIC3060_PCI_DEVICE_ID	0x3060		// 1T/1R miniCard

#ifdef  DFS_SUPPORT
#define DFS_2_SUPPORT

#define DFS_INTERRUPT_SUPPORT
#define DFS_HWTIMER_SUPPORT
#endif // DFS_SUPPORT //
/* use CHIPSET = 3562 compile */
#define NIC3592_PCIe_DEVICE_ID	0x3592		// 2T/2R miniCard

#endif // RT35xx //

#endif //__RT35XX_H__ //

