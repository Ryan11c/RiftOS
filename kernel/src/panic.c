#include "panic.h"

//stop the machine when the kernel reaches a state it cannot recover from
//the message is kept for the future console/serial logger
void panic(const char *message) {
    (void)message;

    //disable interrupts and halt forever so the failure does not keep running
    for (;;) {
        __asm__ volatile ("cli; hlt");
    }
}
