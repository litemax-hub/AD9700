#define _MSAPI_MSBHK_C_

#include "board.h"
#include "types.h"
#include "ms_reg.h"
#include "Ms_rwreg.h"
#include "misc.h"
#include "msAPI_MSBHK.h"

#define ENABLE_MSBHK_CMD_DEBUG		1//0

#if ENABLE_MSTV_UART_DEBUG && ENABLE_MSBHK_CMD_DEBUG
#define MSBHK_PRINT_DATA(str, value)	printData(str, value)
#define MSBHK_PRINT_MSG(str) 			printMsg(str)
#else
#define MSBHK_PRINT_DATA(str, value)
#define MSBHK_PRINT_MSG(str)
#endif

BYTE code MSBHKStatus = 0xEE;
void MSBHKeDummy(void)
{
    BYTE xdata i = MSBHKStatus;
    i = i;
}

#if ENABLE_MSBHK

BYTE CommandSeq = 0;
XDATA ePMStatus ucMSBHK_WakeupStatus = ePMSTS_NON;

BYTE MSBHK_RxHandler(void)
{
    BYTE result =0;
    BYTE XDATA RXData[UART_CMD_LEN], TX_FIFO[UART_CMD_LEN], ReplyData[UART_CMD_LEN], Rx_Class, Rx_SubClass, Rx_Cmd;
    WORD XDATA RX_DATA_Size;
    WORD TX_Size;

    if( msdrv_MSBHK_UartRxDecode(SIORxIndex1, SIORxBuffer1, &Rx_Class, &Rx_SubClass, &Rx_Cmd, &RX_DATA_Size, RXData) )
    {
        if(msdrv_MSBHK_ParsingCommand(Rx_Class, Rx_SubClass, Rx_Cmd, RX_DATA_Size, RXData, ReplyData))
        {
            result = 1;
            SIORxIndex1 = 0;

            msdrv_MSBHK_UartTxEncode(CommandSeq, Rx_Class, Rx_SubClass, Rx_Cmd, 0, 0, &TX_Size, TX_FIFO);
            msdrv_MSBHK_UartTxSend(TX_FIFO, TX_Size);
            CommandSeq++;

            if((Rx_Class == API_NOTICE_CLASS) && (Rx_SubClass == NTE_SgnDetected))
            {
				msAPI_MSBHK_SetPMMode(POWER_ON, 0);
			}
        }
    }

    return result;
}

BYTE msdrv_MSBHK_ParsingCommand( BYTE Class, BYTE SubClass, BYTE Cmd, WORD DataSize, BYTE* RxData, BYTE* ReturnData)
{
    BYTE result;
    WORD i;

    for(i=0; i<DataSize; i++)
        *(ReturnData+i) = RxData[i];

    switch(Class)
    {
        case API_NOTICE_CLASS:
            result = msdrv_MSBHK_ParsingNoticeCommand(SubClass, Cmd, DataSize, RxData);
            MSBHK_PRINT_DATA("API_NOTICE_CLASS, reslut = 0x%x", result);
        break;

        default:
            MSBHK_PRINT_DATA("NO MATCH Class, Class = 0x%x", Class);
//            result = NOMATCHCLASS;
        break;
    }

    return result;
}

BYTE msdrv_MSBHK_ParsingNoticeCommand(BYTE SubClass, BYTE Cmd, WORD DataSize, BYTE* RxData)
{
    BYTE result = 0;
    Cmd = Cmd;
    DataSize = DataSize;
    RxData = RxData;

    switch(SubClass)
    {
        case NTE_MCCSDetected: //Map ePM_WakeupStatus of MSBHK to ePMStatus of TSUMG
        {
            switch(RxData[0])
            {
                case 0x05:
                    ucMSBHK_WakeupStatus = ePMSTS_MCCS04_ACT;
                break;

                case 0x06:
                    ucMSBHK_WakeupStatus = ePMSTS_MCCS05_ACT;
                break;

                case 0x07:
                    ucMSBHK_WakeupStatus = ePMSTS_MCCS01_ACT;
                break;

                default:
                    ucMSBHK_WakeupStatus = ePMSTS_NON;
                break;
            }
		}

        case NTE_SgnDetected:

        case NTE_CECPwrOn:

        case NTE_GPIODetected:

        case NTE_BtCmp:

            MSBHK_PRINT_DATA("MATCH Command, SubClass = 0x%x", SubClass);
            result = MATCHCMD;
        break;

        default:
             MSBHK_PRINT_DATA("NO MATCH SubClass, SubClass = 0x%x", SubClass);
//             result = NOMATCHSUBCLASS;
        break;
    }

    return result;
}

void msdrv_MSBHK_UartTxSend(BYTE *TXFIFO, WORD size)
{
    WORD i;

    for (i = 0; i < size; i++)
        putSIOChar_UART1(TXFIFO[i]);
}

BYTE msdrv_MSBHK_UartTxEncode(BYTE Seq, BYTE Class, BYTE SubClass, BYTE Cmd, WORD Data_Size, BYTE* Data, WORD* TX_FIFIO_Size, BYTE* TX_FIFO)
{
    WORD i, u16CheckSum=0;

    *TX_FIFIO_Size = Data_Size+7;   //LEN_H  LEN_L  SEQ,  CLASS,  SUbClass,  Cmd,  Data[],  CKS
    TX_FIFO[0] = (*TX_FIFIO_Size)>>8;
    TX_FIFO[1] = ((*TX_FIFIO_Size) & 0x00FF);
    TX_FIFO[2] = Seq;
    TX_FIFO[3] = Class;
    TX_FIFO[4] = SubClass;
    TX_FIFO[5] = Cmd;
    TX_FIFO[6] = 0;

    for (i = 0; i < 6; i++)
        u16CheckSum += TX_FIFO[i];

    for(i=6; i<((*TX_FIFIO_Size)-1); i++)
    {
        TX_FIFO[i] = Data[i-6];
        u16CheckSum += TX_FIFO[i];
    }

    u16CheckSum &= 0x00FF;
    TX_FIFO[(*TX_FIFIO_Size)-1] = 0x100 - (BYTE)u16CheckSum;


#if 0
    for(i=0; i<((*TX_FIFIO_Size)); i++)
    {
       MSBHK_PRINT_DATA("TX_FIFO = %x",TX_FIFO[i]);
    }
#endif

    return TRUE;
}

BYTE msdrv_MSBHK_UartRxDecode(WORD RX_index, BYTE* RX_FIFO, BYTE* RX_Class, BYTE* RX_SubClass, BYTE* RX_Cmd, WORD* RX_Data_Size, BYTE* RX_Data )
{
    BYTE result=UART_INISTATUS;
    WORD u16CheckSum=0, i;

    if ( (RX_index >= UART_COM_LEN) && (UART_COM_LEN))
    {
        for(i=0; i<UART_COM_LEN; i++)
            u16CheckSum += RX_FIFO[i];

        u16CheckSum &= 0xFF;

        if(!u16CheckSum)
        {
            *RX_Class = RX_FIFO[3];
            *RX_SubClass = RX_FIFO[4];
            *RX_Cmd = RX_FIFO[5];

            *RX_Data_Size = UART_COM_LEN - 7;

            for(i=0; i<*RX_Data_Size; i++)
                *(RX_Data+i) = *(RX_FIFO+6+i);

            result = UART_CHECKSUM_OK;
        }
        else
        {
//            MSBHK_PRINT_DATA("u16CheckSum fail, u16CheckSum = %x", u16CheckSum);
            return UART_CHECKSUM_FAIL;
        }
    }

    return result;
}

// ReplyDataSize !=0: to get the size of ReplyData
// ReplyDataSize ==0: not to get the size of ReplyData
BYTE msAPI_MSBHK_SetCmd( BYTE Class, BYTE SubClass, BYTE Cmd, WORD Data_Size, BYTE* Data, WORD* ReplyDataSize, BYTE* ReplyData)
{
    BYTE UART_RX_Status=UART_INISTATUS;
    BYTE xdata TX_FIFO[UART_CMD_LEN], RXData[UART_CMD_LEN], Rx_Class, Rx_SubClass, Rx_Cmd;
    WORD TX_Size, i, RX_Size;

#if 0
    if(Data_Size)
    {
        for(i=0; i<Data_Size; i++)
            MSBHK_PRINT_DATA("Data = %x", Data[i]);
    }
#endif

    msdrv_MSBHK_UartTxEncode(CommandSeq, Class, SubClass, Cmd, Data_Size, Data, &TX_Size, TX_FIFO);

	msdrv_MSBHK_UartTxSend(TX_FIFO, TX_Size);

    SetTimOutConter(UART_WAIT_TIME);
    do
    {
        UART_RX_Status = msdrv_MSBHK_UartRxDecode(SIORxIndex1, SIORxBuffer1, &Rx_Class, &Rx_SubClass, &Rx_Cmd, &RX_Size, RXData);

        if(UART_RX_Status == CHECKSUM_FAIL)
        {
            MSBHK_PRINT_MSG("MSBHK CHECKSUM_FAIL!");
            break;
        }

    }while(UART_RX_Status==UART_INISTATUS && u16TimeOutCounter);

    if(!u16TimeOutCounter)
    {
        SIORxIndex1 = 0;
        MSBHK_PRINT_MSG("MSBHK Set Cmd TIMEOUT!");
    }

    if(UART_RX_Status == UART_CHECKSUM_OK)
    {
        SIORxIndex1 = 0;

        if((Class == Rx_Class) && (SubClass==Rx_SubClass) && (Cmd==Rx_Cmd)) //check ACK
        {
//            MSBHK_PRINT_MSG("ACK Match");

            for(i=0; i<RX_Size; i++)
            {
                *(ReplyData+i) = RXData[i];
            }
            if(ReplyDataSize)
            {
                *ReplyDataSize = RX_Size;
            }

            CommandSeq++;

            return MATCHCMD;
        }
        else
        {
            MSBHK_PRINT_MSG("ACK Not Match!");
            return NOMATCHCMD;
        }
    }

    return NOACK;
}

BYTE msAPI_MSBHK_GetVersion(WORD* Ver_H, WORD* Ver_L)
{
    BYTE result=0;
    BYTE ReplyData[4];

    MSBHK_PRINT_MSG("msAPI_MSBHK_Get_Version");
    result = msAPI_MSBHK_SetCmd( API_INFO_CLASS, IFO_GetCMD, 0x01, 0, 0, 0, &ReplyData);

    if(result == MATCHCMD)
    {
        *Ver_H = (ReplyData[0]<<8|ReplyData[1]);
        *Ver_L = (ReplyData[2]<<8|ReplyData[3]);
    }

    return result;
}

void msAPI_MSBHK_SetPMMode(EN_PM_MODE Mode, WORD Port)
{
    BYTE WakePort[3];

    WakePort[0] = Mode;     // Mode;  0: Power ON, 1: DPMS, 2: DC Off
    WakePort[1] = Port >> 8;
    WakePort[2] = Port & 0xFF;
    msAPI_MSBHK_SetCmd(API_DEVICE_CLASS, DEV_PMMode, 0, 3, WakePort, 0, 0);
}
#endif

#undef _MSAPI_MSBHK_C_

