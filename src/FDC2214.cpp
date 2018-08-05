// This is a source file for FDC2214 library
// By Harijs Zablockis, Intelitech, March 2018 
// This file is based on NelsonsLog_FDC2214.c by Chris Nelson 
//
// The 16 bit output overflow bug is fixed.
// Configuration is made more dynamic and register comments are made readable 
// Added other channels
// Added masking out error bits from data 
// Some junk code and header form original file left for historical value
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

/**************************************************************************/
/*!
    @file     NelsonsLog_FDC2214.cpp
    @author   Chris Nelson
	@license  BSD (see license.txt)

	This is a library for an FDC2214 capacitive sensor
	----> http://www.nelsonsloxg.com

	@section  HISTORY

    v1.0  - First release
*/
/**************************************************************************/

#include "Arduino.h"
#include <Wire.h>
#include "FDC2214.h"

FDC2214::FDC2214(uint8_t i2caddr) {
	_i2caddr = i2caddr;
    //_i2caddr = FDC2214_I2C_ADDRESS;
}

// Checking for chip ID, if OK, calls chip init
boolean FDC2214::begin(uint8_t chanMask, uint8_t autoscanSeq, uint8_t deglitchValue, bool intOsc) {
    Wire.begin();

    int devId = read16FDC(FDC2214_DEVICE_ID);
    if (devId != 0x3054) {
        if (devId != 0x3055) {
            //two valid device ids for FDC2214 0x3054 and 0x3055
            return false;
        }
    }

    loadSettings(chanMask, autoscanSeq, deglitchValue, intOsc);
//    setGain();

    return true;
}


//Internal routine to do actual chip init
void FDC2214::loadSettings(uint8_t chanMask, uint8_t autoscanSeq, uint8_t deglitchValue, bool intOsc) {

	//Configuration register
	//	Active channel Select: b00 = ch0; b01 = ch1; b10 = ch2; b11 = ch3;
	//  |Sleep Mode: 0 - device active; 1 - device in sleep;
	//  ||Reserved, reserved, set to 1
	//  |||Sensor Activation Mode: 0 - drive sensor with full current. 1 - drive sensor with current set by DRIVE_CURRENT_CHn 
	//  ||||Reserved, set to 1
	//  |||||Reference clock: 0 - use internal; 1 - use external clock
	//  ||||||Reserved, set to 0
	//  |||||||Disable interrupt. 0 - interrupt output on INTB pin; 1 - no interrupt output
	//  ||||||||High current sensor mode: 0 - 1.5mA max. 1 - > 1.5mA, not available if Autoscan is enabled
	//  |||||||||      Reserved, set to 000001
	//  |||||||||      |
	//  CCS1A1R0IH000000 -> 0001 1110 1000 0001 -> 0x1E81 	ExtOsc
	//  CCS1A1R0IH000000 -> 0001 1100 1000 0001 -> 0x1C81	IntOsc
	if (intOsc) {
		write16FDC(FDC2214_CONFIG, 0x1C81);  //set config
	} else {
		write16FDC(FDC2214_CONFIG, 0x1E81);  //set config
	}
	//If channel 1 selected, init it..
	if (chanMask & 0x01) {

		//settle count maximized, slow application
		write16FDC(FDC2214_SETTLECOUNT_CH0, 0x64);

		//rcount maximized for highest accuracy
		write16FDC(FDC2214_RCOUNT_CH0, 0xFFFF);

		//no offset
		write16FDC(FDC2214_OFFSET_CH0, 0x0000);
		
		// Set clock dividers
		//  Reserved
		//  | Sensor Frequency Select. b01 = /1 = sensor freq 0.01 to 8.75MHz; b10 = /2 = sensor freq 0.01 to 10 or 5 to 10 MHz
		//  | | Reserved
		//  | | |         Reference divider. Must be > 1. fref = fclk / this register`
		//  | | |         |
		// 00FF00RRRRRRRRRR -> 0010000000000001 -> 0x2001
		write16FDC(FDC2214_CLOCK_DIVIDERS_CH0, 0x2001);
		//set drive register
		write16FDC(FDC2214_DRIVE_CH0, 0xF800);
	}
	// Init chan2, if selected by channel init mask
	if (chanMask & 0x02) {
		write16FDC(FDC2214_SETTLECOUNT_CH1, 0x64);
		write16FDC(FDC2214_RCOUNT_CH1, 0xFFFF);
		write16FDC(FDC2214_OFFSET_CH1, 0x0000);
		write16FDC(FDC2214_CLOCK_DIVIDERS_CH1, 0x2001);
		write16FDC(FDC2214_DRIVE_CH1, 0xF800);	
	}	
	if (chanMask & 0x04) {
		write16FDC(FDC2214_SETTLECOUNT_CH2, 0x64);
		write16FDC(FDC2214_RCOUNT_CH2, 0xFFFF);
		write16FDC(FDC2214_OFFSET_CH2, 0x0000);
		write16FDC(FDC2214_CLOCK_DIVIDERS_CH2, 0x2001);
		write16FDC(FDC2214_DRIVE_CH2, 0xF800);	
	}	
	if (chanMask & 0x08) {
		write16FDC(FDC2214_SETTLECOUNT_CH3, 0x64);
		write16FDC(FDC2214_RCOUNT_CH3, 0xFFFF);
		write16FDC(FDC2214_OFFSET_CH3, 0x0000);
		write16FDC(FDC2214_CLOCK_DIVIDERS_CH3, 0x2001);
		write16FDC(FDC2214_DRIVE_CH3, 0xF800);	
	}	
	// Autoscan: 0 = single channel, selected by CONFIG.ACTIVE_CHAN
	// | Autoscan sequence. b00 for chan 1-2, b01 for chan 1-2-3, b10 for chan 1-2-3-4
	// | |         Reserved - must be b0001000001
	// | |         |  Deglitch frequency. b001 for 1 MHz, b100 for 3.3 MHz, b101 for 10 Mhz, b111 for 33 MHz
	// | |         |  |
    // ARR0001000001DDD -> b0000 0010 0000 1000 -> h0208
	uint16_t muxVal = 0x0208 | ((uint16_t)autoscanSeq << 13) | deglitchValue;
	//
    write16FDC(FDC2214_MUX_CONFIG, muxVal);  //set mux config for channels
}

///**************************************************************************/
///*!
//    @brief  Given a reading calculates the sensor frequency
//*/
///**************************************************************************/
//long long NelsonsLog_FDC2214::calculateFsensor(unsigned long reading){
////    Serial.println("reading: "+ String(reading));
//    //fsensor = (CH_FIN_SEL * fref * data) / 2 ^ 28
//    //should be mega hz so can truncate to long long
//    Serial.println("FDC reading: " + String(reading));
//    unsigned long long temp;
//    temp = 1 * 40000000 * reading;
//    temp = temp / (2^28);
////    Serial.println("frequency: " + String((long)temp));
//    return temp;
//}

///**************************************************************************/
///*!
//    @brief  Given sensor frequency calculates capacitance
//*/
///**************************************************************************/
//double NelsonsLog_FDC2214::calculateCapacitance(long long fsensor){
//    //differential configuration
//    //c sensor = 1                            - (Cboard + Cparacitic)
//    //             / (L * (2*pi * fsensor)^2)
//
//    double pi = 3.14159265359;
//    double L = 18; //uH
//    double Cboard = 33; //pf
//    double Cparacitic = 3; //pf
//
//    double temp = 2 * pi * fsensor;
//    temp = temp * temp;
//
//    temp = temp / 1000000; //uH
//    temp *= L;
//
////    Serial.println("capacitance: " + String(temp));
//    return temp;
//
//}



// Gets 28bit reading for FDC2212 and FDC2214
// Takes in channel number, gives out the formatted 28 bit reading.
unsigned long FDC2214::getReading28(uint8_t channel) {
    int timeout = 100;
    unsigned long reading = 0;
    long long fsensor = 0;
    int status = read16FDC(FDC2214_STATUS);
    uint8_t addressMSB;
	uint8_t addressLSB;
	uint8_t bitUnreadConv;
	switch (channel){
		case (0):
			addressMSB = FDC2214_DATA_CH0_MSB;
			addressLSB = FDC2214_DATA_CH0_LSB;
			bitUnreadConv = FDC2214_CH0_UNREADCONV;
		break;
		case (1):
			addressMSB = FDC2214_DATA_CH1_MSB;
			addressLSB = FDC2214_DATA_CH1_LSB;
			bitUnreadConv = FDC2214_CH1_UNREADCONV;
		break;
		case (2):
			addressMSB = FDC2214_DATA_CH2_MSB;
			addressLSB = FDC2214_DATA_CH2_LSB;
			bitUnreadConv = FDC2214_CH2_UNREADCONV;
		break;
		case (3):
			addressMSB = FDC2214_DATA_CH3_MSB;
			addressLSB = FDC2214_DATA_CH3_LSB;
			bitUnreadConv = FDC2214_CH3_UNREADCONV;
		break;
		default: return 0;
	}
	
	while (timeout && !(status & bitUnreadConv)) {
        status = read16FDC(FDC2214_STATUS);
        timeout--;
    }
    if (timeout == 100) {
// #####################################################################################################
// There was this weird double read, as "first readout could be stale" in Nelsons file. 
// I have not confirmed the existence of this silicon bug.
// I suspect that it might be due to crappy breadboard or rats nest wiring or lack of signal integrity for other reason
// 
// On the other hand, I have done far too little testing to be sure, so I am leaving that bit in for now.
//	
// #####################################################################################################
		//could be stale grab another //could it really it? ?????
        //read the 28 bit result
        reading = (uint32_t)(read16FDC(addressMSB) & FDC2214_DATA_CHx_MASK_DATA) << 16;
        reading |= read16FDC(addressLSB);
        while (timeout && !(status & bitUnreadConv)) {
            status = read16FDC(FDC2214_STATUS);
            timeout--;
        }
    }
    if (timeout) {
        //read the 28 bit result
        reading = (uint32_t)(read16FDC(addressMSB) & FDC2214_DATA_CHx_MASK_DATA) << 16;
        reading |= read16FDC(addressLSB);
        return reading;
    } else {
		// Could not get data, chip readynes flag timeout
        return 0;
    }
}

// Gets 16bit reading for FDC2112 and FDC2114
// Takes in channel number, gives out the formatted 28 bit reading.
unsigned long FDC2214::getReading16(uint8_t channel) {
    int timeout = 100;
    unsigned long reading = 0;
    long long fsensor = 0;
    int status = read16FDC(FDC2214_STATUS);
    uint8_t addressMSB;
	uint8_t bitUnreadConv;
	switch (channel){
		case (0):
			addressMSB = FDC2214_DATA_CH0_MSB;
			bitUnreadConv = FDC2214_CH0_UNREADCONV;
		break;
		case (1):
			addressMSB = FDC2214_DATA_CH1_MSB;
			bitUnreadConv = FDC2214_CH1_UNREADCONV;
		break;
		case (2):
			addressMSB = FDC2214_DATA_CH2_MSB;
			bitUnreadConv = FDC2214_CH2_UNREADCONV;
		break;
		case (3):
			addressMSB = FDC2214_DATA_CH3_MSB;
			bitUnreadConv = FDC2214_CH3_UNREADCONV;
		break;
		default: return 0;
	}
	
	while (timeout && !(status & bitUnreadConv)) {
        status = read16FDC(FDC2214_STATUS);
        timeout--;
    }
    if (timeout == 100) {
// #####################################################################################################
// There was this weird double read, as "first readout could be stale" in Nelsons file. 
// I have not confirmed the existence of this silicon bug.
// I suspect that it might be due to crappy breadboard or rats nest wiring or lack of signal integrity for other reason
// 
// On the other hand, I have done far too little testing to be sure, so I am leaving that bit in for now.
//	
// #####################################################################################################
		//could be stale grab another //could it really it? ?????
        //read the 28 bit result
        reading = (uint32_t)(read16FDC(addressMSB) & FDC2214_DATA_CHx_MASK_DATA) << 16;
        while (timeout && !(status & bitUnreadConv)) {
            status = read16FDC(FDC2214_STATUS);
            timeout--;
        }
    }
    if (timeout) {
        //read the 16 bit result
        reading = (uint32_t)(read16FDC(addressMSB) & FDC2214_DATA_CHx_MASK_DATA) << 16;
        return reading;
    } else {
		// Could not get data, chip readynes flag timeout
		return 0;
    }
}

///**************************************************************************/
///*!
//    @brief  Takes a reading and calculates capacitance from it
//*/
///**************************************************************************/
//double NelsonsLog_FDC2214::readCapacitance() {
//    int timeout = 100;
//    unsigned long reading = 0;
//    long long fsensor = 0;
//    int status = read16FDC(FDC2214_STATUS_REGADDR);
//    while (timeout && !(status & FDC2214_CH0_UNREADCONV)) {
////        Serial.println("status: " + String(status));
//        status = read16FDC(FDC2214_STATUS_REGADDR);
//        timeout--;
//    }
//    if (timeout) {
//        //read the 28 bit result
//        reading = read16FDC(FDC2214_DATA_CH0_REGADDR) << 16;
//        reading |= read16FDC(FDC2214_DATA_LSB_CH0_REGADDR);
//        fsensor = calculateFsensor(reading);
//        return calculateCapacitance(fsensor);
//    } else {
//        //error not reading
//        Serial.println("error reading fdc");
//        return 0;
//    }
//}


/**************************************************************************/
/*!
    @brief  Scans various gain settings until the amplitude flag is cleared.
            WARNING: Changing the gain setting will generally have an impact on the
            reading.
*/
/**************************************************************************/
//void NelsonsLog_FDC2214::setGain(void) {
//    //todo
//}
/**************************************************************************/
/*!
    @brief  I2C low level interfacing
*/
/**************************************************************************/


// Read 1 byte from the FDC at 'address'
uint8_t FDC2214::read8FDC(uint16_t address) {
    uint8_t data;
    Wire.beginTransmission(_i2caddr);
    Wire.write(address >> 8);
    Wire.write(address);
    Wire.endTransmission(false);
    Wire.requestFrom(_i2caddr, (uint8_t) 1);
    uint8_t r = Wire.read();
    return r;
}

// Read 2 byte from the FDC at 'address'
uint32_t FDC2214::read32FDC(uint16_t address) {
    uint32_t retVal = 0;
	uint8_t data;

    Wire.beginTransmission(_i2caddr);
//    Wire.write(address >> 8);
    Wire.write(address);
    Wire.endTransmission(false);

    Wire.requestFrom(_i2caddr, (uint8_t) 2);
    while (!Wire.available());
    data = Wire.read();
    retVal |= (uint32_t)data << 24; 
    while (!Wire.available());
    data = Wire.read();
    retVal |= (uint32_t)data << 16;
    while (!Wire.available());
    data = Wire.read();
    retVal |= (uint32_t)data << 8;
    while (!Wire.available());
    data = Wire.read();
    retVal |= data;
    return retVal;
}

// Read 2 byte from the FDC at 'address'
uint16_t FDC2214::read16FDC(uint16_t address) {
    uint16_t data;

    Wire.beginTransmission(_i2caddr);
//    Wire.write(address >> 8);
    Wire.write(address);
    Wire.endTransmission(false); //restart

    Wire.requestFrom(_i2caddr, (uint8_t) 2);
    while (!Wire.available());
    data = Wire.read();
    data <<= 8;
    while (!Wire.available());
    data |= Wire.read();
    Wire.endTransmission(true); //end
    return data;
}

// write 1 byte to FDC
void FDC2214::write8FDC(uint16_t address, uint8_t data) {
    Wire.beginTransmission(_i2caddr);
    Wire.write(address >> 8);
    Wire.write(address);
    Wire.write(data);
    Wire.endTransmission();
}

// write 2 bytes to FDC  
void FDC2214::write16FDC(uint16_t address, uint16_t data) {
    Wire.beginTransmission(_i2caddr);
    Wire.write(address & 0xFF);
    Wire.write(data >> 8);
    Wire.write(data);
    Wire.endTransmission();
}
