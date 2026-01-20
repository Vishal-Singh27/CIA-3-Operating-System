# MyOS: A Bare-Metal x86 Kernel (CIA 3 Assignment)

**Subject:** Operating Systems
**Assignment:** CIA 3 - Kernel Development Project
**Date:** January 2026

## 👥 Project Team
1.  **Vishal Singh**
2.  **Tanvi Sheoran**
3.  **Yasmin Devargattu**
4.  **Angela Anthon Antonny**

---

## 📖 1. Abstract
This project implements a minimal, monolithic operating system kernel from scratch. Unlike standard application development, this kernel runs **bare-metal**, meaning it executes directly on the hardware without an underlying OS. It demonstrates core operating system concepts including **Multiboot compliant booting**, **Direct Memory Access (DMA)** for video output, and **Port-Mapped I/O (PMIO)** for keyboard polling.

The kernel is written in **C** (for logic) and **x86 Assembly** (for hardware interaction), compiled using a cross-compiler toolchain, and emulated in QEMU.

## 🚀 2. Features
* **Multiboot Compliance:** Contains the magic header `0x1BADB002` required to be loaded by GRUB.
* **VGA Text Mode Driver:** Direct manipulation of video memory at address `0xB8000`.
* **Polling-Based Keyboard Driver:** Reads raw scancodes from the PS/2 controller (Port `0x60`).
* **Interactive Shell:**
    * **Boot Cleanup:** Clears BIOS artifacts from video memory upon startup.
    * **Scancode Translation:** Maps hardware signals to ASCII characters.
    * **Visual Output:** Supports colored text (Green/White on Black).

---

## 💻 3. Technical Implementation Details

### 3.1 The Boot Process (Assembly Stub)
The BIOS transfers control to the bootloader (GRUB), which searches for a valid **Multiboot Header** in the first 8KB of our binary.
* **Magic Number:** `0x1BADB002` (Identifies the file as a kernel).
* **Checksum:** `-(Magic + Flags)` (Ensures data integrity).
Once verified, the CPU is switched to **32-bit Protected Mode**, and control is jumped to the `kernel_main` function in C.

### 3.2 Memory Map & Linker Script
Standard applications start at address `0x0`, but an OS kernel cannot. The BIOS and memory-mapped hardware occupy the lower 1MB of RAM.
* **Linker Directive:** `. = 1M;` inside `linker.ld`.
* **Why 1MB?** This address (`0x100000`) is the standard load address for kernels to avoid colliding with the BIOS interrupt vector table (IVT) and BIOS data area (BDA).

### 3.3 VGA Video Memory (`0xB8000`)
We do not use `printf` because there is no underlying OS to handle it. Instead, we write directly to the video card's memory buffer.
* **Address:** `0xB8000`
* **Cell Structure:** Each character on the 80x25 screen takes up **2 bytes**:
    1.  **Byte 0:** The ASCII Character (e.g., 'A').
    2.  **Byte 1:** The Attribute Byte (Color).
        * `0x0F` = White Text on Black Background.
        * `0x02` = Green Text on Black Background.

### 3.4 Keyboard Driver (Port-Mapped I/O)
The CPU communicates with the keyboard using specific I/O ports. Since C cannot access hardware ports directly, we use inline assembly wrappers.

#### The Assembly Wrappers
```c
// inb: Read a byte from a hardware port
static inline char inb(short port) {
    char ret;
    asm volatile ( "inb %1, %0" : "=a"(ret) : "Nd"(port) );
    return ret;
}
```

#### The Polling Algorithm
We implemented **Polling** (active waiting) rather than Interrupts for this phase of the project:
1.  **Poll Port `0x64` (Status Register):** Read continuously until `Bit 0` is `1`. This indicates the keyboard buffer is full.
2.  **Read Port `0x60` (Data Register):** Extract the 8-bit **Scancode**.
3.  **Process Scancode:**
    * If the top bit is `0` (e.g., `0x1E`), it is a **Make Code** (Key Press).
    * If the top bit is `1` (e.g., `0x9E`), it is a **Break Code** (Key Release) -> *Ignored*.

---

## 🛠️ 4. Build Environment & Tools

| Component | Tool Used | Purpose |
| :--- | :--- | :--- |
| **Language** | C (GCC) | Core kernel logic |
| **Boot Stub** | NASM | Setting up the stack and multiboot header |
| **Linker** | GNU `ld` | Combining object files and mapping memory sections |
| **Emulator** | QEMU | Testing the OS without rebooting the physical machine |
| **Build System** | GNU Make | Automating the compilation process |

---

## ⚡ 5. Execution Instructions

### Prerequisites
* **macOS:** `brew install x86_64-elf-gcc nasm qemu`
* **Linux:** `sudo apt install build-essential qemu-system-x86 nasm`

### How to Run
1.  **Navigate to the project directory:**
    ```bash
    cd [Project Folder]
    ```
2.  **Run the Makefile:**
    This command compiles the Assembly, compiles the C, links them, and launches QEMU.
    ```bash
    make run
    ```
3.  **Interact:**
    A QEMU window will appear. You can type on your physical keyboard, and the characters will appear on the MyOS screen.

### Manual Build Commands (If Make fails)
If the automation script fails, the kernel can be built manually:
```bash
# 1. Assemble the boot loader
nasm -f elf32 boot.s -o boot.o

# 2. Compile the Kernel (freestanding = no standard library)
x86_64-elf-gcc -m32 -ffreestanding -c kernel.c -o kernel.o

# 3. Link (Map code to 1MB)
x86_64-elf-ld -m elf_i386 -T linker.ld boot.o kernel.o -o myos.bin

# 4. Emulate
qemu-system-i386 -kernel myos.bin
```

---

## 🔧 6. Challenges & Solutions

### Issue 1: "Booting from ROM..." Freeze
* **Problem:** QEMU would freeze at the BIOS screen.
* **Root Cause:** The linker was placing our code *before* the Multiboot Header. The bootloader must find the header in the first 8KB.
* **Solution:** Modified `linker.ld` to force the `.multiboot` section to the very top using the `KEEP()` directive.

### Issue 2: Cross-Compiler Compatibility on macOS
* **Problem:** The default `clang` compiler on macOS targets ARM64 (Apple Silicon), causing link errors (`unknown arch: armv4t`).
* **Solution:** Installed the dedicated `x86_64-elf-gcc` toolchain to ensure cross-compilation for the Intel i386 architecture.

### Issue 3: Make Syntax Errors
* **Problem:** `Makefile: No rule to make target`.
* **Solution:** Learned that Makefiles strictly require **Tabs** for indentation, not spaces. Re-wrote the Makefile to adhere to POSIX standards.

---

## 📸 7. Screenshots
* **Screenshot 1:** QEMU booting successfully with Green text.
<img width="711" height="432" alt="Screenshot 2026-01-20 at 12 12 19 PM" src="https://github.com/user-attachments/assets/3ce8bbde-647d-49b1-87ad-d6fd3d76c419" />

---

## 📚 8. References
1.  **OSDev Wiki:** Memory Map & VGA Hardware Specifications.
2.  **GNU Multiboot Specification v0.6.96.**
3.  **Intel® 64 and IA-32 Architectures Software Developer’s Manual** (for I/O port instructions).
