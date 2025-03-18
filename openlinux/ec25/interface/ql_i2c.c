
#include "ql_oe.h"

#define DEBUG_EN  0
#if DEBUG_EN
#define _DEBUG(fmtString, ...)     printf(fmtString, ##__VA_ARGS__)
#else
#define _DEBUG(fmtString, ...)
#endif


#define I2C_DEV  "/dev/i2c-2"

int Ql_I2C_Init(unsigned char slaveAddr)
{
    int fd_i2c = open(I2C_DEV, O_RDWR);
    _DEBUG("< open(I2C_DEV, O_RDWR)=%d >\n", fd_i2c);
    if (fd_i2c < 0)  
    {
        _DEBUG("< Fail to open i2c >\n");
        return -1;  
    }  

    /*
     * Write slave address
     */
    int iRet = ioctl(fd_i2c, I2C_SLAVE, slaveAddr);
    _DEBUG("< ioctl(fd_i2c, I2C_SLAVE, I2C_SLAVE_ADDR)=%d >\n", iRet);
    if (iRet < 0)  
    {  
        _DEBUG("< ioctl error >\n");  
        return -2;  
    }

    return fd_i2c;
}

int Ql_I2C_Read(int fd, unsigned short slaveAddr, unsigned char ofstAddr,  unsigned char* ptrBuff, unsigned short length)
{
	int iRet = 0;
	
	struct i2c_msg i2c_msgs[] = {
	    [0] = {
		    .addr  = slaveAddr,
		    .flags = 0, // write
		    .buf   = &ofstAddr,
		    .len   = 1,
	    },
	    [1] = {
		    .addr  = slaveAddr,
		    .flags = I2C_M_RD,
		    .buf   = ptrBuff,
		    .len   = length, 
	    },
	};

	struct i2c_rdwr_ioctl_data msgset = {
		.msgs = i2c_msgs,
		.nmsgs = 2,
	};

	iRet = ioctl(fd, I2C_RDWR, &msgset);
 	if (iRet < 0)
	{
		_DEBUG("%s, read failed rc : %d \n", __FUNCTION__, iRet);
	}
	_DEBUG("%s, line: %d, addr: 0x%x, value[0]: %x , rc: %d \n", __FUNCTION__, __LINE__, i2c_msgs[0].addr, ptrBuff[0], iRet);

	return iRet;
}


int Ql_I2C_Write(int fd, unsigned short slaveAddr, unsigned char ofstAddr,  unsigned char* ptrData, unsigned short length)
{
	int iRet = 0;
	
	struct i2c_msg i2c_msgs = {
	    .addr  = slaveAddr,
	    .flags = 0, // write
	    .buf   = NULL,
	    .len   = length,
	};
	i2c_msgs.buf = (unsigned char*)malloc(length + 1);
	i2c_msgs.buf[0] = ofstAddr; // target address
	memcpy(&i2c_msgs.buf[1], ptrData, length);

	struct i2c_rdwr_ioctl_data msgset = {
		.msgs  = &i2c_msgs,
		.nmsgs = 1,
	};

	iRet = ioctl(fd, I2C_RDWR, &msgset);
	free(i2c_msgs.buf);
 	if (iRet < 0)
	{
		_DEBUG("%s, write failed, iRet=%d \n", __FUNCTION__, iRet);
	}

	return iRet;
}
