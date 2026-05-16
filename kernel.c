#include "screen.h"
#include <stdint.h>

// FARE (MOUSE) SÜRÜCÜSÜ DEĞİŞKENLERİ
int mouse_x = 40; // Ekranın ortasından başlasın (0-79)
int mouse_y = 12; // Ekranın ortasından başlasın (0-24)
uint8_t mouse_cycle = 0;
int8_t mouse_byte[3];

#define VGA_START 0xB8000
// VGA Renk Kodları (BG: BackGround, FG: ForeGround)
#define COLOR_BLACK_ON_BLACK 0x00 // Simsiyah ekran (İz kalmasın diye)
#define COLOR_RED_ON_BLACK   0x0C // Siyah arka plan üzerinde Parlak Kırmızı imleç

// Port İşlemleri
static inline uint8_t inb(uint16_t port) {
    uint8_t data;
    __asm__ __volatile__("inb %1, %0" : "=a"(data) : "Nd"(port));
    return data;
}

static inline void outb(uint16_t port, uint8_t val) {
    __asm__ __volatile__("outb %0, %1" : : "a"(val), "Nd"(port));
}

// Zaman oyalama fonksiyonu
void sky_mini_delay(int count) {
    for (volatile int i = 0; i < count * 500; i++);
}

// Ekranı Tamamen Simsiyah Yapan Grafik Motoru (İz Bırakmayı ÖNLEMEK İÇİN)
void paint_black_bg(void) {
    uint16_t *vga_buffer = (uint16_t*)VGA_START;
    for (int i = 0; i < 80 * 25; i++) {
        // Tüm hücreleri siyah arka plan, siyah harf (görünmez) olarak boyuyoruz
        // Bu, fare gerisinde iz bırakmasını donanımsal olarak engeller!
        vga_buffer[i] = (COLOR_BLACK_ON_BLACK << 8) | ' '; 
    }
}

// Ekranda Fare İmlecini Güncelleyen Fonksiyon (İz Bırakmadan!)
void refresh_mouse_pointer(int old_x, int old_y) {
    uint16_t *vga_buffer = (uint16_t*)VGA_START;
    
    // Farenin eski durduğu yerdeki 'X'i sil, orayı tamamen siyah bir boşluğa geri döndür
    vga_buffer[old_y * 80 + old_x] = (COLOR_BLACK_ON_BLACK << 8) | ' ';
    
    // Farenin yeni koordinatına Parlak Kırmızı renkli bir 'X' imleci bas!
    vga_buffer[mouse_y * 80 + mouse_x] = (COLOR_RED_ON_BLACK << 8) | 'X';
}

// Fare Donanımını Uyandırma
void init_mouse(void) {
    outb(0x64, 0xA8); 
    while((inb(0x64) & 0x02));
    outb(0x64, 0x20); 
    while(!(inb(0x64) & 0x01));
    uint8_t status = (inb(0x60) | 0x02); 
    while((inb(0x64) & 0x02));
    outb(0x64, 0x60);
    while((inb(0x64) & 0x02));
    outb(0x60, status);
    while((inb(0x64) & 0x02));
    outb(0x64, 0xD4);
    while((inb(0x64) & 0x02));
    outb(0x60, 0xF4); 
    inb(0x60); 
}

// Kesmesiz Saf Fare Polling Motoru
void handle_mouse_polling(void) {
    uint8_t status = inb(0x64);
    
    if ((status & 0x01) && (status & 0x20)) {
        uint8_t data = inb(0x60);
        mouse_byte[mouse_cycle++] = data;
        
        if (mouse_cycle == 3) { 
            mouse_cycle = 0;
            
            int old_x = mouse_x;
            int old_y = mouse_y;
            
            int8_t move_x = mouse_byte[1];
            int8_t move_y = mouse_byte[2];
            
            // Fare hassasiyeti (Çok hızlı uçmasın diye 4'e bölüyoruz)
            mouse_x += (move_x / 4);
            mouse_y -= (move_y / 4); 
            
            // Ekran Taşma Koruması
            if (mouse_x < 0) mouse_x = 0;
            if (mouse_x > 79) mouse_x = 79;
            if (mouse_y < 0) mouse_y = 0;
            if (mouse_y > 24) mouse_y = 24;
            
            if (old_x != mouse_x || old_y != mouse_y) {
                refresh_mouse_pointer(old_x, old_y);
            }
        }
    }
}

void kernel_main(void) {
    // 1. Ekranı tamamen karart (Hiçbir iz kalmayacak)
    paint_black_bg();
    
    // 2. İlk imleci tam ortada oluştur
    uint16_t *vga_buffer = (uint16_t*)VGA_START;
    vga_buffer[mouse_y * 80 + mouse_x] = (COLOR_RED_ON_BLACK << 8) | 'X';
    
    // 3. Fareyi uyandır
    init_mouse();

    while (1) {
        handle_mouse_polling();
        sky_mini_delay(1); // İşlemciyi dengele
    }
}
