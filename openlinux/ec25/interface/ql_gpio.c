#include "ql_oe.h"
#include "gpioSysfs.h"


int Ql_GPIO_Init(Enum_PinName       pinName, 
                 Enum_PinDirection  dir, 
                 Enum_PinLevel      level, 
                 Enum_PinPullSel    pullSel
                 )
{
	int iRet;
	
	ST_GpioSysfs* pstGpioItem = GetGpioItemByPin(pinName);
	if (!pstGpioItem)
	{// pin not support
		return RES_IO_NOT_SUPPORT;
	}

	//printf("gpioname = %d\n",pstGpioItem->gpioNum);
	unsigned char gpio =pstGpioItem->gpioNum;
	iRet=Ql_Write_Register_all(gpio,0);	

	// export gpio
	iRet = GpioSysfs_ExportGpio(pinName);
	printf("GpioSysfs_ExportGpio(%d)=%d\n", pinName, iRet);

	// set direction
	iRet = GpioSysfs_WriteDirection(pinName, dir);
	printf("GpioSysfs_WriteDirection(%d, %d)=%d\n", pinName, dir, iRet);

	// set level
	if (PINDIRECTION_OUT == dir)
	{
		iRet = GpioSysfs_WriteValue(pinName, level);
		printf("GpioSysfs_WriteValue(%d, %d)=%d\n", pinName, level, iRet);
	}

	// set pull
	// not implemented...

	return iRet;
}

int Ql_GPIO_SetDirection(Enum_PinName pinName, Enum_PinDirection dir)
{
	return GpioSysfs_WriteDirection(pinName, dir);
}

int Ql_GPIO_GetDirection(Enum_PinName pinName)
{
	return GpioSysfs_ReadDirection(pinName);
}

int Ql_GPIO_SetLevel(Enum_PinName pinName, Enum_PinLevel level)
{
	return GpioSysfs_WriteValue(pinName, level);
}

int Ql_GPIO_GetLevel(Enum_PinName pinName)
{
	return GpioSysfs_ReadValue(pinName);
}

int Ql_GPIO_SetPullSelection(Enum_PinName pinName, Enum_PinPullSel pullSel)
{
	return RES_NOT_IMPLEMENTED;
}

int Ql_GPIO_GetPullSelection(Enum_PinName pinName)
{
	return RES_NOT_IMPLEMENTED;
}

int Ql_GPIO_Uninit(Enum_PinName pinName)
{
	int iRet;

	// Unexport gpio
	iRet = GpioSysfs_UnexportGpio(pinName);
	printf("GpioSysfs_UnexportGpio(%d)=%d\n", pinName, iRet);
	return iRet;
}
