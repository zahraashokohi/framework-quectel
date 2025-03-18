#ifndef __QL_NETWORK_H__
#define __QL_NETWORK_H__


//------------------------------------------------------------------------------
/*
* Function:     NW_Status_CB 
* 
* Description:
*               Callback for application to indicate if network is ready. 
*
* Parameters:
*               timeout:
*                   unit in second.
*
*               nw_status_cb:
*                   callback function for indication of network status.
*
* Return:        
*               RES_OK, this function succeeds.
*               else failed to execute the function. 
*/
//------------------------------------------------------------------------------
typedef void (*NW_Status_CB)(int result);


//------------------------------------------------------------------------------
/*
* Function:     Ql_NW_EnableMobileAP 
* 
* Description:
*               Start MobileAP, and detect if network is ready. 
*
* Parameters:
*               timeout:
*                   unit in second.
*
*               nw_status_cb:
*                   callback function for indication of network status.
*
* Return:        
*               RES_OK, this function succeeds.
*               else failed to execute the function. 
*/
//------------------------------------------------------------------------------
int Ql_NW_EnableMobileAP(int timeout, NW_Status_CB nw_status_cb);

#endif  //__QL_NETWORK_H__
