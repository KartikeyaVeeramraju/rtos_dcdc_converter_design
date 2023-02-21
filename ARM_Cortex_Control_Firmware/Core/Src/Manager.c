/*
 * Manager.c
 *
 *  Created on: Apr 8, 2020
 *      Author: kartikeya
 */
#include "main.h"
#include "stm32f4xx_it.h"
#include <stdint.h>
#include <stdbool.h>
#include "Manager.h"
#include "PWM.h"

#define v33 1
#define v25 2

void set_bit_STAREG(uint8_t byte_value, uint8_t conname);
void clear_bit_STAREG(uint8_t byte_value, uint8_t conname);

struct conpar
{
	//general parameters
	uint16_t duty; //duty value
	uint16_t vsensed; //adc sampled voltage
	uint16_t temperature; //adc sampled MOSFET temperature
	uint16_t frequency; //converter switching frequency

	//status register bits
	bool status; //active or inactive
	bool powergood; //gate driver status
	bool alert; //INA300 over current alert signal
	bool overtemperature; //MOSFET temperature bits
	bool controller; //controller status

	uint8_t status_reg; //status register

	//INA300 control bits (not a part of status register)
	bool enable; //INA300 current sense IC Enable after latch
	bool latch; //IAN300 current sense IC latch


	//controller parameters
	uint16_t setpoint; //set point of the target voltage
	uint16_t error;
	uint16_t controller_effort;

};

uint8_t i;
uint8_t ret;

struct conpar converter1;
struct conpar converter2;



/*
 * Description: Takes the incoming command and acts accordingly; fills up the buffer passed by the calling function
   and responds by filling up the buffer with the required data bytes
 * input1: incoming command from serial receive function
 * input2: pointer to UART buffer
 * input3: pointer to a location in the scope of the calling function's file that stores the size of
 * 		   buffer transfer length
 * output: next mode of UART; Receive or Transmit Data (for accommodating complex transeive schemes)
 *
 * */

uint8_t command_resolve(uint8_t cmd, uint8_t *buf, uint8_t *size_var, uint8_t *resolve_status)
{
	//resolve the ping function
	//this is a single read single write function
	//transfer 0xAA (ping response if this is received)
	if(cmd == PING)
	{
		if(i == 1)
		{
			buf[0] = PING_RESP;
			*size_var = 1; //only one byte to transfer next
			ret = TRANSMIT;
			*resolve_status = NOT_RESOLVED; //end of command handling
			i++;
		}
		else if(i == 2)
		{
			//this is being called from TXcpltcallback
			ret = RECEIVE;
			*resolve_status = RESOLVED; //end of command handling
			i = 1;
		}
	}

	//resolve the V33_toggle
	//this is a 2 byte sequence so wait for the next byte to arrive
	//first byte receive is the command identifier
	//second byte receive is the action command
	if(cmd == V33_TOGGLE)
	{
		if(i == 1)
		{

			i++;
			ret = RECEIVE;
			*resolve_status = NOT_RESOLVED; //end of command handling
		}
		else if(i == 2)
		{
			//buffer now has either a switch on r switch off command
			i = 1; //reset index
			//buf[0] has the incoming subcommand
			if(buf[0] == Con_ON)
			{
				update_system(startcon1);
			}
			else if(buf[0] == Con_OFF)
			{
				update_system(stopcon1);
			}
			*resolve_status = RESOLVED; //end of command handling
		}

	}

		//resolve the V25_toggle
		//this is a 2 byte sequence so wait for the next byte to arrive
		//first byte receive is the command identifier
		//second byte receive is the action command
		if(cmd == V25_TOGGLE)
		{
			if(i == 1)
			{

				i++;
				ret = RECEIVE;
				*resolve_status = NOT_RESOLVED;
			}
			else if(i == 2)
			{
				//buffer now has either a switch on r switch off command
				i = 1; //reset index
				//buf[0] has the incoming subcommand
				if(buf[0] == Con_ON)
				{
					update_system(startcon2);
				}
				else if(buf[0] == Con_OFF)
				{
					update_system(stopcon2);
				}
				*resolve_status = RESOLVED;
			}

		}

		//this is a one byte read and two transmit sequence
		//first instance is the command identifier
		//second instance initiates the buffer load and starts the UART transmit with the
		//two status registers of the converters
		if(cmd == HEALTH)
		{
			if(i == 1)
			{
				buf[0] = converter1.status_reg;
				buf[1] = converter2.status_reg;
				*size_var = 2; //only two bytes to transfer next
				ret = TRANSMIT;
				*resolve_status = NOT_RESOLVED; //end of command handling
				i++;
			}
			else if(i == 2)
			{
				//this is being called from TXcpltcallback
				ret = RECEIVE;
				*resolve_status = RESOLVED; //end of command handling
				i = 1;
			}

		}
		//duty override for converter33
		//This is a two byte read sequence similar to toggle
		else if(cmd == DUTY33)
			{
				if(i == 1)
				{

					i++;
					ret = RECEIVE;
					*resolve_status = NOT_RESOLVED; //end of command handling
				}
				else if(i == 2)
				{
					i++; //reset index
					ret = RECEIVE;
					converter1.duty = buf[0];
					converter1.duty = converter1.duty<<8;
					*resolve_status = NOT_RESOLVED; //end of command handling
				}
				else if(i == 3)
				{
					i = 1;
					ret = RECEIVE;
					converter1.duty = (converter1.duty | buf[0]);
					//update_duty(V3_3, converter1.duty);
					update_system(duty1);
					*resolve_status = RESOLVED;
				}

			}
		else if(cmd == DUTY25)
					{
						if(i == 1)
						{
							i++;
							ret = RECEIVE;
							*resolve_status = NOT_RESOLVED; //end of command handling
						}
						else if(i == 2)
						{
							i++; //reset index
							ret = RECEIVE;
							converter2.duty = buf[0];
							converter2.duty = converter2.duty<<8;
							*resolve_status = NOT_RESOLVED; //end of command handling
						}
						else if(i == 3)
						{
							i = 1;
							ret = RECEIVE;
							converter2.duty = (converter2.duty | buf[0]);
							//update_duty(V2_5, converter2.duty);
							update_system(duty2);
							*resolve_status = RESOLVED;
						}

					}

	return ret;
}

void Manager_init(void)
{
	i = 1; //initialize index
}

/***********************************************************************
DESC: Sets a particular bit in the status register. Uses read-modify-write type instructions.
INPUT: Bit value to be set (See status register bits section in PPU_manager.h for details)
RETURNS: Nothing
CAUTION:
************************************************************************/
void set_bit_STAREG(uint8_t byte_value, uint8_t conname)
{
	if(conname == v33)
	{
		converter1.status_reg = (converter1.status_reg | byte_value);
	}
	else if(conname == v25)
	{
		converter2.status_reg = (converter2.status_reg | byte_value);
	}

}

/***********************************************************************
DESC: Clears a particular bit in the status register. Uses read-modify-write type instructions.
INPUT: Bit value to be cleared (See status register bits section in PPU_manager.h for details)
RETURNS: Nothing
CAUTION:
************************************************************************/
void clear_bit_STAREG(uint8_t byte_value, uint8_t conname)
{

    	if(conname == v33)
    	{
    		converter1.status_reg = (converter1.status_reg & (~byte_value));
    	}
    	else if(conname == v25)
    	{
    		converter2.status_reg = (converter2.status_reg & (~byte_value));
    	}
}

void update_system(uint8_t command)
{
	switch(command)
	{

		case startcon1:
					{
						start_pwm(V3_3);
						set_bit_STAREG(status_bit, v33);
						converter1.status = true;
						break;
					}
		case stopcon1:
					{
						stop_pwm(V3_3);
						clear_bit_STAREG(status_bit, v33);
						converter1.status = false;
						break;
					}
		case startcon2:
					{
						start_pwm(V2_5);
						set_bit_STAREG(status_bit, v25);
						converter2.status = true;
						break;
					}
		case stopcon2:
					{
						stop_pwm(V2_5);
						clear_bit_STAREG(status_bit, v25);
						converter2.status = false;
						break;
					}
		case duty1:
					{
						update_duty(V3_3, converter1.duty);
						converter1.controller = false;
						clear_bit_STAREG(controller_status_bit, v33);
						break;
					}
		case duty2:
					{
						update_duty(V2_5, converter2.duty);
						converter2.controller = false;
						clear_bit_STAREG(controller_status_bit, v25);
						break;
					}
	}
}

