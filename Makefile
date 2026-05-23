CC := x86_64-elf-gcc
LD := x86_64-elf-gcc

CFLAGS := -std=gnu11 -ffreestanding -fno-stack-protector -fno-pic -mno-red-zone -mcmodel=kernel -Wall -Wextra -O2 -Ikernel/include -Ilimine
LDFLAGS := -T linker.ld -nostdlib -static -z max-page-size=0x1000

KERNEL := kernel.elf
ISO := RiftOS.iso

SRC := $(wildcard kernel/src/*.c)
OBJ := $(SRC:.c=.o)

all: $(ISO)

%.o: %.c Makefile
	$(CC) $(CFLAGS) -c $< -o $@

$(KERNEL): $(OBJ)
	$(LD) $(LDFLAGS) $^ -o $@

limine/limine:
	$(MAKE) -C limine

$(ISO): $(KERNEL) limine/limine
	rm -rf iso_root
	mkdir -p iso_root/boot/limine
	cp $(KERNEL) iso_root/boot/kernel.elf
	cp limine.conf iso_root/boot/limine/limine.conf
	cp limine/limine-bios.sys iso_root/boot/limine/
	cp limine/limine-bios-cd.bin iso_root/boot/limine/
	cp limine/limine-uefi-cd.bin iso_root/boot/limine/
	mkdir -p iso_root/EFI/BOOT
	cp limine/BOOTX64.EFI iso_root/EFI/BOOT/
	cp limine/BOOTIA32.EFI iso_root/EFI/BOOT/
	xorriso -as mkisofs -b boot/limine/limine-bios-cd.bin \
		-no-emul-boot -boot-load-size 4 -boot-info-table \
		--efi-boot boot/limine/limine-uefi-cd.bin \
		-efi-boot-part --efi-boot-image --protective-msdos-label \
		iso_root -o $(ISO)
	./limine/limine bios-install $(ISO)

run: $(ISO)
	qemu-system-x86_64 -cdrom $(ISO)

clean:
	rm -rf $(OBJ) *.o *.elf *.iso iso_root

.PHONY: all run clean
