#ifndef RIFTOS_FRAMEBUFFER_H
#define RIFTOS_FRAMEBUFFER_H

#include <stdint.h>

#include <limine.h>

//paint every pixel in the framebuffer with one color
void framebuffer_clear(struct limine_framebuffer *framebuffer, uint32_t color);

//draw one character from the built-in bitmap font
void framebuffer_draw_char(struct limine_framebuffer *framebuffer, uint64_t x, uint64_t y, char character, uint32_t color, uint8_t scale);

//draw a null-terminated string, including basic newline handling
void framebuffer_draw_string(struct limine_framebuffer *framebuffer, uint64_t x, uint64_t y, const char *string, uint32_t color, uint8_t scale);

//write one pixel if the coordinates are inside the framebuffer
void framebuffer_put_pixel(struct limine_framebuffer *framebuffer, uint64_t x, uint64_t y, uint32_t color);

//convert 8-bit red/green/blue values into the framebuffer's native layout
uint32_t framebuffer_rgb(struct limine_framebuffer *framebuffer, uint8_t red, uint8_t green, uint8_t blue);

#endif
