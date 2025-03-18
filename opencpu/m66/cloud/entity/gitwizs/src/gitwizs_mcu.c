#include "custom_feature_def.h"
#ifdef __OCPU_SMART_CLOUD_SUPPORT__
#ifdef __GITWIZS_SOLUTION__

#include "gagent.h"
#include "adapter.h"
#include "hal_uart.h"
#include "hal_timer.h"

s32 g_mcu_task_id;

static u16 module_status = 0x000a;


extern u8 global_sn;


//mcu task entry for mcu --- module interactive
void gagent_mcu_task(s32 taskId)
{
    ST_MSG msg;
    s32 ret;
    _tm tm;
    u32 ntp;

    APP_DEBUG("Gagent mcu task starts running\r\n");
    
    g_mcu_task_id = taskId;
    
    Adapter_TimerInit(HAL_MODULE_STATUS_TIMER,PGC);
    
    
    
    GAgent_Init( PPGC );

    
    Adapter_TimerStart(HAL_MODULE_STATUS_TIMER,MODULE_STATUS,TRUE);

    while (TRUE)
    {
        Adapter_GetMsg(&msg);
        switch(msg.message)
        {
            case MSG_ID_MCU_SEND_DATA:
                //Adapter_Memcpy(PGC->rtinfo.Txbuf,PGC->rtinfo.Cloud_Rxbuf,sizeof(packet));
                
                Adapter_TimerStart(HAL_MCU_ACK_TIMER,MCU_ACK_TIME_MS,FALSE);
                //GAgent_LocalDataWriteP0( PGC,PGC->rtinfo.local.uart_fd, (ppacket)msg.param1,MCU_CTRL_CMD );   
                //HAL_Local_SendData(s32 fd,u8 * pData,u32 bufferMaxLen)
                break;
            case MSG_ID_PING_REQUEST:
                GAgent_LocalDataWriteP0(PGC, PGC->rtinfo.local.uart_fd, PGC->rtinfo.Txbuf, MODULE_PING2MCU);
                break;
            case MSG_ID_GSERVER_TIME_RESPONSE:
               
                tm = GAgent_GetLocalTimeForm(PGC->rtinfo.clock);
                ntp = (u32)EndianConvertLToB_Long(tm.timezone);
                
                APP_DEBUG("Gserver Time:%d,%d,%d,%d,%d,%d,%d\r\n",tm.year,tm.month,tm.day,\
                    tm.hour,tm.minute,tm.second,tm.timezone);
                *(u16 *)(PGC->rtinfo.Txbuf->ppayload) = (u16)(EndianConvertLToB(tm.year));
                PGC->rtinfo.Txbuf->ppayload[2] = (u8)(tm.month);
                PGC->rtinfo.Txbuf->ppayload[3] = (u8)(tm.day);
                PGC->rtinfo.Txbuf->ppayload[4] = (u8)(tm.hour);
                PGC->rtinfo.Txbuf->ppayload[5] = (u8)(tm.minute);
                PGC->rtinfo.Txbuf->ppayload[6] = (u8)(tm.second);
                PGC->rtinfo.Txbuf->ppayload[7] = (u8)(ntp >> 24);
                PGC->rtinfo.Txbuf->ppayload[8] = (u8)((ntp & 0x00ff0000)>>16);
                
                PGC->rtinfo.Txbuf->ppayload[9] = (u8)((ntp & 0x0000ff00)>>8);

                PGC->rtinfo.Txbuf->ppayload[10] = (u8)(ntp & 0x000000ff);
                //*(u32 *)(PGC->rtinfo.Txbuf->ppayload+11) = );
                PGC->rtinfo.Txbuf->pend = (PGC->rtinfo.Txbuf->ppayload) + 11;



                HAL_Ack2MCUwithP0( PGC->rtinfo.Txbuf, PGC->rtinfo.local.uart_fd, global_sn, MCU_REQ_GSERVER_TIME_ACK );
                break;
            case MSG_ID_URC_INDICATION:
                
                APP_DEBUG("module status update\r\n");
                gagent_module_status_update(msg.param1,msg.param2,&module_status);
                resetPacket(PGC->rtinfo.Txbuf);
                PGC->rtinfo.Txbuf->ppayload[0]= module_status >> 8;
                PGC->rtinfo.Txbuf->ppayload[1]= module_status & 0x00FF;
                
                PGC->rtinfo.Txbuf->pend = (PGC->rtinfo.Txbuf->ppayload) + 2;
                
                GAgent_LocalDataWriteP0(PGC, PGC->rtinfo.local.uart_fd, PGC->rtinfo.Txbuf, MODULE_STATUS2MCU);
                break;
            case MSG_ID_MODULE_STATUS_UPDATE:
                gagent_module_status_update(0,0,&module_status);
                break;
            default:
                break;
        }
    }
}




#endif
#endif
