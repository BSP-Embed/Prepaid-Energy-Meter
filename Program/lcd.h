#ifndef LCD_H
#define LCD_H

//INCLUDES
#include "includes.h"

//DEFINE CONSTANT
#define LCD_NSCRL	2
#define LCD_DOT		'.'
#define LCD_SPC		' '


//DEFINES PORT/PINS
#define LCD_EN_PIN	P2_3
#define LCD_RS_PIN	P2_2
#define LCD_DATA_PORT	P2


//DEFINE MACROS 
#define lcdclr() lcdwc(0x01)	  /* Clear display LCD */
#define lcdr1()  lcdwc(0x80)	  /* Begin at Line 1 */
#define lcdr2()  lcdwc(0xC0)   /* Begin at Line 2 */

//DEFINE PROTOTYPES
static	void lcden(void);
		void lcdwc(INT8U command);
		void lcdwd(INT8U ascii);
		void lcdws(INT8U *s);
static 	void lcdwb(INT8U  LCDData);
		void lcdinit(void);
		void lcddinit(void);
static 	void lcdrs(void);
	 	void lcdclrr(INT8U rowno);

#endif

