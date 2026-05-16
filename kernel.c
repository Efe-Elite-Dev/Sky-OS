#include "screen.h"
#include <stdint.h>

// SKY-OS OOBE SETUP STATE MACHINE
// 0: Setup, 1: Ulke, 2: Klavye, 3: Ag, 4: Ad, 5: Gizlilik, 6: Ozellestirme, 7: Hosgeldiniz, 8: Ana Ekran (Masaustu)
int setup_state = 0;

// FARE (MOUSE) SÜRÜCÜSÜ DEĞİŞKENLERİ
int mouse_x = 40; 
int mouse_y = 12; 
uint8_t mouse_cycle = 0;
int8_t mouse_byte[3];

// Farenin arkasında iz bırakmaması için orijinal karakter hafızası
uint16_t saved_char_under_mouse = (0x1F << 8) | ' '; 

#define VGA_START 0xB8000
#define COLOR_SKY_BLUE      0x1F  // Arka plan: Mavi, Yazı: Beyaz (Sky-OS Kurulum Teması)
#define COLOR_RED_ON_BLUE   0x1C  // Mavi arka plan üzerinde Parlak Kırmızı 'X' imleci
#define COLOR_DESKTOP_GREEN 0x2F  // Kurulum bitince gelecek klasik yeşil masaüstü

// Port İşlemleri
static inline uint8_t inb(uint16_t port) {
    uint8_t data;
    __asm__ __volatile__("inb %1, %0" : "=a"(data) : "Nd"(port));
    return data;
}

static inline void outb(uint16_t port, uint8_t val) {
    __asm__ __volatile__("outb %0, %1" : : "a"(val), "Nd"(port));
}

// Mikro gecikme motoru
void sky_ui_delay(int count) {
    for (volatile int i = 0; i < count * 800; i++);
}

// Ekranı Seçilen Renge Boyayan Grafik Motoru
void paint_screen_bg(uint8_t color) {
    uint16_t *vga_buffer = (uint16_t*)VGA_START;
    for (int i = 0; i < 80 * 25; i++) {
        vga_buffer[i] = (color << 8) | ' '; 
    }
    sky_clear_screen(); 
}

// Görsel Adımları Çizen Arayüz Motoru (Sky-OS OOBE UI)
void draw_skyos_oobe_ui(void) {
    if (setup_state == 8) {
        paint_screen_bg(COLOR_DESKTOP_GREEN);
    } else {
        paint_screen_bg(COLOR_SKY_BLUE);
    }
    
    switch(setup_state) {
        case 0: // Sky-OS Setup 
            sky_print("\n\n                    S K Y - O S   S E T U P\n");
            sky_print("               -------------------------------\n\n\n\n");
            sky_print("                     +-----------------+\n");
            sky_print("                     |     Sky-OS      |\n");
            sky_print("                     +-----------------+\n");
            sky_print("                     |   Kuruluma Hazir|\n");
            sky_print("                     +-----------------+\n\n");
            sky_print("                     [ Install now ]  <-- (TIKLA)\n\n\n\n");
            sky_print("         -----------------------------------------------------\n");
            sky_print("         | Buton Koordinati ->  (X:21-36, Y:12-14)           |\n");
            sky_print("         -----------------------------------------------------\n");
            break;

        case 1: // Ülke/Bölge Seçimi
            sky_print("\n\n                    S K Y - O S   O O B E\n");
            sky_print("               -------------------------------\n\n\n");
            sky_print("               Bu dogru ulke/bolge mi?\n\n");
            sky_print("                 Trinidad ve Tobago\n");
            sky_print("                 Tunus\n");
            sky_print("               > Turkiye  <-- (SECILI)\n");
            sky_print("                 Turkmenistan\n\n\n");
            sky_print("                                              [ Evet ] <-- (TIKLA)\n");
            break;

        case 2: // Klavye Düzeni
            sky_print("\n\n                    S K Y - O S   O O B E\n");
            sky_print("               -------------------------------\n\n\n");
            sky_print("               Bu dogru klavye duzeni veya giris yontemi mi?\n\n");
            sky_print("               > Turkce Q  <-- (SECILI)\n");
            sky_print("                 Turkce F\n");
            sky_print("                 Almanca\n\n\n\n");
            sky_print("                                              [ Evet ] <-- (TIKLA)\n");
            break;

        case 3: // Ağ Bağlantısı
            sky_print("\n\n                    S K Y - O S   O O B E\n");
            sky_print("               -------------------------------\n\n\n");
            sky_print("               Hadi sizi bir aga baglayalim\n");
            sky_print("               Sky-OS servisleri icin internet baglantisi kuruluyor.\n\n");
            sky_print("                 [X] TP-Wi-Fi 6  (Baglanildi)\n");
            sky_print("                     Ag guvenlik anahtari dogrulandi.\n\n\n\n");
            sky_print("                                           [ Sonraki ] <-- (TIKLA)\n");
            break;

        case 4: // Bilgisayar Adı
            sky_print("\n\n                    S K Y - O S   O O B E\n");
            sky_print("               -------------------------------\n\n\n");
            sky_print("               Cihaziniza bir ad verelim\n\n");
            sky_print("                 Cihaz Adi: SkyOS-Elite-PC\n\n\n\n\n\n");
            sky_print("                                           [ Ileri ] <-- (TIKLA)\n");
            break;

        case 5: // Gizlilik Ayarları
            sky_print("\n\n                    S K Y - O S   O O B E\n");
            sky_print("               -------------------------------\n\n\n");
            sky_print("               Sky-OS Gizlilik Ayarlarini Secin\n\n");
            sky_print("                 [X] Yerel Telemetri ve Loglama: EVET\n");
            sky_print("                 [X] Donanimsal Optimizasyon Raporu: EVET\n\n\n\n\n");
            sky_print("                                         [ Kabul et ] <-- (TIKLA)\n");
            break;

        case 6: // Deneyimi Özelleştirme
            sky_print("\n\n                    S K Y - O S   O O B E\n");
            sky_print("               -------------------------------\n\n\n");
            sky_print("               Sky-OS Deneyiminizi Ozellestirelim\n\n");
            sky_print("                 [X] Eglence (Medya ve Web Modülleri)\n");
            sky_print("                 [ ] Oyun (Ultra-Polling Gaming Modu)\n\n\n\n\n");
            sky_print("                                         [ Kabul et ] <-- (TIKLA)\n");
            break;

        case 7: // HOŞ GELDİNİZ EKRANI 
            sky_print("\n\n\n\n\n\n\n\n");
            sky_print("                     ====================================\n");
            sky_print("                     =                                  =\n");
            sky_print("                     =         HOS GELDINIZ...          =\n");
            sky_print("                     =                                  =\n");
            sky_print("                     ====================================\n\n");
            sky_print("                     Sky-OS Masaustu yukleniyor, lutfen bekleyin...");
            break;

        case 8: // ANA EKRAN / MASAÜSTÜ
            sky_print("\n  [SkyOS Desktop v0.5]                                        Saat: 03:55\n");
            sky_print("  =======================================================================\n\n");
            sky_print("    +------------+      +------------+      +------------+\n");
            sky_print("    |  Bilgisayar|      |  Belgeler  |      |  Ayarlar   |\n");
            sky_print("    +------------+      +------------+      +------------+\n\n\n\n\n\n\n\n\n\n\n\n");
            sky_print("  -----------------------------------------------------------------------\n");
            sky_print("  | [ BASLAT ]  SkyOS Safe AI Kernel Basariyla Yuklendi!                |\n");
            sky_print("  -----------------------------------------------------------------------\n");
            break;
    }

    // Fare imleci için arka plan rengini koru ve çiz
    uint16_t *vga_buffer = (uint16_t*)VGA_START;
    saved_char_under_mouse = vga_buffer[mouse_y * 80 + mouse_x];
    
    if (setup_state == 8) {
        vga_buffer[mouse_y * 80 + mouse_x] = ((COLOR_DESKTOP_GREEN & 0xF0) | 0x0C << 8) | 'X';
    } else {
        vga_buffer[mouse_y * 80 + mouse_x] = (COLOR_RED_ON_BLUE << 8) | 'X';
    }
}

// Tıklama Algılayıcı ve Adım Atlatıcı Motor
void handle_mouse_click(void) {
    // Adım 0 -> Adım 1 Geçişi (Install Now)
    if (setup_state == 0) {
        if (mouse_x >= 21 && mouse_x <= 36 && mouse_y >= 12 && mouse_y <= 14) {
            setup_state = 1;
            draw_skyos_oobe_ui();
        }
    }
    // Adım 1-6 arası geçişler
    else if (setup_state >= 1 && setup_state <= 6) {
        if (mouse_x >= 40 && mouse_x <= 75 && mouse_y >= 10 && mouse_y <= 16) {
            setup_state++;
            draw_skyos_oobe_ui();
            
            if (setup_state == 7) {
                sky_ui_delay(3000); // 3 saniye Hoş Geldiniz ekranı
                setup_state = 8;    // Masaüstüne geç
                draw_skyos_oobe_ui();
            }
        }
    }
}

// İz Bırakmayı Önleyen Akıllı Fare İmleci Güncelleyici
void refresh_mouse_pointer(int old_x, int old_y) {
    uint16_t *vga_buffer = (uint16_t*)VGA_START;
    
    // Eski karakteri geri koy
    vga_buffer[old_y * 80 + old_x] = saved_char_under_mouse;
    
    // Yeni karakteri yedekle
    saved_char_under_mouse = vga_buffer[mouse_y * 80 + mouse_x];
    
    // Yeni yere X imlecini bas
    if (setup_state == 8) {
        vga_buffer[mouse_y * 80 + mouse_x] = (0x2C << 8) | 'X'; 
    } else {
        vga_buffer[mouse_y * 80 + mouse_x] = (COLOR_RED_ON_BLUE << 8) | 'X'; 
    }
}

// Fare Donanımını Uyandırma Kodları
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

// Donanımsal Polling Motoru
void handle_mouse_polling(void) {
    uint8_t status = inb(0x64);
    
    if ((status & 0x01) && (status & 0x20)) {
        uint8_t data = inb(0x60);
        mouse_byte[mouse_cycle++] = data;
        
        if (mouse_cycle == 3) { 
            mouse_cycle = 0;
            
            if (mouse_byte[0] & 0x01) {
                handle_mouse_click();
                sky_ui_delay(10);
                return;
            }
            
            int old_x = mouse_x;
            int old_y = mouse_y;
            
            int8_t move_x = mouse_byte[1];
            int8_t move_y = mouse_byte[2];
            
            mouse_x += (move_x / 3);
            mouse_y -= (move_y / 3); 
            
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
    setup_state = 0;           
    draw_skyos_oobe_ui();    // Bize özel Sky-OS ekranını açılışta çiz
    init_mouse();              

    while (1) {
        handle_mouse_polling();
        sky_ui_delay(1);       
    }
}
