#include <stdint.h>

// kernel.c içindeki köprü fonksiyonunu buraya bağlıyoruz
extern void sky_put_char(char c);

// Basit scancode to ascii tablosu
static char scancode_to_ascii[] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
  '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '[', ']', '\n',      
    0,  'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ';', '\'', '`',   0,      
 '\\', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', ',', '.', '/',   0, '*',   0, ' '
};

static inline uint8_t inb(uint16_t port) {
    uint8_t data;
    __asm__ __volatile__("inb %1, %0" : "=a"(data) : "Nd"(port));
    return data;
}

// Polling (Döngü ile klavye okuma) fonksiyonu
void check_keyboard_polling(void) {
    if (inb(0x64) & 1) {
        uint8_t scancode = inb(0x60);
        if (!(scancode & 0x80)) { // Tuşa basılma anı (Release değilse)
            if (scancode < sizeof(scancode_to_ascii)) {
                char ascii = scancode_to_ascii[scancode];
                if (ascii != 0) {
                    sky_put_char(ascii); // kernel.c'deki köprüye gönder
                }
            }
        }
    }
}

// Kesme (Interrupt) geldiğinde çalışacak ana fonksiyon
void keyboard_handler(void) {
    uint8_t scancode = inb(0x60);
    if (!(scancode & 0x80)) {
        if (scancode < sizeof(scancode_to_ascii)) {
            char ascii = scancode_to_ascii[scancode];
            if (ascii != 0) {
                sky_put_char(ascii); 
            }
        }
    }
}
