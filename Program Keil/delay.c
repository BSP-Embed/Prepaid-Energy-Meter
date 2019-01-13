#include "delay.h"

void dlyms(unsigned int time){
	unsigned char pause;		//  Declare variable pause as unsigned char
	while ( time-- ) {			//  loop until value becomes zero	
		pause = 114;	 		//  Initialize pause to 114 (decimal value)
		while ( pause-- );	 	//  Decrement pause until it becomes zero	
	}				 			
}								 


