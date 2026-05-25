#ifndef RIFTOS_IO_H
#define RIFTOS_IO_H

#include <stdint.h>

//read one byte from an x86 I/O port
uint8_t inb(uint16_t port);

//write one byte to an x86 I/O port
void outb(uint16_t port, uint8_t value);

#endif
