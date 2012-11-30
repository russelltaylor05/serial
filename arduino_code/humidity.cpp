#define RHT03_DATA_BIT_COUNT 41
#define PORTD_PIN_NUMBER 3

#define F_CPU 16000000

#include<avr/io.h>
#include<util/delay.h>
#include <stdlib.h>
#include <string.h>

#include "humidity.h"
#include "DigitalPin.h"

// global variable definitions
volatile float _lastHumidity;
volatile float _lastTemperature;

RHT03_ERROR readHumidityData() 
{
    volatile int i, j, retryCount;
    int currentTemperature=0;
    int currentHumidity=0;
    unsigned int checkSum = 0, csPart1, csPart2, csPart3, csPart4;
    unsigned int bitTimes[RHT03_DATA_BIT_COUNT];
    RHT03_ERROR err = RHT_ERROR_NONE;
    //time_t currentTime = time(NULL);
	
	// debug
//	_lastTemperature = 6.66;
 
    DigitalPin  DATA( PORTD_PIN_NUMBER );
       
    for (i = 0; i < RHT03_DATA_BIT_COUNT; i++) {
        bitTimes[i] = 0;
    }
    
    retryCount = 0;


    // Pin needs to start HIGH, wait unit it is HIGH with a timeout
    do {
        if (retryCount > 125) {
            //pc1.printf("RHT22 Bus busy!");
            err = RHT_BUS_HUNG;
            return err;
        }
        retryCount ++;
        _delay_us(2);
		//return err;
    } while (DATA.read() == 0);   // exit on RHT22 return 'High' Signal within 250us
    
	_delay_ms(2000);

    // Send the activate pulse
    // Step 1: MCU send out start signal to RHT22 and RHT22 send
    //         response signal to MCU.
    // If always signal high-voltage-level, it means RHT22 is not 
    // working properly, please check the electrical connection status.
    //
    DATA.output(); // set pin to output data
    DATA.write(0);      // MCU send out start signal to RHT22
    _delay_ms(18);   // 18 ms wait (spec: at least 1ms)
    DATA.write(1);      // MCU pull up 
    _delay_us(30);

	// now wait for sensor's handshake
    DATA.input();  // set pin to receive data
    // Find the start of the ACK Pulse
    retryCount = 0;

    do {
        if (retryCount > 40)  { // (Spec is 20-40 us high)
            //pc1.printf("RHT22 not responding!");
            err = RHT_ERROR_NOT_PRESENT;
            return err;
        }
        retryCount++;
        _delay_us(1);
    } while (DATA.read() == 1);   // Exit on RHT22 pull low within 40us*/

    if (err != RHT_ERROR_NONE) {
        // initialisation failed
		_lastTemperature = 3;
        return err;    
    }
    _delay_us(80); // RHT pull up ready to transmit data
    
    /*
    if (DATA == 0) {
        printf("RHT22 not ready!");
        err = RHT_ERROR_ACK_TOO_LONG;
        return err;
    }
    */
    
    // Reading the 5 byte data stream
    // Step 2: RHT22 send data to MCU
    //         Start bit -> low volage within 50us (actually could be anything from 35-75us)
    //         0         -> high volage within 26-28us (actually could be 10-40us)
    //         1         -> high volage within 70us (actually could be 60-85us)
    //         See http://www.sparkfun.com/products/10167#comment-4f118d6e757b7f536e000000
    
    
    for (i = 0; i < 5; i++) {
        for (j = 0; j < 8; j++) {
            // Instead of relying on the data sheet, just wait while the RHT03 pin is low
            retryCount = 0;
            do {
                if (retryCount > 75)  {
                    //pc1.printf("RHT22 timeout waiting for data!");
                    err = RHT_ERROR_DATA_TIMEOUT;
                }
                retryCount++;
                _delay_us(1);
            } while (DATA.read() == 0);
            // We now wait for 40us
            _delay_us(40);
            if (DATA.read() == 1) {  
                // If pin is still high, bit value is a 1
                bitTimes[i*8+j] = 1;
            } else {  
                // The bit value is a 0
                bitTimes[i*8+j] = 0;
            }
            int count = 0;
            while (DATA.read() == 1 && count < 100) {  
                _delay_us(1); // Delay for 1 microsecond  
                count++;  
            }
        }
    }
    // Re-init RHT22 pin  
    DATA.output();  
    DATA.write(1);
 
    // Now bitTimes have the actual bits
    // that were needed to find the end of each data bit
    // Note: the bits are offset by one from the data sheet, not sure why
    currentHumidity    = 0;
    currentTemperature = 0;
    checkSum           = 0;
    // First 16 bits is Humidity
    for (i=0; i<16; i++) {
        //printf("bit %d: %d  ", i, bitTimes[i+1]);
        if (bitTimes[i+1] > 0) {
            currentHumidity |= ( 1 << (15-i));
        }
    }
    
    // Second 16 bits is Temperature 
    for (i=0; i<16; i ++) {
        //printf("bit %d: %d  ", i+16, bitTimes[i+17]);
        if (bitTimes[i+17] > 0) {
            currentTemperature |= (1 <<(15-i));
        }
    }
 
    // Last 8 bit is Checksum
    for (i=0; i<8; i++) {
        //printf("bit %d: %d  ", i+32, bitTimes[i+33]);
        if (bitTimes[i+33] > 0) {
            checkSum |= (1 << (7-i));
        }
    }
   
    _lastHumidity = (float(currentHumidity) / 10.0);
    
    // if first bit of currentTemperature is 1, it is negative value.
    if ((currentTemperature & 0x8000)==0x8000) {        
        _lastTemperature = (float(currentTemperature & 0x7FFF) / 10.0) * -1.0;
    } else {
        _lastTemperature = float(currentTemperature) / 10.0;
    }
 
 	// debug
	//_lastTemperature = 30.0;

    // Calculate Check Sum
    csPart1 = currentHumidity >> 8;
    csPart2 = currentHumidity & 0xFF;
    csPart3 = currentTemperature >> 8;
    csPart4 = currentTemperature & 0xFF;
    if (checkSum == ((csPart1 + csPart2 + csPart3 + csPart4) & 0xFF)) {
        //_lastReadTime = currentTime;
        //pc1.printf("OK-->Temperature:%f, Humidity:%f\r\n", _lastTemperature, _lastHumidity);
        err = RHT_ERROR_NONE;
    } else {
        //pc1.printf("RHT22 Checksum error!\n");
        //pc1.printf("Calculate check sum is %d\n",(csPart1 + csPart2 + csPart3 + csPart4) & 0xFF);
        //pc1.printf("Reading check sum is %d",checkSum);
        err = RHT_ERROR_CHECKSUM;
    }

    return err;
}

float getHumidity()
{
	return _lastHumidity;
}

float getTemperature()
{
	return _lastTemperature;
}
