#include "custom_feature_def.h"
#ifdef __OCPU_SMART_CLOUD_SUPPORT__
#ifdef __GITWIZS_SOLUTION__

#include "gagent.h"
#include "utils.h"
#include "adapter.h"
#include "hal_uart.h"
#include "hal_timer.h"
#include "cloud.h"


pgcontext pgContextData = NULL;
static u8 g_SN;

extern pfMasterMCU_ReciveData PF_ReceiveDataformMCU;
extern pfMasertMCU_SendData   PF_SendData2MCU;

extern s32 g_cloud_task_id;
extern int g_mcu_ack_count ;
extern s32 g_mcu_task_id;





/****************************************************************
Function    :   GAgent_DevInit
Description :   module init,waiting for ril layer init and network ok
pgc         :   global staruc pointer.
return      :   void
****************************************************************/
void GAgent_DevInit( pgcontext pgc )
{
    Adapter_Module_Init(pgc);
}


/****************************************************************
Function    :   GAgent_Init
Description :   GAgent init 
pgc         :   global staruc pointer.
return      :   void
****************************************************************/
void GAgent_Init( pgcontext *pgc )
{
    
    GAgent_NewVar( pgc );
    GAgent_DevInit(*pgc);
    //LOGLEVEL SETTING
    GAgent_VarInit(pgc);
    GAgent_LocalInit( *pgc );
}

/****************************************************************
Function    :   GAgent_LocalInit
Description :   init local for communication with MCU
pgc         :   global staruc pointer.
return      :   void
****************************************************************/
void GAgent_LocalInit( pgcontext pgc )
{
    Adapter_LocalDataIOInit( pgc );
    Adapter_TimerInit(HAL_MCU_ACK_TIMER,PGC);
    //uh6 : modify the heartbaet mechanism
    Adapter_TimerInit(HAL_MCU_HEATBEAT_TIMER,PGC);
    Adapter_TimerStart(HAL_MCU_HEATBEAT_TIMER,MCU_HEARTBEAT,TRUE);
    GAgent_LocalGetInfo(pgc);
    Adapter_SendMsg(g_cloud_task_id,MSG_ID_GPRS_OK_HINT,(void *)pgc,NULL);

}




/****************************************************************
Function    :   GAgent_NewVar
Description :   malloc New Var 
pgc         :   global staruc pointer.
return      :   void
****************************************************************/
void GAgent_NewVar( pgcontext *pgc )
{
    *pgc = (pgcontext)Adapter_Mem_Alloc( sizeof( gcontext ));
    while(NULL == *pgc)
    {
        *pgc = (pgcontext)Adapter_Mem_Alloc( sizeof( gcontext ));
        Adapter_Sleep(1);
    }
    Adapter_Memset(*pgc,0,sizeof(gcontext) );
}


/****************************************************************
FunctionName  :     GAgent_LocalGetInfo
Description   :     get localinfo like pk.
return        :     void
****************************************************************/
void GAgent_LocalGetInfo( pgcontext pgc )
{
    s32 ret;
    u8 i=0;
    u8 count = 10;
    
    for( i=0;i<count;i++ )
    {
        ret = GAgent_LocalDataWriteP0(pgc, pgc->rtinfo.local.uart_fd, pgc->rtinfo.Txbuf, MCU_INFO_CMD);
        if(RET_SUCCESS == ret)
        {
            APP_DEBUG("GAgent get local info ok.\r\n");
            APP_DEBUG("MCU Protocol Vertion:%s.\r\n",pgc->mcu.protocol_ver);
            APP_DEBUG("MCU P0 Vertion:%s.\r\n",pgc->mcu.p0_ver);
            APP_DEBUG("MCU Hard Vertion:%s.\r\n",pgc->mcu.hard_ver);
            APP_DEBUG("MCU Soft Vertion:%s.\r\n",pgc->mcu.soft_ver);
            APP_DEBUG("MCU old product_key:%s.\r\n",pgc->gc.old_productkey);
            APP_DEBUG("MCU product_key:%s.\r\n",pgc->mcu.product_key);
            for( i=0;i<MCU_MCUATTR_LEN;i++ )
            {
                APP_DEBUG("MCU mcu_attr[%d]= 0x%x.\r\n",i, (u32)pgc->mcu.mcu_attr[i]);
            }
            return ;
        }
        
    }
    if( count==i )
    {
    
        APP_DEBUG(" GAgent get local info fail ...\r\n ");
        APP_DEBUG(" Please check your local data,and restart GAgent again !!\r\n");
        Adapter_DevReset();
    }
}

/****************************************************************
FunctionName    :   GAgent_Clean_Config
Description     :   GAgent clean the device config                  
pgc             :   global staruc 
return          :   NULL
****************************************************************/
void GAgent_Clean_Config( pgcontext pgc )
{
    Adapter_Memset( pgc->gc.old_did,0,DID_LEN);
    Adapter_Memset( pgc->gc.old_modulepasscode,0,PASSCODE_MAXLEN + 1);
  
    Adapter_Memcpy( pgc->gc.old_did,pgc->gc.DID,DID_LEN );
    Adapter_Memcpy( pgc->gc.old_modulepasscode,pgc->gc.modulepasscode,PASSCODE_MAXLEN + 1 );
    APP_DEBUG("Reset GAgent and goto Disable Device !\r\n");  
    Cloud_ReqDisable( pgc );
   // pgc->rtinfo.waninfo.CloudStatus = CLOUD_RES_DISABLE_DID;
    Adapter_Memset( pgc->gc.DID,0,DID_LEN);
    Adapter_Memset( pgc->gc.modulepasscode,0,PASSCODE_MAXLEN + 1);

    
    Adapter_Memset( pgc->gc.GServer_ip,0,IP_LEN_MAX + 1);
    Adapter_Memset( pgc->gc.m2m_ip,0,IP_LEN_MAX + 1);

    Adapter_DevSaveConfigData( &(pgc->gc) );
}




/****************************************************************
FunctionName    :   GAgent_LocalDataWriteP0
Description     :   send p0 to local io and add 0x55 after 0xff
                    auto.
cmd             :   MCU_CTRL_CMD or WIFI_STATUS2MCU
return          :   0-ok other -error
****************************************************************/
s32 GAgent_LocalDataWriteP0( pgcontext pgc,s32 fd,ppacket pTxBuf,u8 cmd )
{
    s8 ret = RET_FAILED;
    u16 datalen = 0;
    u16 flag = 0;
    u16 sendLen = 0;
    u32 msgid;
    ST_MSG msg;

    if(pgc->mcu.isBusy)
    {
        APP_DEBUG(" local is busy, please wait and resend!!! ");
        return GAGENT_ERROR_MCU_BUSY;
    }

    /* step 1. add head... */
    /* head(0xffff)| len(2B) | cmd(1B) | sn(1B) | flag(2B) |  payload(xB) | checksum(1B) */
    pTxBuf->phead = pTxBuf->ppayload - 8;
    pTxBuf->phead[0] = MCU_HDR_FF;
    pTxBuf->phead[1] = MCU_HDR_FF;
    datalen = pTxBuf->pend - pTxBuf->ppayload + 5;    //p0 + cmd + sn + flag + checksum
    *(u16 *)(pTxBuf->phead + 2) = EndianConvertLToB(datalen);
    pTxBuf->phead[4] = cmd;
    pTxBuf->phead[5] = GAgent_NewSN();
    *(u16 *)(pTxBuf->phead + 6) = EndianConvertLToB(flag);
    *( pTxBuf->pend )  = GAgent_SetCheckSum(pTxBuf->phead, (pTxBuf->pend)-(pTxBuf->phead) );
    pTxBuf->pend += 1;  /* add 1 Byte of checksum */

    sendLen = (pTxBuf->pend) - (pTxBuf->phead);
    sendLen = Gagent_Local_DataAdapter( (pTxBuf->phead)+2,( (pTxBuf->pend) ) - ( (pTxBuf->phead)+2 ) );
    
    pgc->mcu.TxbufInfo.cmd = pTxBuf->phead[4];
    pgc->mcu.TxbufInfo.sn = pTxBuf->phead[5];
    pgc->mcu.TxbufInfo.local_send_len = sendLen;
    /* step 2. send data */
    HAL_Local_SendData( fd, pTxBuf->phead,sendLen );
    pgc->mcu.isBusy = 1;
    if( ((pTxBuf->type)&(LOCAL_DATA_OUT)) == LOCAL_DATA_OUT )
    {
        Adapter_SendMsg(g_mcu_task_id,MSG_ID_MCU_SEND_DATA,(void *)pgc,NULL);
    }
    else
    {
        Adapter_TimerStart(HAL_MCU_ACK_TIMER,MCU_ACK_TIME_MS,FALSE);
    }
    while(TRUE)
    {
         Adapter_GetMsg(&msg);
         switch(msg.message)
         {

            case MSG_ID_MCU_ACK_TIMEOUT:
                APP_DEBUG("send failed this time \r\n");
                ret = RET_FAILED;
                goto label;
                break;

            case MSG_ID_MCU_ACK_SUCCESS:
                APP_DEBUG("ack success \r\n");
                g_mcu_ack_count = 0;
                ret = RET_SUCCESS;
                goto label;
                break;
            default:
                break;
             
         }
    }
label:

    /* clear communicate flag */
    pgc->mcu.isBusy = 0;
    resetPacket(pTxBuf);
 
    return ret;
}

/****************************************************************
Function    :   GAgent_NewSN
Description :   generate a new serial no.
pgc         :   void
return      :   SN
****************************************************************/
u8 GAgent_NewSN(void)
{
    if(g_SN >= 255)
    {
        g_SN = 1;
    }
    return g_SN++;
}

/******************************************************
 *      FUNCTION        :   update info
 *      new_pk          :   new productkey
 *
 ********************************************************/
void GAgent_UpdateInfo( pgcontext pgc,u8 *new_pk )
{
    APP_DEBUG("\r\n a new productkey is :%s.\r\n",new_pk);
    /*the necessary information to disable devices*/
    Adapter_Memset( pgc->gc.old_did,0,DID_LEN);
    Adapter_Memset( pgc->gc.old_modulepasscode,0,PASSCODE_MAXLEN+1);
    Adapter_Strncpy( pgc->gc.old_did,pgc->gc.DID,DID_LEN);   
    Adapter_Strncpy( pgc->gc.old_modulepasscode,pgc->gc.modulepasscode,PASSCODE_MAXLEN+1);  
    Adapter_Memset( pgc->gc.old_productkey,0,PK_LEN + 1);
    Adapter_Strncpy( pgc->gc.old_productkey,new_pk,PK_LEN + 1);
    pgc->gc.old_productkey[PK_LEN] = '\0';
    
    /*neet to reset info */
    Adapter_Memset( pgc->gc.DID,0,DID_LEN );

    make_rand(pgc->gc.modulepasscode);
    Adapter_DevSaveConfigData( &(pgc->gc) );
}



/****************************************************************
FunctionName    :   GAgent_LocalDataAdapter
Dercription     :   the function will add 0x55 after local send data,
pData           :   the source of data need to change.
dataLen         :   the length of source data.
destBuf         :   the data after change.
return          :   the length of destBuf.                                  
****************************************************************/
s32 Gagent_Local_DataAdapter( u8 *pData,s32 dataLen )
{
    s32 i=0,j=0,len = 0;

    len = 2;//MCU_LEN_NO_PAYLOAD;
    len += dataLen;

    for( i=0;i<dataLen;i++ )
    {
        if( 0xFF==pData[i] )
        {
            GAgent_MoveOneByte( &pData[i+1],(dataLen-i),0 );
            pData[i+1] =0x55;
            j++;
            dataLen++;
        }
    }
    return len+j;
}

/****************************************************************
FunctionName    :   GAgent_MoveOneByte
Description     :   move the array one byte to left or right
pData           :   need to move data pointer.
dataLen         :   data length 
flag            :   0 move right
                    1 move left.
return          :   NULL
****************************************************************/
void GAgent_MoveOneByte( u8 *pData,s32 dataLen,u8 flag )
{
    s32 i=0;
    if( 0==flag)
    {
        for( i=dataLen;i>0;i-- )
        {
            pData[i] = pData[i-1];
        }
    }
    else if( 1==flag )
    {
        for( i=0;i<dataLen;i++ )
        {
            pData[i] = pData[i+1];
        }
    }
    return;
}



/*********************************************************
*
*       buf     :   need to get checksum buf, need not include the checksum of 
                    received package;
*       bufLen  :   bufLen      
*       return  :   checksum
**********************************************************/
u8 GAgent_SetCheckSum(  u8 *buf,int packLen )
{
    u8 checkSum=0;
    int i;
    
    /*i=2, remove the first two byte*/
    for( i=2;i<(packLen);i++ )
    {
        checkSum = checkSum+buf[i];
    }

    return checkSum;
}

/****************************************************************
Function    :   GAgent_VarInit
Description :   init global var and malloc memory 
pgc         :   global staruc pointer.
return      :   void
****************************************************************/
void GAgent_VarInit( pgcontext *pgc )
{
    int totalCap = BUF_LEN + BUF_HEADLEN;
    int bufCap = BUF_LEN;
    u32 ret;
    int i = 1;
    
    (*pgc)->rtinfo.firstStartUp = 1;
    //uart rxbuf
    (*pgc)->rtinfo.Rxbuf = (ppacket)Adapter_Mem_Alloc( sizeof(packet) );
    (*pgc)->rtinfo.Rxbuf->allbuf = (u8 *)Adapter_Mem_Alloc( totalCap );
    while( (*pgc)->rtinfo.Rxbuf->allbuf==NULL )
    {
        (*pgc)->rtinfo.Rxbuf->allbuf = (u8 *)Adapter_Mem_Alloc( totalCap );
        Adapter_Sleep(1);
    }
    Adapter_Memset( (*pgc)->rtinfo.Rxbuf->allbuf,0,totalCap );
    (*pgc)->rtinfo.Rxbuf->totalcap = totalCap;
    (*pgc)->rtinfo.Rxbuf->bufcap = bufCap;
    //uart txbuf
    (*pgc)->rtinfo.Txbuf = (ppacket)Adapter_Mem_Alloc( sizeof(packet) );
    (*pgc)->rtinfo.Txbuf->allbuf = (u8 *)Adapter_Mem_Alloc( totalCap );
    while( (*pgc)->rtinfo.Txbuf->allbuf==NULL )
    {
        (*pgc)->rtinfo.Txbuf->allbuf = (u8 *)Adapter_Mem_Alloc( totalCap );
        Adapter_Sleep(1);
    }
    Adapter_Memset( (*pgc)->rtinfo.Txbuf->allbuf,0,totalCap );
    (*pgc)->rtinfo.Txbuf->totalcap = totalCap;
    (*pgc)->rtinfo.Txbuf->bufcap = bufCap;
    //cloud rxbuf
    (*pgc)->rtinfo.Cloud_Rxbuf = (ppacket)Adapter_Mem_Alloc( sizeof(packet) );
    (*pgc)->rtinfo.Cloud_Rxbuf->allbuf = (u8 *)Adapter_Mem_Alloc( totalCap );
    while( (*pgc)->rtinfo.Cloud_Rxbuf->allbuf==NULL )
    {
        (*pgc)->rtinfo.Cloud_Rxbuf->allbuf = (u8 *)Adapter_Mem_Alloc( totalCap );
        Adapter_Sleep(1);
    }
    Adapter_Memset( (*pgc)->rtinfo.Cloud_Rxbuf->allbuf,0,totalCap );
    (*pgc)->rtinfo.Cloud_Rxbuf->totalcap = totalCap;
    (*pgc)->rtinfo.Cloud_Rxbuf->bufcap = bufCap;
    //cloud txbuf
    (*pgc)->rtinfo.Cloud_Txbuf = (ppacket)Adapter_Mem_Alloc( sizeof(packet) );
    (*pgc)->rtinfo.Cloud_Txbuf->allbuf = (u8 *)Adapter_Mem_Alloc( totalCap );
    while( (*pgc)->rtinfo.Cloud_Txbuf->allbuf==NULL )
    {
        (*pgc)->rtinfo.Cloud_Txbuf->allbuf = (u8 *)Adapter_Mem_Alloc( totalCap );
        Adapter_Sleep(1);
    }
    Adapter_Memset( (*pgc)->rtinfo.Cloud_Txbuf->allbuf,0,totalCap );
    (*pgc)->rtinfo.Cloud_Txbuf->totalcap = totalCap;
    (*pgc)->rtinfo.Cloud_Txbuf->bufcap = bufCap;
    
    resetPacket( (*pgc)->rtinfo.Rxbuf );
    resetPacket( (*pgc)->rtinfo.Txbuf );
    resetPacket( (*pgc)->rtinfo.Cloud_Rxbuf );
    resetPacket( (*pgc)->rtinfo.Cloud_Txbuf );
    

        /* get config data from flash */
    ret = Adapter_DevGetConfigData( &(*pgc)->gc );
   (*pgc)->rtinfo.waninfo.CloudStatus=CLOUD_INIT;


    /* get module imei */
    Adapter_Memset( (*pgc)->minfo.imei,0,IMEI_LEN+1 );
    Adapter_DevGetIMEI((*pgc)->minfo.imei);

    APP_DEBUG("module imei is :%s\r\n",(*pgc)->minfo.imei);
    
    APP_DEBUG("magic num is :%d\r\n",(*pgc)->gc.magicNumber);

    if((*pgc)->gc.magicNumber != GAGENT_MAGIC_NUM)
    {
        Adapter_Memset(&((*pgc)->gc), 0, sizeof(GAGENT_CONFIG_S));
        (*pgc)->gc.magicNumber = GAGENT_MAGIC_NUM;
        
    }
    else
    {
        if( Adapter_Strlen( (*pgc)->gc.DID )!=(DID_LEN-2) )
        {
            Adapter_Memset( ((*pgc)->gc.DID ),0,DID_LEN );
        }
        if( Adapter_Strlen( (*pgc)->gc.old_did )!=(DID_LEN-2))
        {
            Adapter_Memset( ((*pgc)->gc.old_did ),0,DID_LEN );
        }
        if( Adapter_Strlen( ((*pgc)->gc.modulepasscode) ) != PASSCODE_LEN )
        {    
            Adapter_Memset( ((*pgc)->gc.modulepasscode ),0,PASSCODE_MAXLEN + 1);
            make_rand( (*pgc)->gc.modulepasscode );
        }
        if( Adapter_Strlen( ((*pgc)->gc.old_modulepasscode) )!=PASSCODE_LEN || Adapter_Strlen( ((*pgc)->gc.old_did) )!= (DID_LEN-2) )
        {    
            Adapter_Memset( ((*pgc)->gc.old_modulepasscode ),0,PASSCODE_LEN );
            Adapter_Memset( ((*pgc)->gc.old_did ),0,DID_LEN );
        }
        if( Adapter_Strlen( ((*pgc)->gc.old_productkey) )!=(PK_LEN) )
        {
            Adapter_Memset( (*pgc)->gc.old_productkey,0,PK_LEN + 1 );
        }
        if( Adapter_Strlen( (*pgc)->gc.m2m_ip)>IP_LEN_MAX || Adapter_Strlen( (*pgc)->gc.m2m_ip)<IP_LEN_MIN )
        {
            Adapter_Memset( (*pgc)->gc.m2m_ip,0,IP_LEN_MAX + 1 );
        }
        
        if( Adapter_Strlen( (*pgc)->gc.GServer_ip)>IP_LEN_MAX || Adapter_Strlen( (*pgc)->gc.GServer_ip)<IP_LEN_MIN )
        {
            Adapter_Memset( (*pgc)->gc.GServer_ip,0,IP_LEN_MAX + 1 );
        }
    }
    Adapter_DevSaveConfigData( &((*pgc)->gc) );
    
}


u32 GAgent_Get_Gserver_Time( u32 *clock, u8 *Http_recevieBuf, s32 respondCode )
{
    s8 *p_start = NULL;
    s8 *p_end =NULL;
    s8 stime[20]={0};
    u32 time;

    if( 200 != respondCode )
    {
        return (u32)RET_FAILED;
    }
    p_start = Adapter_StrStr((char *)Http_recevieBuf, "server_ts=");
    if( p_start==NULL )
        return (u32)RET_FAILED;
    p_start = p_start+Adapter_Strlen("server_ts=");
    p_end = Adapter_StrStr( p_start,"&" );
    if( p_end == NULL )
    {
        p_end = Adapter_StrStr( p_start,"\r" );
    }
    Adapter_Memcpy(stime,p_start,( p_end-p_start));
    time = Adapter_Atoi(stime);
    *clock = time;
    return (u32)RET_SUCCESS;
}

static int isleap(int year)
{   
	return year % 4 == 0 && (year % 100 != 0 || year % 400 == 0);
}

static int get_yeardays(int year) 
{
    if (isleap(year))
        return 366;
    return 365;
}

_tm GAgent_GetLocalTimeForm(u32 time)
{
	_tm tm;
	int x;
	int i=1970, mons[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    tm.timezone = time;
    time += Eastern8th;
	for(i=1970; time>0;)
	{
	    x=get_yeardays(i);
	    if(time >= x*DAY_SEC)
	    {
	        time -= x*DAY_SEC;
	        i++;
	    }
	    else
	    {
	        break;
	    }
	}
    tm.year = i;
    
	for(i=0; time>0;)
	{
        if (isleap(tm.year))
            mons[1]=29;       
        if(time >= mons[i]*DAY_SEC)
	    {
	        time -= mons[i]*DAY_SEC;
	        i++;
	    }
	    else
	    {
	        break;
	    }
	}
	mons[1]=28;
	tm.month = i+1;
   
    for(i=1; time>0;)
	{
        if(time >= DAY_SEC)
	    {
	        time -= DAY_SEC;
	        i++;
	    }
	    else
	    {
	        break;
	    }
	}	
	tm.day=i;
    
    tm.hour = time/(60*60);
    tm.minute = time%(60*60)/60;
    tm.second = time%60;
    	
	return tm;
}

static s32 qeng_callback(char* line, u32 len, void* userData)
{
    pGprsInfo_t pgprsInfo = (GprsInfo_t *)userData;

    s32 dump ;

    u8 lac[8] = {0};
    u8 cellid[8] = {0};

    u32 lac_val,cellid_val;
   

    APP_DEBUG("line = %s\r\n",line);

    if (Ql_RIL_FindString(line, len, "+QENG:"))// find <CR><LF>OK<CR><LF>,OK<CR><LF>, <CR>OK<CR>¡ê?<LF>OK<LF>
    {
        Ql_sscanf(line,"%*[^:]: %d",&dump);
        
        APP_DEBUG("dump = %d\r\n",dump);
#if 1
        if(dump == 0)
        {
            pgprsInfo->CellNum = 1;
            pgprsInfo->CellInfoLen = 5;

            Ql_sscanf(line,"%*[^,],%[^,],%[^,],",pgprsInfo->MCC,pgprsInfo->MNC);
            Ql_sscanf(line,"%*[^,],%*[^,],%*[^,],%[^,],%[^,],",lac,cellid);

            Ql_HexStrToInt(lac,&lac_val);
            Ql_HexStrToInt(cellid,&cellid_val);

            pgprsInfo->CellInfo[0].LACID = lac_val;
            pgprsInfo->CellInfo[0].CellID = cellid_val;

            
            


            
            APP_DEBUG("mcc:%s ,mnc:%s ,lac :%d ,cellid:%d\r\n",pgprsInfo->MCC,pgprsInfo->MNC,pgprsInfo->CellInfo[0].LACID,pgprsInfo->CellInfo[0].CellID);
            
        }
        #endif
    }

    if (Ql_RIL_FindLine(line, len, "OK"))// find <CR><LF>OK<CR><LF>,OK<CR><LF>, <CR>OK<CR>¡ê?<LF>OK<LF>
    {
        return  RIL_ATRSP_SUCCESS;
    }
    else if (Ql_RIL_FindLine(line, len, "ERROR")) // find <CR><LF>ERROR<CR><LF>, <CR>ERROR<CR>¡ê?<LF>ERROR<LF>
    {
        return  RIL_ATRSP_FAILED;
    }
    else if (Ql_RIL_FindString(line, len, "+CME ERROR:") || 
              Ql_RIL_FindString(line, len, "+CMS ERROR:"))
    {
        return  RIL_ATRSP_FAILED;
    }
    return RIL_ATRSP_CONTINUE; //continue wait
    
}


static u32 GAgent_getGprsInfo(GprsInfo_t **GprsInfo)
{


    u32 rssi,ber;

    Adapter_Memset( (*GprsInfo)->IMEI,0,16 );
    Adapter_DevGetIMEI((*GprsInfo)->IMEI);
    
    APP_DEBUG("imei= %s\r\n",(*GprsInfo)->IMEI);
            /* get module imsi */
    Adapter_Memset( (*GprsInfo)->IMSI,0,16 );
    Adapter_DevGetIMSI((*GprsInfo)->IMSI);

    
    APP_DEBUG("imsi= %s\r\n",(*GprsInfo)->IMSI);

    //qeng
    
    Adapter_DevSetQeng(qeng_callback,*GprsInfo);

    Adapter_Get_Signal_Quality(&rssi,&ber);
    (*GprsInfo)->CellInfo[0].RSSI = rssi;
    
    APP_DEBUG("rssi= %d\r\n", (*GprsInfo)->CellInfo[0].RSSI);
    return RET_SUCCESS;
}



static u32 GAgent_sendGprsInfo( pgcontext pgc )
{   
    s32 i,pos = 0;
    GprsInfo_t GprsInfo;
    pGprsInfo_t pgprsInfo;
    pgprsInfo = &GprsInfo;
    
    Adapter_Memset(&GprsInfo,0,sizeof(GprsInfo_t));

    if(0 != GAgent_getGprsInfo(&pgprsInfo))
    {
        APP_DEBUG("gagent get GPRS info failed!");
        return RET_FAILED;
    }
    
    /* ModuleType */
    pgc->rtinfo.Txbuf->ppayload[0] = 0x02;
    pos += 1;
    
    /* MCU_PROTOCOLVER */
    Adapter_Memcpy( pgc->rtinfo.Txbuf->ppayload+pos, pgc->mcu.protocol_ver, MCU_PROTOCOLVER_LEN );
    pos += MCU_PROTOCOLVER_LEN;
    
    /* HARDVER */
    Adapter_Memcpy( pgc->rtinfo.Txbuf->ppayload+pos, MODULE_HARDVER, 7 );
    pos += 7;
    
    /* SOFTVAR */
    Adapter_Memcpy( pgc->rtinfo.Txbuf->ppayload+pos, MODULE_SOFTVAR, 9 );
    pos += 9;

    /* MCU_MCUATTR */
    Adapter_Memcpy( pgc->rtinfo.Txbuf->ppayload+pos, pgc->mcu.mcu_attr, MCU_MCUATTR_LEN);
    pos += MCU_MCUATTR_LEN;
#if 1
    /* IMEI */
    Adapter_Memcpy( pgc->rtinfo.Txbuf->ppayload+pos, GprsInfo.IMEI, 16 );
    pos += 16;

    /* IMSI */
    Adapter_Memcpy( pgc->rtinfo.Txbuf->ppayload+pos, GprsInfo.IMSI, 16 );
    pos += 16;

    /* MCC */
    Adapter_Memset(pgc->rtinfo.Txbuf->ppayload+pos, 0 , 8);
    Adapter_Memcpy( pgc->rtinfo.Txbuf->ppayload+pos, GprsInfo.MCC, Adapter_Strlen(GprsInfo.MCC) );
    pos += 8;
    
    /* MNC */
    Adapter_Memset(pgc->rtinfo.Txbuf->ppayload+pos, 0 , 8);
    Adapter_Memcpy( pgc->rtinfo.Txbuf->ppayload+pos, GprsInfo.MNC, Adapter_Strlen(GprsInfo.MNC) );
    pos += 8;
    
    /* CellNum */
    pgc->rtinfo.Txbuf->ppayload[pos] = GprsInfo.CellNum;
    pos += 1;

    /* CellInfoLen */
    pgc->rtinfo.Txbuf->ppayload[pos]  = GprsInfo.CellInfoLen;
    pos += 1;

    /* CellInfo*/
    for(i=0; i<GprsInfo.CellNum; i++)
    {
        *(u8 *)(pgc->rtinfo.Txbuf->ppayload+pos) = (u8)(EndianConvertLToB(GprsInfo.CellInfo[i].LACID) & 0x00ff);
        pos += 1;
        *(u8 *)(pgc->rtinfo.Txbuf->ppayload+pos) = (u8)(EndianConvertLToB(GprsInfo.CellInfo[i].LACID) >> 8);
        pos += 1;
        *(u8 *)(pgc->rtinfo.Txbuf->ppayload+pos) = (u8)(EndianConvertLToB(GprsInfo.CellInfo[i].CellID) & 0x00ff);
        pos += 1;
        *(u8 *)(pgc->rtinfo.Txbuf->ppayload+pos) = (u8)(EndianConvertLToB(GprsInfo.CellInfo[i].CellID) >>8 );
        pos += 1;
         *(u8 *)(pgc->rtinfo.Txbuf->ppayload+pos)  = (u8)(GprsInfo.CellInfo[i].RSSI);
        pos += 1;
    }
    
    pgc->rtinfo.Txbuf->pend = pgc->rtinfo.Txbuf->ppayload + pos;
 #endif
    return RET_SUCCESS;
}



u32 GAgent_sendmoduleinfo( pgcontext pgc )
{
    return GAgent_sendGprsInfo(pgc);
}


extern bool g_mqtt_connect_status;

void gagent_module_status_update(u32 param1,u32 param2,u16 *status)
{
    u32 rssi,ber;

    if(status == NULL)
    {
        return;
    }
    
        
    if(param1 == GAGENT_GSM_NW_STATE_IND || param1 == GAGENT_GPRS_NW_STATE_IND)
    {
        
        if(param2 == GAGENT_NW_STAT_REGISTERED || param2 == GAGENT_NW_STAT_REGISTERED_ROAMING)
        {
            (*status) |= 0x0010;
            Adapter_Get_Signal_Quality(&rssi,&ber);
            rssi = rssi/4;
            (*status) |= (0x0700 | ((u16)rssi << 8));
        }
        else
        {
            (*status) &= 0xFFEF;
            (*status) &= 0xF8FF;
        }
        
    }
    else
    {
        
        Adapter_Network_Query();
        return;
        
    }
    if(g_mqtt_connect_status)
    {
        (*status) |= 0x0020;        
    }
    else
    {
        
        (*status) &= 0xFFDF;
        
    }

    if(PGC->rtinfo.waninfo.wanclient_num > 0)
    {
        
(*status) |= 0x0800;
        
        APP_DEBUG("app exsits\r\n");
    }
    else
    {
        (*status) &= 0xF7FF;
    }
    
}




#endif
#endif
