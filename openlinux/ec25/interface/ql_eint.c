#include "ql_oe.h"
#include "gpioSysfs.h"
#include "ql_eint.h"


int Ql_EINT_Open(Enum_PinName eintPinName)
{
	int iRet;

	// export gpio
	iRet = GpioSysfs_ExportGpio(eintPinName);
	printf("GpioSysfs_ExportGpio(%d)=%d\n", eintPinName, iRet);
	if (iRet < RES_OK)
	{
	    return iRet;
	}

	// set direction
	iRet = GpioSysfs_WriteDirection(eintPinName, PINDIRECTION_IN);
	printf("GpioSysfs_WriteDirection(%d, %d)=%d\n", eintPinName, PINDIRECTION_IN, iRet);
	if (iRet < RES_OK)
	{
	    return iRet;
	}

    // Open FD
    return GpioSysfs_OpenValue(eintPinName);    
}

int Ql_EINT_Enable(Enum_PinName eintPinName, Enum_EintType eintType)
{
    if (eintType <= EINT_SENSE_NONE)
    {
        printf("Parameter is invalid\n");
        return RES_BAD_PARAMETER;
    }
    int iRet = GpioSysfs_SetEdgeSense(eintPinName, eintType);
	printf("GpioSysfs_SetEdgeSense(%d, %d)=%d\n", eintPinName, eintType, iRet);
    return iRet;
}

int Ql_EINT_Disable(Enum_PinName eintPinName)
{
    return GpioSysfs_SetEdgeSense(eintPinName, EINT_SENSE_NONE);
}

int Ql_EINT_Close(Enum_PinName eintPinName)
{
	// Unexport gpio
	return GpioSysfs_UnexportGpio(eintPinName);
}

