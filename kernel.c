#include <stdint.h>

// ====================================================================
// 1. %100 UYUMLU GÜVENLİ GRAFİK MODU (SAYFALARI KAYDIRMAYAN ÇÖZÜNÜRLÜK)
// ====================================================================
// Bu çözünürlük (800x600) her sanal makinede (VirtualBox, QEMU, VMWare)
// pürüzsüzce açılır ve dikey çizgili artifact hatasını kökten bitirir.
#define VBE_START_ADDR   0xE0000000 
#define SCREEN_WIDTH     800
#define SCREEN_HEIGHT    600
#define TOTAL_PIXELS     (SCREEN_WIDTH * SCREEN_HEIGHT)

// Windows 11 / Akıcı Arayüz Renk Paleti (Hex Format)
#define COLOR_BG         0x0A0F24  
#define COLOR_TASKBAR    0x1A2342  
#define COLOR_ACCENT     0x0078D4  
#define COLOR_WIN_BG     0xFFFFFF  
#define COLOR_WIN_TITLE  0x0F172A  
#define COLOR_MODERN_RED 0xE81123  
#define COLOR_CURSOR     0x00A2ED  
#define COLOR_TEXT_WHITE 0xFFFFFF
#define COLOR_TEXT_DARK  0x1E293B

// Sistem Durum Değişkenleri
int active_window = 0; 
uint32_t system_ticks = 0;

int mouse_x = 400; // Ekran ortasında başla
int mouse_y = 300; 
uint8_t mouse_cycle = 0;
int8_t mouse_byte[3];

// ====================================================================
// 2. GELİŞMİŞ PENCERE YÖNETİCİSİ YAPISI (DRAGGING DESTEKLİ)
// ====================================================================
typedef struct {
    int x;
    int y;
    int width;
    int height;
    int is_dragging;   // Fareyle sürükleniyor mu?
    int drag_offset_x; // Tıklanan noktanın pencere köşesine olan uzaklığı
    int drag_offset_y;
} Window;

// 800x600 için ideal pencere boyutu (Genişlik: 450, Yükseklik: 320)
Window app_window = {175, 120, 450, 320, 0, 0, 0};

// ====================================================================
// 3. MODERN FONT RENDER MOTORU VE BITMAP FONT (8x16)
// ====================================================================
unsigned char font_bitmap[128][16] = {
    ['A'] = {0x18, 0x24, 0x42, 0x42, 0x7E, 0x42, 0x42, 0x42, 0x42, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    ['B'] = {0x7C, 0x42, 0x42, 0x42, 0x7C, 0x42, 0x42, 0x42, 0x7C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    ['C'] = {0x3C, 0x42, 0x42, 0x40, 0x40, 0x40, 0x42, 0x42, 0x3C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    ['D'] = {0x78, 0x44, 0x42, 0x42, 0x42, 0x42, 0x42, 0x44, 0x78, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    ['E'] = {0x7E, 0x40, 0x40, 0x40, 0x76, 0x40, 0x40, 0x40, 0x7E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    ['I'] = {0x3C, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x3C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    ['K'] = {0x42, 0x44, 0x48, 0x70, 0x50, 0x48, 0x44, 0x42, 0x42, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    ['L'] = {0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x7E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    ['M'] = {0x42, 0x66, 0x5A, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    ['N'] = {0x42, 0x62, 0x52, 0x4A, 0x46, 0x42, 0x42, 0x42, 0x42, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    ['O'] = {0x3C, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x3C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    ['P'] = {0x7C, 0x42, 0x42, 0x42, 0x7C, 0x40, 0x40, 0x40, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    ['R'] = {0x7C, 0x42, 0x42, 0x42, 0x7C, 0x48, 0x44, 0x42, 0x42, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    ['S'] = {0x3C, 0x42, 0x40, 0x3C, 0x02, 0x02, 0x42, 0x42, 0x3C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    ['T'] = {0x7E, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    ['U'] = {0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x3C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    ['W'] = {0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x5A, 0x66, 0x42, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    ['X'] = {0x42, 0x42, 0x24, 0x18, 0x18, 0x24, 0x42, 0x42, 0x42, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    ['Y'] = {0x42, 0x42, 0x42, 0x24, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    ['-'] = {0x00, 0x00, 0x00, 0x00, 0x3E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    ['>'] = {0x40, 0x20, 0x10, 0x08, 0x04, 0x08, 0x10, 0x20, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    ['_'] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7E, 0x00},
    [' '] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
};

void gui_put_pixel(int x, int y, uint32_t color);

void gui_draw_char(int x, int y, char c, uint32_t color) {
    if (c < 0 || c >= 128) return;
    for (int row = 0; row < 16; row++) {
        unsigned char bits = font_bitmap[(int)c][row];
        for (int col = 0; col < 8; col++) {
            if (bits & (0x80 >> col)) {
                gui_put_pixel(x + col, y + row, color);
            }
        }
    }
}

void gui_draw_string(int x, int y, const char *str, uint32_t color) {
    while (*str) {
        gui_draw_char(x, y, *str, color);
        x += 8; // Her harf arası 8 piksel
        str++;
    }
}

// ====================================================================
// 4. GÜVENLİ DOUBLE BUFFERING MOTORU
// ====================================================================
uint32_t back_buffer[TOTAL_PIXELS];

void gui_put_pixel(int x, int y, uint32_t color) {
    if (x < 0 || x >= SCREEN_WIDTH || y < 0 || y >= SCREEN_HEIGHT) return;
    back_buffer[y * SCREEN_WIDTH + x] = color;
}

void vga_flush(void) {
    uint32_t *vram = (uint32_t*)VBE_START_ADDR;
    for (int i = 0; i < TOTAL_PIXELS; i++) {
        vram[i] = back_buffer[i];
    }
}

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

void gui_draw_rect(int start_x, int start_y, int width, int height, uint32_t color) {
    for (int y = start_y; y < start_y + height; y++) {
        for (int x = start_x; x < start_x + width; x++) {
            gui_put_pixel(x, y, color);
        }
    }
}

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
// 5. MASAÜSTÜ VE GÖREV ÇUBUĞU RENDER MOTORU (800x600 UYUMLU)
// ====================================================================
void gui_render_wallpaper(void) {
    for (int y = 0; y < SCREEN_HEIGHT; y++) {
        for (int x = 0; x < SCREEN_WIDTH; x++) {
            uint8_t r = 12 + (y / 36);  
            uint8_t g = 18 + (x / 45) + (y / 45);  
            uint8_t b = 45 + (x / 18); 
            back_buffer[y * SCREEN_WIDTH + x] = (r << 16) | (g << 8) | b;
        }
    }
}

void gui_draw_icons(void) {
    // EXE Subsystem İkonu
    gui_draw_rounded_window(40, 40, 40, 40, 8, 0x0078D4, 255);
    gui_draw_string(48, 90, "EXE", COLOR_TEXT_WHITE);
    
    // DEB Subsystem İkonu
    gui_draw_rounded_window(40, 120, 40, 40, 8, 0xA8193D, 255); 
    gui_draw_string(48, 170, "DEB", COLOR_TEXT_WHITE);
    
    // SKY AI Modülü İkonu
    gui_draw_rounded_window(40, 200, 40, 40, 8, 0x8B5CF6, 255); 
    gui_draw_string(48, 250, "SKY", COLOR_TEXT_WHITE);
}

void gui_draw_window_frame(Window *win, const char *title) {
    // Pencere Gölgesi
    gui_draw_rect_alpha(win->x + 6, win->y + 6, win->width, win->height, 0x000000, 70);
    // Ana Gövde
    gui_draw_rounded_window(win->x, win->y, win->width, win->height, 12, COLOR_WIN_BG, 240); 
    // Sürüklenebilir Üst Başlık Barı (Title Bar)
    gui_draw_rounded_window(win->x, win->y, win->width, 36, 12, COLOR_WIN_TITLE, 255);
    gui_draw_rect(win->x, win->y + 24, win->width, 12, COLOR_WIN_TITLE); 
    
    // Pencere Başlığını Çizdiriyoruz!
    gui_draw_string(win->x + 16, win->y + 10, title, COLOR_TEXT_WHITE);
    
    // Kapatma Butonu (X)
    gui_draw_rounded_window(win->x + win->width - 44, win->y + 6, 36, 24, 6, COLOR_MODERN_RED, 255);
    gui_draw_string(win->x + win->width - 30, win->y + 10, "X", COLOR_TEXT_WHITE); 
}

void gui_render_taskbar(uint32_t tick_count) {
    // Görev Çubuğu Tabanı (600px yükseklik sınırına göre milimetrik yerleşim)
    gui_draw_rounded_window(12, 542, SCREEN_WIDTH - 24, 46, 14, COLOR_TASKBAR, 210);
    gui_draw_string(30, 556, "SKY-OS", COLOR_TEXT_WHITE);

    int search_bar_x = (SCREEN_WIDTH / 2) - 100; 
    gui_draw_rounded_window(search_bar_x, 548, 200, 34, 8, 0xFFFFFF, 255);
    
    // Sistem Durum Işığı (Hızlı Nabız Animasyonu)
    if ((tick_count / 30) % 2 == 0) {
        gui_draw_rounded_window(755, 556, 16, 16, 8, 0x00E676, 255); 
    } else {
        gui_draw_rounded_window(755, 556, 16, 16, 8, 0x00B0FF, 255); 
    }
}

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
// 6. MODÜLER ALTSİSTEM İÇERİKLERİ (800x600 UYUMLU)
// ====================================================================
void run_exe_subsystem(int win_x, int win_y) {
    gui_draw_rect(win_x + 15, win_y + 50, 420, 255, 0xF8FAFC);
    gui_draw_string(win_x + 30, win_y + 70, "EXE SUBSYSTEM ONLINE", COLOR_TEXT_DARK);
}

void run_deb_subsystem(int win_x, int win_y) {
    gui_draw_rect(win_x + 15, win_y + 50, 420, 255, 0x0F172A); // Koyu Terminal
    gui_draw_string(win_x + 30, win_y + 70, "SKY-OS > DEB SUBSYSTEM", 0x38BDF8);
    gui_draw_string(win_x + 30, win_y + 95, "READY _", 0x38BDF8);
}

void run_sky_subsystem(int win_x, int win_y, uint32_t current_tick) {
    gui_draw_rect(win_x + 15, win_y + 50, 420, 255, 0x050515);
    gui_draw_string(win_x + 30, win_y + 70, "SKY AI CORE ENGINE", 0x818CF8);
    int pulse = (current_tick / 4) % 10;
    // Animasyon barı 800x600'e sığacak şekilde küçültüldü
    for (int i = 0; i < 6; i++) {
        int h = 30 + (i * pulse) % 70;
        gui_draw_rounded_window(win_x + 55 + (i * 65), win_y + 230 - h, 20, h, 4, 0x818CF8, 255);
    }
}

void gui_refresh_desktop(int mx, int my, uint32_t tick) {
    gui_render_wallpaper();
    gui_draw_icons();
    
    if (active_window != 0) {
        const char *title = "WINDOW";
        if (active_window == 1) title = "EXE SUBSYSTEM LAYER";
        if (active_window == 2) title = "DEB TERMINAL WINDOW";
        if (active_window == 3) title = "SKY AI CORE PULSE";
        
        gui_draw_window_frame(&app_window, title);
        
        if (active_window == 1)      run_exe_subsystem(app_window.x, app_window.y);
        else if (active_window == 2) run_deb_subsystem(app_window.x, app_window.y);
        else if (active_window == 3) run_sky_subsystem(app_window.x, app_window.y, tick);
    }
    
    gui_render_taskbar(tick);
    gui_draw_mouse(mx, my);
    vga_flush();
}

// ====================================================================
// 7. SÜREKLİ DRAG-DROP VE TIKLAMA ALGORİTMASI
// ====================================================================
void handle_desktop_click(int is_pressed) {
    if (active_window != 0) {
        // Kapatma butonu (X) tıklandı mı kontrolü
        if (mouse_x >= (app_window.x + app_window.width - 44) && 
            mouse_x <= (app_window.x + app_window.width - 8) && 
            mouse_y >= (app_window.y + 6) && mouse_y <= (app_window.y + 30)) {
            if (is_pressed) {
                active_window = 0;
                app_window.is_dragging = 0;
                gui_refresh_desktop(mouse_x, mouse_y, system_ticks);
                return;
            }
        }

        // Pencerenin üst başlık çubuğuna (Title Bar) tıklandı mı? (Sürükleme Başlangıcı)
        if (mouse_x >= app_window.x && mouse_x <= (app_window.x + app_window.width) &&
            mouse_y >= app_window.y && mouse_y <= (app_window.y + 36)) {
            if (is_pressed && !app_window.is_dragging) {
                app_window.is_dragging = 1;
                app_window.drag_offset_x = mouse_x - app_window.x;
                app_window.drag_offset_y = mouse_y - app_window.y;
            }
        }
    }

    // Fare sol tıkı bırakıldıysa sürüklemeyi anında bitir
    if (!is_pressed) {
        app_window.is_dragging = 0;
    }

    // İkonlara tıklandıysa ilgili alt sistemi aç
    if (active_window == 0 && is_pressed) {
        if (mouse_x >= 40 && mouse_x <= 80) {
            if (mouse_y >= 40 && mouse_y <= 80) active_window = 1; 
            else if (mouse_y >= 120 && mouse_y <= 160) active_window = 2; 
            else if (mouse_y >= 200 && mouse_y <= 240) active_window = 3; 
            gui_refresh_desktop(mouse_x, mouse_y, system_ticks);
        }
    }
}

// ====================================================================
// 8. FARE POLLEME SÜRÜCÜSÜ VE ANA KATMAN LOOPU
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

void handle_mouse_polling(void) {
    uint8_t status = inb(0x64);
    if ((status & 0x01) && (status & 0x20)) {
        uint8_t data = inb(0x60);
        mouse_byte[mouse_cycle++] = data;
        
        if (mouse_cycle == 3) { 
            mouse_cycle = 0;
            
            // Fare buton durumunu yakala
            int left_button = (mouse_byte[0] & 0x01);
            
            int8_t move_x = mouse_byte[1];
            int8_t move_y = mouse_byte[2];
            
            mouse_x += move_x;
            mouse_y -= move_y; 
            
            // Ekran sınırlarını 800x600'e göre kilitliyoruz
            if (mouse_x < 0) mouse_x = 0;
            if (mouse_x > SCREEN_WIDTH - 6) mouse_x = SCREEN_WIDTH - 6;
            if (mouse_y < 0) mouse_y = 0;
            if (mouse_y > SCREEN_HEIGHT - 6) mouse_y = SCREEN_HEIGHT - 6;
            
            // Eğer sürükleme modu aktifse, pencereyi farenin hareketine göre canlı güncelle!
            if (app_window.is_dragging && left_button) {
                app_window.x = mouse_x - app_window.drag_offset_x;
                app_window.y = mouse_y - app_window.drag_offset_y;
                
                // Pencerenin ekrandan tamamen çıkmasını engelle
                if (app_window.x < 0) app_window.x = 0;
                if (app_window.y < 0) app_window.y = 0;
                if (app_window.x + app_window.width > SCREEN_WIDTH) app_window.x = SCREEN_WIDTH - app_window.width;
                if (app_window.y + app_window.height > SCREEN_HEIGHT) app_window.y = SCREEN_HEIGHT - app_window.height;
            } else {
                // Sürükleme durumu haricindeki normal tıklamaları işle
                handle_desktop_click(left_button);
            }
            
            gui_refresh_desktop(mouse_x, mouse_y, system_ticks);
        }
    }
}

void kernel_main(void) {
    gui_refresh_desktop(mouse_x, mouse_y, system_ticks); 
    init_mouse();          

    while (1) {
        handle_mouse_polling();
        system_ticks++;
        
        // Ekranda pırıltı olmaması için yenileme hızını optimize ettik
        if (system_ticks % 250 == 0) { 
            gui_refresh_desktop(mouse_x, mouse_y, system_ticks);
        }
        
        sky_kernel_delay(1); 
    }
}
