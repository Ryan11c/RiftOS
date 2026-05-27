#include <stdint.h>

#include "io.h"
#include "serial.h"

#define SERIAL_COM1 0x3f8

#define SERIAL_DATA_REGISTER 0
#define SERIAL_INTERRUPT_ENABLE_REGISTER 1
#define SERIAL_FIFO_CONTROL_REGISTER 2
#define SERIAL_LINE_CONTROL_REGISTER 3
#define SERIAL_MODEM_CONTROL_REGISTER 4
#define SERIAL_LINE_STATUS_REGISTER 5

#define SERIAL_LINE_ENABLE_DLAB 0x80
#define SERIAL_LINE_8_BITS_NO_PARITY_ONE_STOP 0x03
#define SERIAL_FIFO_ENABLE_CLEAR_14_BYTE 0xc7
#define SERIAL_MODEM_DTR_RTS_OUT2 0x0b
#define SERIAL_TRANSMIT_EMPTY 0x20

//set up com1 as a simple polling serial port at 38400 baud
void serial_init(void) {
    outb(SERIAL_COM1 + SERIAL_INTERRUPT_ENABLE_REGISTER, 0x00);
    outb(SERIAL_COM1 + SERIAL_LINE_CONTROL_REGISTER, SERIAL_LINE_ENABLE_DLAB);
    outb(SERIAL_COM1 + SERIAL_DATA_REGISTER, 0x03);
    outb(SERIAL_COM1 + SERIAL_INTERRUPT_ENABLE_REGISTER, 0x00);
    outb(SERIAL_COM1 + SERIAL_LINE_CONTROL_REGISTER, SERIAL_LINE_8_BITS_NO_PARITY_ONE_STOP);
    outb(SERIAL_COM1 + SERIAL_FIFO_CONTROL_REGISTER, SERIAL_FIFO_ENABLE_CLEAR_14_BYTE);
    outb(SERIAL_COM1 + SERIAL_MODEM_CONTROL_REGISTER, SERIAL_MODEM_DTR_RTS_OUT2);
}

//wait until the uart can accept another byte
static int serial_can_transmit(void) {
    return (inb(SERIAL_COM1 + SERIAL_LINE_STATUS_REGISTER) & SERIAL_TRANSMIT_EMPTY) != 0;
}

//write one raw byte to com1
static void serial_write_byte(char character) {
    while (!serial_can_transmit()) {
    }
    outb(SERIAL_COM1 + SERIAL_DATA_REGISTER, (uint8_t)character);
}

//write one character, expanding newlines for serial terminals
void serial_write_char(char character) {
    if (character == '\n') {
        serial_write_byte('\r');
    }
    serial_write_byte(character);
}

//write a string one character at a time so early logs stay simple
void serial_write_string(const char *string) {
    for (const char *character = string; *character != '\0'; character++) {
        serial_write_char(*character);
    }
}
