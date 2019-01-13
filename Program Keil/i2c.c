//include i2c header file

#include"i2c.h"

//DEFINE GLOABAL VARIABLES
unsigned char bdata temp_data;

//GLOABL VARIABLES
bit i2c_busy, no_ack, bus_fault;

sbit LSB = temp_data^0;
sbit MSB = temp_data^7; 

/************************************
*       Sending Stop Condition
*************************************/
void I2Cinit(void)
{
	SDA = SCL = HIGH;
}
bit CheckEEPROM(void)
{
	if(no_ack || bus_fault)
		return 1;
	return 0;
}
void send_stop(void)
{
	SDA = LOW;
	SCL = HIGH;
	dlyms(1);
	SDA = HIGH;

	i2c_busy = 0;
}
/************************************
*      I2C Start Condition
*
*     NOTE: need to use it.
************************************/
void master(int8u slave_addr)
{
	i2c_busy = 1;
	no_ack = 0;
	bus_fault = 0;
	if(!SCL||!SDA)
		bus_fault = 1;
	else {
		SDA = LOW;
		dlyms(1);
		SCL = LOW;
		send_byte(slave_addr);
	}
}
/*************************************
*     Sending a byte on I2C Bus
*************************************/
void send_byte(int8u i2c_data)
{
	int8u i;
	temp_data = i2c_data;

	for (i = 0; i < 8; i++)	{
		SDA = MSB;
		dlyms(1);
		SCL = HIGH;
		dlyms(1);
		SCL = LOW;
		temp_data <<= 1;
	}
	SDA = HIGH;
	dlyms(1);
	SCL = HIGH;
	dlyms(1);

	if(SDA)
		no_ack = 1;
	
	SCL = LOW;
	dlyms(10);
}

/*****************************************************
*       Sending data on I2C bus
*
*   Usage:
*         send_data(0xD0,0x10, 0x20, send_buffer)
*
*         0XD0: Slave address, Must me with write bit
*         0x10: Starting address, or sub-address
*         0x20: number of bytes to send
*         send_buffer: adress of the buffer pointer
*
******************************************************/
void I2CWrite(int8u  slave_add, int8u sub_add, int8u bytes, int8u *data_in)
{
	master(slave_add);
	send_byte(sub_add);
	if(!no_ack) {
		while(bytes--) {
			send_byte(*data_in++);
			if(no_ack)
				break;
		}
	}
	send_stop();
	dlyms(1);
}

/**********************************************
*      Recieve a single byte from I2C Bus
*
*    Note: if you are going to recieve
*          a single byte then the passing
*          argument should be 1.
***********************************************/
int8u recv_byte(int8u cnt)
{
	int8u i;
	for (i = 0; i < 8; i++)	{
		SCL = HIGH;
		dlyms(1);
		LSB = SDA;
		if(i < 7)
			temp_data <<= 1;
		SCL = LOW;
		dlyms(1);
	}
	if(cnt == 1)
		SDA = HIGH;
	else
		SDA = LOW;

	SCL = HIGH;
	dlyms(1);
	SCL = LOW;
	SDA = HIGH;
	dlyms(10);

	return temp_data;
}

/*****************************************************
*       Recieving bulk data on I2C bus
*
*   Usage:
*         recv_data(0xD0,0x10, 0x20, send_buffer)
*
*         0XD0: Slave address, Must me with write bit
*         0x10: Starting address, or sub-address
*         0x20: number of bytes to recieve
*         send_buffer: adress of the recieve buffer
*                      pointer
*
******************************************************/
void I2CRead(int8u slave_add, int8u sub_add, int8u byte_cnt, int8u *recv_buf)
{
	int8u i;
	SCL = SDA = HIGH;
	master(slave_add);
	send_byte(sub_add);
	SCL = SDA = 1;
	master(slave_add+1);
	if(no_ack)
	{	
		goto exit;
	}
	for(i=0;i<byte_cnt;i++)
		recv_buf[i]=recv_byte(byte_cnt-i);
 exit:
  	send_stop();
}