ENTRY(start)

SECTIONS {
    /* The BIOS loads the kernel at 1MB */
    . = 1M;

    /* FORCE the Multiboot header to be at the very start */
    .multiboot : {
        KEEP(*(.multiboot))
    }

    /* Then put the code */
    .text : {
        *(.text)
    }

    /* Read-only data */
    .rodata : {
        *(.rodata)
    }

    /* Read-write data */
    .data : {
        *(.data)
    }

    /* Stack and empty variables */
    .bss : {
        *(.bss)
    }
}