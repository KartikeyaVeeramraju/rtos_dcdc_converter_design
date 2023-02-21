/*
 * Manager.h
 *
 *  Created on: Apr 8, 2020
 *      Author: kartikeya
 */

#ifndef INC_MANAGER_H_
#define INC_MANAGER_H_
#include <stdint.h>
#include <stdbool.h>
#define TRANSMIT 0
#define RECEIVE 1

#define RESOLVED 1
#define NOT_RESOLVED 0

#define PING 0x00
#define V33_TOGGLE      0x01
#define V25_TOGGLE      0x02
#define HEALTH 0x03
#define DUTY33 0x04
#define DUTY25 0x05

#define PING_RESP 0xAA
#define Con_ON 0x01
#define Con_OFF 0x00

//update system function defines
#define startcon1 1
#define stopcon1 2
#define startcon2 3
#define stopcon2 4
#define duty1 5
#define duty2 6

//status register bit fields
#define status_bit (1<<0)
#define power_good_bit (1<<1)
#define alert_bit (1<<2)
#define overtempretaure_bit (1<<3)
#define controller_status_bit (1<<4)


uint8_t command_resolve(uint8_t commmand, uint8_t *buf, uint8_t *size_var, uint8_t *resolve_status);
void update_system(uint8_t command);
//uint8_t update_status(command);



void Manager_init(void);

#endif /* INC_MANAGER_H_ */
