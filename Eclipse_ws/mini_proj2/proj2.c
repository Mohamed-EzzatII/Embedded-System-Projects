#include<avr/io.h>
#include<avr/interrupt.h>
#include<avr/delay.h>

void pins_configration(void);
void interrupts_enable(void);
void timer1_configuration(void);

unsigned char sec=0;//seconds
unsigned char min=0;//minutes
unsigned char hr=0;//hours

int main(void)
{
	pins_configration();
	interrupts_enable();
	timer1_configuration();

	PORTA=0b00111111;//enable the 6 seven segments
	while(1)
	{
		//we completed a minute
		if(sec==60)
		{
			sec=0;
			min++;
		}

		/*for the first 7segment from the right*/
		PORTA=0b00000001;
		PORTC=sec%10;//get the first digit from sec variable
		_delay_ms(3);

		/*for the second 7segment from the right*/
		PORTA=0b00000010;
		PORTC=(sec/10)%10;//get the second digit from sec variable
		_delay_ms(3);

		//we completed an hour
		if(min==60)
		{
			min=0;
			hr++;
		}

		/*for the third 7segment from the right*/
		PORTA=0b00000100;
		PORTC=min%10;//get the first digit from min variable
		_delay_ms(3);

		/*for the fourth 7segment from the right*/
		PORTA=0b000001000;
		PORTC=(min/10)%10;//get the second digit from min variable
		_delay_ms(3);

		//we completed day
		if(hr==24)
		{
			hr=0;
		}

		/*for the fifth 7segment from the right*/
		PORTA=0b00010000;
		PORTC=hr%10;;//get the first digit from hr variable
		_delay_ms(3);

		/*for the sixth 7segment from the right*/
		PORTA=0b00100000;
		PORTC=(hr/10)%10;;//get the second digit from hr variable
		_delay_ms(3);

	}

}

void pins_configration(void)
{
	//Enable/disable for 6 7-segments
	DDRA=0b00111111;
	PORTA=0b00111111;

	//The output to 7-segment
	DDRC=0X0F;
	PORTC=0;

	//INT(O~2) set as input
	DDRD&=~(1<<PD2);
	DDRD&=~(1<<PD3);
	DDRB&=~(1<<PB2);

	//configure PD2 - PB2 as internal pull up
	PORTD|=(1<<PD2);
	PORTB|=(1<<PB2);
}

void interrupts_enable(void)
{
	//INT0 falling edge
	MCUCR|=(1<<ISC01);
	GICR|=(1<<INT0);

	//INT1 rising edge
	MCUCR|=(1<<ISC11)|(1<<ISC10);
	GICR|=(1<<INT1);

	//INT2 falling edge
	MCUCSR&=~(1<<ISC2);
	GICR|=(1<<INT2);

	//I-bit enable
	SREG|=(1<<7);
}

void timer1_configuration(void)
{
	TCNT1 = 0;//initial counter=0
	ICR1 = 976;//the TCNT1 max is 976 and then raise a flag

	TIMSK |= (1<<OCIE1A);//Timer1A is enabled

	TCCR1A=(1<<FOC1A);//Non-PWM mode
	//COM1A1 & COM1A0 =00
	TCCR1A|=(1<<WGM13)|(1<<WGM12);//compare mode 12 in table 47

	TCCR1B=(1<<WGM13)|(1<<WGM12);//compare mode 12 in table 47
	TCCR1B|=(1<<CS12) | (1<<CS10);//N=1024
	//TCCR1B = (1<<WGM12) | (1<<CS12) | (1<<CS10);
}

// ISR of timer1
ISR(TIMER1_COMPA_vect)
{
	//increment the sec with every IRQ
	sec++;
}

// reset the timer
ISR(INT0_vect)
{
	sec=min=hr=0;
	TCNT1=0;
}

//stop the timer
ISR(INT1_vect)
{
	TIMSK&=~(1<<OCIE1A);
}

//start the timer
ISR(INT2_vect)
{
	TIMSK|=(1<<OCIE1A);
}
