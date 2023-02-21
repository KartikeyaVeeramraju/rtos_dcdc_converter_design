#ifndef SERIAL_H
#define SERIAL_H

#include <stdint.h>

void serial_init(void);

//Warning!!!: Size of arrays restricted to 255!
uint8_t serial_write(uint8_t *array_name, uint8_t array_size);

uint8_t serial_read(uint8_t *array_name, uint8_t array_size);

void close_serial_port(void);

#endif
