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


#ifdef RT35xx

#include "rt_config.h"


#ifndef RTMP_RF_RW_SUPPORT
#error "You Should Enable compile flag RTMP_RF_RW_SUPPORT for this chip"
#endif // RTMP_RF_RW_SUPPORT //

REG_PAIR   RF3572_RFRegTable[] = {
	{RF_R00,		0x70},
	{RF_R01,		0x81},
	{RF_R02,		0xF1},
	{RF_R03,		0x02},
	{RF_R04,		0x4C},
	{RF_R05,		0x05},
	{RF_R06,		0x4A},
	{RF_R07,		0xD8},
//	{RF_R08,		0x80},
	{RF_R09,		0xC3},

	{RF_R10,		0xF1},
	{RF_R11,		0xB9},
	{RF_R12,		0x70},
	{RF_R13,		0x65},
	{RF_R14,		0xA0},
	{RF_R15,		0x53},
	{RF_R16,		0x4C},
	{RF_R17,		0x23},
	{RF_R18,		0xAC},
	{RF_R19,		0x93},

	{RF_R20,		0xB3},
	{RF_R21,		0xD0},
	{RF_R22,		0x00},	
	{RF_R23,		0x3C},
	{RF_R24,		0x16},
	{RF_R25,		0x15},
	{RF_R26,		0x85},
	{RF_R27,		0x00},
	{RF_R28,		0x00},
	{RF_R29,		0x9B},
	{RF_R30,		0x09},
	{RF_R31,		0x10},
};

#define	NUM_RF_3572REG_PARMS	(sizeof(RF3572_RFRegTable) / sizeof(REG_PAIR))

VOID NICInitRT3572RFRegisters(IN PRTMP_ADAPTER pAd)
{
	INT i;
	UINT32 RfReg = 0;
	UINT32 data;
	
	/*
		Driver must read EEPROM to get RfIcType before initial RF registers
		Initialize RF register to default value
		Init RF calibration
		Driver should toggle RF R30 bit7 before init RF registers
    */
    RT30xxReadRFRegister(pAd, RF_R30, (PUCHAR)&RfReg);
    RfReg |= 0x80;
    RT30xxWriteRFRegister(pAd, RF_R30, (UCHAR)RfReg);
    RTMPusecDelay(1000);
    RfReg &= 0x7F;
    RT30xxWriteRFRegister(pAd, RF_R30, (UCHAR)RfReg);        

	/* Initialize RF register to default value */
	for (i = 0; i < NUM_RF_3572REG_PARMS; i++)
	{
		RT30xxWriteRFRegister(pAd, RF3572_RFRegTable[i].Register, RF3572_RFRegTable[i].Value);
	}

	/* Driver should set RF R6 bit6 on before init RF registers */
	RT30xxReadRFRegister(pAd, RF_R06, (PUCHAR)&RfReg);
	RfReg |= 0x40;
	RT30xxWriteRFRegister(pAd, RF_R06, (UCHAR)RfReg);

	/* init R31 */
	//RT30xxWriteRFRegister(pAd, RF_R31, 0x14);

	if ((pAd->NicConfig2.field.DACTestBit == 1) && ((pAd->MACVersion & 0xffff) < 0x0211))
	{
		/* patch tx EVM issue temporarily */
		RTMP_IO_READ32(pAd, LDO_CFG0, &data);
		data = ((data & 0xF0FFFFFF) | 0x0D000000);
		RTMP_IO_WRITE32(pAd, LDO_CFG0, data);
	}
	else
	{
		// Patch for SRAM, increase voltage to 1.35V on core voltage and down to 1.2V after 1 msec
		RTMP_IO_READ32(pAd, LDO_CFG0, &data);
		data = ((data & 0xE0FFFFFF) | 0x0D000000);
		RTMP_IO_WRITE32(pAd, LDO_CFG0, data);

		RTMPusecDelay(1000);

		data = ((data & 0xE0FFFFFF) | 0x01000000);
		RTMP_IO_WRITE32(pAd, LDO_CFG0, data);
	}

	/* patch LNA_PE_G1 (toggle GPIO_SWITCH) is not necessary for 3572 */
	/*
	RTMP_IO_READ32(pAd, GPIO_SWITCH, &data);
	data &= ~(0x20);
	RTMP_IO_WRITE32(pAd, GPIO_SWITCH, data);
	*/
	
	/* For RF filter Calibration */
	RTMPFilterCalibration(pAd);

	/* save R25, R26 for 2.4GHz */
	BBP_IO_READ8_BY_REG_ID(pAd, BBP_R25, &pAd->Bbp25);
	BBP_IO_READ8_BY_REG_ID(pAd, BBP_R26, &pAd->Bbp26);

	/* set led open drain enable */
	RTMP_IO_READ32(pAd, OPT_14, &data);
	data |= 0x01;
	RTMP_IO_WRITE32(pAd, OPT_14, data);
}


/*
	==========================================================================
	Description:

	Reverse RF sleep-mode setup
	
	==========================================================================
 */
VOID RT3572ReverseRFSleepModeSetup(
	IN PRTMP_ADAPTER 	pAd)
{
	UCHAR RFValue;

	// RF_BLOCK_en, RF R1 register Bit 0 to 1
	RT30xxReadRFRegister(pAd, RF_R01, &RFValue);
	RFValue |= 0x01;
	RT30xxWriteRFRegister(pAd, RF_R01, RFValue);

	// VCO_IC, RF R7 register Bit 4 & Bit 5 to 1
	RT30xxReadRFRegister(pAd, RF_R07, &RFValue);
	RFValue |= 0x30;
	RT30xxWriteRFRegister(pAd, RF_R07, RFValue);

	// Idoh, RF R9 register Bit 1, Bit 2 & Bit 3 to 1
	RT30xxReadRFRegister(pAd, RF_R09, &RFValue);
	RFValue |= 0x0E;
	RT30xxWriteRFRegister(pAd, RF_R09, RFValue);

	// RX_CTB_en, RF R21 register Bit 7 to 1
	RT30xxReadRFRegister(pAd, RF_R21, &RFValue);
	RFValue |= 0x80;
	RT30xxWriteRFRegister(pAd, RF_R21, RFValue);
	RT30xxWriteRFRegister(pAd,RF_R08,(UCHAR)0x80);
}

/*
	========================================================================
	
	Routine Description: 3572/3592 R66 writing must select BBP_R27

	Arguments:

	Return Value:

	IRQL = 
	
	Note:
	
	========================================================================
*/
NTSTATUS	RT3572WriteBBPR66(
	IN	PRTMP_ADAPTER	pAd,
	IN	UCHAR			Value)
{
	NTSTATUS NStatus = STATUS_UNSUCCESSFUL;
	UCHAR	bbpData = 0;

	if (!IS_RT3572(pAd) && !IS_RT3593(pAd))
	{
		DBGPRINT(RT_DEBUG_ERROR, ("%s: Incorrect MAC version, pAd->MACVersion = 0x%X\n", 
			__FUNCTION__, 
			pAd->MACVersion));
		return NStatus;
	}
	RTMP_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R27, &bbpData);

	// R66 controls the gain of Rx0
	bbpData &= ~(0x60);	//clear bit 5,6
	{
#ifdef RTMP_MAC_PCI
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R27, bbpData);
#endif // RTMP_MAC_PCI //
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R66, Value);
	}

	// R66 controls the gain of Rx1
	bbpData |= 0x20;		// set bit 5
	{
#ifdef RTMP_MAC_PCI
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R27, bbpData);
#endif // RTMP_MAC_PCI //
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R66, Value);
		NStatus = STATUS_SUCCESS;
	}


	return NStatus;
}

#endif // RT35xx //

