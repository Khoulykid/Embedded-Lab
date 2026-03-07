/* This program blinks the red LED on the
 * TI Tiva LaunchPad. The connections are:
 * PF1 - red LED
 * PF2 - blue LED
 * PF3 - green LED
 * They are high active (a '1' turns on the LED).
 */
#include "TM4C123GH6PM.h"

void delayMs(int n, int *mode, int *flag);
int main(void) {
	int color = 0x04;
	int mode = 0;
	int flag = 0;
	int state = color;
  // enable clock to GPIOF at clock gating control register
  SYSCTL -> RCGCGPIO |= 0x20;
  // enable the GPIO pins for the LED (PF3, 2 1) as output
  GPIOF -> LOCK |= 0x4C4F434B;
	GPIOF -> CR |= 0X11; // 00010001
	GPIOF -> DIR |= 0x0e;
  // enable the GPIO pins for digital function
  GPIOF -> DEN |= 0x1f; // 11111
	
	GPIOF -> PUR |=  0x11; // 00010001
	// GPIOF -> LOCK &= 0xfe; 
	
	

	
	
	while (1) {

		
			
		
			if (mode == 0)
				color = 0x04;
			else if (mode == 1)
				color = 0x08;
			else if (mode == 2)
				color = 0x02;
			else if (mode == 3)
			{
				if (color == 0x04) 
					color = 0x08;
				else if (color == 0x08) 
					color = 0x02;
				else if (color == 0x02)
					color = 0x04;
			}
			GPIOF -> DATA |= color; // turn on red LED
			delayMs(1000, &mode, &flag);
		
			GPIOF -> DATA &= 0x11;
			delayMs(1000, &mode, &flag);
			
		}
    
  }
	
		//if ((GPIOF -> DATA & 0x10) == 0)
		//{
			//if ((GPIOF -> DATA & 0x0e) != 0x00)
			//GPIOF -> DATA |= color; // turn off red LED
			//else 
			//	GPIOF -> DATA &= 0x11;
		//}

// delay in milliseconds (16 MHz CPU clock)
void delayMs(int n, int *mode, int *flag) {
	int val = (n * 16000) - 1;
	if (*flag == 0)
		val = (n * 16000) - 1;
	else if (*flag == 1)
		val = (n * 8000) - 1;
	else
		val = (n * 4000) - 1;
	
  SysTick -> LOAD = val;
  SysTick -> CTRL = 0x5; /*Enable the timer and choose sysclk */
  while ((SysTick -> CTRL & 0x10000) == 0) /*wait until the Count flag is set */ {
		if ((GPIOF -> DATA & 1) == 0)
		{
			while((GPIOF -> DATA & 1) == 0)
			{
			}
			
			if (*mode == 0) 
				*mode = 1;
			else if (*mode == 1) 
				*mode = 2;
			else if (*mode == 2)
				*mode = 3;
			else *mode = 0;
			

	}
		
		if ( (GPIOF -> DATA & 0x10) == 0)
		{
			while ((GPIOF -> DATA & 0x10) == 0){}
			if (*flag == 0)
			{
				*flag = 1;
			}
			else if (*flag == 1)
			{				
				*flag = 2;
			}
			else
			{
				*flag = 0;
			}
		}
	}
  SysTick -> CTRL = 0; /*Stop the timer (Enable = 0) */
}
