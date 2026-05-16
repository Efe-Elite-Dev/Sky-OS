#include <stdint.h>

// ====================================================================
// 1. MODERN SİSTEM TANIMLAMALARI VE ÇÖZÜNÜRLÜK AYARLARI
// ====================================================================
#define VBE_START_ADDR   0xE0000000 
#define SCREEN_WIDTH     1024
#define SCREEN_HEIGHT    768
#define TOTAL_PIXELS     (SCREEN_WIDTH * SCREEN_HEIGHT)

// Windows 11 / Akıcı Arayüz Renk Paleti (Hex Format)
#define COLOR_BG         0x0A0F24  // Derin Modern Gece Mavisi Arka Plan
#define COLOR_TASKBAR    0x1A2342  // Yarı Saydam Efektli Koyu Görev Çubuğu (Blended)
#define COLOR_ACCENT     0x0078D4  // Standart Microsoft Mavisi
#define COLOR_WIN_BG     0xFFFFFF  // Net Beyaz Pencere İçeriği
#define COLOR_WIN_TITLE  0x0F172A  // Premium Koyu Başlık Çubuğu
#define COLOR_MODERN_RED 0xE81123  // Kapatma Butonu Kırmızısı
#define COLOR_CURSOR     0x00A2ED  // Parlak Mavi Modern İmleç

// Sistem Durum Değişkenleri
int active_window = 0; 
uint32_t system_ticks = 0;

int mouse_x = 512; 
int mouse_y = 384; 
uint8_t mouse_cycle = 0;
int8_t mouse_byte[3];

// ====================================================================
// 2. DOUBLE BUFFERING (ÇİFT TAMPON) MOTORU - GLITCH ENGELLEYİCİ
// ====================================================================
// Ekran kartı belleğine (VRAM) doğrudan sürekli yazmak yerine, 
// önce RAM üzerinde her şeyi çizeceğimiz sahte bir ekran tamponu oluşturuyoruz.
uint32_t back_buffer[TOTAL_PIXELS];

// RAM'deki sahte ekranda piksel boyama
void gui_put_pixel(int x, int y, uint32_t color) {
    if (x < 0 || x >= SCREEN_WIDTH || y < 0 || y >= SCREEN_HEIGHT) return;
    back_buffer[y * SCREEN_WIDTH + x] = color;
}

// RAM'de hazırlanan sahte ekranı TEK SEFERDE gerçek VRAM'e fırlatma fonksiyonu
void vga_flush(void) {
    uint32_t *vram = (uint32_t*)VBE_START_ADDR;
    // Donanımsal olarak en hızlı kopyalama yöntemi (Unrolled loop veya flat copy)
    for (int i = 0; i < TOTAL_PIXELS; i++) {
        vram[i] = back_buffer[i];
    }
}

// Modern Donanımsal Renk Karıştırma (Alpha Blending)
uint32_t mix_colors(uint32_t bg, uint32_t fg, uint8_t alpha) {
    if (alpha == 255) return fg;
    if (alpha == 0) return bg;

    uint8_t bg_r = (bg >> 16) & 0xFF; uint8_t bg_g = (bg >> 8) & 0xFF; uint8_t bg_b = bg & 0xFF;
    uint8_t fg_r = (fg >> 16) & 0xFF; uint8_t fg_g = (fg >> 8) & 0xFF; uint8_t fg_b = fg & 0xFF;

    uint8_t res_r = ((fg_r * alpha) + (bg_r * (255 - alpha))) / 255;
    uint8_t res_g = ((fg_g * alpha) + (bg_g * (255 - alpha))) / 255;
    uint8_t res_b = ((fg_b * alpha) + (bg_b * (255 - alpha))) / 255;

    return (res_r << 16) | (res_g << 8) | res_b;
}

// Modern Saydam Dikdörtgen Çizimi
void gui_draw_rect_alpha(int start_x, int start_y, int width, int height, uint32_t color, uint8_t alpha) {
    for (int y = start_y; y < start_y + height; y++) {
        for (int x = start_x; x < start_x + width; x++) {
            if (x >= 0 && x < SCREEN_WIDTH && y >= 0 && y < SCREEN_HEIGHT) {
                uint32_t bg_color = back_buffer[y * SCREEN_WIDTH + x];
                back_buffer[y * SCREEN_WIDTH + x] = mix_colors(bg_color, color, alpha);
            }
        }
    }
}

// Mat Düz Renk Blok Çizimi (Hızlı render)
void gui_draw_rect(int start_x, int start_y, int width, int height, uint32_t color) {
    for (int y = start_y; y < start_y + height; y++) {
        for (int x = start_x; x < start_x + width; x++) {
            gui_put_pixel(x, y, color);
        }
    }
}

// Oval Köşeli Modern Pencere Çizim Motoru
void gui_draw_rounded_window(int sx, int sy, int w, int h, int r, uint32_t color, uint8_t alpha) {
    for (int y = sy; y < sy + h; y++) {
        for (int x = sx; x < sx + w; x++) {
            int dl_x = x - (sx + r); int dr_x = x - (sx + w - r - 1);
            int dt_y = y - (sy + r); int db_y = y - (sy + h - r - 1);

            int is_corner = 0;
            if (x < sx + r && y < sy + r && (dl_x*dl_x + dt_y*dt_y > r*r)) is_corner = 1; 
            if (x > sx + w - r - 1 && y < sy + r && (dr_x*dr_x + dt_y*dt_y > r*r)) is_corner = 1; 
            if (x < sx + r && y > sy + h - r - 1 && (dl_x*dl_x + db_y*db_y > r*r)) is_corner = 1; 
            if (x > sx + w - r - 1 && y > sy + h - r - 1 && (dl_x*dl_x + db_y*db_y > r*r)) is_corner = 1; 

            if (!is_corner && x >= 0 && x < SCREEN_WIDTH && y >= 0 && y < SCREEN_HEIGHT) {
                uint32_t bg_color = back_buffer[y * SCREEN_WIDTH + x];
                back_buffer[y * SCREEN_WIDTH + x] = mix_colors(bg_color, color, alpha);
            }
        }
    }
}

// ====================================================================
// 3. MODERN WINDOWS 11 STİLİ ARAYÜZ BİLEŞENLERİ
// ====================================================================

// Kusursuz Akıcı Degrade Duvar Kağıdı
void gui_render_wallpaper(void) {
    for (int y = 0; y < SCREEN_HEIGHT; y++) {
        for (int x = 0; x < SCREEN_WIDTH; x++) {
            // Ağır işlem çarpanları temizlendi, akıcı ve şık degrade geçişi
            uint8_t r = 15 + (y / 32);  
            uint8_t g = 20 + (x / 40) + (y / 40);  
            uint8_t b = 50 + (x / 16); 
            back_buffer[y * SCREEN_WIDTH + x] = (r << 16) | (g << 8) | b;
        }
    }
}

// Sol Kenar Masaüstü Uygulama İkonları
void gui_draw_icons(void) {
    // 1. EXE Subsystem İkonu
    gui_draw_rounded_window(40, 40, 40, 40, 8, 0x0078D4, 255);
    gui_draw_rect(50, 52, 20, 16, 0xFFFFFF); 
    
    // 2. DEB Subsystem İkonu
    gui_draw_rounded_window(40, 105, 40, 40, 8, 0xA8193D, 255); 
    gui_draw_rect(54, 113, 12, 24, 0xE0E0E0); 
    
    // 3. SKY AI Subsystem İkonu
    gui_draw_rounded_window(40, 170, 40, 40, 8, 0x8B5CF6, 255); 
    gui_draw_rect(52, 182, 16, 16, 0x1E1B4B); 
}

// Modern Akrilik Tasarımlı Pencere Kasası
void gui_draw_window_frame(int win_x, int win_y, int win_w, int win_h) {
    // Pencere Gölgesi (Glow efekti)
    gui_draw_rect_alpha(win_x + 6, win_y + 6, win_w, win_h, 0x000000, 70);
    // Ana Pencere Gövdesi (Yarı Saydam Akrilik Beyaz)
    gui_draw_rounded_window(win_x, win_y, win_w, win_h, 12, COLOR_WIN_BG, 240); 
    // Başlık Çubuğu
    gui_draw_rounded_window(win_x, win_y, win_w, 36, 12, COLOR_WIN_TITLE, 255);
    gui_draw_rect(win_x, win_y + 24, win_w, 12, COLOR_WIN_TITLE); // Köşe birleştirme yaması
    // Sağ Üst Kapatma (X) Butonu
    gui_draw_rounded_window(win_x + win_w - 44, win_y + 6, 36, 24, 6, COLOR_MODERN_RED, 255);
}

// Ortalanmış Windows 11 Tarzı Görev Çubuğu (Taskbar)
void gui_render_taskbar(uint32_t tick_count) {
    // Alt Bar Yuvarlatılmış Şeffaf Panel Ekranı Saplaması
    gui_draw_rounded_window(12, 714, SCREEN_WIDTH - 24, 46, 14, COLOR_TASKBAR, 210);
    
    // Ortadaki Arama Çubuğu (Search Bar)
    int search_bar_x = (SCREEN_WIDTH / 2) - 100; 
    gui_draw_rounded_window(search_bar_x, 720, 200, 34, 8, 0xFFFFFF, 255);
    
    // Başlat Menüsü ve Sabit Uygulama İkonları
    int start_icon_x = search_bar_x - 140;
    gui_draw_rounded_window(start_icon_x, 721, 32, 32, 8, COLOR_ACCENT, 255);      
    gui_draw_rounded_window(start_icon_x + 40, 721, 32, 32, 8, 0xFFB900, 255); 
    gui_draw_rounded_window(start_icon_x + 80, 721, 32, 32, 8, 0x107C41, 255); 

    // Sağ Alt Saat/Bildirim Alanındaki Küçük Yapay Zeka Nabız İndikatörü
    if ((tick_count / 30) % 2 == 0) {
        gui_draw_rounded_window(980, 728, 16, 16, 8, 0x00E676, 255); // Canlı Yeşil
    } else {
        gui_draw_rounded_window(980, 728, 16, 16, 8, 0x00B0FF, 255); // Akıllı Mavi
    }
}

// Keskin ve Modern Fare İmleci Tasarımı
void gui_draw_mouse(int mx, int my) {
    uint32_t c = COLOR_CURSOR; uint32_t w = 0xFFFFFF; 
    gui_put_pixel(mx, my, c);
    gui_put_pixel(mx, my + 1, c);   gui_put_pixel(mx + 1, my + 1, c);
    gui_put_pixel(mx, my + 2, c);   gui_put_pixel(mx + 1, my + 2, w); gui_put_pixel(mx + 2, my + 2, c);
    gui_put_pixel(mx, my + 3, c);   gui_put_pixel(mx + 1, my + 3, w); gui_put_pixel(mx + 2, my + 3, w); gui_put_pixel(mx + 3, my + 3, c);
    gui_put_pixel(mx, my + 4, c);   gui_put_pixel(mx + 1, my + 4, w); gui_put_pixel(mx + 2, my + 4, c);
    gui_put_pixel(mx, my + 5, c);   gui_put_pixel(mx + 1, my + 5, c);
}

// ====================================================================
// 4. MODÜLER ALTSİSTEM İÇERİK RENDERLARI
// ====================================================================

void run_exe_subsystem(int win_x, int win_y) {
    gui_draw_rect(win_x + 15, win_y + 50, 470, 285, 0xF8FAFC);
    // Örnek modern Windows uygulama kutucukları
    gui_draw_rounded_window(win_x + 40, win_y + 80, 80, 60, 6, 0x38BDF8, 255);
    gui_draw_rounded_window(win_x + 140, win_y + 80, 80, 60, 6, 0x34D399, 255);
}

void run_deb_subsystem(int win_x, int win_y) {
    gui_draw_rect(win_x + 15, win_y + 50, 470, 285, 0x0F172A); // Terminal Koyu Siyahı
    gui_draw_rect(win_x + 35, win_y + 70, 20, 4, 0x38BDF8);    // Yanıp sönen imleç simülasyonu
}

void run_sky_subsystem(int win_x, int win_y, uint32_t current_tick) {
    gui_draw_rect(win_x + 15, win_y + 50, 470, 285, 0x050515);
    
    // Yapay Zeka Görsel Ekolayzır Döngüsü (Hafif ve optimize)
    int pulse = (current_tick / 4) % 10;
    gui_draw_rect_alpha(win_x + 30, win_y + 120, 440, 160, 0x1E1B4B, 150); 
    for (int i = 0; i < 7; i++) {
        int h = 30 + (i * pulse) % 70;
        gui_draw_rounded_window(win_x + 55 + (i * 60), win_y + 250 - h, 20, h, 4, 0x818CF8, 255);
    }
}

// Tüm ekranı arka planda birleştiren ana üst fonksiyon
void gui_refresh_desktop(int mx, int my, uint32_t tick) {
    gui_render_wallpaper();
    gui_draw_icons();
    
    if (active_window != 0) {
        int wx = 262, wy = 184;
        gui_draw_window_frame(wx, wy, 500, 350);
        if (active_window == 1)      run_exe_subsystem(wx, wy);
        else if (active_window == 2) run_deb_subsystem(wx, wy);
        else if (active_window == 3) run_sky_subsystem(wx, wy, tick);
    }
    
    gui_render_taskbar(tick);
    gui_draw_mouse(mx, my);
    
    // ARKA PLANDA ÇİZİLEN HER ŞEYİ TEK SEFERDE VRAM'E BASIYORUZ! (Sıfır Yırtılma)
    vga_flush();
}

// ====================================================================
// 5. DONANIM SÜRÜCÜLERİ VE ÇEKİRDEK ANA DÖNGÜSÜ
// ====================================================================

static inline uint8_t inb(uint16_t port) {
    uint8_t data;
    __asm__ __volatile__("inb %1, %0" : "=a"(data) : "Nd"(port));
    return data;
}

static inline void outb(uint16_t port, uint8_t val) {
    __asm__ __volatile__("outb %0, %1" : : "a"(val), "Nd"(port));
}

void sky_kernel_delay(int count) {
    for (volatile int i = 0; i < count * 800; i++);
}

// Masaüstü Tıklama Algılama Motoru
void handle_desktop_click(void) {
    if (active_window != 0) {
        // Kapatma butonuna tıklandı mı? (X alanı)
        if (mouse_x >= 718 && mouse_x <= 754 && mouse_y >= 190 && mouse_y <= 214) {
            active_window = 0; 
            gui_refresh_desktop(mouse_x, mouse_y, system_ticks);
            return;
        }
    }
    if (active_window == 0) {
        // İkonların tıklama koordinatları kontrolü
        if (mouse_x >= 40 && mouse_x <= 80) {
            if (mouse_y >= 40 && mouse_y <= 80) active_window = 1; 
            else if (mouse_y >= 105 && mouse_y <= 145) active_window = 2; 
            else if (mouse_y >= 170 && mouse_y <= 210) active_window = 3; 
            gui_refresh_desktop(mouse_x, mouse_y, system_ticks);
        }
    }
}

// PS/2 Fare Donanımı İlklendirme
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

// Fare Verilerini Polling Yöntemiyle Kesintisiz Toplama
void handle_mouse_polling(void) {
    uint8_t status = inb(0x64);
    if ((status & 0x01) && (status & 0x20)) {
        uint8_t data = inb(0x60);
        mouse_byte[mouse_cycle++] = data;
        
        if (mouse_cycle == 3) { 
            mouse_cycle = 0;
            
            // Sol tık kontrolü
            if (mouse_byte[0] & 0x01) {
                handle_desktop_click();
                sky_kernel_delay(15);
                return;
            }
            
            int8_t move_x = mouse_byte[1];
            int8_t move_y = mouse_byte[2];
            
            mouse_x += move_x;
            mouse_y -= move_y; 
            
            // Ekran Sınır Koruyucuları
            if (mouse_x < 0) mouse_x = 0;
            if (mouse_x > SCREEN_WIDTH - 6) mouse_x = SCREEN_WIDTH - 6;
            if (mouse_y < 0) mouse_y = 0;
            if (mouse_y > SCREEN_HEIGHT - 6) mouse_y = SCREEN_HEIGHT - 6;
            
            // Fare her hareket ettiğinde arka planda çiz ve ekrana bas!
            gui_refresh_desktop(mouse_x, mouse_y, system_ticks);
        }
    }
}

// ====================================================================
// ÇEKİRDEK ANA GİRİŞ NOKTASI (KERNEL ENTRY)
// ====================================================================
void kernel_main(void) {
    // İlk temiz ekranı arka plandan VRAM'e fırlat
    gui_refresh_desktop(mouse_x, mouse_y, system_ticks); 
    init_mouse();          

    while (1) {
        // Fare donanımını dinle
        handle_mouse_polling();
        
        system_ticks++;
        
        // Görev çubuğundaki AI pulse animasyonu için ekranı belli aralıklarla hafifçe tetikle
        if (system_ticks % 250 == 0) { 
            gui_refresh_desktop(mouse_x, mouse_y, system_ticks);
        }
        
        sky_kernel_delay(1); 
    }
}
