#include "mouse.h"

// Global değişkenler
int mouse_x = 400; 
int mouse_y = 300; 
static uint8_t mouse_cycle = 0;
static int8_t mouse_byte[3];

// Giriş/Çıkış Port Fonksiyonları
static inline uint8_t inb(uint16_t port) {
    uint8_t data;
    __asm__ __volatile__("inb %1, %0" : "=a"(data) : "Nd"(port));
    return data;
}

static inline void outb(uint16_t port, uint8_t val) {
    __asm__ __volatile__("outb %0, %1" : : "a"(val), "Nd"(port));
}

// Fare okunun 12x19 matrisi (Sprite)
const uint8_t mouse_pointer_sprite[19][12] = {
    {1,0,0,0,0,0,0,0,0,0,0,0},
    {1,1,0,0,0,0,0,0,0,0,0,0},
    {1,1,1,0,0,0,0,0,0,0,0,0},
    {1,1,1,1,0,0,0,0,0,0,0,0},
    {1,1,1,1,1,0,0,0,0,0,0,0},
    {1,1,1,1,1,1,0,0,0,0,0,0},
    {1,1,1,1,1,1,1,0,0,0,0,0},
    {1,1,1,1,1,1,1,1,0,0,0,0},
    {1,1,1,1,1,1,1,1,1,0,0,0},
    {1,1,1,1,1,1,1,1,1,1,0,0},
    {1,1,1,1,1,1,1,1,1,1,1,0},
    {1,1,1,1,1,1,1,0,0,0,0,0},
    {1,1,1,0,1,1,1,1,0,0,0,0},
    {1,1,0,0,1,1,1,1,0,0,0,0},
    {1,0,0,0,0,1,1,1,1,0,0,0},
    {0,0,0,0,0,1,1,1,1,0,0,0},
    {0,0,0,0,0,0,1,1,1,0,0,0},
    {0,0,0,0,0,0,1,1,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0}
};

// PS/2 Fareyi Aktif Etme
void init_mouse(void) {
    outb(0x64, 0xA8); 
    while(inb(0x64) & 0x02);
    
    outb(0x64, 0xD4); 
    while(inb(0x64) & 0x02);
    
    outb(0x60, 0xF4); 
    while(inb(0x64) & 0x02);
    
    inb(0x60);        
}

// Fare Verilerini Tarayan Akıllı Polling Fonksiyonu
void handle_mouse_polling(void) {
    uint8_t status = inb(0x64);
    
    // Veri var mı ve fareye mi ait kontrolü
    if ((status & 0x01) && (status & 0x20)) {
        uint8_t data = inb(0x60);
        
        // 🛠️ KRİTİK DÜZELTME: Eğer ilk byte'ı bekliyorsak ve 3. bit 1 değilse packet kaymıştır!
        // Bu durumda döngüyü sıfırla ve senkronizasyonu yeniden yakala.
        if (mouse_cycle == 0 && !(data & 0x08)) {
            return; 
        }
        
        mouse_byte[mouse_cycle++] = data;
        
        if (mouse_cycle == 3) { 
            mouse_cycle = 0;
            
            // Taşma (Overflow) bitlerini kontrol et, eğer taşma varsa bu paketi atla
            if ((mouse_byte[0] & 0x40) || (mouse_byte[0] & 0x80)) {
                return;
            }
            
            int8_t move_x = mouse_byte[1];
            int8_t move_y = mouse_byte[2];
            
            // İşaret bitlerine göre negatif hareketleri düzelt (Sign Extension)
            if ((mouse_byte[0] & 0x10) && move_x == 0) move_x = -1;
            if ((mouse_byte[0] & 0x20) && move_y == 0) move_y = -1;
            
            // Hassasiyet çarpanı (Gereksiz zıplamaları önlemek için stabilizasyon)
            mouse_x += (int)(move_x); 
            mouse_y -= (int)(move_y); 
            
            // Ekran sınırlarından dışarı taşmayı engelleme
            if (mouse_x < 0) mouse_x = 0;
            if (mouse_x > SCREEN_WIDTH - 12) mouse_x = SCREEN_WIDTH - 12;
            if (mouse_y < 0) mouse_y = 0;
            if (mouse_y > SCREEN_HEIGHT - 19) mouse_y = SCREEN_HEIGHT - 19;
        }
    }
}
