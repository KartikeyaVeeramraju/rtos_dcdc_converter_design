#include <stdio.h>
#include <stdint.h>
#include <sys/select.h>
#include <fcntl.h>  /* File Control Definitions          */
#include <termios.h>/* POSIX Terminal Control Definitions*/
#include <unistd.h> /* UNIX Standard Definitions         */
#include <errno.h>  /* ERROR Number Definitions          */
#include "error_codes.h"
int fd; //serial instance variable

void serial_init(void)
{

  /*------------------------------- Opening the Serial Port -------------------------------*/

  /* Change /dev/ttyUSB0 to the one corresponding to your system */

        fd = open("/dev/ttyACM0",O_RDWR | O_NOCTTY | O_NDELAY);	/* ttyUSB0 is the FT232 based USB2SERIAL Converter   */
                  /* O_RDWR Read/Write access to serial port           */
                /* O_NOCTTY - No terminal will control the process   */
                /* O_NDELAY -Non Blocking Mode,Does not care about-  */
                /* -the status of DCD line,Open() returns immediatly */

        if(fd == -1)						/* Error Checking */
               printf("\n  Error! in Opening ttyACM0  ");
        else
               printf("\n  ttyACM0 Opened Successfully ");


		/*---------- Setting the Attributes of the serial port using termios structure --------- */

		struct termios SerialPortSettings;	/* Create the structure                          */

		tcgetattr(fd, &SerialPortSettings);	/* Get the current attributes of the Serial port */

		/* Setting the Baud rate */
		cfsetispeed(&SerialPortSettings,B115200); /* Set Read  Speed as 115200                       */
		cfsetospeed(&SerialPortSettings,B115200); /* Set Write Speed as 115200                       */

    //cfsetispeed(&SerialPortSettings,B57600); /* Set Read  Speed as 57600                      */
		//cfsetospeed(&SerialPortSettings,B57600); /* Set Write Speed as 57600                       */

		/* 8N1 Mode */
		SerialPortSettings.c_cflag &= ~PARENB;   /* Disables the Parity Enable bit(PARENB),So No Parity   */
		SerialPortSettings.c_cflag &= ~CSTOPB;   /* CSTOPB = 2 Stop bits,here it is cleared so 1 Stop bit */
		SerialPortSettings.c_cflag &= ~CSIZE;	 /* Clears the mask for setting the data size             */
		SerialPortSettings.c_cflag |=  CS8;      /* Set the data bits = 8                                 */

		SerialPortSettings.c_cflag &= ~CRTSCTS;       /* No Hardware flow Control                         */
		SerialPortSettings.c_cflag |= CREAD | CLOCAL; /* Enable receiver,Ignore Modem Control lines       */


		SerialPortSettings.c_iflag &= ~(IXON | IXOFF | IXANY);          /* Disable XON/XOFF flow control both i/p and o/p */
		SerialPortSettings.c_iflag &= ~(ICANON | ECHO | ECHOE | ISIG);  /* Non Cannonical mode                            */

		SerialPortSettings.c_oflag &= ~OPOST;/*No Output Processing*/

		/* Setting Time outs */
		SerialPortSettings.c_cc[VMIN] = 10; /* Read at least 10 characters */
		SerialPortSettings.c_cc[VTIME] = 0; /* Wait indefinetly   */


		if((tcsetattr(fd,TCSANOW,&SerialPortSettings)) != 0) /* Set the attributes to the termios structure*/
		    printf("\n  ERROR ! in Setting attributes");
		else
                    printf("\n  BaudRate = 115200 \n  StopBits = 1 \n  Parity   = none");

	        /*------------------------------- Read data from serial port -----------------------------*/

		tcflush(fd, TCIFLUSH);   /* Discards old data in the rx buffer            */


}

/*
Desc: This function prints the input string via serial terminal
Input: pointer to array, size of array
Output: Error Codes
Warning: Size of input strings restricted to 255 for now
*/

uint8_t serial_write(uint8_t *array_name, uint8_t array_size)
{
	int  bytes_written  = 0;  	/* Value for storing the number of bytes written to the port */

		bytes_written = write(fd,array_name,array_size);
										 /* use write() to send data to port                                            */
										 /* "fd"                   - file descriptor pointing to the opened serial port */
										 /*	"write_buffer"         - address of the buffer containing data	            */
										 /* "sizeof(write_buffer)" - No of bytes to write*/
		if(bytes_written != array_size) return SERIAL_WRITE_FAIL;
		else if(bytes_written == array_size) return SERIAL_WRITE_SUCCESS;
}


uint8_t serial_read(uint8_t *array_name, uint8_t array_size)
{
	int  bytes_read = 0;    /* Number of bytes read by the read() system call */
	int timeout_flag = 0; // no timeout occured
	int fdt = 0, sret;
	fd_set readfds;
	struct timeval timeout;
  //bytes_read = (fd,array_name,1);

	bytes_read = read(fd,array_name,array_size); // Read the data
	//wait for the byte to come in
	/*while(array_size != 0) //read the number of incoming bytes
	{
		FD_ZERO(&readfds);
		FD_SET(fdt, &readfds);
		timeout.tv_usec = 10000; //10 millisecond timeout
		sret = select(8, &readfds, NULL, NULL, &timeout);

			while((bytes_read = read(fd,array_name,1)) == 0) //wait till zero bytes have been read
			{
				if(sret == 0)
				{
					timeout_flag = 1;
					break;
				}
			}
			if(timeout_flag == 1) break;
			array_size--;
	}*/
	if(timeout_flag == 1) return SERIAL_TIMEOUT_ERROR;
	else if(bytes_read != array_size) return SERIAL_READ_FAIL;
	else if((bytes_read == array_size) && (timeout_flag == 0)) return SERIAL_READ_SUCCESS;
}


void close_serial_port(void)
{
    	close(fd);
      printf("Closed Serial Port USB:ttyACM0\n");
}
