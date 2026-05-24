#ifndef RIFTOS_PANIC_H
#define RIFTOS_PANIC_H

//halt the kernel when boot cannot continue safely
void panic(const char *message);

#endif
