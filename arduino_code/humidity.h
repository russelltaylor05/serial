// Interface for interacting with Humidity Sensor
//   Based on open source code for the mbed platform
//   https://mbed.org/users/tristanjph/code/RHT03/file/153e20f26d54/RHT03.h
// Modified by David Cosgrove for Arduino compatibility

typedef enum {
    RHT_ERROR_NONE = 0,
    RHT_BUS_HUNG,
    RHT_ERROR_NOT_PRESENT,
    RHT_ERROR_ACK_TOO_LONG,
    RHT_ERROR_SYNC_TIMEOUT,
    RHT_ERROR_DATA_TIMEOUT,
    RHT_ERROR_CHECKSUM,
    RHT_ERROR_TOO_QUICK
} RHT03_ERROR;


//! Trigger the device to take a reading - must be 2s between calls
RHT03_ERROR readHumidityData();

//! Get the last read value for humidity
float getHumidity();

float getTemperature();
