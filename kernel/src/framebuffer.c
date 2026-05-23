#include "framebuffer.h"

uint32_t framebuffer_rgb(struct limine_framebuffer *framebuffer, uint8_t red, uint8_t green, uint8_t blue) {
    return ((uint32_t)red << framebuffer->red_mask_shift) |
           ((uint32_t)green << framebuffer->green_mask_shift) |
           ((uint32_t)blue << framebuffer->blue_mask_shift);
}

void framebuffer_put_pixel(struct limine_framebuffer *framebuffer, uint64_t x, uint64_t y, uint32_t color) {
    if (x >= framebuffer->width || y >= framebuffer->height || framebuffer->bpp != 32) {
        return;
    }

    uint32_t *pixel = (uint32_t *)((uint8_t *)framebuffer->address + y * framebuffer->pitch + x * 4);
    *pixel = color;
}

void framebuffer_clear(struct limine_framebuffer *framebuffer, uint32_t color) {
    for (uint64_t y = 0; y < framebuffer->height; y++) {
        for (uint64_t x = 0; x < framebuffer->width; x++) {
            framebuffer_put_pixel(framebuffer, x, y, color);
        }
    }
}
