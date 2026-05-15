cat << 'EOF' > kernel.c
#define VGA_WIDTH 80
#define VGA_HEIGHT 25
const int VGA_COLOR_WHITE_ON_BLACK = 0x07;

void kernel_main(void) {
    volatile char* video_memory = (volatile char*)0xB8000;

    for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT * 2; i += 2) {
        video_memory[i] = ' ';
        video_memory[i + 1] = VGA_COLOR_WHITE_ON_BLACK;
    }

    const char* str = "Sky-OS Saf AI Kernel Basariyla Baslatildi!";
    int i = 0;
    while (str[i] != '\0') {
        video_memory[i * 2] = str[i];
        video_memory[i * 2 + 1] = VGA_COLOR_WHITE_ON_BLACK;
        i++;
    }
}
EOF
