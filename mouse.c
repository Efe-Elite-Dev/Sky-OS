#include "mouse.h"

// Global değişkenlerin tanımlanması
int mouse_x = 400; 
int mouse_y = 300; 
static uint8_t mouse_cycle = 0;
static int8_t mouse_byte[3];

// Giriş/Çıkış Port Fonksiyonları (Static inline olarak sürücü içinde)
static inline uint8_t inb(uint16_t port) {
    uint8_t data;
    __asm__ __volatile__("inb %1, %0" : "=a"(data) : "Nd"(port));
    return data;
}

static inline void outb(uint16_t port, uint8_t val) {
    __asm__ __volatile__("outb %0, %1" : : "a"(val), "Nd"(port));
}

// Fare imlecinin görsel matrisi
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

// PS/2 Fareyi Aktif Etme Fonksiyonu
void init_mouse(void) {
    // Yardımcı işlemciyi (Auxiliary Device) aktif et
    outb(0x64, 0xA8); 
    while(inb(0x64) & 0x02);
    
    // Komut göndereceğimizi bildir
    outb(0x64, 0xD4); 
    while(inb(0x64) & 0x02);
    
    // Veri portuna fareyi aktifleştirme komutunu (0xF4) yolla
    outb(0x60, 0xF4); 
    while(inb(0x64) & 0x02);
    
    // ACK (onay) sinyalini porttan erit
    inb(0x60);        
}

// Fare Verilerini Tarayan Polling Fonksiyonu
void handle_mouse_polling(void) {
    uint8_t status = inb(0x64);
    // Veri hazır mı (0x01) ve bu veri fareye mi ait (0x20) kontrolü
    if ((status & 0x01) && (status & 0x20)) {
        uint8_t data = inb(0x60);
        mouse_byte[mouse_cycle++] = data;
        
        if (mouse_cycle == 3) { 
            mouse_cycle = 0;
            
            // PS/2 Fare paket çözümü
            int8_t move_x = mouse_byte[1];
            int8_t move_y = mouse_byte[2];
            
            // Hassasiyeti biraz yumuşatıp koordinatları güncelle
            mouse_x += move_x / 2; 
            mouse_y -= move_y / 2; 
            
            // Taşma (Ekran dışına çıkma) kontrolleri
            if (mouse_x < 0) mouse_x = 0;
            if (mouse_x > SCREEN_WIDTH - 12) mouse_x = SCREEN_WIDTH - 12;
            if (mouse_y < 0) mouse_y = 0;
            if (mouse_y > SCREEN_HEIGHT - 19) mouse_y = SCREEN_HEIGHT - 19;
        }
    }
}
