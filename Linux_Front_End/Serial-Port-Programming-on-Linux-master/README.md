# Programming the Serial port on a Linux System using C and termios API
<img src="http://xanthium.in/sites/default/files/site-images/serial-prog-linux/serial-programming-in-linux-tutorials.jpg">
--------------------------------------------------------------------------------------------------------------------------------------
Code for programming the serial port on a Linux System using **ANSI C** and **Terminos API**.
The codes can be used for transmitting and receiving data from an x86 Linux PC and a Microcontroller deevelopment board through 3 wire Serial Link or RS232 Port. 

###<a href = http://xanthium.in/Serial-Port-Programming-on-Linux>You can find the **original Tutorial** here </a>

<img src="http://xanthium.in/sites/default/files/site-images/serial-prog-linux/SerialPort-Write.jpeg" alt ="Screenshot of the serial port programming code running on Linux">

--------------------------------------------------------------------------------------------------------------------------------------
##Features

- PC side is written in **ANSI C** using **Terminos API** to Communicate with the Serial Port.
- Transmission and Reception are kept seperate in different C files so that user can easily understand the mechanisms involved.
- PC communicates with MSP430 microcontroller using a 3 wire serial Link (RX,TX,GND).Refer <a href = http://xanthium.in/Serial-Port-Programming-on-Linux> Tutorial for Circuit </a>
- The Microcontroller and PC are connected in **null modem configuration** using  3 signals (TX,RX and Ground).
- Code will Work With Standard **RS232 Serial ports** or any **USB to Serial Converter**.
- Microcontroller side code is written in **Embedded C** and can be Compiled using **Code Composer Studio for MSP430**.
- The Hardware used is MSP430G2553 on Launch pad development board.

--------------------------------------------------------------------------------------------------------------------------------------
##Code Explanation

- **serial.c** - A simple C file which opens a connection to Linux Serial Port and closes it. 

- **USB2SERIAL_Read** - **Serial Reception** Program which runs on the Linux PC and receives data from the MSP430 Microcontroller.The Received Data is then displayed on Console Window.
  - <img src = "http://xanthium.in/sites/default/files/site-images/serial-prog-linux/SerialPort-Read-String-Received.jpeg"/>

- **USB2SERIAL_Write** - **Serial Transmission** which runs on the Linux PC and Transmits a charcter to the MSP430 Microcontroller.The MSP430 on Receiving the Character lights up an LED on the Launchpad Development board.
  - <img src = "http://xanthium.in/sites/default/files/site-images/serial-prog-linux/SerialPort-Write.jpeg"/>

 -------------------------------------------------------------------------------------------------------------------------------------
##Circuit 

- <a href ="http://xanthium.in/Serial-Port-Programming-on-Linux">Available  in the original Tutorial</a>

<img src ="http://xanthium.in/sites/default/files/site-images/serial-prog-linux/MSP430-Connected-to-USB2SERIAL_Marked.jpg"/>

-------------------------------------------------------------------------------------------------------------------------------------
##Hardware used 

- <img src="http://www.xanthium.in/sites/default/files/site-images/product-page/usb_to_rs485_converter_250px.jpg"/>
   - **USB to RS485/RS232/Serial Converter**
   - <a href = "http://xanthium.in/USB-to-Serial-RS232-RS485-Converter">Details can be found here </a>
   
- <img src="http://www.xanthium.in/sites/default/files/site-images/msp430-launchpad-tutorial-brief-intro/msp430-launchpad-tutorial-for-beginners.jpg"/>
 - **MSP430 Launchpad Development Board**
 - <a href = "http://www.xanthium.in/brief-introduction-msp430g2xxx-using-ti-launchpad-development-board">A short tutorial can be found here</a>
  






