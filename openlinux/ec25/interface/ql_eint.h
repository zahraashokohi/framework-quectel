/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of Quectel Co., Ltd. 2016
*
*****************************************************************************/
/*****************************************************************************
 *
 * Filename:
 * ---------
 *   ql_eint.h 
 *
 * Project:
 * --------
 *   OpenLinux
 *
 * Description:
 * ------------
 *   External interrupt API definition.
 *
 * Author:
 * -------
 * Stanley YONG
 * -------
 *
 *============================================================================
 *             HISTORY
 *----------------------------------------------------------------------------
 * WHO            WHEN                WHAT
 *----------------------------------------------------------------------------
 * Stanley.YONG   26/07/2016		  Create
 ****************************************************************************/

#ifndef __QL_EINT_H__
#define __QL_EINT_H__

#include "ql_gpio_def.h"


//------------------------------------------------------------------------------
/**
 * The type of GPIO Edge Sensivity.
 */
//------------------------------------------------------------------------------
typedef enum {
    EINT_SENSE_NONE,       // pin is input, but no an interrupt pin.
    EINT_SENSE_RISING,
    EINT_SENSE_FALLING,
    EINT_SENSE_BOTH
}Enum_EintType;


//------------------------------------------------------------------------------
/*
* Description:
*     Definition for EINT callback function.
* 
* Parameters:
*     eintPinName:
*         EINT pin name, one value of Enum_PinName.
*
*     levelState:
*         The EINT level value, one value of Enum_PinLevel. 
*
*     param:
*         reserved.
*/
//------------------------------------------------------------------------------
typedef void (*Callback_onInterrupt)(Enum_PinName  eintPinName, boolean levelState, void* param);


//------------------------------------------------------------------------------
/*
* Function:     Ql_EINT_Init 
* 
* Description:
*               Initialize the specified pin to interrupt mode. 
*
* Parameters:
*               eintPinName:
*                   EINT pin name, one value of Enum_PinName that has 
*                   the interrupt function.
*
*               callback_eint:
*                   The interrupt handler.
*
*               customParam:
*                   A customized parameter, which can be use in interrupt handler. 
* Return:        
*               RES_OK, this function succeeds.
*               else failed to execute the function. 
*/
//------------------------------------------------------------------------------
int Ql_EINT_Open(Enum_PinName eintPinName);


//------------------------------------------------------------------------------
/*
* Function:     Ql_EINT_Enable 
* 
* Description:
*               Set the interrupt sense mode, and enable interrupt. 
*
* Parameters:
*               eintPinName:
*                   EINT pin name, one value of Enum_PinName that has 
*                   the interrupt function.
*
*               eintType:
*                   Interrupt type, level-triggered or edge-triggered.
*                   Now, only level-triggered interrupt is supported.
*
* Return:        
*               RES_OK, this function succeeds.
*               else failed to execute the function. 
*/
//------------------------------------------------------------------------------
int Ql_EINT_Enable(Enum_PinName eintPinName, Enum_EintType eintType);


//------------------------------------------------------------------------------
/*
* Function:     Ql_EINT_Disable 
* 
* Description:
*               Disable the interrupt sense. 
*
* Parameters:
*               eintPinName:
*                   EINT pin name, one value of Enum_PinName that has 
*                   the interrupt function.
*
* Return:        
*               RES_OK, this function succeeds.
*               else failed to execute the function. 
*/
//------------------------------------------------------------------------------
int Ql_EINT_Disable(Enum_PinName eintPinName);


//------------------------------------------------------------------------------
/*
* Function:     Ql_EINT_Close 
* 
* Description:
*               This function releases the specified EINT pin that was 
*               initialized by calling Ql_EINT_Init() previously.
*               After releasing, the pin can be used for other purpose.
* Parameters:
*               eintPinName:
*                   EINT pin name, one value of Enum_PinName that has 
*                   the interrupt function.
* Return:        
*               RES_OK, this function succeeds.
*               else failed to execute the function. 
*/
//------------------------------------------------------------------------------
int Ql_EINT_Close(Enum_PinName eintPinName);


#endif  // __QL_EINT_H__
