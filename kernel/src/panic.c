#include "panic.h"
#include "serial.h"

//stop the machine when the kernel reaches a state it cannot recover from
//write the message to serial first so early boot failures are visible
void panic(const char *message) {
    serial_write_string("KERNEL PANIC: ");

    if (message != 0) {
        serial_write_string(message);
    }

    serial_write_string("\n");

    //disable interrupts and halt forever so the failure does not keep running
    for (;;) {
        __asm__ volatile ("cli; hlt");
    }
}
