//DEFINE MAIN HEADER FILE
#ifndef MAIN_H
#define MAIN_H

#include <includes.h>


//DEFINE PORTS/PINS
#define LOAD_RLY_PIN		P0_2
#define LOAD_PIN				P3_3


//DEFINE CONSTANT
#define DISP_EEPROM_MSG
#define MAGIC_NO				68
#define DEV_ADDR				0xA0
#define MAGIC_ADDR			0x00
#define POW_UNITS_ADDR	0x01

#define	DEF_POW_UNITS		5
#define LOW_REF_UNITS		5


//DEFINE MACROS

#define loadon()	 	LOAD_RLY_PIN = LOW
#define loadoff() 	LOAD_RLY_PIN = HIGH


#define starttmr()	TR1 = HIGH
#define stoptmr()		TR1 = LOW
#define saveunits()	I2CWrite(DEV_ADDR, \
														 POW_UNITS_ADDR, \
														 1, \
														 &powunits)


//FUNCTION PROTOTYPES
static 	void 	init				(void);
static 	void 	disptitle		(void);
static 	void	tmrinit			(void);
static 	INT8U	verpass 		(INT8U pmsg[]);
static 	void 	chkEEPROM		(void);

#endif