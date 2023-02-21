#ifndef COMMANDS_H
#define COMMANDS_H

#include <stdio.h>
#include <stdint.h>
#include <unistd.h>

#define V3_3 1
#define V2_5 2

#define ON 1
#define OFF 0

#define PING            0x00
#define V33_TOGGLE      0x01
#define V25_TOGGLE      0x02
#define HEALTH 0x03
#define DUTY33 0x04
#define DUTY25 0x05
#define CONTROLLER1_TOGGLE 0x06
#define CONTROLLER2_TOGGLE 0x07

#define PING_RESPONSE   0xAA

//Status Register 1 Bits
#define v33_bit (1<<0)
#define v25_bit (1<<1)
#define v33_controller (1<<2)
#define v25_controller (1<<3)
#define v33_drv_good (1<<4)
#define v25_drv_good (1<<5)
#define v33_short (1<<6)
#define v25_short (1<<7)
//Status Register 2 Bits
#define v33_MOSFET_ov_temp (1<<0)
#define v25_MOSFET_ov_temp (1<<0)


int converter_init(void);

int ping(void);

void exit_ser(void);

void con_health_check(void);

int Con_Toggle(int select, int converter);

uint8_t Duty_change(uint8_t converter, uint16_t value);

void start_stream(void);

int Controller_Toggle(int select, int converter);


#endif
