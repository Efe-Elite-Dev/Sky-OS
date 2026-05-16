CC = gcc
AS = nasm
LDFLAGS = -m32 -nostdlib -nodefaultlibs -T linker.ld
CFLAGS = -m32 -c -ffreestanding -O2 -Wall -Wextra -I.

OBJS = boot.o sky_subsystem.o kernel.o mouse.o screen.o gui.o ai_subsystem.o deb_subsystem.o exe_subsystem.o keyboard.o idt.o

all: skyos.iso

boot.o: boot.asm
	$(AS) -f elf32 boot.asm -o boot.o

%.o: %.c
	$(CC) $(CFLAGS) $< -o $@

kernel.bin: $(OBJS)
	ld $(LDFLAGS) $(OBJS) -o kernel.bin

skyos.iso: kernel.bin
	mkdir -p iso_root/boot/grub
	cp kernel.bin iso_root/boot/
	@if [ ! -f iso_root/boot/grub/grub.cfg ]; then \
		echo 'menuentry "SKY OS Core" {' > iso_root/boot/grub/grub.cfg; \
		echo '    multiboot /boot/kernel.bin' >> iso_root/boot/grub/grub.cfg; \
		echo '    boot' >> iso_root/boot/grub/grub.cfg; \
		echo '}' >> iso_root/boot/grub/grub.cfg; \
	fi
	genisoimage -R -b boot/grub/stage2_eltorito -no-emul-boot -boot-load-size 4 -boot-info-table -o skyos.iso iso_root 2>/dev/null || \
	grub-mkrescue -o skyos.iso iso_root || \
	genisoimage -R -o skyos.iso iso_root

clean:
	rm -rf *.o kernel.bin skyos.iso iso_root
