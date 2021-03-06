#include "lcd.h"

static void  lcden(void){
    LCD_EN_PIN = HIGH; 
    dlyms(5);
    LCD_EN_PIN = LOW; 
}
void lcdwc(INT8U LCDData){
    LCD_RS_PIN = LOW; 
    lcdwb(LCDData);
}
void lcdwd(INT8U LCDData){
    LCD_RS_PIN = HIGH; 
    lcdwb(LCDData);
}
void lcdws(INT8U *s){
	while (*s) lcdwd(*s++);
}
void lcdinit(void){
    LCD_EN_PIN		= OUTPUT_PIN; 
    LCD_RS_PIN		= OUTPUT_PIN;
    LCD_DATA_PORT	&=  0x0f;
   	lcdrs();
    lcdwc(0x28);
    lcdwc(0x0C);
    lcdwc(0x06);
    lcdwc(0x01); 
	#ifdef DISPLAY_INIT
		lcddinit();
	#endif
}
static void lcdrs(void){
	lcdwc(0x33);
	lcdwc(0x33);
	lcdwc(0x32);
}
void lcddinit(void){
	INT8U i , j , adr;
	lcdws("  INITIALIZING");
	lcdr2();
	for ( j = 0; j < LCD_NSCRL; j++ ) {
		adr = 0xc0;						// 2nd row, first coloumn
		for ( i = 0; i < 16; i++ ) {
			lcdwc(adr);			
			lcdwd(LCD_DOT);				
			if ( i < 8 ) dlyms(200+(50*i)); else dlyms(25);
			lcdwc(adr);			
			lcdwd(LCD_SPC);			
			adr++;					// increment display aadress
		}
	}
	lcdclr();
}
static void  lcdwb(INT8U  LCDData){
    LCD_DATA_PORT = (LCD_DATA_PORT & 0x0F)|(LCDData & 0xF0);
    lcden();
    LCD_DATA_PORT = (LCD_DATA_PORT & 0x0F)|(LCDData << 4);
    lcden();
    dlyms(5);
}
void lcdclrr(INT8U rowno){
	INT8U i;
	switch (rowno) {
		case 0:
				lcdr1();
				break;
		case 1:
				lcdr2();
				break;
	}
	for (i = 0; i < 16; i++)
		lcdwd(LCD_SPC);

}
