# FDC2214 Arduino Library
Library for Texas Instruments FDC2xxx family capacitative sensor front-ends.

# Supported devices
>* FDC2112 
>* FDC2114
>* FDC2212
>* FDC2214

# Revision
>* 1.0 - 1 - Initial release 
>* 1.1 - 1 - Fixed channel 2 and 3 support. 
>* 1.1 - 2 - Added support for internal oscillator. Not reccomended for any fairly precise aplication. 

# Usage
Include header, Make instance, Init and acquire data.

```cpp
#include "FDC2214.h"
FDC2214 capsense(FDC2214_I2C_ADDR_0); // Use FDC2214_I2C_ADDR_1 for ADDR = VCC
...
void setup() {
    ...
    Wire.begin();
    bool capOk = capsense.begin(0x3, 0x4, 0x5, false); //setup first two channels, autoscan with 2 channels, deglitch at 10MHz, use external oscillator 
    ...
}
void loop(){
    ...
        unsigned long capa[i] = capsense.getReading28(i);  
    ...
}
```

# Hardware
FDC2xxx family is 3.3V powered, unlike most of Arduinos, that are powered form 5V.
**To use this chip with Arduino, you will have to either:**
> 1. use 3.3V version of Arduino, like Arduino Pro Mini 8MHz 3.3V 
> 2. use I2C level shifter to interface the FDC chip with arduino.

**To run examples, connect FDC with arduino as follows:**
 >* ARDUINO <--> FDC 
 >* A4 <-------> SDA
 >* A5 --------> SCL
 >* GND -------> ADR
 >* GND -------> SD
  
# Tools
To view nice real-time graph of the sensor output, it is highly recommended to use tool like SerialPlot.
https://bitbucket.org/hyOzd/serialplot
Binary downloads avialable here: https://bitbucket.org/hyOzd/serialplot/downloads/

**Setting up port:**
![Port](https://github.com/zharijs/FDC2214/blob/master/extras/images/fdc-on-serialplot-port.PNG?raw=true "Port")

**Setting up data format:**
![Data Format](https://github.com/zharijs/FDC2214/blob/master/extras/images/fdc-on-serialplot-dataformat.PNG?raw=true "Data Format")

**Proximity sensing waveform:**
![Signal](https://github.com/zharijs/FDC2214/blob/master/extras/images/fdc-on-serialplot-signal.PNG?raw=true "Signal")

**Noise waveform:**
![Noise](https://github.com/zharijs/FDC2214/blob/master/extras/images/fdc-on-serialplot-noise.PNG?raw=true "Noise")

#Have Fun
