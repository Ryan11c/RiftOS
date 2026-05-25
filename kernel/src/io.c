#include "io.h"

//read a byte from a port-mapped device register
uint8_t inb(uint16_t port) {
    uint8_t value;

    __asm__ volatile ("inb %1, %0" : "=a"(value) : "Nd"(port));

    return value;
}

//write a byte to a port-mapped device register
void outb(uint16_t port, uint8_t value) {
    __asm__ volatile ("outb %0, %1" : : "a"(value), "Nd"(port));
}
