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
 *   gpioSysfs.c
 *
 * Project:
 * --------
 *   OpenLinux
 *
 * Description:
 * ------------
 *   API implementation for GPIO driver.
 *
 * Author:
 * ------------
 * Stanley YONG
 * ------------
 *
 *============================================================================
 *             HISTORY
 *----------------------------------------------------------------------------
 * WHO            WHEN                WHAT
 *----------------------------------------------------------------------------
 * Stanley.YONG   15/07/2016		  Create
 * Stanley.YONG   26/07/2016		  Add interrupt implementation.
 ****************************************************************************/
#include <unistd.h>
#include <pthread.h>
#include <sys/epoll.h>
#include "gpioSysfs.h"

#define DEBUG_EN  0
#if DEBUG_EN
#define _DEBUG(fmtString, ...)     printf(fmtString, ##__VA_ARGS__)
#else
#define _DEBUG(fmtString, ...)
#endif


//--------------------------------------------------------------------------------------------------
/**
 * GPIO signals have paths like /sys/class/gpio/gpio24/ (for GPIO #24)
 */
//--------------------------------------------------------------------------------------------------
#define SYSFS_GPIO_PATH    "/sys/class/gpio"

static ST_GpioSysfs SysfsGpioPins[] = {
	/*PIN-1*/  {PINNAME_GPIO1, 		25, "gpio25\0", FALSE, 0, NULL, NULL},
	/*PIN-2*/  {PINNAME_GPIO2,  	10, "gpio10\0", FALSE, 0, NULL, NULL},
	/*PIN-3*/  {PINNAME_GPIO3, 		42, "gpio42\0", FALSE, 0, NULL, NULL},
	/*PIN-4*/  {PINNAME_GPIO4, 		11, "gpio11\0", FALSE, 0, NULL, NULL},
	/*PIN-5*/  {PINNAME_GPIO5,  	24, "gpio24\0", FALSE, 0, NULL, NULL},
	/*PIN-13*/ {PINNAME_USIM_PRESENCE, 34, "gpio34\0", FALSE, 0, NULL, NULL},
	/*PIN-23*/ {PINNAME_SD_INT_DET,	26, "gpio26\0", FALSE, 0, NULL, NULL},
	/*PIN-24*/ {PINNAME_PCM_IN,    	76, "gpio76\0", FALSE, 0, NULL, NULL},
	/*PIN-25*/ {PINNAME_PCM_OUT,   	77, "gpio77\0", FALSE, 0, NULL, NULL},
	/*PIN-26*/ {PINNAME_PCM_SYNC,  	79, "gpio79\0", FALSE, 0, NULL, NULL},
	/*PIN-27*/ {PINNAME_PCM_CLK,   	78, "gpio78\0", FALSE, 0, NULL, NULL},
	/*PIN-37*/ {PINNAME_SPI_CS_N,  	22, "gpio22\0", FALSE, 0, NULL, NULL},
	/*PIN-38*/ {PINNAME_SPI_MOSI,  	20, "gpio20\0", FALSE, 0, NULL, NULL},
	/*PIN-39*/ {PINNAME_SPI_MISO,  	21, "gpio21\0", FALSE, 0, NULL, NULL},
	/*PIN-40*/ {PINNAME_SPI_CLK,   	23, "gpio23\0", FALSE, 0, NULL, NULL},
	/*PIN-41*/ {PINNAME_I2C1_SCL,   7,  "gpio7\0",  FALSE, 0, NULL, NULL},
	/*PIN-42*/ {PINNAME_I2C1_SDA,   6,  "gpio6\0",  FALSE, 0, NULL, NULL},
	/*PIN-62*/ {PINNAME_GPIO6,      75, "gpio75\0", FALSE, 0, NULL, NULL},
	/*PIN-63*/ {PINNAME_UART1_TX,   4,  "gpio4\0",  FALSE, 0, NULL, NULL},
	/*PIN-66*/ {PINNAME_UART1_RX,   5,  "gpio5\0",  FALSE, 0, NULL, NULL},
	/*PIN-73*/ {PINNAME_GPIO7,    	49, "gpio49\0", FALSE, 0, NULL, NULL},
	/*PIN-119*/{PINNAME_EPHY_RST_N,	29, "gpio29\0", FALSE, 0, NULL, NULL},
	/*PIN-120*/{PINNAME_EPHY_INT_N,	30, "gpio30\0", FALSE, 0, NULL, NULL},
	/*PIN-121*/{PINNAME_SGMII_DATA,	28, "gpio28\0", FALSE, 0, NULL, NULL},
	/*PIN-122*/{PINNAME_SGMII_CLK,	27, "gpio27\0", FALSE, 0, NULL, NULL},
	/*PIN-129*/{PINNAME_SDC1_DATA3,	12, "gpio12\0", FALSE, 0, NULL, NULL},
	/*PIN-130*/{PINNAME_SDC1_DATA2,	13, "gpio13\0", FALSE, 0, NULL, NULL},
	/*PIN-131*/{PINNAME_SDC1_DATA1,	14, "gpio14\0", FALSE, 0, NULL, NULL},
	/*PIN-132*/{PINNAME_SDC1_DATA0,	15, "gpio15\0", FALSE, 0, NULL, NULL},
	/*PIN-133*/{PINNAME_SDC1_clk,	16, "gpio16\0", FALSE, 0, NULL, NULL},
	/*PIN-134*/{PINNAME_SDC1_CMD,	17, "gpio17\0", FALSE, 0, NULL, NULL},
	/*PIN-135*/{PINNAME_WAKE_WLAN,	59, "gpio59\0", FALSE, 0, NULL, NULL},
	/*PIN-136*/{PINNAME_WLAN_EN,	38, "gpio38\0", FALSE, 0, NULL, NULL},
	/*PIN-137*/{PINNAME_COEX_UART_RX,37,"gpio37\0", FALSE, 0, NULL, NULL},
	/*PIN-138*/{PINNAME_COEX_UART_TX,36,"gpio36\0", FALSE, 0, NULL, NULL},
	/*PIN-141*/{PINNAME_I2C2_CLK,  	19, "gpio19\0", FALSE, 0, NULL, NULL},
	/*PIN-142*/{PINNAME_I2C2_SDA,  	18, "gpio18\0", FALSE, 0, NULL, NULL},
	/*PIN-143*/{PINNAME_GPIO8,   	53, "gpio53\0", FALSE, 0, NULL, NULL},
	/*PIN-144*/{PINNAME_GPIO9,   	52, "gpio52\0", FALSE, 0, NULL, NULL}
};

static void __printGpioTbl(void)
{
    int i;
	int nCnt = sizeof(SysfsGpioPins) / sizeof(SysfsGpioPins[0]);

    for (i = 0; i < nCnt; i++)
    {
        _DEBUG("PIN[%d]: pinName=%d, gpioNum=%d, gpioName=%s, inUse=%d, monitorFd=%d proc=%X\n", i,
            SysfsGpioPins[i].pinName,
            SysfsGpioPins[i].gpioNum,
            SysfsGpioPins[i].gpioName,
            SysfsGpioPins[i].inUse,
            SysfsGpioPins[i].monitorFd,
            (uint32_t)SysfsGpioPins[i].fdMonitor_proc);
    }
}

ST_GpioSysfs* GetGpioItemByPin(const Enum_PinName pinName)
{
	int nCnt = sizeof(SysfsGpioPins) / sizeof(SysfsGpioPins[0]);
	int i;
	for (i = 0; i < nCnt; i++)
	{
		if (pinName == SysfsGpioPins[i].pinName)
		{
			return &SysfsGpioPins[i];
		}
	}
	return NULL;
}

static ST_GpioSysfs* GetGpioItemByFd(const int monFd)
{
	int nCnt = sizeof(SysfsGpioPins) / sizeof(SysfsGpioPins[0]);
	int i;
    _DEBUG("%s, monFd=%d\n", __func__, monFd);
	for (i = 0; i < nCnt; i++)
	{
		if (monFd == SysfsGpioPins[i].monitorFd)
		{
		    _DEBUG("found: pin=%d, gpio=%d,fd=%d, ptr=%X\n", 
		        SysfsGpioPins[i].pinName,
		        SysfsGpioPins[i].gpioNum,
		        SysfsGpioPins[i].monitorFd,
		        (uint32_t)SysfsGpioPins[i].fdMonitor_proc);
			return &SysfsGpioPins[i];
		}
	}
	return NULL;    
}

//--------------------------------------------------------------------------------------------------
/**
 * Check if sysfs gpio path exists.
 * @return
 * - true: gpio path exists
 * - FALSE: gpio path does not exist
 */
//--------------------------------------------------------------------------------------------------
static boolean GpioSysfs_CheckExist(const char* path)
{
    DIR* dir;

    dir = opendir(path);
    if (dir)
    {
        /* Directory exists. */
        closedir(dir);
    }
    else if (ENOENT == errno)
    {
        return FALSE;
    }

    return TRUE;
}


//--------------------------------------------------------------------------------------------------
/**
 * Export a GPIO in the sysfs.
 * @return
 * - RES_OK if exporting was successful
 * - RES_IO_NOT_SUPPORT pin is not supported
 * - RES_IO_ERROR if it failed
 */
//--------------------------------------------------------------------------------------------------
int GpioSysfs_ExportGpio(const Enum_PinName pinName)
{
    char path[128];
    char export[128];
    char gpioStr[8];
    FILE *fp = NULL;
	ST_GpioSysfs* pstGpioItem = GetGpioItemByPin(pinName);
	if (!pstGpioItem)
	{// pin not support
		return RES_IO_NOT_SUPPORT;
	}
	//printf("gpioname = %d\n",pstGpioItem->gpioNum);
	unsigned char gpio =pstGpioItem->gpioNum;
	printf("gpio num = %d\n",gpio);
	int ret = Ql_Write_Register(gpio,0);
	if(ret <0)
	{
	return -1;
	}

    // First check if the GPIO has already been exported
    snprintf(path, sizeof(path), "%s/%s", SYSFS_GPIO_PATH, pstGpioItem->gpioName);
    if (GpioSysfs_CheckExist(path))
    {
        return RES_OK;
    }

    // Write the GPIO number to the export file
    snprintf(export, sizeof(export), "%s/%s", SYSFS_GPIO_PATH, "export");
    snprintf(gpioStr, sizeof(gpioStr), "%d", pstGpioItem->gpioNum);
    do
    {
        fp = fopen(export, "w");
    } while ((fp == NULL) && (errno == EINTR));

    if (!fp)
	{
        _DEBUG("Error opening file %s for writing.\n", export);
        return RES_IO_ERROR;
    }

    ssize_t written = fwrite(gpioStr, 1, strlen(gpioStr), fp);
    fflush(fp);

    int file_err = ferror(fp);
    fclose(fp);
    if (file_err != 0)
    {
        _DEBUG("Failed to export GPIO %s. Error %s\n", gpioStr, strerror(file_err));
        return RES_IO_ERROR;
    }

    if (written < strlen(gpioStr))
    {
        _DEBUG("Data truncated while exporting GPIO %s.\n", gpioStr);
        return RES_IO_ERROR;
    }

    // Now check again that it has been exported
    if (GpioSysfs_CheckExist(path))
    {
        return RES_OK;
    }
    _DEBUG("Failed to export GPIO %s.\n", gpioStr);
    return RES_IO_ERROR;
}

int GpioSysfs_UnexportGpio(const Enum_PinName pinName)
{
    char path[128];
    char export[128];
    char gpioStr[8];
    FILE *fp = NULL;
	ST_GpioSysfs* pstGpioItem = GetGpioItemByPin(pinName);
	if (!pstGpioItem)
	{// pin not support
		return RES_IO_NOT_SUPPORT;
	}

    // First check if the GPIO has already been exported
    snprintf(path, sizeof(path), "%s/%s", SYSFS_GPIO_PATH, pstGpioItem->gpioName);
    if (!GpioSysfs_CheckExist(path))
    {
        return RES_OK;
    }

    // Write the GPIO number to the unexport file
    snprintf(export, sizeof(export), "%s/%s", SYSFS_GPIO_PATH, "unexport");
    snprintf(gpioStr, sizeof(gpioStr), "%d", pstGpioItem->gpioNum);
    do
    {
        fp = fopen(export, "w");
    } while ((fp == NULL) && (errno == EINTR));

    if (!fp)
	{
        _DEBUG("Error opening file %s for writing.\n", export);
        return RES_IO_ERROR;
    }

    ssize_t written = fwrite(gpioStr, 1, strlen(gpioStr), fp);
    fflush(fp);

    int file_err = ferror(fp);
    fclose(fp);
    if (file_err != 0)
    {
        _DEBUG("Failed to export GPIO %s. Error %s\n", gpioStr, strerror(file_err));
        return RES_IO_ERROR;
    }

    if (written < strlen(gpioStr))
    {
        _DEBUG("Data truncated while exporting GPIO %s.\n", gpioStr);
        return RES_IO_ERROR;
    }

    // Now check again that it has been unexported
    if (!GpioSysfs_CheckExist(path))
    {
        return RES_OK;
    }
    _DEBUG("Failed to unexport GPIO %s.\n", gpioStr);
    return RES_IO_ERROR;
}

//--------------------------------------------------------------------------------------------------
/**
 * Set sysfs GPIO signals attributes
 *
 * GPIO signals have paths like /sys/class/gpio/gpioN/
 * and have the following read/write attributes:
 * - "direction"
 * - "value"
 * - "edge"
 * - "active_low"
 * - "pull"
 *
 * @return
 * - RES_IO_ERROR: write sysfs gpio error
 * - RES_OK: successfully
 */
//--------------------------------------------------------------------------------------------------
static int GpioSysfs_WriteAttr(
    const char *path,        // [IN] path to sysfs gpio signal
    const char *attr         // [IN] GPIO signal write attribute
)
{
    FILE *fp = NULL;

    if (!GpioSysfs_CheckExist(path))
    {
        _DEBUG("GPIO %s does not exist (probably not exported)\n", path);
        return RES_BAD_PARAMETER;
    }

    do
    {
        fp = fopen(path, "w");
    }
    while ((fp == NULL) && (errno == EINTR));

    if (!fp) 
	{
        _DEBUG("Error opening file %s for writing.\n", path);
        return RES_IO_ERROR;
    }

    ssize_t written = fwrite(attr, 1, strlen(attr), fp);
    fflush(fp);

    int file_err = ferror(fp);
    if (file_err != 0)
    {
        _DEBUG("Failed to write %s to GPIO config %s. Error %s\n", attr, path, strerror(file_err));
        fclose(fp);
        return RES_IO_ERROR;
    }

    if (written < strlen(attr))
    {
        _DEBUG("Data truncated while writing %s to GPIO config %s.\n", path, attr);
        fclose(fp);
        return RES_IO_ERROR;
    }
    fclose(fp);
    return RES_OK;
}


//--------------------------------------------------------------------------------------------------
/**
 * Get sysfs GPIO attribute
 *
 * GPIO signals have paths like /sys/class/gpio/gpioN/
 * and have the following read/write attributes:
 * - "direction"
 * - "value"
 * - "edge"
 * - "active_low"
 * - "pull"
 *
 * @return
 * - -1: write sysfs gpio error
 * -  0: successfully
 */
//--------------------------------------------------------------------------------------------------
static int GpioSysfs_ReadAttr(
    const char *path,        // [IN] path to sysfs gpio
    int attr_size,           // [IN] the size of attribute content
    char *attr               // [OUT] GPIO signal read attribute content
)
{
    int i;
    char c;
    FILE *fp = NULL;
    char *result = attr;

    if (!GpioSysfs_CheckExist(path))
    {
        _DEBUG("File %s does not exist\n", path);
        return RES_BAD_PARAMETER;
    }

    do
    {
        fp = fopen(path, "r");
    } while ((fp == NULL) && (errno == EINTR));

    if(!fp) 
	{
        _DEBUG("Error opening file %s for reading.\n", path);
        return RES_IO_ERROR;
    }

    i = 0;
    while (((c = fgetc(fp)) != EOF) && (i < (attr_size - 1))) {
        result[i] = c;
        i++;
    }
    result[i] = '\0';
    fclose (fp);

    _DEBUG("Read result: %s from %s\n", result, path);

    return RES_OK;
}

//--------------------------------------------------------------------------------------------------
/**
 * setup GPIO Direction INPUT or OUTPUT mode.
 *
 * "direction" ... reads as either "in" or "out". This value may
 *        normally be written. Writing as "out" defaults to
 *        initializing the value as low. To ensure glitch free
 *        operation, values "low" and "high" may be written to
 *        configure the GPIO as an output with that initial value
 */
//--------------------------------------------------------------------------------------------------
int GpioSysfs_WriteDirection(
    Enum_PinName  pinName,    // [IN] pin name
    Enum_GPIO_Dir dir         // [IN] gpio direction input/output mode
)
{
    char path[64];
    char attr[16];
	ST_GpioSysfs* pstGpioItem = GetGpioItemByPin(pinName);
	if (!pstGpioItem)
	{// pin not support
		return RES_IO_NOT_SUPPORT;
	}

    snprintf(path, sizeof(path), "%s/%s/%s", SYSFS_GPIO_PATH, pstGpioItem->gpioName, "direction");
    snprintf(attr, sizeof(attr), "%s", (GPIO_DIR_OUTPUT == dir) ? "out": "in");
    _DEBUG("path:%s, attr:%s\n", path, attr);

    return GpioSysfs_WriteAttr(path, attr);
}

//--------------------------------------------------------------------------------------------------
/**
 * read GPIO Direction INPUT or OUTPUT mode.
 *
 *
 * "direction" ... reads as either "in" or "out". This value may
 *        normally be written. Writing as "out" defaults to
 *        initializing the value as low. To ensure glitch free
 *        operation, values "low" and "high" may be written to
 *        configure the GPIO as an output with that initial value
 *
 * @return
 *      Gpio direction mode, 0 (outpu) and 1 (input).
 */
//--------------------------------------------------------------------------------------------------
int GpioSysfs_ReadDirection(Enum_PinName pinName)
{
    char path[64];
    char result[17];
    int dir_value;
	ST_GpioSysfs* pstGpioItem = GetGpioItemByPin(pinName);
	if (!pstGpioItem)
	{// pin not support
		return RES_IO_NOT_SUPPORT;
	}

    snprintf(path, sizeof(path), "%s/%s/%s", SYSFS_GPIO_PATH, pstGpioItem->gpioName, "direction");
	memset(result, 0x0, sizeof(result));
    GpioSysfs_ReadAttr(path, sizeof(result), result);
    dir_value = atoi(result);
    _DEBUG("result:%s Dir:%s\n", result, (dir_value==1) ? "input": "output");

    return dir_value;
}

//--------------------------------------------------------------------------------------------------
/**
 * write value to GPIO output, low or high
 */
//--------------------------------------------------------------------------------------------------
int GpioSysfs_WriteValue(
    Enum_PinName pinName,           // [IN] pin name
    Enum_GPIO_Level level           // [IN] High or low
)
{
    char path[64];
    char attr[16];
	ST_GpioSysfs* pstGpioItem = GetGpioItemByPin(pinName);
	if (!pstGpioItem)
	{// pin not support
		return RES_IO_NOT_SUPPORT;
	}

    snprintf(path, sizeof(path), "%s/%s/%s", SYSFS_GPIO_PATH, pstGpioItem->gpioName, "value");
    snprintf(attr, sizeof(attr), "%d", level);
    _DEBUG("path:%s, attr:%s\n", path, attr);

    return GpioSysfs_WriteAttr(path, attr);
}

//--------------------------------------------------------------------------------------------------
/**
 * read level value from GPIO input mode.
 *
 *
 * "value" ... reads as either 0 (low) or 1 (high). If the GPIO
 *        is configured as an output, this value may be written;
 *        any nonzero value is treated as high.
 *
 * @return
 *      An active type, the status of pin: HIGH or LOW
 */
//--------------------------------------------------------------------------------------------------
int GpioSysfs_ReadValue(Enum_PinName pinName)
{
    char path[64];
    char result[17];
    int level_value;
	ST_GpioSysfs* pstGpioItem = GetGpioItemByPin(pinName);
	if (!pstGpioItem)
	{// pin not support
		return RES_IO_NOT_SUPPORT;
	}

    snprintf(path, sizeof(path), "%s/%s/%s", SYSFS_GPIO_PATH, pstGpioItem->gpioName, "value");
	memset(result, 0x0, sizeof(result));
    GpioSysfs_ReadAttr(path, sizeof(result), result);
    level_value = atoi(result);
    _DEBUG("result:%s, value:%s\n", result, (level_value==1) ? "high": "low");

    return level_value;
}

//--------------------------------------------------------------------------------------------------
/**

 * Open the device file of level value.
 *
 *
 * "pinName" ... pin name.

 *
 * @return
 *      A handle for file descriptor.

 */
//--------------------------------------------------------------------------------------------------
int GpioSysfs_OpenValue(Enum_PinName pinName)
{
    char monFile[128];
    int valueFd = 0;

    _DEBUG("\n--- %s ---\n", __func__);

	ST_GpioSysfs* pstGpioItem = GetGpioItemByPin(pinName);
	if (!pstGpioItem)
	{// pin not support
		return RES_IO_NOT_SUPPORT;
	}

    // Start monitoring the fd for the correct GPIO
    snprintf(monFile, sizeof(monFile), "%s/%s/%s", SYSFS_GPIO_PATH, pstGpioItem->gpioName, "value");

    do
    {
        valueFd = open(monFile, O_RDONLY);
    } while ((0 == valueFd) && (EINTR == errno));
    if (0 == valueFd)
    {
        _DEBUG("Fail to open GPIO file for monitoring\n");
        return -2;
    }
    _DEBUG("\nopen(monFile..)=%d\n", valueFd);
    return valueFd;    
}

//--------------------------------------------------------------------------------------------------
/**
 * Rising or Falling of Edge sensitivity
 *
 * "edge" ... reads as either "none", "rising", "falling", or
 * "both". Write these strings to select the signal edge(s)
 * that will make poll(2) on the "value" file return.

 * This file exists only if the pin can be configured as an
 * interrupt generating input pin.

 */
//--------------------------------------------------------------------------------------------------
int GpioSysfs_SetEdgeSense(
    Enum_PinName  pinName,                  // [IN] gpio name
    Enum_GpioEdgeSenseMode edgeSense        // [IN] The mode of GPIO Edge Sensivity.
)
{
    char path[64];
    char attr[11];

	ST_GpioSysfs* pstGpioItem = GetGpioItemByPin(pinName);
	if (!pstGpioItem)
	{// pin not support
		return RES_IO_NOT_SUPPORT;
	}

    snprintf(path, sizeof(path), "%s/%s/%s", SYSFS_GPIO_PATH, pstGpioItem->gpioName, "edge");
    switch (edgeSense)
    {
        case GPIO_EDGE_SENSE_RISING:
            snprintf(attr, 10, "rising");
            break;
        case GPIO_EDGE_SENSE_FALLING:
            snprintf(attr, 10, "falling");
            break;
        case GPIO_EDGE_SENSE_BOTH:
            snprintf(attr, 10, "both");
            break;
        default:
            snprintf(attr, 10, "none");
            break;
    }
    _DEBUG("path:%s, attr:%s", path, attr);

    return GpioSysfs_WriteAttr(path, attr);
}

//--------------------------------------------------------------------------------------------------
/**
 * setup GPIO polarity.
 */
//--------------------------------------------------------------------------------------------------
int GpioSysfs_WritePolarity(
    Enum_PinName  pinName,           // [IN] gpio name
    Enum_GPIO_Polarity_Level level   // [IN] Active-high or active-low
)
{
    char path[64];
    char attr[16];
	ST_GpioSysfs* pstGpioItem = GetGpioItemByPin(pinName);
	if (!pstGpioItem)
	{// pin not support
		return RES_IO_NOT_SUPPORT;
	}

    snprintf(path, sizeof(path), "%s/%s/%s", SYSFS_GPIO_PATH, pstGpioItem->gpioName, "active_low");
    snprintf(attr, sizeof(attr), "%d", level);
    _DEBUG("path:%s, attr:%s\n", path, attr);

    return GpioSysfs_WriteAttr(path, attr);
}

//--------------------------------------------------------------------------------------------------
/**
 * Get the current value the pin polarity.
 *
 * @return The current configured value
 */
//--------------------------------------------------------------------------------------------------
int GpioSysfs_ReadPolarity(Enum_PinName  pinName)
{
    char path[64];
    char result[17];
    Enum_GpioEdgeSenseMode type;

	ST_GpioSysfs* pstGpioItem = GetGpioItemByPin(pinName);
	if (!pstGpioItem)
	{// pin not support
		return RES_IO_NOT_SUPPORT;
	}

    snprintf(path, sizeof(path), "%s/%s/%s", SYSFS_GPIO_PATH, pstGpioItem->gpioName, "active_low");
    GpioSysfs_ReadAttr(path, sizeof(result), result);
    type = atoi(result);
    _DEBUG("result:%s", result);
    return type;
}
//--------------------------------------------------------------------------------------------------
/* gpio pull 
* val 0-----gpio no pull
*     1-----gpio pull down
*     3-----gpio pull up
*/    
//--------------------------------------------------------------------------------------------------

int Ql_Write_Register(unsigned char gpio,unsigned char val)
{
    uintptr_t addr = 0x1000000+0x1000*gpio;
    uintptr_t endaddr = addr +3;
    uint32_t reg_val,temp;

    int fd = open("/dev/mem", O_RDWR | O_SYNC);
    if(fd < 0) {
        //fprintf(stderr,"cannot open /dev/mem\n");
	printf("cannot open /dev/mem\n");
        return -1;
    }
	
    off64_t mmap_start = addr & ~(PAGE_SIZE - 1);
    size_t mmap_size = endaddr - mmap_start + 1;
    mmap_size = (mmap_size + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1);

    void* page = mmap(0, mmap_size, PROT_READ | PROT_WRITE,
                        MAP_SHARED, fd, mmap_start);

    if(page == MAP_FAILED){
      //  fprintf(stderr,"cannot mmap region\n");
	printf("cannot mmap region\n");
        return -1;
    }

    uint32_t* x = (uint32_t*) (((uintptr_t) page) + (addr & 4095));
	reg_val =*x;
	temp =reg_val >>2;
	*x =(temp <<2) +val;
	printf("width 4 ---%08"PRIxPTR": %08x\n", addr, *x);
    return 1;
}

int Ql_Write_Register_all(unsigned char gpio,unsigned char val)
{
    uintptr_t addr = 0x1000000+0x1000*gpio;
    uintptr_t endaddr = addr +3;

    int fd = open("/dev/mem", O_RDWR | O_SYNC);
    if(fd < 0) {
        //fprintf(stderr,"cannot open /dev/mem\n");
	printf("cannot open /dev/mem\n");
        return -1;
    }
	
    off64_t mmap_start = addr & ~(PAGE_SIZE - 1);
    size_t mmap_size = endaddr - mmap_start + 1;
    mmap_size = (mmap_size + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1);

    void* page = mmap(0, mmap_size, PROT_READ | PROT_WRITE,
                        MAP_SHARED, fd, mmap_start);

    if(page == MAP_FAILED){
      //  fprintf(stderr,"cannot mmap region\n");
	printf("cannot mmap region\n");
        return -1;
    }

    uint32_t* x = (uint32_t*) (((uintptr_t) page) + (addr & 4095));
	*x =val;
	printf("width 4 ---%08"PRIxPTR": %08x\n", addr, *x);
    return 1;
}



//--------------------------------------------------------------------------------------------------
/**
 * Converts a set of poll(2) event flags into a set of epoll(7) event flags.
 *
 * @return Bit map containing epoll(7) events flags.
 */
//--------------------------------------------------------------------------------------------------
static uint32_t PollToEPoll(
    short  pollFlags   // [in] Bit map containing poll(2) event flags.
)
//--------------------------------------------------------------------------------------------------
{
    uint32_t epollFlags = 0;

    if (pollFlags & POLLIN)
    {
        epollFlags |= EPOLLIN;
    }

    if (pollFlags & POLLPRI)
    {
        epollFlags |= EPOLLPRI;
    }

    if (pollFlags & POLLOUT)
    {
        epollFlags |= EPOLLOUT;
    }

    if (pollFlags & POLLHUP)
    {
        epollFlags |= EPOLLHUP;
    }

    if (pollFlags & POLLRDHUP)
    {
        epollFlags |= EPOLLRDHUP;
    }

    if (pollFlags & POLLERR)
    {
        epollFlags |= EPOLLERR;
    }

    return epollFlags;
}

