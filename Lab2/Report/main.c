#include "TM4C123GH6PM.h"
void delayMs(int n);
unsigned int col = 0;
unsigned int menu = 0;
int flag = 0;
int flag2 = 0;

unsigned int colors[] = {0x08, 0x02, 0x04};
int main(void)
{
	SYSCTL->RCGCGPIO |= 0x20; /* enable clock to PORTF */
	/* PORTF0 has special function, need to unlock to modify */
	GPIOF->LOCK = 0x4C4F434B; /* unlock commit register */
	GPIOF->CR |= 0x01; /* make PORTF0 configurable */
	GPIOF->LOCK = 0; /* lock commit register */
	/* configure PORTF for switch input and LED output */
	GPIOF->DIR &= ~0x11; /* make PORTF4,0 input for switch */
	GPIOF->DIR |= 0x0E; /* make PORTF3, 2, 1 output for LEDs */
	GPIOF->DEN |= 0x1F; /* make PORTF4-0 digital pins */
	GPIOF->PUR |= 0x11; /* enable pull up for PORTF4,0 */
	/* configure PORTF4, 0 for   falling edge trigger interrupt */
	GPIOF->IS &= ~0x11; /* make bit 4, 0 edge sensitive */
	GPIOF->IBE &= ~0x11; /* trigger is controlled by IEV */
	GPIOF->IEV &= ~0x11; /* falling edge trigger */
	GPIOF->ICR |= 0x11; /* clear any prior interrupt */
	GPIOF->IM |= 0x11; /* unmask interrupt for PF4,PF0 */
	/* enable interrupt in NVIC and set priority to 3 */
	NVIC->IP[30] = 0 << 5; /* set interrupt priority to 3 */
	NVIC->ISER[0] |= 1<<30; /* enable IRQ30 (D30 of ISER[0]) */
	__enable_irq(); /* global enable IRQs */
	/* toggle the green/violet LED colors continuously */
	SysTick->LOAD = 16000000-1; /* reload with number of clocks per second */
	SysTick->CTRL = 7; /* enable SysTick interrupt, use system clock */
	__enable_irq(); /* global enable interrupt */
	while(1)
	{
		
	}
}
/* SW1 is connected to PF4 pin, SW2 is connected to PF0. */
/* Both of them trigger PORTF interrupt */
void GPIOF_Handler(void)
{
	volatile int readback;
	if (GPIOF->MIS & 0x01){
		if ( col < 3 ){
			col += 1;
		}
		else{
			col = 0;
		}
	}
	if (GPIOF->MIS & 0x10){
		flag2 = flag2 + 1;
		if (flag2 == 0)
			SysTick->LOAD = 16000000-1;
		else if (flag2 == 1)
			SysTick->LOAD = 8000000-1;
		else if (flag2 == 2)
			SysTick->LOAD = 4000000-1;
		else
		{
			flag2 = 0;
			SysTick->LOAD = 16000000-1;
		}
			
	}
	GPIOF->ICR |= 0x11; /* clear the interrupt flag before return */
	readback = GPIOF->ICR; /* a read to force clearing of interrupt flag */
}
/* delay n milliseconds (16 MHz CPU clock) */
void delayMs(int n)
{
	SysTick->LOAD = 15999*n;
	SysTick->CTRL = 0x5; /*Enable the timer and choose sysclk */
	while((SysTick->CTRL & 0x10000) == 0) /*wait till Count flag is set */
	{ }
	SysTick->CTRL = 0; /*Stop the timer (Enable = 0) */
}

void SysTick_Handler(void)
{
		if (flag && col < 3)
			GPIOF->DATA |= colors[col]; /* toggle the red LED */
		else if (flag && col == 3) {
			GPIOF->DATA |= colors[menu];
			menu += 1;
			if (menu == 3)
				menu = 0;
		}
			
		else
			GPIOF->DATA &= ~ 0x0e;
		flag = !flag;
}
