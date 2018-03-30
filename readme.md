# FDC2214 Arduino Library
Library for Texas Instruments FDC2112, FDC2114, FDC2212 and FDC2214 capacitative sensor front-ends.




# Hardware
Tested on YB1F2 contproller.


# Protocol
The protocol is similar to NEC with different timing and data fields. Low level decode is implemented. High level protocol decode shows about hald of data fields. Other half is missing. It seems that there might be a block with checksum that has to be decoded.

# Tools
To view nice real-time graph of the sensor output, it is highly recommended to use tool like SerialPlot.
https://bitbucket.org/hyOzd/serialplot
Binary downloads avialable here: https://bitbucket.org/hyOzd/serialplot/downloads/

Settings for SerialPlot:
![Transmitter Front](https://raw.githubusercontent.com/zharijs/AlpicAir-Remote-Pulseview/master/pics/aa-front.jpg "Transmitter Front")
![Transmitter Back](https://raw.githubusercontent.com/zharijs/AlpicAir-Remote-Pulseview/master/pics/aa-back.jpg "Transmitter Back")



# Usage
Add to your Sigrok/PulseView install directory.

Example:
C:\Program Files (x86)\sigrok\PulseView\share\libsigrokdecode\decoders\
This 