/* main.c:	application program for overloaded vehicle detection */
#include "main.h"
/* comment below line for release */
#define TESTING

#ifdef TESTING
	code const INT8U PhNum1[] = "7975000000";
#else
	code const INT8U PhNum1[] = "9900000000";
#endif

code const INT8U msg1[]		 	= "Your account has been successfully recharged.";
code const INT8U msg2[] 		= "Low balance, Kindly recharge soon.";
code const INT8U msg3[] 		= "Power removed as your account balance is zero.";
code const INT8U msg4[] 		= "Power theft from your meter. Please contact KPTCL.";

code const INT8U rch1[] 		= "RCH10";
code const INT8U rch2[] 		= "RCH35";
code const INT8U rch3[] 		= "RCH90";
code const INT8U powresm[] 	= "12345";

INT8U powunits,init_read;
extern INT8U lcdptr;

bit low_ref_flag 			= 0;
bit zero_units_flag		= 0;
bit theftflag 				= 0;
bit buztheftflag 			= 0;
bit dispflag 					= 1;

void main(void)
{								
	INT8U i, x, oldread, pmsg[6], s[5];
	init();
	
	pmsg[5] = '\0';
	oldread = 0;
	
	while (TRUE)	{
		if (powunits < LOW_REF_UNITS && low_ref_flag == 0) {
				buzon();
				low_ref_flag = 1;
				GSMsndmsg(PhNum1, msg2);
				disptitle();
				lcdclrr(1);
				lcdws("  Low Balance");
				buzoff();
				dlyms(2000);
		}
		if (powunits == 0 && zero_units_flag == 0) {
				zero_units_flag = 1;
				beep(5,100);
				loadoff();
				GSMsndmsg(PhNum1, msg3);
				disptitle();
				lcdclrr(1);
				lcdws("  Zero Balance");
				dlyms(2000);
		}
		if (SBUF == '<') {	
			i = 0;
			RI = 0;
			while ((pmsg[i++] = getchar()) != '>');
			pmsg[5] = '\0';
			lcdclr();
			lcdws("Message Received");
			beep(2, 100);
			lcdclrr(1);
			lcdr2();
			switch (x = verpass(pmsg)) {
				case 1: case 2: case 3:
						powunits += 10 * x;
						init_read = powunits;
						TL0 = 0;
					//	saveunits();
						lcdws("Recharge Success.");
						GSMsndmsg(PhNum1, msg1);
						zero_units_flag = 0;
						low_ref_flag = 0;
						if (buztheftflag == 0)
									loadon();
						break;
				case 4:
						lcdws("Power Resumed");
						beep(2, 100);
						loadon();
						buztheftflag = 0;
						TR0 = 1;						/* Enable Counter */
						break;
			default:
						lcdws("Not Verified");
						break;
			} 
			RI = 0;
			dlyms(1000);
			disptitle();
		}
		if (RI == 1) RI = 0;
		
		if (TL0 != oldread) {
			powunits = init_read - TL0;
//			saveunits();
			oldread = TL0;
		}
		if (theftflag == 1) {
			buzon();
			theftflag = 0;
			buztheftflag = 1;
			GSMsndmsg(PhNum1, msg4);
			dlyms(1000);
			buzoff();
			lcdclr();
			lcdws(" Theft Occured");
			lcdr2();
			lcdws(" System Locked!");
			loadoff();
			TR0 = 0;			/* Disable Counter */
		}
		if (dispflag == 1) {
			dispflag = 0;
			lcdclrr(1);
			lcdws("Pre:   Bal:");
			lcdptr = 0xc4;
			itoa(TL0,s);
			lcdws(s);
			itoa(powunits,s);
			lcdptr = 0xcb;
			lcdws(s);
		}
		
	}
}
static INT8U verpass (INT8U pmsg[])
{
	if (strcmp(pmsg,rch1) == 0)
		return 1;
	else if(strcmp(pmsg,rch2) == 0)
		return 2; 
	else if (strcmp(pmsg,rch3) == 0)
		return 3;
	else if (strcmp(pmsg, powresm) == 0)
		return 4;
	else
		return 0;
}
static void disptitle(void)
{
	lcdclr();
	lcdws("PREPAID EN METER");
}
static void init(void)
{
	beep(2,75);
	I2Cinit();
	tmrinit();
	lcdinit();			  
	uartinit();
	chkEEPROM();
	GSMinit();			 
	disptitle();
	starttmr();
	loadon();
}
static void tmrinit(void)
{
	TMOD = 0x15;		/* TMR0 as counter */
	TH1 = 0x4B;
	TL1 = 0xFD;
	TL0 = TH0 = 0;
	TR0 = 1;
	EA = ET1 = 1;
}
/* timer overflows at every 50ms */
static void tmr1ISR() interrupt 3 using 1
{
	static INT8U i,j, mtrread;
	static bit monitflag;
	static INT16U tmrtick;
	
	TH1 = 0x4B;
	TL1 = 0xFD;
	if (++i >= 2) {
		i = 0;
		if (++j >= 50) {
			j = 0;
			dispflag = 1;
		}
		switch(j) {
			case 0: case 2: ledon(); if (buztheftflag == 1) buzon(); break;
			case 1: case 3: ledoff(); if (buztheftflag == 1) buzoff(); break;
		}
		if (monitflag == 0) {
			mtrread = TL0;
			tmrtick = 0;
			monitflag = 1;
		} 
		if (LOAD_PIN == HIGH && ++tmrtick >= 300) {
			if (mtrread == TL0)
					theftflag = 1;
			monitflag = 0;
		}
	}
} 
static void chkEEPROM(void)
{
	
	INT8U MagicNumber;
	#ifdef 	DISP_EEPROM_MSG           												  // starting address of eeprom to check MAGIC NUMBER
		lcdclr();
		lcdws("Init EEPROM: ");
	#endif

	I2CRead(DEV_ADDR,MAGIC_ADDR,1,&MagicNumber);  // read magic number

	if (CheckEEPROM()) {									// if error in eeprom then system operates on default settings 
		#ifdef 	DISP_EEPROM_MSG 
			lcdws("ERR");
			dlyms(500);
		#endif
	}	else {
		if(MagicNumber == MAGIC_NO ) {  // PRORAMMED EEPROM - SO RETRIVE VALUES FROM EEPROM 
			#ifdef 	DISP_EEPROM_MSG 
				lcdws("OK");
				dlyms(500);
			#endif
			/*RETRIVE Random number */
			I2CRead(DEV_ADDR,POW_UNITS_ADDR,1,&powunits);  // read magic number
			init_read = powunits;
		} else { 
			#ifdef 	DISP_EEPROM_MSG 		// New EEPROM Detected so loadd values
				lcdws("NEW");
				dlyms(500);
			#endif
			MagicNumber = MAGIC_NO;			
			powunits = init_read = DEF_POW_UNITS;
	    I2CWrite(DEV_ADDR,MAGIC_ADDR,1,&MagicNumber);		// now save MAGIC NUMBER to EEPROM 
			I2CWrite(DEV_ADDR,POW_UNITS_ADDR,1,&powunits);
		} /* END OF CHECKING NEW EEPROM AND SAVING DATA */
 }
}

	
