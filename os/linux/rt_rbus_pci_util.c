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


// Function for TxDesc Memory allocation.
void RTMP_AllocateTxDescMemory(
	IN	PRTMP_ADAPTER pAd,
	IN	UINT	Index,
	IN	ULONG	Length,
	IN	BOOLEAN	Cached,
	OUT	PVOID	*VirtualAddress,
	OUT	PNDIS_PHYSICAL_ADDRESS PhysicalAddress)
{
	POS_COOKIE pObj = (POS_COOKIE)pAd->OS_Cookie;

	*VirtualAddress = (PVOID)pci_alloc_consistent(pObj->pci_dev,sizeof(char)*Length, PhysicalAddress);

}


// Function for MgmtDesc Memory allocation.
void RTMP_AllocateMgmtDescMemory(
	IN	PRTMP_ADAPTER pAd,
	IN	ULONG	Length,
	IN	BOOLEAN	Cached,
	OUT	PVOID	*VirtualAddress,
	OUT	PNDIS_PHYSICAL_ADDRESS PhysicalAddress)
{
	POS_COOKIE pObj = (POS_COOKIE)pAd->OS_Cookie;

	*VirtualAddress = (PVOID)pci_alloc_consistent(pObj->pci_dev,sizeof(char)*Length, PhysicalAddress);

}


// Function for RxDesc Memory allocation.
void RTMP_AllocateRxDescMemory(
	IN	PRTMP_ADAPTER pAd,
	IN	ULONG	Length,
	IN	BOOLEAN	Cached,
	OUT	PVOID	*VirtualAddress,
	OUT	PNDIS_PHYSICAL_ADDRESS PhysicalAddress)
{
	POS_COOKIE pObj = (POS_COOKIE)pAd->OS_Cookie;

	*VirtualAddress = (PVOID)pci_alloc_consistent(pObj->pci_dev,sizeof(char)*Length, PhysicalAddress);

}


// Function for free allocated Desc Memory.
void RTMP_FreeDescMemory(
	IN	PRTMP_ADAPTER pAd,
	IN	ULONG	Length,
	IN	PVOID	VirtualAddress,
	IN	NDIS_PHYSICAL_ADDRESS PhysicalAddress)
{
	POS_COOKIE pObj = (POS_COOKIE)pAd->OS_Cookie;
	
	pci_free_consistent(pObj->pci_dev, Length, VirtualAddress, PhysicalAddress);
}


// Function for TxData DMA Memory allocation.
void RTMP_AllocateFirstTxBuffer(
	IN	PRTMP_ADAPTER pAd,
	IN	UINT	Index,
	IN	ULONG	Length,
	IN	BOOLEAN	Cached,
	OUT	PVOID	*VirtualAddress,
	OUT	PNDIS_PHYSICAL_ADDRESS PhysicalAddress)
{
	POS_COOKIE pObj = (POS_COOKIE)pAd->OS_Cookie;

	*VirtualAddress = (PVOID)pci_alloc_consistent(pObj->pci_dev,sizeof(char)*Length, PhysicalAddress);
}


void RTMP_FreeFirstTxBuffer(
	IN	PRTMP_ADAPTER pAd,
	IN	ULONG	Length,
	IN	BOOLEAN	Cached,
	IN	PVOID	VirtualAddress,
	IN	NDIS_PHYSICAL_ADDRESS PhysicalAddress)
{
	POS_COOKIE pObj = (POS_COOKIE)pAd->OS_Cookie;

	pci_free_consistent(pObj->pci_dev, Length, VirtualAddress, PhysicalAddress);
}


/*
 * FUNCTION: Allocate a packet buffer for DMA
 * ARGUMENTS:
 *     AdapterHandle:  AdapterHandle
 *     Length:  Number of bytes to allocate
 *     Cached:  Whether or not the memory can be cached
 *     VirtualAddress:  Pointer to memory is returned here
 *     PhysicalAddress:  Physical address corresponding to virtual address
 * Notes:
 *     Cached is ignored: always cached memory
 */
PNDIS_PACKET RTMP_AllocateRxPacketBuffer(
	IN	PRTMP_ADAPTER pAd,
	IN	ULONG	Length,
	IN	BOOLEAN	Cached,
	OUT	PVOID	*VirtualAddress,
	OUT	PNDIS_PHYSICAL_ADDRESS PhysicalAddress)
{
	struct sk_buff *pkt;

//	pkt = dev_alloc_skb(Length);
	DEV_ALLOC_SKB(pAd, pkt, Length);

	if (pkt == NULL) {
		DBGPRINT(RT_DEBUG_ERROR, ("can't allocate rx %ld size packet\n",Length));
	}

	if (pkt) {
		RTMP_SET_PACKET_SOURCE(OSPKT_TO_RTPKT(pkt), PKTSRC_NDIS);
		*VirtualAddress = (PVOID) pkt->data;	
//#ifdef CONFIG_5VT_ENHANCE
//		*PhysicalAddress = PCI_MAP_SINGLE(pAd, *VirtualAddress, 1600, PCI_DMA_FROMDEVICE);
//#else
		*PhysicalAddress = PCI_MAP_SINGLE(pAd, *VirtualAddress, Length,  -1, PCI_DMA_FROMDEVICE);
//#endif
	} else {
		*VirtualAddress = (PVOID) NULL;
		*PhysicalAddress = (NDIS_PHYSICAL_ADDRESS) NULL;
	}	

	return (PNDIS_PACKET) pkt;
}

/*
 * invaild or writeback cache 
 * and convert virtual address to physical address 
 */
dma_addr_t linux_pci_map_single(void *handle, void *ptr, size_t size, int sd_idx, int direction)
{
	PRTMP_ADAPTER pAd;
	POS_COOKIE pObj;
	
	/* 
		------ Porting Information ------
		> For Tx Alloc:
			mgmt packets => sd_idx = 0
			SwIdx: pAd->MgmtRing.TxCpuIdx
			pTxD : pAd->MgmtRing.Cell[SwIdx].AllocVa;
	 
			data packets => sd_idx = 1
	 		TxIdx : pAd->TxRing[pTxBlk->QueIdx].TxCpuIdx 
	 		QueIdx: pTxBlk->QueIdx 
	 		pTxD  : pAd->TxRing[pTxBlk->QueIdx].Cell[TxIdx].AllocVa;

	 	> For Rx Alloc:
	 		sd_idx = -1
	*/

	pAd = (PRTMP_ADAPTER)handle;
	pObj = (POS_COOKIE)pAd->OS_Cookie;
	
	if (sd_idx == 1)
	{
		PTX_BLK		pTxBlk;
		pTxBlk = (PTX_BLK)ptr;
		return pci_map_single(pObj->pci_dev, pTxBlk->pSrcBufData, pTxBlk->SrcBufLen, direction);
	}
	else
	{
		return pci_map_single(pObj->pci_dev, ptr, size, direction);
	}

}

void linux_pci_unmap_single(void *handle, dma_addr_t dma_addr, size_t size, int direction)
{
	PRTMP_ADAPTER pAd;
	POS_COOKIE pObj;

	pAd=(PRTMP_ADAPTER)handle;
	pObj = (POS_COOKIE)pAd->OS_Cookie;
	
	if (size > 0)
		pci_unmap_single(pObj->pci_dev, dma_addr, size, direction);
	
}

#ifdef CONFIG_STA_SUPPORT
#ifdef PCIE_PS_SUPPORT
/*
	========================================================================
	
	Routine Description:

	Arguments:
		Level = RESTORE_HALT : Restore PCI host and Ralink PCIe Link Control field to its default value.
		Level = Other Value : Restore from dot11 power save or radio off status. And force PCI host Link Control fields to 0x1

	========================================================================
*/
VOID RTMPPCIeLinkCtrlValueRestore(
	IN	PRTMP_ADAPTER	pAd,
	IN   UCHAR		Level)
{
	USHORT  PCIePowerSaveLevel, reg16;
	USHORT	Configuration;
	POS_COOKIE 	pObj;

	pObj = (POS_COOKIE) pAd->OS_Cookie;

	if (!OPSTATUS_TEST_FLAG(pAd, fOP_STATUS_ADVANCE_POWER_SAVE_PCIE_DEVICE))
		return;

	// Check PSControl Configuration
	if (pAd->StaCfg.PSControl.field.EnableNewPS == FALSE)
		return;

	//3090 will not execute the following codes.
    	// Check interface : If not PCIe interface, return. 
#ifdef RT2860
	if (!((pObj->DeviceID == NIC2860_PCIe_DEVICE_ID) 
		||(pObj->DeviceID == NIC2790_PCIe_DEVICE_ID)))
		return;
#endif // RT2860 //



#ifdef RT3590
	if ((pObj->DeviceID == NIC3590_PCIe_DEVICE_ID) 
		||(pObj->DeviceID == NIC3591_PCIe_DEVICE_ID)
		||(pObj->DeviceID == NIC3592_PCIe_DEVICE_ID))
		return;
#endif // RT3390 //

	DBGPRINT(RT_DEBUG_TRACE, ("%s.===>\n", __FUNCTION__));
	PCIePowerSaveLevel = pAd->PCIePowerSaveLevel;
	if ((PCIePowerSaveLevel&0xff) == 0xff)
	{
		DBGPRINT(RT_DEBUG_TRACE,("return  \n"));
		return;
	}

	if (pObj->parent_pci_dev && (pAd->HostLnkCtrlOffset != 0))
    {
        PCI_REG_READ_WORD(pObj->parent_pci_dev, pAd->HostLnkCtrlOffset, Configuration);
        if ((Configuration != 0) &&
            (Configuration != 0xFFFF))
        {
    		Configuration &= 0xfefc;
    		// If call from interface down, restore to orginial setting.
    		if (Level == RESTORE_CLOSE)
    		{
    			Configuration |= pAd->HostLnkCtrlConfiguration;
    		}
    		else
    			Configuration |= 0x0;
            PCI_REG_WIRTE_WORD(pObj->parent_pci_dev, pAd->HostLnkCtrlOffset, Configuration);
    		DBGPRINT(RT_DEBUG_TRACE, ("Restore PCI host : offset 0x%x = 0x%x\n", pAd->HostLnkCtrlOffset, Configuration));
        }
        else
            DBGPRINT(RT_DEBUG_ERROR, ("Restore PCI host : PCI_REG_READ_WORD failed (Configuration = 0x%x)\n", Configuration));
    }
	
    if (pObj->pci_dev && (pAd->RLnkCtrlOffset != 0))
    {           
        PCI_REG_READ_WORD(pObj->pci_dev, pAd->RLnkCtrlOffset, Configuration);
        if ((Configuration != 0) &&
            (Configuration != 0xFFFF))
        {
    		Configuration &= 0xfefc;
			// If call from interface down, restore to orginial setting.
			if (Level == RESTORE_CLOSE)
            	Configuration |= pAd->RLnkCtrlConfiguration;
			else
				Configuration |= 0x0;
            PCI_REG_WIRTE_WORD(pObj->pci_dev, pAd->RLnkCtrlOffset, Configuration);
    		DBGPRINT(RT_DEBUG_TRACE, ("Restore Ralink : offset 0x%x = 0x%x\n", pAd->RLnkCtrlOffset, Configuration));
        }
        else
            DBGPRINT(RT_DEBUG_ERROR, ("Restore Ralink : PCI_REG_READ_WORD failed (Configuration = 0x%x)\n", Configuration));
	}
    
	DBGPRINT(RT_DEBUG_TRACE,("%s <===\n", __FUNCTION__));
}

/*
	========================================================================
	
	Routine Description:

	Arguments:
		Max : limit Host PCI and Ralink PCIe device's LINK CONTROL field's value. 
		Because now frequently set our device to mode 1 or mode 3 will cause problem.
		
	========================================================================
*/
VOID RTMPPCIeLinkCtrlSetting(
	IN	PRTMP_ADAPTER	pAd,
	IN 	USHORT		Max)
{
	USHORT  PCIePowerSaveLevel, reg16;
	USHORT	Configuration;
	POS_COOKIE 	pObj;

	pObj = (POS_COOKIE) pAd->OS_Cookie;

	if (!OPSTATUS_TEST_FLAG(pAd, fOP_STATUS_ADVANCE_POWER_SAVE_PCIE_DEVICE))
		return;

	// Check PSControl Configuration
	if (pAd->StaCfg.PSControl.field.EnableNewPS == FALSE)
		return;

	// Check interface : If not PCIe interface, return.
	//Block 3090 to enter the following function
	
#ifdef RT2860
	if (!((pObj->DeviceID == NIC2860_PCIe_DEVICE_ID) 
		||(pObj->DeviceID == NIC2790_PCIe_DEVICE_ID)))
		return;
#endif // RT2860 //

	if (!RTMP_TEST_PSFLAG(pAd, fRTMP_PS_CAN_GO_SLEEP))
	{
		DBGPRINT(RT_DEBUG_INFO, ("RTMPPCIePowerLinkCtrl return on fRTMP_PS_CAN_GO_SLEEP flag\n"));			
		return;
	}
	DBGPRINT(RT_DEBUG_TRACE,("%s===>\n", __FUNCTION__));
	PCIePowerSaveLevel = pAd->PCIePowerSaveLevel;
	if ((PCIePowerSaveLevel&0xff) == 0xff)
	{
		DBGPRINT(RT_DEBUG_TRACE,("return  \n"));
		return;
	}
	PCIePowerSaveLevel = PCIePowerSaveLevel>>6;	    

    // Skip non-exist deice right away
	if (pObj->parent_pci_dev && (pAd->HostLnkCtrlOffset != 0))
	{
        PCI_REG_READ_WORD(pObj->parent_pci_dev, pAd->HostLnkCtrlOffset, Configuration);
		switch (PCIePowerSaveLevel)
		{
			case 0:
				// Set b0 and b1 of LinkControl (both 2892 and PCIe bridge) to 00 
				Configuration &= 0xfefc;
				break;
			case 1:
				// Set b0 and b1 of LinkControl (both 2892 and PCIe bridge) to 01 
				Configuration &= 0xfefc;
				Configuration |= 0x1;
				break;
			case 2:
				//  Set b0 and b1 of LinkControl (both 2892 and PCIe bridge) to 11 
				Configuration &= 0xfefc;
				Configuration |= 0x3;
				break;
			case 3:
				// Set b0 and b1 of LinkControl (both 2892 and PCIe bridge) to 11 and bit 8 of LinkControl of 2892 to 1
				Configuration &= 0xfefc;
				Configuration |= 0x103;				
				break;
		}
        PCI_REG_WIRTE_WORD(pObj->parent_pci_dev, pAd->HostLnkCtrlOffset, Configuration);
		DBGPRINT(RT_DEBUG_TRACE, ("Write PCI host offset 0x%x = 0x%x\n", pAd->HostLnkCtrlOffset, Configuration));
	}

	if (pObj->pci_dev && (pAd->RLnkCtrlOffset != 0))
	{
		// first 2892 chip not allow to frequently set mode 3. will cause hang problem.
		if (PCIePowerSaveLevel > Max)
			PCIePowerSaveLevel = Max;        

        PCI_REG_READ_WORD(pObj->pci_dev, pAd->RLnkCtrlOffset, Configuration);
		Configuration |= 0x100;
        PCI_REG_WIRTE_WORD(pObj->pci_dev, pAd->RLnkCtrlOffset, Configuration);
		DBGPRINT(RT_DEBUG_TRACE, ("Write Ralink device : offset 0x%x = 0x%x\n", pAd->RLnkCtrlOffset, Configuration));
	}

	DBGPRINT(RT_DEBUG_TRACE,("RTMPPCIePowerLinkCtrl <==============\n"));
}
#endif // PCIE_PS_SUPPORT //
#endif // CONFIG_STA_SUPPORT //

/* End of rt_usb_util.c */
