#ifndef _BUZ_H
#define _BUZ_H

//INCLUDES
#include "includes.h"

// define constants
#define OFF_TIME	1000

// DEFINE PORTS
#define BUZ_PIN			P0_1
#define BUZ_PORT		P0


//DEFINE MACRO
#define	buzon()			BUZ_PIN = 0
#define	buzoff()		BUZ_PIN = 1

//FUNCTION PROTOTYPES
void beep		(INT8U nbeep, INT8U ontime);
void buzinit	(void);
#endif

