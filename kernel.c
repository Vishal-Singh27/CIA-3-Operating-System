/* kernel.c - Final Version with Screen Clearing */

// Assembly wrapper to read hardware ports
static inline char inb(short port) {
    char ret;
    asm volatile ( "inb %1, %0" : "=a"(ret) : "Nd"(port) );
    return ret;
}

// THE NEW FUNCTION: Wipe the screen clean
void clear_screen() {
    char* video_memory = (char*) 0xb8000;
    
    // VGA Text Mode is 80 columns wide x 25 rows high
    // Each cell is 2 bytes (Character + Color), so 80*25*2 = 4000 bytes
    for (int i = 0; i < 80 * 25 * 2; i += 2) {
        video_memory[i] = ' ';       // Space character
        video_memory[i+1] = 0x07;    // Color: Light Grey on Black
    }
}

void kernel_main() {
    // 1. CLEAR THE SCREEN FIRST!
    clear_screen();

    char* video_memory = (char*) 0xb8000;
    const char* str = "Welcome to MyOS! Start typing below:";
    int cursor = 0;

    // 2. Print the header message
    int i = 0;
    while (str[i] != '\0') {
        video_memory[cursor] = str[i];
        video_memory[cursor+1] = 0x02; // Green on Black
        i++;
        cursor += 2;
    }

    // Move cursor to the next line (Row 2, Column 0)
    // One row is 80 chars * 2 bytes = 160 bytes
    cursor = 160; 

    // 3. The Main Loop
    while(1) {
        if (inb(0x64) & 0x01) {
            char scancode = inb(0x60);
            
            if ((scancode & 0x80) == 0) {
                char ascii = 0; // Use 0 for "no character"

                // Expanded Key Map
                if (scancode == 0x1E) ascii = 'a';
                else if (scancode == 0x30) ascii = 'b';
                else if (scancode == 0x2E) ascii = 'c';
                else if (scancode == 0x20) ascii = 'd';
                else if (scancode == 0x12) ascii = 'e';
                else if (scancode == 0x21) ascii = 'f';
                else if (scancode == 0x22) ascii = 'g';
                else if (scancode == 0x23) ascii = 'h';
                else if (scancode == 0x17) ascii = 'i';
                else if (scancode == 0x24) ascii = 'j';
                else if (scancode == 0x25) ascii = 'k';
                else if (scancode == 0x26) ascii = 'l';
                else if (scancode == 0x32) ascii = 'm';
                else if (scancode == 0x31) ascii = 'n';
                else if (scancode == 0x18) ascii = 'o';
                else if (scancode == 0x19) ascii = 'p';
                else if (scancode == 0x10) ascii = 'q';
                else if (scancode == 0x13) ascii = 'r';
                else if (scancode == 0x1F) ascii = 's';
                else if (scancode == 0x14) ascii = 't';
                else if (scancode == 0x16) ascii = 'u';
                else if (scancode == 0x2F) ascii = 'v';
                else if (scancode == 0x11) ascii = 'w';
                else if (scancode == 0x2D) ascii = 'x';
                else if (scancode == 0x15) ascii = 'y';
                else if (scancode == 0x2C) ascii = 'z';
                else if (scancode == 0x39) ascii = ' '; 
                else if (scancode == 0x1C) { // ENTER Key
                    // Move to start of next line
                    int current_row = cursor / 160;
                    cursor = (current_row + 1) * 160;
                }

                if (ascii != 0) {
                    video_memory[cursor] = ascii;
                    video_memory[cursor+1] = 0x0F; // White text
                    cursor += 2;
                }
                
                // Input delay
                for(int d=0; d<2000000; d++) { asm("nop"); }
            }
        }
    }
}