// This is a header file for FDC2214 library
// By Harijs Zablockis, Intelitech, March 2018 
// This file is heavily based on NelsonsLog_FDC2214.h by Chris Nelson 
// Masks and channels added
//
// There is no warranty to the code. There is no support, don't ask for it.
// Use or skip on your own responsibility.
// NOT ALL FEATURES ARE TESTED! 
//
// The code might get revisited at some point or it might not.
// The code does more than I need at the moment.
//
// Feel free to do whatever you want with it. No licensing BS. No limitations.
//


#ifndef _FDC2214_H_
#define _FDC2214_H_

#if ARDUINO >= 100
#include "Arduino.h"
#define WIRE_WRITE Wire.write
#else
#include "WProgram.h"
#define WIRE_WRITE Wire.send
#endif

#if defined(__SAM3X8E__)
typedef volatile RwReg PortReg;
 typedef uint32_t PortMask;
#define HAVE_PORTREG
#elif defined(ARDUINO_ARCH_SAMD)
// not supported
#elif defined(ESP8266) || defined(ESP32) || defined(ARDUINO_STM32_FEATHER) || defined(__arc__)
typedef volatile uint32_t PortReg;
  typedef uint32_t PortMask;
#elif defined(__AVR__)
typedef volatile uint8_t PortReg;
  typedef uint8_t PortMask;
#define HAVE_PORTREG
#else
// chances are its 32 bit so assume that
typedef volatile uint32_t PortReg;
typedef uint32_t PortMask;
#endif

#define FDC2214_I2C_ADDR_0   0x2A
#define FDC2214_I2C_ADDR_1   0x2B
// Address is 0x2A (default) or 0x2B (if ADDR is high)

//bitmasks
#define FDC2214_CH0_UNREADCONV 0x0008         //denotes unread CH0 reading in STATUS register
#define FDC2214_CH1_UNREADCONV 0x0004         //denotes unread CH1 reading in STATUS register
#define FDC2214_CH2_UNREADCONV 0x0002         //denotes unread CH2 reading in STATUS register
#define FDC2214_CH3_UNREADCONV 0x0001         //denotes unread CH3 reading in STATUS register


//registers
#define FDC2214_DEVICE_ID           		0x7F
#define FDC2214_MUX_CONFIG          		0x1B
#define FDC2214_CONFIG              		0x1A
#define FDC2214_RCOUNT_CH0          		0x08
#define FDC2214_RCOUNT_CH1          		0x09
#define FDC2214_RCOUNT_CH2          		0x0A
#define FDC2214_RCOUNT_CH3          		0x0B
#define FDC2214_OFFSET_CH0		          	0x0C
#define FDC2214_OFFSET_CH1          		0x0D
#define FDC2214_OFFSET_CH2    			    0x0E
#define FDC2214_OFFSET_CH3         			0x0F
#define FDC2214_SETTLECOUNT_CH0     		0x10
#define FDC2214_SETTLECOUNT_CH1     		0x11
#define FDC2214_SETTLECOUNT_CH2     		0x12
#define FDC2214_SETTLECOUNT_CH3     		0x13
#define FDC2214_CLOCK_DIVIDERS_CH0  		0x14
#define FDC2214_CLOCK_DIVIDERS_CH1  		0x15
#define FDC2214_CLOCK_DIVIDERS_CH2  		0x16
#define FDC2214_CLOCK_DIVIDERS_CH3  		0x17
#define FDC2214_STATUS              		0x18
#define FDC2214_DATA_CH0_MSB	            0x00
#define FDC2214_DATA_CH0_LSB    		    0x01
#define FDC2214_DATA_CH1_MSB	            0x02
#define FDC2214_DATA_CH1_LSB    		    0x03
#define FDC2214_DATA_CH2_MSB	            0x04
#define FDC2214_DATA_CH2_LSB    		    0x05
#define FDC2214_DATA_CH3_MSB	            0x06
#define FDC2214_DATA_CH3_LSB    		    0x07
#define FDC2214_DRIVE_CH0           		0x1E
#define FDC2214_DRIVE_CH1           		0x1F
#define FDC2214_DRIVE_CH2           		0x20
#define FDC2214_DRIVE_CH3           		0x21

// mask for 28bit data to filter out flag bits
#define FDC2214_DATA_CHx_MASK_DATA         	0x0FFF  
#define FDC2214_DATA_CHx_MASK_ERRAW        	0x1000  
#define FDC2214_DATA_CHx_MASK_ERRWD        	0x2000  



class FDC2214 {
public:
    FDC2214(uint8_t i2caddr);
	//_i2caddr = i2caddr
	
    boolean begin(uint8_t chanMask, uint8_t autoscanSeq, uint8_t deglitchValue, bool intOsc);

 //   double readCapacitance();
// To be used with FDC2112 and FDC2114
    unsigned long getReading16(uint8_t channel);
// To be used with FDC2212 and FDC2214
    unsigned long getReading28(uint8_t channel);

private:
    void loadSettings(uint8_t chanMask, uint8_t autoscanSeq, uint8_t deglitchValue, bool intOsc);
//    void setGain(void);
//    double calculateCapacitance(long long fsensor);
//    long long calculateFsensor(unsigned long reading);
    void write8FDC(uint16_t address, uint8_t data);
    void write16FDC(uint16_t address, uint16_t data);
    uint32_t read32FDC(uint16_t address);
    uint16_t read16FDC(uint16_t address);
    uint8_t read8FDC(uint16_t address);
    uint8_t _i2caddr;
};

#endif //include guard
