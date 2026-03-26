#include <stdint.h>

#include "tm4c123gh6pm.h"
char potato[7];
int level;
float freq;
float uptime;
float actual;
int flag;
void UART0Tx(char c);
void delayMs(int n);
int main(void) {
	level = 0;
	flag = 0;
	freq = 1;
	uptime = 0.5;
	actual = uptime;


  SYSCTL -> RCGCUART |= 5; /* provide clock to UART2 */
  SYSCTL -> RCGCGPIO |= 11; /* enable clock to PORTD and PORTB*/
  SysTick -> LOAD = 16000000 - 1; /* reload with number of clocks per second */
  SysTick -> CTRL = 7; /* enable SysTick interrupt, use system clock */
  /* UART0 initialization */
  UART2 -> CTL = 0; /* disable UART2 */
  UART2 -> CC = 0; /* use system clock */
  UART2 -> IBRD = 104; /* 16MHz/(16*9600 baud rate) = 104.1666666666 */
  UART2 -> FBRD = 11; /* fraction part = 0.1666666*64+0.5 = 11.1666666 */

  UART2 -> LCRH = 0x60; /* 8-bit, no parity, 1-stop bit, no FIFO */
  UART2 -> CTL = 0x301; /* enable UART2(bit0), TXE(bit8), RXE(bit9) */
  UART2 -> IM |= 0x0010; /* Enable RX interrupts for UART2 */
  /* UART2 TX0 and RX0 use PB1 and PB0. Set them up. */
  //GPIOB -> DEN = 0x03; /* Make PB0 and PB1 as digital */
  //GPIOB -> AFSEL = 0x03; /* Use PB0,PB1 alternate function */
  //GPIOB -> PCTL = 0x11000000; /* configure PD6 and PD7 for UART2 */
	
		/* UART0 initialization */
	UART0->CTL = 0; /* disable UART0 */
	UART0->IBRD = 104; /* 16MHz/(16*9600 baud rate) = 104.1666666666 */
	UART0->FBRD = 11; /* fraction part= 0.1666666*64+0.5 = 11.1666666 */
	UART0->CC = 0; /* use system clock */
	UART0->LCRH = 0x60; /* 8-bit, no parity, 1-stop bit, no FIFO */
	UART0->IM |= 0x0010; /* enable RX interrupt */
	UART0->CTL = 0x301; /* enable UART0, TXE, RXE */
/* UART0 TX0 and RX0 use PA1 and PA0. Set them up. */
	GPIOA->DEN = 0x03; /* Make PA0 and PA1 as digital */
	GPIOA->AFSEL = 0x03; /* Use PA0,PA1 alternate function */
	GPIOA->PCTL = 0x11; /* configure PA0 and PA1 for UART */
  /* Set up PortD for uhh.. the signal thingie */
	GPIOD->LOCK = 0x4C4F434B; /* unlock commit register */
	GPIOD->CR |= 0xFF; /* make PORTD7 configurable */
	GPIOD->LOCK = 0; /* lock commit register */
  GPIOD -> DIR = 0x01;
  GPIOD -> DEN = 0xC1;
	GPIOD->AFSEL|= 0xC0;
	GPIOD->PCTL  = 0x11000000;

  NVIC -> IP[33] = 3 << 5; /* set interrupt no 5 priority to 3 */
  NVIC -> ISER[1] |= 0x00000002; /* enable IRQ5 for UART3 */

  //delayMs(25); /* wait for output line to stabilize */
  __enable_irq();

  for (;;) {

  }
}
/* UART0 Transmit */
/* This function waits until the transmit buffer is available then writes */
/* the character in it. It does not wait for transmission to complete */
void UART2_Handler(void) {
  volatile int readback;
  char c;
  if (UART2 -> MIS & 0x0010) /* if a receive interrupt has occurred */ {
		c = UART2->DR;
		UART0Tx(c);
		if ((c == 'f' || c == 'd') && !level)
		{
			potato[0] = c;
			level = 1;
		}
		else if (level == 1)
		{
			if ((c == 'r' && potato[0] == 'f') || (c == 'u' && potato[0] == 'd'))
			{
				potato[1] = c;
				level = 2;
			}
			else level = 0;
		}
		else if (level == 2)
		{
			if ((c == 't' && potato[1] == 'u') || (c == 'e' && potato[1] == 'r'))
			{
				potato[2] = c;
				level = 3;
			}
			else level = 0;
		}
			else if (level == 3)
		{
			if ((c == 'y' && potato[2] == 't') || (c == 'q' && potato[2] == 'e'))
			{
				potato[3] = c;
				level = 4;
			}
			else level = 0;
		}
		else if (level == 4)
		{
			if ((c == '-' || c == '+'))
			{
				potato[4] = c;
				level = 5;
			}
			else level = 0;
		}
		else if (level == 5)
		{
			if ( c == '1' && potato[0] == 'd')
			{
				potato[5] = c;
				level = 6;
			}
			else if (c == '1' && potato[0] == 'f')
			{
				if (potato[4] == '+')
					freq = (float) freq + 0.1;
				else if (freq > 0.2)
					freq = (float) freq - 0.1;
				else
					freq = 1.0;
				level = 0;
			}
			else if (c == '2' && potato[0] == 'f')
			{
				if (potato[4] == '+')
					freq = (float) freq + 0.2;
				else if (freq > 0.3)
					freq = (float) freq - 0.2;
				else
					freq = 1;
				level = 0;
			}
			
			else if (c == '5' && potato[0] == 'd')
			{
				if (potato[4] == '+' && uptime < 0.95)
					uptime = uptime + 0.05;
				else if (potato[4] == '+')
					uptime = 0.5;
				else if (uptime > 0.05)
					uptime = uptime - 0.05;
				else
					uptime = 0.5;
				flag = 1;
				level = 0;
			}
			else level =0;
			}
		else if (level == 6 && c == '0')
		{
			if (potato[4] == '+' && uptime < 0.9)
					uptime = uptime + 0.1;
				else if (potato[4] == '+')
					uptime = 0.5;
				else if (uptime > 0.1)
					uptime = uptime - 0.1;
				else
					uptime = 0.5;
				flag = 1;
				level = 0;
			}
			else level =0;
		
    UART2 -> ICR = 0x0010; /* clear Rx interrupt flag */
    readback = UART2 -> ICR; /* a read to force clearing of interrupt flag */
  } else {
    /* should not get here. But if it does, */
    UART2 -> ICR = UART2 -> MIS; /* clear all interrupt flags */
    readback = UART2 -> ICR; /* a read to force clearing of interrupt flag */
  }

}

void SysTick_Handler(void)
{
	if (flag)
	{
		flag = 0;
		actual = uptime;
		GPIOD->DATA = GPIOD->DATA & 0xFE;
	}

	GPIOD->DATA = GPIOD->DATA ^ 0x01;
	actual = 1 - actual;
	SysTick->LOAD =  actual*(16000000/freq) - 1;
}
void UART0Tx(	char c)
{
while((UART0->FR & 0x20)!= 0){} // Wait until Tx buffer is not full
 UART0->DR = c; // Write byte

	 return;
}

void delayMs(int n) {
  SysTick -> LOAD = 15999 * n;
  SysTick -> CTRL = 0x5; /*Enable the timer and choose sysclk */
  while ((SysTick -> CTRL & 0x10000) == 0) /*wait till Count flag is set */ {}
  SysTick -> CTRL = 0; /*Stop the timer (Enable = 0) */
}