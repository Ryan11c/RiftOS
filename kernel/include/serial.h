#ifndef RIFTOS_SERIAL_H
#define RIFTOS_SERIAL_H

//set up the first x86 serial port for early kernel logs
void serial_init(void);

//write one character to the serial port
void serial_write_char(char character);

//write a null-terminated string to the serial port
void serial_write_string(const char *string);

#endif
