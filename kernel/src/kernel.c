#include <stdint.h>

#include <limine.h>

#include "framebuffer.h"
#include "panic.h"

//limine finds boot requests by scanning these special sections in the kernel
//image. keeping the start marker first makes the layout easier to eyeball and
//matches the order limine documents
__attribute__((used, section(".limine_requests_start")))
static volatile LIMINE_REQUESTS_START_MARKER;

//ask limine for protocol base revision 2 so the request structures match
__attribute__((used, section(".limine_requests")))
static volatile LIMINE_BASE_REVISION(2);

//for now the framebuffer is the whole "console", so this is the one request we need
__attribute__((used, section(".limine_requests")))
static volatile struct limine_framebuffer_request framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST,
    .revision = 0
};

__attribute__((used, section(".limine_requests_end")))
static volatile LIMINE_REQUESTS_END_MARKER;

//draw the first real thing riftos can say on screen
static void draw_boot_screen(struct limine_framebuffer *framebuffer) {
    uint32_t background = framebuffer_rgb(framebuffer, 15, 17, 23);
    uint32_t highlight = framebuffer_rgb(framebuffer, 240, 244, 248);

    framebuffer_clear(framebuffer, background);
    framebuffer_draw_string(framebuffer, 32, 32, "RiftOS", highlight, 4);
    framebuffer_draw_string(framebuffer, 32, 80, "Hello, world.", highlight, 4);
}

//kernel entry point. limine jumps here after loading the elf
void kernel_main(void) {
    if (!LIMINE_BASE_REVISION_SUPPORTED) {
        panic("Limine base revision is unsupported");
    }

    if (framebuffer_request.response == 0 || framebuffer_request.response->framebuffer_count < 1) {
        panic("No framebuffer was provided by Limine");
    }

    struct limine_framebuffer *framebuffer = framebuffer_request.response->framebuffers[0];
    draw_boot_screen(framebuffer);

    //nothing else exists yet, so park the cpu instead of returning into nowhere
    for (;;) {
        __asm__ volatile ("hlt");
    }
}
