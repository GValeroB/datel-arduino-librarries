#ifndef DallasTemperatureMinimal_h
#define DallasTemperatureMinimal_h

#define DALLASTEMPLIBVERSION "3.7.2 Minimal"

// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.


#include <inttypes.h>
#include <OneWire.h>

// Model IDs
#define DS18S20MODEL 0x10
#define DS18B20MODEL 0x28
#define DS1822MODEL  0x22

// OneWire commands
#define STARTCONVO      0x44  // Tells device to take a temperature reading and put it on the scratchpad
#define COPYSCRATCH     0x48  // Copy EEPROM
#define READSCRATCH     0xBE  // Read EEPROM
#define WRITESCRATCH    0x4E  // Write to EEPROM
#define RECALLSCRATCH   0xB8  // Reload from last known
#define READPOWERSUPPLY 0xB4  // Determine if device needs parasite power
#define ALARMSEARCH     0xEC  // Query bus for devices with an alarm condition

// Scratchpad locations
#define TEMP_LSB        0
#define TEMP_MSB        1
#define HIGH_ALARM_TEMP 2
#define LOW_ALARM_TEMP  3
#define CONFIGURATION   4
#define INTERNAL_BYTE   5
#define COUNT_REMAIN    6
#define COUNT_PER_C     7
#define SCRATCHPAD_CRC  8

// Device resolution
#define TEMP_9_BIT  0x1F //  9 bit
#define TEMP_10_BIT 0x3F // 10 bit
#define TEMP_11_BIT 0x5F // 11 bit
#define TEMP_12_BIT 0x7F // 12 bit

// Error Codes
#define DEVICE_DISCONNECTED -127

typedef uint8_t DeviceAddress[8];

class DallasTemperatureMinimal
{
  public:

  DallasTemperatureMinimal(OneWire*);

  // initalise bus
  void begin(void);

  // returns the number of devices found on the bus
  uint8_t getDeviceCount(void);
  
 
  // returns true if address is valid
  bool validAddress(uint8_t*);

  // finds an address at a given index on the bus 
  bool getAddress(uint8_t*, const uint8_t);
  
  // attempt to determine if the device at the given address is connected to the bus
  bool isConnected(uint8_t*);

  // attempt to determine if the device at the given address is connected to the bus
  // also allows for updating the read scratchpad
  bool isConnected(uint8_t*, uint8_t*);

  // read device's scratchpad
  void readScratchPad(uint8_t*, uint8_t*);

  // write device's scratchpad
  void writeScratchPad(uint8_t*, const uint8_t*);

  // sends command for all devices on the bus to perform a temperature conversion 
  void requestTemperatures(void);
   
  // returns temperature in degrees C
  float getTempC(uint8_t*);

  private:
  typedef uint8_t ScratchPad[9];
  

  // count of devices on the bus
  uint8_t devices;
  
  // Take a pointer to one wire instance
  OneWire* _wire;

  // reads scratchpad and returns the temperature in degrees C
  float calculateTemperature(uint8_t*, uint8_t*);
  
  
  #if REQUIRESALARMS

  // required for alarmSearch 
  uint8_t alarmSearchAddress[8];
  char alarmSearchJunction;
  uint8_t alarmSearchExhausted;

  // the alarm handler function pointer
  AlarmHandler *_AlarmHandler;

  #endif
  
};
#endif
