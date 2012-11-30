#define F_CPU 16000000

#include<avr/io.h>
#include<util/delay.h>
#include <stdlib.h>
#include <stdio.h>

#include "humidity.h"
#include "DigitalPin.h"

#define BAUD_PRESCALE 103

/* Pressure Sensor */
#define PRESH	0x00	// 80
#define	PRESL	0x02	// 82
#define	TEMPH	0x04	// 84
#define	TEMPL	0x06	// 86
#define A0MSB	0x08	// 88
#define A0LSB	0x0A	// 8A
#define B1MSB	0x0C	// 8C
#define B1LSB	0x0E	// 8E
#define	B2MSB	0x10	// 90
#define B2LSB	0x12	// 92
#define C12MSB	0x14	// 94
#define	C12LSB	0x16	// 96
#define	C11MSB	0x18	// 98
#define C11LSB	0x1A	// 9A
#define C22MSB	0x1C	// 9C
#define C22LSB	0x1E	// 9E


//======================//
// 		 Macros			//
//======================//
#define sbi(var, mask)   ((var) |= (uint8_t)(1 << mask))
#define cbi(var, mask)   ((var) &= (uint8_t)~(1 << mask))

//======================//
// 	 MPL115A1 Defines	//
//======================//
#define CS 0 //pin for chip select
#define SDN 1	// pin for interrupt

//======================//
//	General Functions	//
//======================//
void ioinit(void);
void delay_ms(uint16_t x);
void delay_us(uint16_t x);

//======================//
//	MPL115A1 Functions	//
//======================//
void init_SPI(void);
void txdata(char data);
char rxdata(void);
char read(uint8_t address);
void write(uint8_t address, char data);
float calculatePressure(void);

//======================//
//	 Global Variables	//
//======================//
signed char sia0MSB, sia0LSB;
signed char sib1MSB, sib1LSB;
signed char sib2MSB, sib2LSB;
signed char sic12MSB, sic12LSB;
signed char sic11MSB, sic11LSB;
signed char sic22MSB, sic22LSB;
signed int sia0, sib1, sib2, sic12, sic11, sic22, siPcomp;
float decPcomp;
signed long lt1, lt2, lt3, si_c11x1, si_a11, si_c12x2;
signed long si_a1, si_c22x2, si_a2, si_a1x1, si_y1, si_a2x2;
unsigned int uiPadc, uiTadc;
unsigned char uiPH, uiPL, uiTH, uiTL;


void Initialize_ADC(void);
int grabVoltage(int adcChannel);

// Temp conversion functions
int celcius(int v_out);
int fahrenheit(int celcius);

// Formatted output
void printDecimal(int decimal, char *buffer);
void printString(char *buffer);
void printFloat(float, char* buf);

/* usart stuff */
void usart_init(uint16_t baudin, uint32_t clk_speedin);
void usart_send(uint8_t data);
uint8_t  usart_recv(void);
uint8_t  usart_istheredata(void);


int main (void)
{
	uint8_t i = 25;
	char *buffer = (char*)malloc(sizeof(char) * 100);
	float pressure;

	DDRB = 0b11101111; //MISO input

	Initialize_ADC();

	ioinit();
	init_SPI();
	sbi(PORTB,CS);	// CS Idle High
	sbi(PORTB,SDN);	// SDN high turns sensor on

	usart_init(9600, 16000000);

	while(1) {

		i = usart_recv();
		if(i == 'T') {

			/* print out data in JSON format */
			printString("{");
			printString("\"data\": {");
			
			printString("\"temp\":\"");
			printDecimal( fahrenheit(100 * getTemperature()), buffer );
			printString("\"");
		
			printString(",\"light\":\"");
			printDecimal(grabVoltage(0x02), buffer);
			printString("\"");

			printString(",\"pressure\":\"");
			printDecimal(calculatePressure() * 100, buffer);
			printString("\"");

			printString(",\"humidity\":\"");
			printFloat( getHumidity(), buffer );
			printString("\"");
			printString("}}\n\r");


		} else if(i == 'Z') {
			pressure = calculatePressure() * 100;			
			printDecimal(pressure, buffer);
			printString("\n\r");
		} 
		else if(i == 'H') { // tell the humidity sensor to sample - gets both temperature and humidity %
			readHumidityData();
		}else {
			usart_send(i);
		}
	}
	return 0;
}


/* 
 * Input: ADC Channel
 * Returns: ADC Voltage in mv, Voltage * 1000
 */
int grabVoltage(int adcChannel) {

	int adc_value;
	int adc_voltage;

	ADMUX = adcChannel;
	ADCSRA = 0xC7; // start conversion
	_delay_us(260);				
		
	adc_value =  ADC & 0x3FF;

	/* Convert ADC value into mv, Voltage * 1000 */ 
	adc_voltage = (5 * adc_value) * (1000.0/1024.0);

	return adc_voltage;
}

void Initialize_ADC(void)
{
	ADCSRA = 0x87;		//Turn On ADC and set prescaler (CLK/128--62 kHz)
										//MAX A/D conversion rate 5 kHz @ 62 kHz frequency    
	ADCSRB = 0x00;		//turn off autotrigger
}



/* input ADC in millivolts
 * returns celcius * 100 to preserve 2 decimal places
 */
int celcius(int v_out) 
{
	// Celcius = Vout(mv) - 500 / 10	
	int celcius = 0;
	celcius = (v_out - 500) * (100/10);
	return celcius;
}

/* input = celcius * 100
 * returns fahrenheit * 100 to preserve 2 decimal places
 */
int fahrenheit(int celcius) 
{
	// °C  x  9/5 + 32 = °F	 
	int fahrenheit = celcius * 1.8 + 3200 ;
	return fahrenheit;
}

/* Assumes decimal number is already multiplied by 100
 * 2 decimal places preserved
 */
void printDecimal(int decimal, char *buffer) 
{
	itoa (decimal, buffer, 10);
	while(*buffer != '\0') {
		usart_send(*buffer);
		buffer++;
	}
}

void printString(char *buffer) 
{
	while(*buffer != '\0') {
		usart_send(*buffer);
		buffer++;
	}
}
void printFloat(float f, char* buffer)
{
	int approx = f * 100;
	printDecimal(approx, buffer);
}




void usart_init(uint16_t baudin, uint32_t clk_speedin)
{
	uint32_t ubrr = (clk_speedin/16UL)/baudin-1;
	UBRR0H = (unsigned char)(ubrr>>8);
	UBRR0L = (unsigned char)ubrr;
		
	/* Enable receiver and transmitter */
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);
	
	/* Set frame format: 8data, 1stop bit */
	UCSR0C = (1<<USBS0)|(3<<UCSZ00);
	UCSR0A &= ~(1<<U2X0);
}
 
/*the send function will put 8bits on the trans line. */
void usart_send(uint8_t data)
{
	while ( !( UCSR0A & (1<<UDRE0)) );
	UDR0 = data;
}
 
uint8_t  usart_recv(void)
{
	/* Wait for data to be received */
	while ( !(UCSR0A & (1<<RXC0)) )
		;
	/* Get and return received data from buffer */
	return UDR0;
}
 
uint8_t  usart_istheredata(void)
{
	return (UCSR0A & (1<<RXC0));
}





float calculatePressure()
{
	write(0x24, 0x00);	// Start Both Conversions
	// write(0x20, 0x00);	// Start Pressure Conversion
	// write(0x22, 0x00);	// Start temperature conversion
	delay_ms(10);	// Typical wait time is 3ms
	
	// Read pressure
	uiPH = read(PRESH);
	uiPL = read(PRESL);
	uiTH = read(TEMPH);
	uiTL = read(TEMPL);
	
	uiPadc = (unsigned int) uiPH << 8;
	uiPadc += (unsigned int) uiPL & 0x00FF;
	uiTadc = (unsigned int) uiTH << 8;
	uiTadc += (unsigned int) uiTL & 0x00FF;
	
	// Placing Coefficients into 16-bit Variables
	// a0
	sia0MSB = read(A0MSB);
	sia0LSB = read(A0LSB);
	sia0 = (signed int) sia0MSB << 8;
	sia0 += (signed int) sia0LSB & 0x00FF;
	
	// b1
	sib1MSB = read(B1MSB);
	sib1LSB = read(B1LSB);
	sib1 = (signed int) sib1MSB << 8;
	sib1 += (signed int) sib1LSB & 0x00FF;
	
	// b2
	sib2MSB = read(B2MSB);
	sib2LSB = read(B2LSB);
	sib2 = (signed int) sib2MSB << 8;
	sib2 += (signed int) sib2LSB & 0x00FF;
	
	// c12
	sic12MSB = read(C12MSB);
	sic12LSB = read(C12LSB);
	sic12 = (signed int) sic12MSB << 8;
	sic12 += (signed int) sic12LSB & 0x00FF;
	
	// c11
	sic11MSB = read(C11MSB);
	sic11LSB = read(C11LSB);
	sic11 = (signed int) sic11MSB << 8;
	sic11 += (signed int) sic11LSB & 0x00FF;
	
	// c22
	sic22MSB = read(C22MSB);
	sic22LSB = read(C22LSB);
	sic22 = (signed int) sic22MSB << 8;
	sic22 += (signed int) sic22LSB & 0x00FF;
	
	// Coefficient 9 equation compensation
	uiPadc = uiPadc >> 6;
	uiTadc = uiTadc >> 6;
	
	// Step 1 c11x1 = c11 * Padc
	lt1 = (signed long) sic11;
	lt2 = (signed long) uiPadc;
	lt3 = lt1*lt2;
	si_c11x1 = (signed long) lt3;
	
	// Step 2 a11 = b1 + c11x1
	lt1 = ((signed long)sib1)<<14;
	lt2 = (signed long) si_c11x1;
	lt3 = lt1 + lt2;
	si_a11 = (signed long)(lt3>>14);
	
	// Step 3 c12x2 = c12 * Tadc
	lt1 = (signed long) sic12;
	lt2 = (signed long) uiTadc;
	lt3 = lt1*lt2;
	si_c12x2 = (signed long)lt3;
	
	// Step 4 a1 = a11 + c12x2
	lt1 = ((signed long)si_a11<<11);
	lt2 = (signed long)si_c12x2;
	lt3 = lt1 + lt2;
	si_a1 = (signed long) lt3>>11;
	
	// Step 5 c22x2 = c22*Tadc
	lt1 = (signed long)sic22;
	lt2 = (signed long)uiTadc;
	lt3 = lt1 * lt2;
	si_c22x2 = (signed long)(lt3);
	
	// Step 6 a2 = b2 + c22x2
	lt1 = ((signed long)sib2<<15);
	lt2 = ((signed long)si_c22x2>1);
	lt3 = lt1+lt2;
	si_a2 = ((signed long)lt3>>16);
	
	// Step 7 a1x1 = a1 * Padc
	lt1 = (signed long)si_a1;
	lt2 = (signed long)uiPadc;
	lt3 = lt1*lt2;
	si_a1x1 = (signed long)(lt3);
	
	// Step 8 y1 = a0 + a1x1
	lt1 = ((signed long)sia0<<10);
	lt2 = (signed long)si_a1x1;
	lt3 = lt1+lt2;
	si_y1 = ((signed long)lt3>>10);
	
	// Step 9 a2x2 = a2 * Tadc
	lt1 = (signed long)si_a2;
	lt2 = (signed long)uiTadc;
	lt3 = lt1*lt2;
	si_a2x2 = (signed long)(lt3);
	
	// Step 10 pComp = y1 + a2x2
	lt1 = ((signed long)si_y1<<10);
	lt2 = (signed long)si_a2x2;
	lt3 = lt1+lt2;
	
	// Fixed point result with rounding
	//siPcomp = ((signed int)lt3>>13);
	siPcomp = lt3/8192;
	
	// decPcomp is defined as a floating point number
	// Conversion to decimal value from 1023 ADC count value
	// ADC counts are 0 to 1023, pressure is 50 to 115kPa respectively
	decPcomp = ((65.0/1023.0)*siPcomp)+50;
	
	return decPcomp;
}

void write(uint8_t address, char data)
{
	//write any data byte to any single address
	//adds a 0 to the MSB of the address byte (WRITE mode)

	address &= 0x7F;	

	//printf("\nWriting 0x%x to 0x%x\n", data, address);

	cbi(PORTB,CS);
	delay_ms(1);
	txdata(address);
	delay_ms(1);
	txdata(data);
	delay_ms(1);
	sbi(PORTB,CS);
}

char read(uint8_t address)
{
	//returns the contents of any 1 byte register from any address
	//sets the MSB for every address byte (READ mode)

	char byte;

	address |= 0x80;

	cbi(PORTB,CS);
	txdata(address);
	byte = rxdata();
	sbi(PORTB,CS);

	return byte;
}

char rxdata(void)
{
	SPDR = 0x55;
	
	while((SPSR&0x80) == 0x00)
		;

	return SPDR;
}

void txdata(char data)
{
	SPDR = data;
	while((SPSR&0x80) == 0x00)
		;
}

void init_SPI(void)
{
	sbi(SPCR,MSTR); //make SPI master
	cbi(SPCR,CPOL); cbi(SPCR,CPHA); //SCL idle low, sample data on rising edge
	cbi(SPCR,SPR1); cbi(SPCR,SPR0); sbi(SPSR,SPI2X); //Fosc/4 is SPI frequency
	sbi(SPCR,SPE); //enable SPI
}

void ioinit (void)
{
	int MYUBRR = 51;	// Results in 9600bps@8MHz or 19200bps@16MHz

	UBRR0H = (MYUBRR) >> 8;
	UBRR0L = MYUBRR;
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);
	UCSR0C = (3<<UCSZ00);

	TCCR2B = (1<<CS21);
}

//General short delays
void delay_ms(uint16_t x)
{
	for (; x > 0 ; x--)
		delay_us(1000);
}

//General short delays
void delay_us(uint16_t x)
{
    while(x > 256)
    {
        TIFR2 = (1<<TOV2); //Clear any interrupt flags on Timer2
        TCNT2 = 0; //256 - 125 = 131 : Preload timer 2 for x clicks. Should be 1us per click
        while( (TIFR2 & (1<<TOV2)) == 0);

        x -= 256;
    }

    TIFR2 = (1<<TOV2); //Clear any interrupt flags on Timer2
    TCNT2= 256 - x; //256 - 125 = 131 : Preload timer 2 for x clicks. Should be 1us per click
    while( (TIFR2 & (1<<TOV2)) == 0);
}





