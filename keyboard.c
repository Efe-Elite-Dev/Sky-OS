#include "keyboard.h"
#include "screen.h"

static const char kbd_us[128] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
  '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0,  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',   0,
 '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/',   0,   '*',   0,
  ' ',   0
};

static inline unsigned char inb(unsigned short port) {
    unsigned char data;
    __asm__ __volatile__("inb %1, %0" : "=a"(data) : "Nd"(port));
    return data;
}

static inline void outb(unsigned short port, unsigned char val) {
    __asm__ __volatile__("outb %0, %1" : : "a"(val), "Nd"(port));
}

void init_keyboard(void) {
    unsigned char dummy = inb(0x60); // Portu temizle
}

// Donanımsal Polling Fonksiyonu (IDT'den bağımsız kesin çözüm)
void check_keyboard_polling(void) {
    // 0x64 portu klavyenin durum (status) bilgisini verir.
    // Eğer en sağdaki bit (0x01) 1 ise, klavyede yeni bir tuş verisi var demektir!
    if (inb(0x64) & 0x01) {
        unsigned char scancode = inb(0x60); // Harfi oku
        
        // Sadece tuşa basılma anını yakala
        if (!(scancode & 0x80)) {
            char key = kbd_us[scancode];
            if (key != 0) {
                sky_put_char(key);
            }
        }
    }
}

// IDT üzerinden tetiklenen eski handler (Yine de dursun)
void keyboard_handler(void) {
    unsigned char scancode = inb(0x60);
    if (!(scancode & 0x80)) {
        char key = kbd_us[scancode];
        if (key != 0) {
            sky_put_char(key);
        }
    }
    outb(0x20, 0x20); // EOI
}
