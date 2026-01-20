CC = x86_64-elf-gcc
LD = x86_64-elf-ld
NASM = nasm
QEMU = qemu-system-i386

CFLAGS = -m32 -ffreestanding
ASFLAGS = -f elf32
LDFLAGS = -m elf_i386 -T linker.ld

all: myos.bin

myos.bin: boot.o kernel.o linker.ld
	$(LD) $(LDFLAGS) boot.o kernel.o -o myos.bin

kernel.o: kernel.c
	$(CC) $(CFLAGS) -c kernel.c -o kernel.o

boot.o: boot.s
	$(NASM) $(ASFLAGS) boot.s -o boot.o

run: myos.bin
	$(QEMU) -kernel myos.bin

clean:
	rm -f *.o myos.bin
