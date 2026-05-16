#include "mouse.h"

// Global fare koordinatları ve buton durumları
int mouse_x = 400; 
int mouse_y = 300; 
int mouse_left_click = 0;
int mouse_right_click = 0;
int mouse_middle_click = 0;

static uint8_t mouse_cycle = 0;
static unsigned char mouse_packet[3];

// İşlemcinin fiziksel portundan 1 bayt veri okuyan saf Assembly köprüsü
unsigned char inb(unsigned short port) {
    unsigned char result;
    __asm__ volatile("inb %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}

// Porttan veri yazan köprü
void outb(unsigned short port, unsigned char data) {
    __asm__ volatile("outb %0, %1" : : "a"(data), "Nd"(port));
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

// PS/2 Fareyi Aktif Etme Dizisi
void init_mouse(void) {
    outb(0x64, 0xA8); 
    while(inb(0x64) & 0x02);
    
    outb(0x64, 0xD4); 
    while(inb(0x64) & 0x02);
    
    outb(0x60, 0xF4); 
    while(inb(0x64) & 0x02);
    
    inb(0x60); // İlk gelen ACK onayını temizle
}

/* Fareden gelen paketleri filtreleyen ve jilet gibi işleyen nöbetçi fonksiyon */
void handle_mouse_polling(void) {
    unsigned char status = inb(MOUSE_STATUS_PORT);
    
    // Portta veri var mı (0x01) ve fareye mi ait (0x20) kontrolü
    if ((status & 0x01) && (status & 0x20)) {
        unsigned char current_byte = inb(MOUSE_DATA_PORT);
        
        // 🛠️ OPTİMİZASYON 1: Paket kayması kontrolü (Bit Sync)
        if (mouse_cycle == 0 && !(current_byte & MOUSE_BIT_SYNC)) {
            return; 
        }
        
        mouse_packet[mouse_cycle] = current_byte;
        mouse_cycle++;

        // 3 baytlık paket tamamlandıysa işlem başlasın
        if (mouse_cycle == 3) {
            mouse_cycle = 0; 

            // 🛠️ OPTİMİZASYON 2: Donanımsal Sıçrama/Taşma (Overflow) Filtresi
            if ((mouse_packet[0] & MOUSE_X_OVERFLOW) || (mouse_packet[0] & MOUSE_Y_OVERFLOW)) {
                return; // Eğer ani taşma varsa bu hatalı paketi çöpe at
            }

            // Buton durumlarını güncelle (Senin mantığın)
            mouse_left_click   = mouse_packet[0] & 1;  
            mouse_right_click  = mouse_packet[0] & 2;  
            mouse_middle_click = mouse_packet[0] & 4;  

            // X ve Y ham hareket miktarlarını al (Sign Extension korumasıyla)
            int move_x = (char)mouse_packet[1];
            int move_y = (char)mouse_packet[2];

            // Koordinatları ekran kartı matrisine göre eşitle
            mouse_x += move_x;
            mouse_y -= move_y; 

            // Ekran dışına çıkmayı engelleme (Clipping)
            if (mouse_x < 0) mouse_x = 0;
            if (mouse_x > SCREEN_WIDTH - 12) mouse_x = SCREEN_WIDTH - 12;
            if (mouse_y < 0) mouse_y = 0;
            if (mouse_y > SCREEN_HEIGHT - 19) mouse_y = SCREEN_HEIGHT - 19;

            // Siber tetikleyici: Sol tıka basıldıysa kurulum tamamlansın
            if (mouse_left_click) {
                extern int setup_completed;
                setup_completed = 1;
            }
        }
    }
}
