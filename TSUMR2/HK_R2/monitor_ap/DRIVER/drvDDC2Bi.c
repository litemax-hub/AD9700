
#include "Global.h"
#include "MsOS.h"
#include "MsIRQ.h"

BYTE idata rxInputPort; // Annie 2006.7.5
BYTE idata rxComboPort; // Combo port index
BYTE idata rxIndex = 0;
BYTE idata rxStatus;
BYTE idata rxLength = 0;
#if !D2B_HW_CheckSum
BYTE idata rxChkSum = 0;
#endif
BYTE idata txStatus;
idata BYTE *txBufferPtr;
BYTE idata txLength;
//BYTE sendData;

/*code*/ BYTE nullMsg1[3] =
{
    0x6e, 0x80, 0xbe
};
/*code*/ BYTE nullMsg[3] =
{
    0x51, 0x80, 0xbe
};

#if D2B_XShared_DDCBuffer
BYTE *DDCBuffer;
#else
BYTE xdata DDCBuffer[DDC_BUFFER_LENGTH];
#endif

#if D2B_FIFO_Mode
WORD idata txIndex;
WORD idata Previous_DDC2MCU_CUR_ADR;
WORD idata Present_DDC2MCU_CUR_ADR;
WORD idata Present_DDC2MCU_START_ADR;
BYTE idata D2BErrorFlag;
#if D2B_XDATA_DEBUG
//BYTE xdata XSRAMBuffer[D2B_FIFO_XdataSize_Total+256];// _at_ XDATA_DDC_ADDR_START;
BYTE xdata IntCount;
BYTE xdata IntCountTmp;
#else
//BYTE xdata XSRAMBuffer[D2B_FIFO_XdataSize_Total];// _at_ XDATA_DDC_ADDR_START;
#endif
#endif

BYTE D2B_FIFOMODE_DDCPORT2INPUT(BYTE ComboPort)
{
    BYTE u8rtn;

    if(ComboPort == InputCombo_A0)
        u8rtn = Input_VGA;
#if USB_HUB
#if (CHIP_ID == CHIP_MT9701)
    else if(ComboPort == InputCombo_D2)
#else
    else if(ComboPort == InputCombo_D3)
#endif
        u8rtn = 0xFF; // return 0xFF for HUB port to prevent write DDC to SRAM fail if drvDDC2Bi_MessageReady() treats DDC_HUB as DP port type
#endif
    else
        u8rtn = msDrvMapComboToPortIndex(ComboPort);
    
    return u8rtn;
}

#if D2B_FIFO_Mode
BYTE DDC2Bi_FIFOMODE_READ_CUR(BYTE InputPort)
{
    BYTE CurValue=0;

    if(InputPort==InputCombo_A0) // A0
        CurValue = msRegs(REG_003EE4);
#if DDC_Port_D0
    else if(InputPort==InputCombo_D0) // D0
        CurValue = msRegs(REG_003EC6);
#endif
#if DDC_Port_D1
    else if(InputPort==InputCombo_D1) // D1
        CurValue = msRegs(REG_003ECC);
#endif
#if DDC_Port_D2
    else if(InputPort==InputCombo_D2) // D2
        CurValue = msRegs(REG_003ED2);
#endif
#if DDC_Port_D3
	else if(InputPort==InputCombo_D3) // D3
		CurValue = msRegs(REG_003EBA);
#endif

    return CurValue;
}

#if (D2B_SIMPLIFIED_MODE == 0)
void DDC2Bi_FIFOMODE_ADR_UPDATE(BYTE InputPort)
{
    if(InputPort==InputCombo_A0) // A0
        msRegs(REG_003EE4) = BIT0;
#if DDC_Port_D0
    else if(InputPort==InputCombo_D0) // D0
        msRegs(REG_003EC6) = BIT0;
#endif
#if DDC_Port_D1
    else if(InputPort==InputCombo_D1) // D1
        msRegs(REG_003ECC) = BIT0;
#endif
#if DDC_Port_D2
    else if(InputPort==InputCombo_D2) // D2
        msRegs(REG_003ED2) = BIT0;
#endif

#if DDC_Port_D3
    else if(InputPort==InputCombo_D3) // D3
        msRegs(REG_003EBA) = BIT0;
#endif



    return;
}
#endif

BYTE DDC2Bi_FIFOMODE_READ_START(BYTE InputComboPort)
{
    BYTE StartValue=0;

    if(InputComboPort==InputCombo_A0) // A0
        StartValue = msRegs(REG_003EE5);
#if DDC_Port_D0
    else if(InputComboPort==InputCombo_D0) // D0
        StartValue = msRegs(REG_003EC7);
#endif
#if DDC_Port_D1
    else if(InputComboPort==InputCombo_D1) // D1
        StartValue = msRegs(REG_003ECD);
#endif
#if DDC_Port_D2
    else if(InputComboPort==InputCombo_D2)// D2
        StartValue = msRegs(REG_003ED3);
#endif
#if DDC_Port_D3
	else if(InputComboPort==InputCombo_D3) // D3
		StartValue = msRegs(REG_003EBB);
#endif

    return StartValue;
}

WORD DDC2Bi_FIFOMODE_XDATA_ADDR_OFFSET(BYTE InputComboPort, BYTE u8TXBuffer)
{
    WORD XdataAddrStart=0;
#if !D2B_DOUBLE_FIFO_Mode
        u8TXBuffer = 0;
#endif

    if(InputComboPort==InputCombo_A0) // A0
        XdataAddrStart = u8TXBuffer?D2B_FIFO_XdataWriteAddressStart_A0:D2B_FIFO_XdataAddressStart_A0;
#if DDC_Port_D0
    else if(InputComboPort==InputCombo_D0) // D0
        XdataAddrStart = u8TXBuffer?D2B_FIFO_XdataWriteAddressStart_D0:D2B_FIFO_XdataAddressStart_D0;
#endif
#if DDC_Port_D1
    else if(InputComboPort==InputCombo_D1) // D1
        XdataAddrStart = u8TXBuffer?D2B_FIFO_XdataWriteAddressStart_D1:D2B_FIFO_XdataAddressStart_D1;
#endif
#if DDC_Port_D2
    else if(InputComboPort==InputCombo_D2) // D2
        XdataAddrStart = u8TXBuffer?D2B_FIFO_XdataWriteAddressStart_D2:D2B_FIFO_XdataAddressStart_D2;
#endif
#if DDC_Port_D3
	else if(InputComboPort==InputCombo_D3) // D3
		XdataAddrStart = u8TXBuffer?D2B_FIFO_XdataWriteAddressStart_D3:D2B_FIFO_XdataAddressStart_D3;
#endif

    return XdataAddrStart-D2B_FIFO_XdataAddressStart;
}

void DDC2Bi_FIFOMODE_SET_RW_DONE(BYTE InputComboPort)
{
    if(InputComboPort==InputCombo_A0) // A0
        msRegs(REG_003EE6) |= (BIT3);
#if DDC_Port_D0
    else if(InputComboPort==InputCombo_D0) // D0
        msRegs(REG_003EC8) |= (BIT3);
#endif
#if DDC_Port_D1
    else if(InputComboPort==InputCombo_D1) // D1
        msRegs(REG_003ECE) |= (BIT3);
#endif
#if DDC_Port_D2
    else if(InputComboPort==InputCombo_D2) // D2
        msRegs(REG_003EE0) |= (BIT3);
#endif
#if DDC_Port_D3
    else if(InputComboPort==InputCombo_D3) // D3
        msRegs(REG_003EBC) |= (BIT3);
#endif

}

void DDC2Bi_FIFOMODE_CLR_EMPTYFULL_FLAG(BYTE InputComboPort)
{
    if(InputComboPort==InputCombo_A0) // A0
        msRegs(REG_003EE6) |= (BIT5|BIT4);
#if DDC_Port_D0
    else if(InputComboPort==InputCombo_D0) // D0
        msRegs(REG_003EC8) |= (BIT5|BIT4);
#endif
#if DDC_Port_D1
    else if(InputComboPort==InputCombo_D1) // D1
        msRegs(REG_003ECE) |= (BIT5|BIT4);
#endif
#if DDC_Port_D2
    else if(InputComboPort==InputCombo_D2) // D2
        msRegs(REG_003EE0) |= (BIT5|BIT4);
#endif
#if DDC_Port_D3
    else if(InputComboPort==InputCombo_D3) // D3
        msRegs(REG_003EBC) |= (BIT5|BIT4);
#endif

}

BYTE DDC2Bi_FIFOMODE_READ_FULL_FLAG(BYTE InputComboPort)
{
    BYTE FlagValue=0;

    if(InputComboPort==InputCombo_A0) // A0
        FlagValue = msRegs(REG_003EE6);
#if DDC_Port_D0
    else if(InputComboPort==InputCombo_D0) // D0
        FlagValue = msRegs(REG_003EC8);
#endif
#if DDC_Port_D1
    else if(InputComboPort==InputCombo_D1) // D1
        FlagValue = msRegs(REG_003ECE);
#endif
#if DDC_Port_D2
	else if(InputComboPort==InputCombo_D2)// D2
	    FlagValue = msRegs(REG_003EE0);
#endif
#if DDC_Port_D3
	else if(InputComboPort==InputCombo_D3) // D3
		FlagValue = msRegs(REG_003EBC);
#endif

    return FlagValue&BIT6;
}

#if !D2B_XShared_DDCBuffer
BYTE DDC2Bi_FIFOMODE_READ_EMPTY_FLAG(BYTE InputComboPort)
{
    BYTE FlagValue=0;

    if(InputComboPort==InputCombo_A0) // A0
        FlagValue = msRegs(REG_003EE6);
#if DDC_Port_D0
    else if(InputComboPort==InputCombo_D0) // D0
        FlagValue = msRegs(REG_003EC8);
#endif
#if DDC_Port_D1
    else if(InputComboPort==InputCombo_D1) // D1
        FlagValue = msRegs(REG_003ECE);
#endif
#if DDC_Port_D2
    else if(InputComboPort==InputCombo_D2)// D2
        FlagValue = msRegs(REG_003EE0);
#endif
#if DDC_Port_D3
	else if(InputComboPort==InputCombo_D3) // D3
		FlagValue = msRegs(REG_003EBC);
#endif

    return FlagValue&BIT7;
}
#endif

#if D2B_HW_CheckSum
BYTE DDC2Bi_FIFOMODE_READ_HW_CHKSUM_FLAG(BYTE InputComboPort)
{
    BYTE HWCSValue=0;

    if(InputComboPort==InputCombo_A0) // A0
        HWCSValue = msRegs(REG_003EC2)&BIT0;
#if DDC_Port_D0
    else if(InputComboPort==InputCombo_D0) // D0
        HWCSValue = msRegs(REG_003EC2)&BIT1;
#endif
#if DDC_Port_D1
    else if(InputComboPort==InputCombo_D1) // D1
        HWCSValue = msRegs(REG_003EF0)&BIT0;
#endif
#if DDC_Port_D2
    else if(InputComboPort==InputCombo_D2)// D2
        HWCSValue = msRegs(REG_003EF0)&BIT1;
#endif
#if DDC_Port_D3
	else if(InputComboPort==InputCombo_D3) // D3
		HWCSValue = msRegs(REG_003EB6)&BIT5;
#endif

    return HWCSValue;
}
#endif

void DDC2Bi_CLR_INT(BYTE InputComboPort, BYTE ClrBits)
{
    if(InputComboPort==InputCombo_A0) // A0
        DDCADC_INT_CLR |= ClrBits;
#if DDC_Port_D0
    else if(InputComboPort==InputCombo_D0) // D0
        DDCDVI0_INT_CLR |= ClrBits;
#endif
#if DDC_Port_D1
    else if(InputComboPort==InputCombo_D1) // D1
        DDCDVI1_INT_CLR |= ClrBits;
#endif
#if DDC_Port_D2
    else if(InputComboPort==InputCombo_D2)// D2
        DDCDVI2_INT_CLR |= ClrBits;
#endif
#if DDC_Port_D3
    else if(InputComboPort==InputCombo_D3) // D3
        DDCDVI3_INT_CLR |= ClrBits;
#endif

}
#endif

#if D2B_Auto_Clk_Gating

void DDC2Bi_Auto_Clk_TurnOn(void)
{
        if(DDC_ADC_Busy_Status)  //Check DDC Busy status
        {
            D2B_Enable_Clk_Gating_A0(FALSE);
        }
        else
        {
            D2B_Enable_Clk_Gating_A0(TRUE);
        }
    #if DDC_Port_D0
        if(DDC_DVI0_Busy_Status) //Check DDC Busy status
        {
            D2B_Enable_Clk_Gating_D0(FALSE); //D0 auto clock gating disable
        }
        else
        {
            D2B_Enable_Clk_Gating_D0(TRUE); //D0 auto clock gating enable
        }

    #endif

    #if DDC_Port_D1
        if(DDC_DVI1_Busy_Status) //Check DDC Busy status
        {
            D2B_Enable_Clk_Gating_D1(FALSE); //D1 auto clock gating disable
        }
        else
        {
            D2B_Enable_Clk_Gating_D1(TRUE); //D1 auto clock gating enable
        }
    #endif
    #if DDC_Port_D2
        if(DDC_DVI2_Busy_Status) //Check DDC Busy status
        {
            D2B_Enable_Clk_Gating_D2(FALSE); //D2 auto clock gating disable
        }
        else
        {
           D2B_Enable_Clk_Gating_D2(TRUE); //D2 auto clock gating enable
        }
    #endif
    #if DDC_Port_D3
        if(DDC_DVI3_Busy_Status) //Check DDC Busy status
        {
            D2B_Enable_Clk_Gating_D3(FALSE); //D3 auto clock gating disable
        }
        else
        {
           D2B_Enable_Clk_Gating_D3(TRUE); //D3 auto clock gating enable
        }
    #endif

}
#endif


void DDC2Bi_SET_RBUF(BYTE InputComboPort, BYTE RBufData)
{
    if(InputComboPort==InputCombo_A0) // A0
        ADC_RBUF_WDP = RBufData;
#if DDC_Port_D0
    else if(InputComboPort==InputCombo_D0) // D0
        DDCDVI0_RBUF_WDP = RBufData;
#endif
#if DDC_Port_D1
    else if(InputComboPort==InputCombo_D1) // D1
        DDCDVI1_RBUF_WDP = RBufData;
#endif
#if DDC_Port_D2
    else if(InputComboPort==InputCombo_D2) // D2
        DDCDVI2_RBUF_WDP = RBufData;
#endif
#if DDC_Port_D3
    else if(InputComboPort==InputCombo_D3) // D3
        DDCDVI3_RBUF_WDP = RBufData;
#endif

}

void DDC2Bi_SET_WP_READY(BYTE InputComboPort)
{
    if(InputComboPort==InputCombo_A0) // A0
        DDC_ADC_WP_READY();
#if DDC_Port_D0
    else if(InputComboPort==InputCombo_D0) // D0
        DDC_DVI0_WP_READY();
#endif
#if DDC_Port_D1
    else if(InputComboPort==InputCombo_D1) // D1
        DDC_DVI1_WP_READY();
#endif
#if DDC_Port_D2
    else if(InputComboPort==InputCombo_D2) // D2
        DDC_DVI2_WP_READY();
#endif
#if DDC_Port_D3
    else if(InputComboPort==InputCombo_D3) // D3
        DDC_DVI3_WP_READY();
#endif

}

BYTE DDC2Bi_Read_INT(BYTE InputComboPort)
{
    BYTE IntData=0;

    if(InputComboPort==InputCombo_A0) // A0
        IntData = DDCADC_INT_FLAG;
#if DDC_Port_D0
    else if(InputComboPort==InputCombo_D0) // D0
        IntData = DDCDVI0_INT_FLAG;
#endif
#if DDC_Port_D1
    else if(InputComboPort==InputCombo_D1) // D1
        IntData = DDCDVI1_INT_FLAG;
#endif
#if DDC_Port_D2
    else if(InputComboPort==InputCombo_D2)// D2
        IntData = DDCDVI2_INT_FLAG;
#endif
#if DDC_Port_D3
	else if(InputComboPort==InputCombo_D3) // D3
		IntData = DDCDVI3_INT_FLAG;
#endif

    return IntData;
}

BYTE DDC2Bi_Read_WBUF(BYTE InputComboPort)
{
    BYTE WbufData=0;

    if(InputComboPort==InputCombo_A0) // A0
        WbufData = ADC_WBUF_RDP;
#if DDC_Port_D0
    else if(InputComboPort==InputCombo_D0) // D0
        WbufData = DDCDVI0_WBUF_RDP;
#endif
#if DDC_Port_D1
    else if(InputComboPort==InputCombo_D1) // D1
        WbufData = DDCDVI1_WBUF_RDP;
#endif
#if DDC_Port_D2
    else if(InputComboPort==InputCombo_D2)// D2
        WbufData = DDCDVI2_WBUF_RDP;
#endif
#if DDC_Port_D3
	else if(InputComboPort==InputCombo_D3) // D3
		WbufData = DDCDVI3_WBUF_RDP;
#endif

    return WbufData;
}

void drvDDC2Bi_Init( void )
{

    // mask all ddc interrupt
    D2B_INT_MASK_ALL(); // drvDDC2Bi_Init();

    DDC2Bi_InitRx();
    txStatus = DDC2B_CLEAR;

    txBufferPtr = &nullMsg1[0];
    txLength = sizeof( nullMsg1 );

    D2B_SET_ID_VGA(TRUE);
    D2B_INT_MASK_VGA();
    CLR_ADC_INT_FLAG();
    DDCADC_RBUF_WDP = *txBufferPtr++;
    DDC_ADC_WP_READY();

#if DDC_Port_D0
    D2B_SET_ID_DVI0(TRUE);
    D2B_INT_MASK_DVI0();
    CLR_DVI0_INT_FLAG();
    DDCDVI0_RBUF_WDP = *txBufferPtr++;
    DDC_DVI0_WP_READY();
#endif
#if DDC_Port_D1
    D2B_SET_ID_DVI1(TRUE);
    D2B_INT_MASK_DVI1();
    CLR_DVI1_INT_FLAG();
    DDCDVI1_RBUF_WDP = *txBufferPtr++;
    DDC_DVI1_WP_READY();
#endif
#if DDC_Port_D2
    D2B_SET_ID_DVI2(TRUE);
    D2B_INT_MASK_DVI2();
    CLR_DVI2_INT_FLAG();
    DDCDVI2_RBUF_WDP = *txBufferPtr++;
    DDC_DVI2_WP_READY();
#endif
#if DDC_Port_D3
    D2B_SET_ID_DVI3(TRUE);
    D2B_INT_MASK_DVI3();
    CLR_DVI3_INT_FLAG();
    DDCDVI3_RBUF_WDP = *txBufferPtr++;
    DDC_DVI3_WP_READY();
#endif


#if ENABLE_DP_INPUT
   // D2B_SET_ID_DP(TRUE);
#endif

#if 0//ENABLE_3D_FUNCTION && GLASSES_TYPE==GLASSES_NVIDIA
    DDCDVI_NV3D_DDCCI_ID = 0xFD;
    DDCDVI_NV3D_INTMASK_EN = BIT6 | BIT5 | BIT2 | BIT1;
    CLR_DDC_DVI_NV3D_INT_FLAG();
#endif

//Clear_Botton_FIFO();

#if D2B_FIFO_Mode
  #if D2B_XShared_DDCBuffer
    //DDCBuffer = &XSRAMBuffer[0];
     DDCBuffer = D2B_XSRAM_READ(0);
  #endif
    //DDC_Port_A0
    D2B_SET_FIFOMODE_VGA(TRUE);
    D2B_SET_FIFOMODE_ENHANCE_VGA(D2B_FIFO_Enhance);
    D2B_SET_FIFOMODE_SIZE_VGA(D2B_FIFOSize_INDEX);
    D2B_SET_FIFOMODE_XDATAMAP_VGA(D2B_FIFO_XdataAddressStart_A0);
    D2B_SET_FIFOMODE_SIMPLIFIED_VGA(D2B_SIMPLIFIED_MODE);
#if D2B_DOUBLE_FIFO_Mode
    D2B_SET_DOUBLE_FIFOMODE_VGA(TRUE);
    D2B_SET_FIFOMODE_TX_SIZE_VGA(D2B_FIFOSize_INDEX);
    D2B_SET_FIFOMODE_TX_XDATAMAP_VGA(D2B_FIFO_XdataWriteAddressStart_A0);
#endif
  #if DDC_Port_D0
    D2B_SET_FIFOMODE_D0(TRUE);
    D2B_SET_FIFOMODE_ENHANCE_D0(D2B_FIFO_Enhance);
    D2B_SET_FIFOMODE_SIZE_D0(D2B_FIFOSize_INDEX);
    D2B_SET_FIFOMODE_XDATAMAP_D0(D2B_FIFO_XdataAddressStart_D0);
    D2B_SET_FIFOMODE_SIMPLIFIED_D0(D2B_SIMPLIFIED_MODE);
#if D2B_DOUBLE_FIFO_Mode
    D2B_SET_DOUBLE_FIFOMODE_D0(TRUE);
    D2B_SET_FIFOMODE_TX_SIZE_D0(D2B_FIFOSize_INDEX);
    D2B_SET_FIFOMODE_TX_XDATAMAP_D0(D2B_FIFO_XdataWriteAddressStart_D0);
#endif
  #endif
  #if DDC_Port_D1
    D2B_SET_FIFOMODE_D1(TRUE);
    D2B_SET_FIFOMODE_ENHANCE_D1(D2B_FIFO_Enhance);
    D2B_SET_FIFOMODE_SIZE_D1(D2B_FIFOSize_INDEX);
    D2B_SET_FIFOMODE_XDATAMAP_D1(D2B_FIFO_XdataAddressStart_D1);
    D2B_SET_FIFOMODE_SIMPLIFIED_D1(D2B_SIMPLIFIED_MODE);
#if D2B_DOUBLE_FIFO_Mode
    D2B_SET_DOUBLE_FIFOMODE_D1(TRUE);
    D2B_SET_FIFOMODE_TX_SIZE_D1(D2B_FIFOSize_INDEX);
    D2B_SET_FIFOMODE_TX_XDATAMAP_D1(D2B_FIFO_XdataWriteAddressStart_D1);
#endif
  #endif
  #if DDC_Port_D2
    D2B_SET_FIFOMODE_D2(TRUE);
    D2B_SET_FIFOMODE_ENHANCE_D2(D2B_FIFO_Enhance);
    D2B_SET_FIFOMODE_SIZE_D2(D2B_FIFOSize_INDEX);
    D2B_SET_FIFOMODE_XDATAMAP_D2(D2B_FIFO_XdataAddressStart_D2);
    D2B_SET_FIFOMODE_SIMPLIFIED_D2(D2B_SIMPLIFIED_MODE);
#if D2B_DOUBLE_FIFO_Mode
    D2B_SET_DOUBLE_FIFOMODE_D2(TRUE);
    D2B_SET_FIFOMODE_TX_SIZE_D2(D2B_FIFOSize_INDEX);
    D2B_SET_FIFOMODE_TX_XDATAMAP_D2(D2B_FIFO_XdataWriteAddressStart_D2);
#endif
  #endif
  #if DDC_Port_D3
	  D2B_SET_FIFOMODE_D3(TRUE);
	  D2B_SET_FIFOMODE_ENHANCE_D3(D2B_FIFO_Enhance);
	  D2B_SET_FIFOMODE_SIZE_D3(D2B_FIFOSize_INDEX);
	  D2B_SET_FIFOMODE_XDATAMAP_D3(D2B_FIFO_XdataAddressStart_D3);
      D2B_SET_FIFOMODE_SIMPLIFIED_D3(D2B_SIMPLIFIED_MODE);
#if D2B_DOUBLE_FIFO_Mode
	  D2B_SET_DOUBLE_FIFOMODE_D3(TRUE);
	  D2B_SET_FIFOMODE_TX_SIZE_D3(D2B_FIFOSize_INDEX);
	  D2B_SET_FIFOMODE_TX_XDATAMAP_D3(D2B_FIFO_XdataWriteAddressStart_D3);
#endif
  #endif
#endif
#if D2B_HW_CheckSum

    D2B_SET_D2B_HW_CheckSum(_ENABLE,  DDC_Port_D3, DDC_Port_D2, DDC_Port_D1, DDC_Port_D0);
#endif

    //INT_IRQ_D2B_ENABLE(TRUE); //enable d2b int
    MsOS_EnableInterrupt(E_INT_PM_IRQ_D2B); //enable d2b int
#if(CHIP_ID == MST9U5 || CHIP_ID == CHIP_MT9700)
    //default disable
    #if D2B_Auto_Clk_Gating
    msWriteBit(REG_003EFF, 1, BIT7); // Auto clock gate enable.
    #else
    msWriteBit(REG_003EFF, 0, BIT7); // Auto clock gate disable.
    #endif
#endif
}

void drvDDC2Bi_MessageReady( void )
{
    BYTE ChecksumIndex;
    
    DDCBuffer[0] |= DDC2Bi_CONTROL_STATUS_FLAG;
    ChecksumIndex = (DDCBuffer[0] &~DDC2Bi_CONTROL_STATUS_FLAG ) + 1;
    DDCBuffer[ChecksumIndex] = DDC2Bi_ComputeChecksum(ChecksumIndex);
    DDC2Bi_GetTxBuffer();
    txBufferPtr = &DDCBuffer[0];
#if DDCCI_DEBUG_ENABLE
    Osd_Show();
    Osd_Draw4Num( 1, 0, DDCBuffer[0] );
    Osd_Draw4Num( 1, 1, DDCBuffer[1] );
    Osd_Draw4Num( 1, 2, DDCBuffer[2] );
    Osd_Draw4Num( 1, 3, DDCBuffer[3] );
    Osd_Draw4Num( 1, 4, DDCBuffer[4] );
    Osd_Draw4Num( 1, 5, DDCBuffer[5] );
    Osd_Draw4Num( 1, 6, DDCBuffer[6] );
    Osd_Draw4Num( 1, 7, DDCBuffer[7] );
    Osd_Draw4Num( 1, 8, DDCBuffer[8] );
    Osd_Draw4Num( 1, 9, DDCBuffer[9] );
    Delay1ms( 1000 );
#endif
    txLength = ( DDCBuffer[0] & ( ~DDC2Bi_CONTROL_STATUS_FLAG ) ) + 2;
    
#if D2B_FIFO_Mode

#if 0//ENABLE_BOE_NEW_SZ_DDCCI_SPEC
  if (1)
#else
 if((!INPUT_IS_DISPLAYPORT(rxInputPort)) && (!INPUT_IS_USBTYPEC(rxInputPort)) && txLength > 0)    //20121030
#endif

  {
#if !D2B_XShared_DDCBuffer
        WORD i=0;
#endif
        BYTE WriteXdataST_Add;

#if D2B_FIFO_Enhance
        WriteXdataST_Add = DDC2Bi_FIFOMODE_READ_CUR(rxComboPort) + 1;
#else
        WriteXdataST_Add= 0;
#endif

        //XSRAMBuffer[DDC2Bi_FIFOMODE_XDATA_ADDR_OFFSET(rxComboPort,1)+(WriteXdataST_Add%D2B_FIFO_Size)] = 0x6E;
        D2B_XSRAM_WRITE(DDC2Bi_FIFOMODE_XDATA_ADDR_OFFSET(rxComboPort,1)+(WriteXdataST_Add%D2B_FIFO_Size), 0x6E);
#if !D2B_XShared_DDCBuffer
        for( i = 0; i < txLength; i++ )
        {
            //XSRAMBuffer[DDC2Bi_FIFOMODE_XDATA_ADDR_OFFSET(rxComboPort,1)+(( WriteXdataST_Add+i+1 )%D2B_FIFO_Size)] = DDCBuffer[i];
            D2B_XSRAM_WRITE(DDC2Bi_FIFOMODE_XDATA_ADDR_OFFSET(rxComboPort,1)+(( WriteXdataST_Add+i+1 )%D2B_FIFO_Size), DDCBuffer[i]);
            if( i==(D2B_FIFO_Size-2) )
            {
                txIndex = D2B_FIFO_Size-1;
                break;
            }
        }
#endif
        DDC2Bi_SET_WP_READY(rxComboPort);
  }
#endif
}

void drvDDC2Bi_ParseCommand( void )
{
    BYTE length; //, command;
    length = DDCBuffer[LENGTH] & 0x7F;
    if( rxIndex <= 0 )
        return ;
    if( length <= 0 )
    {
        txBufferPtr = &nullMsg1[1];
        txLength = sizeof( nullMsg1 );

        DDC2Bi_SET_RBUF(rxComboPort, DDC2B_DEST_ADDRESS);
    }

#if D2B_XDATA_DEBUG
    {
        BYTE ii;
        for (ii=0;ii</*16*/24;ii++)
            //XSRAMBuffer[FIFODBGOFFSET+D2B_FIFO_Size+24/*32*/+ii] = DDCBuffer[ii];
            D2B_XSRAM_WRITE(FIFODBGOFFSET+D2B_FIFO_Size+24/*32*/+ii, DDCBuffer[ii]);
    }
#endif

#if DDCCI_FILTER_FUNCTION
    if(DDCCICommandExecutable())
#endif
    length = AlignControl();

    if( length <= 0 )
    {
        DDCBuffer[0] = 0x80;
        //return ;
    }
    else if( length == 0xFE || length == 0xFD )
    {
        DDCBuffer[0] = 0x80;
    }
    DDC2Bi_MessageReady();


    DDC2Bi_SET_RBUF(rxComboPort, DDC2B_DEST_ADDRESS);

#if D2B_Auto_Clk_Gating
    DDC2Bi_Auto_Clk_TurnOn();
#endif

}

#if D2B_FIFO_Mode
void drvDoDDCCI( BYTE Source ) // Annie 2006.7.5
{
    BYTE intFlag;
    WORD u8AddPoint;

    intFlag = DDC2Bi_Read_INT(Source);

    if( intFlag )
    {
        if(DDC_RW_SRAM_FLAG) // DDC access xdata fail
        {
            D2BErrorFlag=1;
            DDC2Bi_InitRx();

#if D2B_XDATA_DEBUG
            //XSRAMBuffer[FIFODBGOFFSET+D2B_FIFO_Size+64+(IntCount%16)+(IntCount/16)*80] = 0x33;
            //XSRAMBuffer[FIFODBGOFFSET+D2B_FIFO_Size+80+(IntCount%16)+(IntCount/16)*80] = 0x33;
            //XSRAMBuffer[FIFODBGOFFSET+D2B_FIFO_Size+96+(IntCount%16)+(IntCount/16)*80] = msRegs[REG_3EEA];
            //XSRAMBuffer[FIFODBGOFFSET+D2B_FIFO_Size+112+(IntCount%16)+(IntCount/16)*80] = msRegs[REG_3EE6];
            D2B_XSRAM_WRITE(FIFODBGOFFSET+D2B_FIFO_Size+64+(IntCount%16)+(IntCount/16)*80, 0x33);
            D2B_XSRAM_WRITE(FIFODBGOFFSET+D2B_FIFO_Size+80+(IntCount%16)+(IntCount/16)*80, 0x33);
            D2B_XSRAM_WRITE(FIFODBGOFFSET+D2B_FIFO_Size+96+(IntCount%16)+(IntCount/16)*80, msRegs(REG_003EEA));
            D2B_XSRAM_WRITE(FIFODBGOFFSET+D2B_FIFO_Size+112+(IntCount%16)+(IntCount/16)*80, msRegs(REG_003EE6));
            IntCount++;
#endif
            DDC_RW_SRAM_CLR;
            DDC2Bi_FIFOMODE_SET_RW_DONE(Source);
            DDC2Bi_FIFOMODE_CLR_EMPTYFULL_FLAG(Source);
            if( _bit7_( DDC2Bi_Read_INT(Source)) )
                DDC2Bi_CLR_INT(Source, BIT7);
            return;
        }

        switch( intFlag &( TXBI | RCBI ) )
        {
            // TX int
            case TXBI:
                //txStatus = DDC2B_MSGON;                // no characters left
                Previous_DDC2MCU_CUR_ADR = Present_DDC2MCU_CUR_ADR;
                #if (D2B_SIMPLIFIED_MODE == 0)
                DDC2Bi_FIFOMODE_ADR_UPDATE(Source);
                #endif
                Present_DDC2MCU_CUR_ADR = DDC2Bi_FIFOMODE_READ_CUR(Source); // MUST
#if D2B_XDATA_DEBUG
                if(IntCount>=32)
                {
                    IntCount=31;
                    //XSRAMBuffer[FIFODBGOFFSET+D2B_FIFO_Size+48+(IntCount%16)+(IntCount/16)*80] = IntCount|0xC0;
                    D2B_XSRAM_WRITE(FIFODBGOFFSET+D2B_FIFO_Size+48+(IntCount%16)+(IntCount/16)*80, IntCount|0xC0);
                }else
                    //XSRAMBuffer[FIFODBGOFFSET+D2B_FIFO_Size+48+(IntCount%16)+(IntCount/16)*80] = IntCount|0x80;
                    D2B_XSRAM_WRITE(FIFODBGOFFSET+D2B_FIFO_Size+48+(IntCount%16)+(IntCount/16)*80, IntCount|0x80);
                //XSRAMBuffer[FIFODBGOFFSET+D2B_FIFO_Size+64+(IntCount%16)+(IntCount/16)*80] = Present_DDC2MCU_CUR_ADR;
                //XSRAMBuffer[FIFODBGOFFSET+D2B_FIFO_Size+80+(IntCount%16)+(IntCount/16)*80] = 0x77;
                //XSRAMBuffer[FIFODBGOFFSET+D2B_FIFO_Size+96+(IntCount%16)+(IntCount/16)*80] = txIndex;
                //XSRAMBuffer[FIFODBGOFFSET+D2B_FIFO_Size+112+(IntCount%16)+(IntCount/16)*80] = msRegs[REG_3EE6];

                //XSRAMBuffer[FIFODBGOFFSET+D2B_FIFO_Size+9] = D2BErrorFlag;
                //XSRAMBuffer[FIFODBGOFFSET+D2B_FIFO_Size+10] = (msRegs[REG_3EE6]);
                D2B_XSRAM_WRITE(FIFODBGOFFSET+D2B_FIFO_Size+64+(IntCount%16)+(IntCount/16)*80, Present_DDC2MCU_CUR_ADR);
                D2B_XSRAM_WRITE(FIFODBGOFFSET+D2B_FIFO_Size+80+(IntCount%16)+(IntCount/16)*80, 0x77);
                D2B_XSRAM_WRITE(FIFODBGOFFSET+D2B_FIFO_Size+96+(IntCount%16)+(IntCount/16)*80, txIndex);
                D2B_XSRAM_WRITE(FIFODBGOFFSET+D2B_FIFO_Size+112+(IntCount%16)+(IntCount/16)*80, msRegs(REG_003EE6));

                D2B_XSRAM_WRITE(FIFODBGOFFSET+D2B_FIFO_Size+9, D2BErrorFlag[Source]);
                D2B_XSRAM_WRITE(FIFODBGOFFSET+D2B_FIFO_Size+10, msRegs(REG_003EE6));
#endif

                if(txIndex) // Remaining Tx
                {
#if !D2B_XShared_DDCBuffer
                    if( (DDC2Bi_FIFOMODE_READ_EMPTY_FLAG(Source)) || (D2BErrorFlag==1) ) // Buffer Read Empty
                    {
                        D2BErrorFlag=1;
                        //u8TxStatus=DDC2B_CLEAR;
#if D2B_XDATA_DEBUG
                        //XSRAMBuffer[FIFODBGOFFSET+D2B_FIFO_Size+64+(IntCount%16)+(IntCount/16)*80] = 0x77;
                        //XSRAMBuffer[FIFODBGOFFSET+D2B_FIFO_Size+80+(IntCount%16)+(IntCount/16)*80] = 0x77;
                        //XSRAMBuffer[FIFODBGOFFSET+D2B_FIFO_Size+96+(IntCount%16)+(IntCount/16)*80] = 0x77;
                        //XSRAMBuffer[FIFODBGOFFSET+D2B_FIFO_Size+112+(IntCount%16)+(IntCount/16)*80] = msRegs[REG_3EE6];
                        D2B_XSRAM_WRITE(FIFODBGOFFSET+D2B_FIFO_Size+64+(IntCount%16)+(IntCount/16)*80, 0x77);
                        D2B_XSRAM_WRITE(FIFODBGOFFSET+D2B_FIFO_Size+80+(IntCount%16)+(IntCount/16)*80, 0x77);
                        D2B_XSRAM_WRITE(FIFODBGOFFSET+D2B_FIFO_Size+96+(IntCount%16)+(IntCount/16)*80, 0x77);
                        D2B_XSRAM_WRITE(FIFODBGOFFSET+D2B_FIFO_Size+112+(IntCount%16)+(IntCount/16)*80, msRegs(REG_003EE6));
                        IntCount++;
#endif
                        DDC2Bi_FIFOMODE_SET_RW_DONE(Source);
                        DDC2Bi_FIFOMODE_CLR_EMPTYFULL_FLAG(Source);
                        return;
                    }

                    Present_DDC2MCU_START_ADR = DDC2Bi_FIFOMODE_READ_START(Source);


                    if( _bit7_( DDC2Bi_Read_INT(Source)) )
                    {
                        if( Present_DDC2MCU_CUR_ADR < Present_DDC2MCU_START_ADR )
                        {
                            Present_DDC2MCU_CUR_ADR += ( BIT0 << ( D2B_FIFOSize_INDEX + 3 ) );
                        }
#if D2B_XDATA_DEBUG
                        //XSRAMBuffer[FIFODBGOFFSET+D2B_FIFO_Size+64+(IntCount%16)+(IntCount/16)*80] = Present_DDC2MCU_START_ADR|0x80;
                        //XSRAMBuffer[FIFODBGOFFSET+D2B_FIFO_Size+80+(IntCount%16)+(IntCount/16)*80] = Present_DDC2MCU_CUR_ADR|0x80;
                        //XSRAMBuffer[FIFODBGOFFSET+D2B_FIFO_Size+96+(IntCount%16)+(IntCount/16)*80] = Previous_DDC2MCU_CUR_ADR|0x80;
                        //XSRAMBuffer[FIFODBGOFFSET+D2B_FIFO_Size+112+(IntCount%16)+(IntCount/16)*80] = msRegs[REG_3EE6];
                        D2B_XSRAM_WRITE(FIFODBGOFFSET+D2B_FIFO_Size+64+(IntCount%16)+(IntCount/16)*80, Present_DDC2MCU_START_ADR|0x80);
                        D2B_XSRAM_WRITE(FIFODBGOFFSET+D2B_FIFO_Size+80+(IntCount%16)+(IntCount/16)*80, Present_DDC2MCU_CUR_ADR|0x80);
                        D2B_XSRAM_WRITE(FIFODBGOFFSET+D2B_FIFO_Size+96+(IntCount%16)+(IntCount/16)*80, Previous_DDC2MCU_CUR_ADR|0x80);
                        D2B_XSRAM_WRITE(FIFODBGOFFSET+D2B_FIFO_Size+112+(IntCount%16)+(IntCount/16)*80, msRegs(REG_003EE6));
#endif
                        for( u8AddPoint = Present_DDC2MCU_START_ADR; u8AddPoint <= Present_DDC2MCU_CUR_ADR; u8AddPoint++ )
                        {
                            //XSRAMBuffer[DDC2Bi_FIFOMODE_XDATA_ADDR_OFFSET(Source,1)+u8AddPoint%D2B_FIFO_Size] = DDCBuffer[txIndex++] ;
                            D2B_XSRAM_WRITE(DDC2Bi_FIFOMODE_XDATA_ADDR_OFFSET(Source,1)+u8AddPoint%D2B_FIFO_Size, DDCBuffer[txIndex++]) ;
                            if( txIndex >= txLength )
                            {
                                txIndex=0;
                                Present_DDC2MCU_CUR_ADR &= (D2B_FIFO_Size-1);
                                break;
                            }
                        }
                    }
                    else
                    {
                        while( Present_DDC2MCU_CUR_ADR<Previous_DDC2MCU_CUR_ADR )
                        {
                            Present_DDC2MCU_CUR_ADR += ( BIT0 << ( D2B_FIFOSize_INDEX + 3 ) );
                        }
#if D2B_XDATA_DEBUG
                        //XSRAMBuffer[FIFODBGOFFSET+D2B_FIFO_Size+64+(IntCount%16)+(IntCount/16)*80] = Present_DDC2MCU_START_ADR;
                        //XSRAMBuffer[FIFODBGOFFSET+D2B_FIFO_Size+80+(IntCount%16)+(IntCount/16)*80] = Present_DDC2MCU_CUR_ADR;
                        //XSRAMBuffer[FIFODBGOFFSET+D2B_FIFO_Size+96+(IntCount%16)+(IntCount/16)*80] = Previous_DDC2MCU_CUR_ADR;
                        //XSRAMBuffer[FIFODBGOFFSET+D2B_FIFO_Size+112+(IntCount%16)+(IntCount/16)*80] = msRegs[REG_3EE6];
                        D2B_XSRAM_WRITE(FIFODBGOFFSET+D2B_FIFO_Size+64+(IntCount%16)+(IntCount/16)*80, Present_DDC2MCU_START_ADR);
                        D2B_XSRAM_WRITE(FIFODBGOFFSET+D2B_FIFO_Size+80+(IntCount%16)+(IntCount/16)*80, Present_DDC2MCU_CUR_ADR);
                        D2B_XSRAM_WRITE(FIFODBGOFFSET+D2B_FIFO_Size+96+(IntCount%16)+(IntCount/16)*80, Previous_DDC2MCU_CUR_ADR);
                        D2B_XSRAM_WRITE(FIFODBGOFFSET+D2B_FIFO_Size+112+(IntCount%16)+(IntCount/16)*80, msRegs(REG_003EE6));
#endif
                        for( u8AddPoint = Previous_DDC2MCU_CUR_ADR + 1; u8AddPoint <= Present_DDC2MCU_CUR_ADR; u8AddPoint++ )
                        {
                            //XSRAMBuffer[DDC2Bi_FIFOMODE_XDATA_ADDR_OFFSET(Source,1)+u8AddPoint%D2B_FIFO_Size] = DDCBuffer[txIndex++] ;
                            D2B_XSRAM_WRITE(DDC2Bi_FIFOMODE_XDATA_ADDR_OFFSET(Source,1)+u8AddPoint%D2B_FIFO_Size, DDCBuffer[txIndex++]) ;
                            if( txIndex >= txLength )
                            {
                                txIndex=0;
                                Present_DDC2MCU_CUR_ADR &= (D2B_FIFO_Size-1);
                                break;
                            }
                        }
                    }

                    if(DDC2Bi_FIFOMODE_READ_EMPTY_FLAG(Source)) // Buffer Read Empty at Race condition
                    {
                        D2BErrorFlag=1;
                        //u8TxStatus=DDC2B_CLEAR;
#if D2B_XDATA_DEBUG
                        //XSRAMBuffer[FIFODBGOFFSET+D2B_FIFO_Size+64+(IntCount%16)+(IntCount/16)*80] = Present_DDC2MCU_START_ADR;
                        //XSRAMBuffer[FIFODBGOFFSET+D2B_FIFO_Size+80+(IntCount%16)+(IntCount/16)*80] = Present_DDC2MCU_CUR_ADR;
                        //XSRAMBuffer[FIFODBGOFFSET+D2B_FIFO_Size+96+(IntCount%16)+(IntCount/16)*80] = 0x66;
                        //XSRAMBuffer[FIFODBGOFFSET+D2B_FIFO_Size+112+(IntCount%16)+(IntCount/16)*80] = msRegs[REG_3EE6];
                        D2B_XSRAM_WRITE(FIFODBGOFFSET+D2B_FIFO_Size+64+(IntCount%16)+(IntCount/16)*80, Present_DDC2MCU_START_ADR);
                        D2B_XSRAM_WRITE(FIFODBGOFFSET+D2B_FIFO_Size+80+(IntCount%16)+(IntCount/16)*80, Present_DDC2MCU_CUR_ADR);
                        D2B_XSRAM_WRITE(FIFODBGOFFSET+D2B_FIFO_Size+96+(IntCount%16)+(IntCount/16)*80, 0x66);
                        D2B_XSRAM_WRITE(FIFODBGOFFSET+D2B_FIFO_Size+112+(IntCount%16)+(IntCount/16)*80, msRegs(REG_003EE6));
#endif
                        DDC2Bi_FIFOMODE_CLR_EMPTYFULL_FLAG(Source); // clear
                    }
#endif // #if !D2B_XShared_DDCBuffer
                }
                else
                {
                   DDC2Bi_FIFOMODE_CLR_EMPTYFULL_FLAG(Source); // clear
                }

                DDC2Bi_FIFOMODE_SET_RW_DONE(Source); // To prevent Full_Error_flag
                if( _bit7_( DDC2Bi_Read_INT(Source)) )
                    DDC2Bi_CLR_INT(Source, BIT7);

                break;

            // RX int
            case RCBI:
                Previous_DDC2MCU_CUR_ADR = Present_DDC2MCU_CUR_ADR;
                #if (D2B_SIMPLIFIED_MODE == 0)
                DDC2Bi_FIFOMODE_ADR_UPDATE(Source);
                #endif
                Present_DDC2MCU_CUR_ADR = DDC2Bi_FIFOMODE_READ_CUR(Source);
                Present_DDC2MCU_START_ADR = DDC2Bi_FIFOMODE_READ_START(Source);

#if D2B_XDATA_DEBUG
                if( _bit7_( DDC2Bi_Read_INT(Source)) )//( _bit7_( msRegs[REG_3E10] ) == 1 )
                    IntCount=0;
                //XSRAMBuffer[FIFODBGOFFSET+D2B_FIFO_Size+11] = D2BErrorFlag;
                //XSRAMBuffer[FIFODBGOFFSET+D2B_FIFO_Size+12] = (msRegs[REG_3EE6]);
                //XSRAMBuffer[FIFODBGOFFSET+D2B_FIFO_Size+48+(IntCount%16)+(IntCount/16)*80] = IntCount;
                D2B_XSRAM_WRITE(FIFODBGOFFSET+D2B_FIFO_Size+11, D2BErrorFlag);
                D2B_XSRAM_WRITE(FIFODBGOFFSET+D2B_FIFO_Size+12, (msRegs(REG_003EE6)));
                D2B_XSRAM_WRITE(FIFODBGOFFSET+D2B_FIFO_Size+48+(IntCount%16)+(IntCount/16)*80, IntCount);
#endif

                if(DDC2Bi_FIFOMODE_READ_FULL_FLAG(Source)) // Buffer Write Overflow
                {
                    D2BErrorFlag=1;
                    DDC2Bi_InitRx();
#if D2B_XDATA_DEBUG
                    //XSRAMBuffer[FIFODBGOFFSET+D2B_FIFO_Size+64+(IntCount%16)+(IntCount/16)*80] = 0xFF;
                    //XSRAMBuffer[FIFODBGOFFSET+D2B_FIFO_Size+80+(IntCount%16)+(IntCount/16)*80] = 0xFF;
                    //XSRAMBuffer[FIFODBGOFFSET+D2B_FIFO_Size+96+(IntCount%16)+(IntCount/16)*80] = 0xFF;
                    //XSRAMBuffer[FIFODBGOFFSET+D2B_FIFO_Size+112+(IntCount%16)+(IntCount/16)*80] = msRegs[REG_3EE6];
                    D2B_XSRAM_WRITE(FIFODBGOFFSET+D2B_FIFO_Size+64+(IntCount%16)+(IntCount/16)*80, 0xFF);
                    D2B_XSRAM_WRITE(FIFODBGOFFSET+D2B_FIFO_Size+80+(IntCount%16)+(IntCount/16)*80, 0xFF);
                    D2B_XSRAM_WRITE(FIFODBGOFFSET+D2B_FIFO_Size+96+(IntCount%16)+(IntCount/16)*80, 0xFF);
                    D2B_XSRAM_WRITE(FIFODBGOFFSET+D2B_FIFO_Size+112+(IntCount%16)+(IntCount/16)*80, msRegs(REG_003EE6));
                    IntCount++;
#endif
                    DDC2Bi_FIFOMODE_SET_RW_DONE(Source);
                    DDC2Bi_FIFOMODE_CLR_EMPTYFULL_FLAG(Source);
                    if( _bit7_( DDC2Bi_Read_INT(Source)) )
                        DDC2Bi_CLR_INT(Source, BIT7);

                    return;
                }
                else if( (D2BErrorFlag==1) && ( _bit7_( DDC2Bi_Read_INT(Source))==0 ) ) // Not New Command
                {
                    DDC2Bi_InitRx();
#if D2B_XDATA_DEBUG
                    //XSRAMBuffer[FIFODBGOFFSET+D2B_FIFO_Size+64+(IntCount%16)+(IntCount/16)*80] = 0x88;
                    //XSRAMBuffer[FIFODBGOFFSET+D2B_FIFO_Size+80+(IntCount%16)+(IntCount/16)*80] = 0x88;
                    //XSRAMBuffer[FIFODBGOFFSET+D2B_FIFO_Size+96+(IntCount%16)+(IntCount/16)*80] = 0xFF;
                    //XSRAMBuffer[FIFODBGOFFSET+D2B_FIFO_Size+112+(IntCount%16)+(IntCount/16)*80] = msRegs[REG_3EE6];
                    D2B_XSRAM_WRITE(FIFODBGOFFSET+D2B_FIFO_Size+64+(IntCount%16)+(IntCount/16)*80, 0x88);
                    D2B_XSRAM_WRITE(FIFODBGOFFSET+D2B_FIFO_Size+80+(IntCount%16)+(IntCount/16)*80, 0x88);
                    D2B_XSRAM_WRITE(FIFODBGOFFSET+D2B_FIFO_Size+96+(IntCount%16)+(IntCount/16)*80, 0xFF);
                    D2B_XSRAM_WRITE(FIFODBGOFFSET+D2B_FIFO_Size+112+(IntCount%16)+(IntCount/16)*80, msRegs(REG_003EE6));
                    IntCount++;
#endif
                    DDC2Bi_FIFOMODE_SET_RW_DONE(Source);

                    return;
                }
                else
                {
                    D2BErrorFlag=0;
                }

#if D2B_XDATA_DEBUG
                //XSRAMBuffer[FIFODBGOFFSET+D2B_FIFO_Size+13] = Previous_DDC2MCU_CUR_ADR;
                //XSRAMBuffer[FIFODBGOFFSET+D2B_FIFO_Size+14] = Present_DDC2MCU_CUR_ADR;
                //XSRAMBuffer[FIFODBGOFFSET+D2B_FIFO_Size+15] = Present_DDC2MCU_START_ADR;
                D2B_XSRAM_WRITE(FIFODBGOFFSET+D2B_FIFO_Size+13, Previous_DDC2MCU_CUR_ADR);
                D2B_XSRAM_WRITE(FIFODBGOFFSET+D2B_FIFO_Size+14, Present_DDC2MCU_CUR_ADR);
                D2B_XSRAM_WRITE(FIFODBGOFFSET+D2B_FIFO_Size+15, Present_DDC2MCU_START_ADR);
#endif

                if( _bit7_( DDC2Bi_Read_INT(Source)) )
                {
                    rxIndex = 0;
                    rxLength = 0;
#if !D2B_HW_CheckSum
                    rxChkSum = INITRxCK;
#endif
#if !D2B_XShared_DDCBuffer
                    if( Present_DDC2MCU_CUR_ADR < Present_DDC2MCU_START_ADR )
                    {
                        Present_DDC2MCU_CUR_ADR += ( BIT0 << ( D2B_FIFOSize_INDEX + 3 ) );
                    }
#endif

#if D2B_XDATA_DEBUG
                    {
                        BYTE xdata ii;
                        for (ii=0;ii</*16*/24;ii++)
                            //XSRAMBuffer[FIFODBGOFFSET+D2B_FIFO_Size+24/*32*/+ii] = 0;
                            D2B_XSRAM_WRITE(FIFODBGOFFSET+D2B_FIFO_Size+24/*32*/+ii, 0);

                        for (ii=0;ii<32;ii++)
                        {
                            //XSRAMBuffer[FIFODBGOFFSET+D2B_FIFO_Size+64+(ii%16)+(ii/16)*80] = 0;
                            //XSRAMBuffer[FIFODBGOFFSET+D2B_FIFO_Size+80+(ii%16)+(ii/16)*80] = 0;
                            //XSRAMBuffer[FIFODBGOFFSET+D2B_FIFO_Size+96+(ii%16)+(ii/16)*80] = 0;
                            //XSRAMBuffer[FIFODBGOFFSET+D2B_FIFO_Size+112+(ii%16)+(ii/16)*80] = 0;
                            D2B_XSRAM_WRITE(FIFODBGOFFSET+D2B_FIFO_Size+64+(ii%16)+(ii/16)*80, 0);
                            D2B_XSRAM_WRITE(FIFODBGOFFSET+D2B_FIFO_Size+80+(ii%16)+(ii/16)*80, 0);
                            D2B_XSRAM_WRITE(FIFODBGOFFSET+D2B_FIFO_Size+96+(ii%16)+(ii/16)*80, 0);
                            D2B_XSRAM_WRITE(FIFODBGOFFSET+D2B_FIFO_Size+112+(ii%16)+(ii/16)*80, 0);
                        }
                    }
#endif


#if D2B_XDATA_DEBUG
                    //XSRAMBuffer[FIFODBGOFFSET+D2B_FIFO_Size+64+(IntCount%16)+(IntCount/16)*80] = Present_DDC2MCU_START_ADR|0x80;
                    //XSRAMBuffer[FIFODBGOFFSET+D2B_FIFO_Size+80+(IntCount%16)+(IntCount/16)*80] = Present_DDC2MCU_CUR_ADR|0x80;
                    //XSRAMBuffer[FIFODBGOFFSET+D2B_FIFO_Size+96+(IntCount%16)+(IntCount/16)*80] = Previous_DDC2MCU_CUR_ADR|0x80;
                    //XSRAMBuffer[FIFODBGOFFSET+D2B_FIFO_Size+112+(IntCount%16)+(IntCount/16)*80] = msRegs[REG_3EE6];
                    D2B_XSRAM_WRITE(FIFODBGOFFSET+D2B_FIFO_Size+64+(IntCount%16)+(IntCount/16)*80, Present_DDC2MCU_START_ADR|0x80);
                    D2B_XSRAM_WRITE(FIFODBGOFFSET+D2B_FIFO_Size+80+(IntCount%16)+(IntCount/16)*80, Present_DDC2MCU_CUR_ADR|0x80);
                    D2B_XSRAM_WRITE(FIFODBGOFFSET+D2B_FIFO_Size+96+(IntCount%16)+(IntCount/16)*80, Previous_DDC2MCU_CUR_ADR|0x80);
                    D2B_XSRAM_WRITE(FIFODBGOFFSET+D2B_FIFO_Size+112+(IntCount%16)+(IntCount/16)*80, msRegs(REG_003EE6));
#endif
#if D2B_XShared_DDCBuffer
                    //DDCBuffer = &XSRAMBuffer[DDC2Bi_FIFOMODE_XDATA_ADDR_OFFSET(Source,0)+1];
                    DDCBuffer = D2B_XSRAM_READ(DDC2Bi_FIFOMODE_XDATA_ADDR_OFFSET(Source,0)+1);
#endif
                    for( u8AddPoint = Present_DDC2MCU_START_ADR + 1; u8AddPoint <= Present_DDC2MCU_CUR_ADR; u8AddPoint++ )
                    {
#if !D2B_XShared_DDCBuffer
                        //DDCBuffer[rxIndex++] = XSRAMBuffer[DDC2Bi_FIFOMODE_XDATA_ADDR_OFFSET(Source,0)+u8AddPoint%D2B_FIFO_Size];
                        DDCBuffer[rxIndex++] = D2B_XSRAM_READ(DDC2Bi_FIFOMODE_XDATA_ADDR_OFFSET(Source,0)+u8AddPoint%D2B_FIFO_Size);

#else
                        rxIndex++;
#endif
#if !D2B_HW_CheckSum
                        //rxChkSum ^= XSRAMBuffer[DDC2Bi_FIFOMODE_XDATA_ADDR_OFFSET(Source,0)+u8AddPoint%D2B_FIFO_Size];
                        rxChkSum ^= D2B_XSRAM_READ(DDC2Bi_FIFOMODE_XDATA_ADDR_OFFSET(Source,0)+u8AddPoint%D2B_FIFO_Size);
#endif
                    }
                }
                else
                {
#if D2B_XDATA_DEBUG
                    //XSRAMBuffer[FIFODBGOFFSET+D2B_FIFO_Size+64+(IntCount%16)+(IntCount/16)*80] = Present_DDC2MCU_START_ADR;
                    //XSRAMBuffer[FIFODBGOFFSET+D2B_FIFO_Size+80+(IntCount%16)+(IntCount/16)*80] = Present_DDC2MCU_CUR_ADR;
                    //XSRAMBuffer[FIFODBGOFFSET+D2B_FIFO_Size+96+(IntCount%16)+(IntCount/16)*80] = Previous_DDC2MCU_CUR_ADR;
                    //XSRAMBuffer[FIFODBGOFFSET+D2B_FIFO_Size+112+(IntCount%16)+(IntCount/16)*80] = msRegs[REG_3EE6];
                    D2B_XSRAM_WRITE(FIFODBGOFFSET+D2B_FIFO_Size+64+(IntCount%16)+(IntCount/16)*80, Present_DDC2MCU_START_ADR);
                    D2B_XSRAM_WRITE(FIFODBGOFFSET+D2B_FIFO_Size+80+(IntCount%16)+(IntCount/16)*80, Present_DDC2MCU_CUR_ADR);
                    D2B_XSRAM_WRITE(FIFODBGOFFSET+D2B_FIFO_Size+96+(IntCount%16)+(IntCount/16)*80, Previous_DDC2MCU_CUR_ADR);
                    D2B_XSRAM_WRITE(FIFODBGOFFSET+D2B_FIFO_Size+112+(IntCount%16)+(IntCount/16)*80, msRegs(REG_003EE6));
#endif
#if !D2B_XShared_DDCBuffer
                    while( Present_DDC2MCU_CUR_ADR<=Previous_DDC2MCU_CUR_ADR )
                    {
                        Present_DDC2MCU_CUR_ADR += ( BIT0 << ( D2B_FIFOSize_INDEX + 3 ) );
                    }
#endif

                    for( u8AddPoint = Previous_DDC2MCU_CUR_ADR + 1; u8AddPoint <= Present_DDC2MCU_CUR_ADR; u8AddPoint++ )
                    {
#if !D2B_XShared_DDCBuffer
                        //DDCBuffer[rxIndex++] = XSRAMBuffer[DDC2Bi_FIFOMODE_XDATA_ADDR_OFFSET(Source,0)+u8AddPoint%D2B_FIFO_Size];
                        DDCBuffer[rxIndex++] = D2B_XSRAM_READ(DDC2Bi_FIFOMODE_XDATA_ADDR_OFFSET(Source,0)+u8AddPoint%D2B_FIFO_Size);


#else
                        rxIndex++;
#endif
#if !D2B_HW_CheckSum
                        //rxChkSum ^= XSRAMBuffer[DDC2Bi_FIFOMODE_XDATA_ADDR_OFFSET(Source,0)+u8AddPoint%D2B_FIFO_Size];
                        rxChkSum ^= D2B_XSRAM_READ(DDC2Bi_FIFOMODE_XDATA_ADDR_OFFSET(Source,0)+u8AddPoint%D2B_FIFO_Size);
#endif
                    }
                }

                // CheckSum
                if(rxIndex>0 && rxLength==0)
                    rxLength = DDCBuffer[0] & ~(BYTE)DDC2Bi_CONTROL_STATUS_FLAG;


#if D2B_HW_CheckSum
                if( ( (rxLength+2) == rxIndex ) && ( DDC2Bi_FIFOMODE_READ_HW_CHKSUM_FLAG(Source) == 0 ) )

#else
                if( ( (rxLength+2) == rxIndex ) && ( rxChkSum == 0x00 && rxIndex > 0 ) )
#endif
                {
                    Set_RxBusyFlag();
                    rxStatus = DDC2B_COMPLETED;
#if !D2B_XShared_DDCBuffer
                    Present_DDC2MCU_CUR_ADR &= (D2B_FIFO_Size-1);
#endif
                    rxInputPort = D2B_FIFOMODE_DDCPORT2INPUT(Source);
                    rxComboPort = Source;
                }
                else
                {
                    rxStatus = DDC2B_CLEAR;
                    Clr_RxBusyFlag();
#if D2B_XDATA_DEBUG
                //XSRAMBuffer[FIFODBGOFFSET+D2B_FIFO_Size+48+(IntCount%16)+(IntCount/16)*80] |= 0x40;
                D2B_XSRAM_WRITE(FIFODBGOFFSET+D2B_FIFO_Size+48+(IntCount%16)+(IntCount/16)*80, D2B_XSRAM_READ(FIFODBGOFFSET+D2B_FIFO_Size+48+(IntCount%16)+(IntCount/16)*80)|0x40);
#endif
                }


                if(DDC2Bi_FIFOMODE_READ_FULL_FLAG(Source)) // Buffer Write Overflow at Race condition
                {
                    D2BErrorFlag=1;
                    DDC2Bi_InitRx();

#if D2B_XDATA_DEBUG
                    //XSRAMBuffer[FIFODBGOFFSET+D2B_FIFO_Size+64+(IntCount%16)+(IntCount/16)*80] = Present_DDC2MCU_START_ADR;
                    //XSRAMBuffer[FIFODBGOFFSET+D2B_FIFO_Size+80+(IntCount%16)+(IntCount/16)*80] = Present_DDC2MCU_CUR_ADR;
                    //XSRAMBuffer[FIFODBGOFFSET+D2B_FIFO_Size+96+(IntCount%16)+(IntCount/16)*80] = 0x55;
                    //XSRAMBuffer[FIFODBGOFFSET+D2B_FIFO_Size+112+(IntCount%16)+(IntCount/16)*80] = msRegs[REG_3EE6];
                    D2B_XSRAM_WRITE(FIFODBGOFFSET+D2B_FIFO_Size+64+(IntCount%16)+(IntCount/16)*80, Present_DDC2MCU_START_ADR);
                    D2B_XSRAM_WRITE(FIFODBGOFFSET+D2B_FIFO_Size+80+(IntCount%16)+(IntCount/16)*80, Present_DDC2MCU_CUR_ADR);
                    D2B_XSRAM_WRITE(FIFODBGOFFSET+D2B_FIFO_Size+96+(IntCount%16)+(IntCount/16)*80, 0x55);
                    D2B_XSRAM_WRITE(FIFODBGOFFSET+D2B_FIFO_Size+112+(IntCount%16)+(IntCount/16)*80, msRegs(REG_003EE6));
#endif

                    DDC2Bi_FIFOMODE_CLR_EMPTYFULL_FLAG(Source); // clear
                }

                DDC2Bi_FIFOMODE_SET_RW_DONE(Source);
                if( _bit7_( DDC2Bi_Read_INT(Source)) )
                    DDC2Bi_CLR_INT(Source, BIT7);

                break;

            // Default
            default:
#if D2B_XDATA_DEBUG
                //XSRAMBuffer[FIFODBGOFFSET+D2B_FIFO_Size+3] = 0x44;
                //XSRAMBuffer[FIFODBGOFFSET+D2B_FIFO_Size+64+(IntCount%16)+(IntCount/16)*80] = 0x44;
                //XSRAMBuffer[FIFODBGOFFSET+D2B_FIFO_Size+80+(IntCount%16)+(IntCount/16)*80] = 0x44;
                //XSRAMBuffer[FIFODBGOFFSET+D2B_FIFO_Size+96+(IntCount%16)+(IntCount/16)*80] = intFlag;
                D2B_XSRAM_WRITE(FIFODBGOFFSET+D2B_FIFO_Size+3, 0x44);
                D2B_XSRAM_WRITE(FIFODBGOFFSET+D2B_FIFO_Size+64+(IntCount%16)+(IntCount/16)*80, 0x44);
                D2B_XSRAM_WRITE(FIFODBGOFFSET+D2B_FIFO_Size+80+(IntCount%16)+(IntCount/16)*80, 0x44);
                D2B_XSRAM_WRITE(FIFODBGOFFSET+D2B_FIFO_Size+96+(IntCount%16)+(IntCount/16)*80, intFlag);
#endif
                DDC2Bi_SET_RBUF(Source, DDC2Bi_Read_WBUF(Source));
                txStatus = DDC2B_CLEAR;
                DDC2Bi_InitRx();
                break;
        }
    }

#if D2B_XDATA_DEBUG
    IntCount++;
    if(rxStatus == DDC2B_COMPLETED)
        IntCountTmp = IntCount;
#endif
}
#else
void drvDoDDCCI( BYTE Source ) // Annie 2006.7.5
{
    BYTE xdata intFlag, rxByte;

    intFlag = DDC2Bi_Read_INT(Source);

    if( intFlag )
    {
        switch( intFlag &( TXBI | RCBI ) )
        {
            case TXBI:                //
                txStatus = DDC2B_MSGON;                // no characters left
                if( !( txLength ) )
                {
                    // clear the transmit status
                    txStatus = DDC2B_CLEAR;

                    DDC2Bi_SET_RBUF(Source, 0);
                    DDC2Bi_SET_WP_READY(Source);

                    return ;
                }
                // send out the current byte
                DDC2Bi_SET_RBUF(Source, *txBufferPtr++);
                txLength--;
                break;

            case RCBI:
                // read the received byte
                rxByte = DDC2Bi_Read_WBUF(Source);

                // depending of the message status
                switch( rxStatus )
                {
                        // in case there is nothing received yet
                    case DDC2B_CLEAR:
                        if( rxByte == DDC2B_SRC_ADDRESS )
                        {
                            rxStatus++; // = DDC2B_SRCADDRESS;
                            rxChkSum = INITRxCK;
                        }
                        else
                        {
                            DDC2Bi_InitRx();
                        }
                        break;
                        //
                    case DDC2B_SRCADDRESS:
                        // get the length
                        rxLength = rxByte &~(BYTE)DDC2Bi_CONTROL_STATUS_FLAG;
                        // put the received byte in DDCBuffer
                        DDCBuffer[rxIndex++] = rxByte;
                        rxChkSum ^= rxByte;
                        // set the receive body state
                        rxStatus++; // = DDC2B_COMMAND;
                        //if it is a NULL message
                        if (rxLength == 0)
                        {
                            rxStatus= DDC2B_COMPLETED;
                            Set_RxBusyFlag();
                        }
                        else if (rxLength >= DDC_BUFFER_LENGTH)
                        {
                            DDC2Bi_InitRx();
                        }
                        break;
                        // get the command
                    case DDC2B_COMMAND:
                        // save the commandbyte
                        rxStatus++; // = DDC2B_RECBODY;
                        // get the message body
                    case DDC2B_RECBODY:
                        DDCBuffer[rxIndex++] = rxByte;
                        rxChkSum ^= rxByte;
                        rxLength--;
                        // the last byte in the message body
                        if( rxLength == 0 )
                        {
                            rxStatus++; // = DDC2B_WAITFORCK;
                        }
                        break;
                        // ...here we are waiting for CheckSum...
                    case DDC2B_WAITFORCK:
                        // if CheckSum match
                        if( rxChkSum == rxByte )
                        {
                            rxInputPort = D2B_FIFOMODE_DDCPORT2INPUT(Source);
                            rxComboPort = Source;
                            rxStatus = DDC2B_COMPLETED;
                            Set_RxBusyFlag();
                        }
                        // elsechecksum error
                        else
                        {
                            // if CheckSum error re-initialize the receive buffer
                            DDC2Bi_InitRx();
                        }
                        break;
                    default:
                        // clear the rxState and the current buffer for a new message
                        if( !RxBusyFlag )
                            DDC2Bi_InitRx();
                        break;
                }
                break;

            default:
                DDC2Bi_SET_RBUF(Source, DDC2Bi_Read_WBUF(Source));
                txStatus = DDC2B_CLEAR;
                DDC2Bi_InitRx();
                break;

        }
    }
}
#endif

static void DDC2Bi_Delay(void)
{
    BYTE i, u8DelayCount;
    
    u8DelayCount = 3 + (mcuGetMcuSpeed()/100000000)*3;
    for(i=0; i<u8DelayCount; i++) //need wait 2T to update data from Hawk SVN27501
    {
         _nop_();
    }
}

void msDrvWriteInternalEDID(BYTE u8Block, BYTE u8Addr, BYTE u8Data)
{
    DWORD u32Timeout = 400; // ~ 550u

    while(DP_EDID_BUSY() && (u32Timeout > 0))
    {
        u32Timeout--;
    }
    
    msWriteByteMask( REG_003EEB, u8Block, 0x1F);    // Select sram base address for cpu read/write
    //msWriteByteMask(REG_003EEB,BIT0|BIT4,BIT0|BIT4);  // Select sram base address for cpu read/write
    msWriteByte( REG_003E4B, u8Addr );    // DDC address port for CPU read/write
    msWriteByte( REG_003E4C, u8Data);  // DDC Data Port for cpu write
    msWriteByteMask( REG_003E43, BIT5, BIT5);    //ADC sram write data pulse gen when cpu write
    TimeOutCounter = 10;
    DDC2Bi_Delay();
    while((msReadByte(REG_003E7B) & BIT5)&&( TimeOutCounter != 0));

////////////////////////////////////////////
    /*msWriteByteMask( REG_003EEB, u8Block, 0x1F);    // Select sram base address for cpu read/write

    msWriteByte( REG_003E4B, u8Addr );              // DDC address port for CPU read/write
    msWriteByte( REG_003E4C, u8Data);               // DDC Data Port for cpu write
    msWriteByteMask( REG_003E43, BIT5, BIT5);       //ADC sram write data pulse gen when cpu write
    SetTimOutConter(10);
    while((msReadByte(REG_003E7B) & BIT5)&&(bTimeOutCounterFlag));
    */
}

BYTE msDrvReadInternalEDID(BYTE u8Block, BYTE u8Addr)
{
    BYTE ucValue = 0;
    DWORD u32Timeout = 400; // ~ 550u

    while(DP_EDID_BUSY() && (u32Timeout > 0))
    {
        u32Timeout--;
    }

    msWriteByteMask( REG_003EEB, u8Block, 0x1F);    // Select sram base address for cpu read/write
    //msWriteByteMask(REG_003EEB,BIT0|BIT4,BIT0|BIT4); // Select sram base address for cpu read/write
    msWriteByte( REG_003E4B, u8Addr );    // DDC address port for CPU read/write
    msWriteByteMask( REG_003E43, BIT4, BIT4);    //ADC sram read data pulse gen when cpu read
    TimeOutCounter = 10;
    DDC2Bi_Delay();
    while((msReadByte(REG_003E7B) & BIT4)&&( TimeOutCounter != 0));

    ucValue = msReadByte( REG_003E0E );  // DDC Data Port for cpu read
    return ucValue;

    //////////////////////////////////////////
    /*BYTE ucValue = 0;
    msWriteByteMask( REG_003EEB, u8Block, 0x1F);    // Select sram base address for cpu read/write
    msWriteByte( REG_003E4B, u8Addr );              // DDC address port for CPU read/write
    msWriteByteMask( REG_003E43, BIT4, BIT4);       //ADC sram read data pulse gen when cpu read
    SetTimOutConter(10);
    while((msReadByte(REG_003E7B) & BIT4)&&(bTimeOutCounterFlag));

    ucValue = msReadByte( REG_003E0E );             // DDC Data Port for cpu read
    return ucValue;
*/

}

