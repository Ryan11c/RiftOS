#include "framebuffer.h"

#define FONT_WIDTH 5
#define FONT_HEIGHT 7

//a visible box for characters i have not drawn into the tiny font yet
static const uint8_t unknown_glyph[FONT_HEIGHT] = {
    0x1f,
    0x11,
    0x15,
    0x15,
    0x11,
    0x1f,
    0x00
};

//tiny hand-made 5x7 font. it only covers the characters used by the first
//boot message, which keeps the early kernel small while the console grows up
static const uint8_t *font_glyph(char character) {
    static const uint8_t space[FONT_HEIGHT] = {0};
    static const uint8_t comma[FONT_HEIGHT] = {
        0x00, 0x00, 0x00, 0x00, 0x0c, 0x04, 0x08
    };
    static const uint8_t period[FONT_HEIGHT] = {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x0c
    };
    static const uint8_t uppercase_h[FONT_HEIGHT] = {
        0x11, 0x11, 0x11, 0x1f, 0x11, 0x11, 0x11
    };
    static const uint8_t uppercase_o[FONT_HEIGHT] = {
        0x0e, 0x11, 0x11, 0x11, 0x11, 0x11, 0x0e
    };
    static const uint8_t uppercase_r[FONT_HEIGHT] = {
        0x1e, 0x11, 0x11, 0x1e, 0x14, 0x12, 0x11
    };
    static const uint8_t uppercase_s[FONT_HEIGHT] = {
        0x0f, 0x10, 0x10, 0x0e, 0x01, 0x01, 0x1e
    };
    static const uint8_t lowercase_d[FONT_HEIGHT] = {
        0x01, 0x01, 0x0d, 0x13, 0x11, 0x13, 0x0d
    };
    static const uint8_t lowercase_e[FONT_HEIGHT] = {
        0x00, 0x00, 0x0e, 0x11, 0x1f, 0x10, 0x0e
    };
    static const uint8_t lowercase_f[FONT_HEIGHT] = {
        0x06, 0x08, 0x08, 0x1c, 0x08, 0x08, 0x08
    };
    static const uint8_t lowercase_i[FONT_HEIGHT] = {
        0x04, 0x00, 0x0c, 0x04, 0x04, 0x04, 0x0e
    };
    static const uint8_t lowercase_l[FONT_HEIGHT] = {
        0x0c, 0x04, 0x04, 0x04, 0x04, 0x04, 0x0e
    };
    static const uint8_t lowercase_o[FONT_HEIGHT] = {
        0x00, 0x00, 0x0e, 0x11, 0x11, 0x11, 0x0e
    };
    static const uint8_t lowercase_r[FONT_HEIGHT] = {
        0x00, 0x00, 0x16, 0x19, 0x10, 0x10, 0x10
    };
    static const uint8_t lowercase_t[FONT_HEIGHT] = {
        0x08, 0x08, 0x1c, 0x08, 0x08, 0x09, 0x06
    };
    static const uint8_t lowercase_w[FONT_HEIGHT] = {
        0x00, 0x00, 0x11, 0x11, 0x15, 0x15, 0x0a
    };

    switch (character) {
    case ' ':
        return space;
    case ',':
        return comma;
    case '.':
        return period;
    case 'H':
        return uppercase_h;
    case 'O':
        return uppercase_o;
    case 'R':
        return uppercase_r;
    case 'S':
        return uppercase_s;
    case 'd':
        return lowercase_d;
    case 'e':
        return lowercase_e;
    case 'f':
        return lowercase_f;
    case 'i':
        return lowercase_i;
    case 'l':
        return lowercase_l;
    case 'o':
        return lowercase_o;
    case 'r':
        return lowercase_r;
    case 't':
        return lowercase_t;
    case 'w':
        return lowercase_w;
    default:
        return unknown_glyph;
    }
}

//pack normal 8-bit rgb values into whatever channel layout limine gave us
//qemu usually gives 32bpp rgb, but using the masks keeps this less brittle
uint32_t framebuffer_rgb(struct limine_framebuffer *framebuffer, uint8_t red, uint8_t green, uint8_t blue) {
    uint32_t red_value = red >> (8 - framebuffer->red_mask_size);
    uint32_t green_value = green >> (8 - framebuffer->green_mask_size);
    uint32_t blue_value = blue >> (8 - framebuffer->blue_mask_size);

    return (red_value << framebuffer->red_mask_shift) |
           (green_value << framebuffer->green_mask_shift) |
           (blue_value << framebuffer->blue_mask_shift);
}

//put one pixel on screen, accepting both common 24bpp and 32bpp framebuffers
void framebuffer_put_pixel(struct limine_framebuffer *framebuffer, uint64_t x, uint64_t y, uint32_t color) {
    if (x >= framebuffer->width || y >= framebuffer->height) {
        return;
    }

    uint16_t bytes_per_pixel = framebuffer->bpp / 8;

    if (bytes_per_pixel < 3 || bytes_per_pixel > 4) {
        return;
    }

    uint8_t *pixel = (uint8_t *)framebuffer->address + y * framebuffer->pitch + x * bytes_per_pixel;

    for (uint16_t byte = 0; byte < bytes_per_pixel; byte++) {
        pixel[byte] = (uint8_t)(color >> (byte * 8));
    }
}

//fill the whole framebuffer with a single color. slow but simple is fine here
void framebuffer_clear(struct limine_framebuffer *framebuffer, uint32_t color) {
    for (uint64_t y = 0; y < framebuffer->height; y++) {
        for (uint64_t x = 0; x < framebuffer->width; x++) {
            framebuffer_put_pixel(framebuffer, x, y, color);
        }
    }
}

//draw one scaled bitmap character at an exact pixel position
void framebuffer_draw_char(struct limine_framebuffer *framebuffer, uint64_t x, uint64_t y, char character, uint32_t color, uint8_t scale) {
    if (scale == 0) {
        return;
    }

    const uint8_t *glyph = font_glyph(character);

    for (uint8_t row = 0; row < FONT_HEIGHT; row++) {
        for (uint8_t col = 0; col < FONT_WIDTH; col++) {
            if ((glyph[row] & (1 << (FONT_WIDTH - 1 - col))) == 0) {
                continue;
            }

            for (uint8_t dy = 0; dy < scale; dy++) {
                for (uint8_t dx = 0; dx < scale; dx++) {
                    framebuffer_put_pixel(framebuffer, x + col * scale + dx, y + row * scale + dy, color);
                }
            }
        }
    }
}

//draw a simple string with newline support. this is the seed of a real console later
void framebuffer_draw_string(struct limine_framebuffer *framebuffer, uint64_t x, uint64_t y, const char *string, uint32_t color, uint8_t scale) {
    if (scale == 0) {
        return;
    }

    uint64_t cursor_x = x;
    uint64_t cursor_y = y;
    uint64_t advance_x = (FONT_WIDTH + 1) * scale;
    uint64_t advance_y = (FONT_HEIGHT + 2) * scale;

    for (const char *character = string; *character != '\0'; character++) {
        if (*character == '\n') {
            cursor_x = x;
            cursor_y += advance_y;
            continue;
        }

        framebuffer_draw_char(framebuffer, cursor_x, cursor_y, *character, color, scale);
        cursor_x += advance_x;
    }
}
