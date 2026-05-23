#ifndef RIFTOS_FRAMEBUFFER_H
#define RIFTOS_FRAMEBUFFER_H

#include <stdint.h>

#include <limine.h>

void framebuffer_clear(struct limine_framebuffer *framebuffer, uint32_t color);
void framebuffer_put_pixel(struct limine_framebuffer *framebuffer, uint64_t x, uint64_t y, uint32_t color);
uint32_t framebuffer_rgb(struct limine_framebuffer *framebuffer, uint8_t red, uint8_t green, uint8_t blue);

#endif
