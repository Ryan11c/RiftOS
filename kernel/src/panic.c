#include "panic.h"

void panic(const char *message) {
    (void)message;

    for (;;) {
        __asm__ volatile ("cli; hlt");
    }
}
