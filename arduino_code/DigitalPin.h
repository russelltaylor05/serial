// wrap the functionality of the single digital pin
#include<avr/io.h>

class DigitalPin
{
public:
	DigitalPin(int pin_num)
	{
		pin_number = pin_num;
		input();
	}

	void output()
	{
		DDRD |= (1 << pin_number);
	}

	void input()
	{
		DDRD &= ~(1 << pin_number);
	}

	int read()
	{
		if( PIND & (1 << pin_number) )
			return 1;
		else
			return 0;
	}

	void write(int v)
	{
		if(v == 1)
			PORTD |= (1 << pin_number);
		else
			PORTD &= ~(1 << pin_number);
	}

private:
	int pin_number;
};
