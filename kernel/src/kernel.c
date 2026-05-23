#include <stdint.h>

#include <limine.h>

#include "framebuffer.h"
#include "panic.h"

__attribute__((used, section(".limine_requests")))
static volatile LIMINE_BASE_REVISION(2);

__attribute__((used, section(".limine_requests")))
static volatile struct limine_framebuffer_request framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST,
    .revision = 0
};

__attribute__((used, section(".limine_requests_start")))
static volatile LIMINE_REQUESTS_START_MARKER;

__attribute__((used, section(".limine_requests_end")))
static volatile LIMINE_REQUESTS_END_MARKER;

static void draw_boot_screen(struct limine_framebuffer *framebuffer) {
    uint32_t background = framebuffer_rgb(framebuffer, 15, 17, 23);
    uint32_t accent = framebuffer_rgb(framebuffer, 70, 180, 150);
    uint32_t highlight = framebuffer_rgb(framebuffer, 240, 244, 248);

    framebuffer_clear(framebuffer, background);

    uint64_t center_x = framebuffer->width / 2;
    uint64_t center_y = framebuffer->height / 2;
    uint64_t size = framebuffer->height / 5;

    for (uint64_t y = center_y - size; y < center_y + size; y++) {
        for (uint64_t x = center_x - size; x < center_x + size; x++) {
            uint64_t local_x = x - (center_x - size);
            uint64_t local_y = y - (center_y - size);

            if (local_x < size / 3 || local_y < size / 3 || local_x > size + local_y / 2) {
                framebuffer_put_pixel(framebuffer, x, y, accent);
            }
        }
    }

    for (uint64_t y = center_y + size + 24; y < center_y + size + 32; y++) {
        for (uint64_t x = center_x - size; x < center_x + size; x++) {
            framebuffer_put_pixel(framebuffer, x, y, highlight);
        }
    }
}

void kernel_main(void) {
    if (!LIMINE_BASE_REVISION_SUPPORTED) {
        panic("Limine base revision is unsupported");
    }

    if (framebuffer_request.response == 0 || framebuffer_request.response->framebuffer_count < 1) {
        panic("No framebuffer was provided by Limine");
    }

    struct limine_framebuffer *framebuffer = framebuffer_request.response->framebuffers[0];
    draw_boot_screen(framebuffer);

    for (;;) {
        __asm__ volatile ("hlt");
    }
}
