#include <stdint.h>

// ====================================================================
// 1. MODERN SİSTEM TANIMLAMALARI VE ÇÖZÜNÜRLÜK AYARLARI
// ====================================================================
#define VBE_START_ADDR   0xE0000000 
#define SCREEN_WIDTH     1024
#define SCREEN_HEIGHT    768
#define TOTAL_PIXELS     (SCREEN_WIDTH * SCREEN_HEIGHT)

// Windows 11 / Akıcı Arayüz Renk Paleti (Hex Format)
#define COLOR_BG         0x0A0F24  
#define COLOR_TASKBAR    0x1A2342  
#define COLOR_ACCENT     0x0078D4  
#define COLOR_WIN_BG     0xFFFFFF  
#define COLOR_WIN_TITLE  0x0F172A  
#define COLOR_MODERN_RED 0xE81123  
#define COLOR_CURSOR     0x00A2ED  

// Sistem Durum Değişkenleri
int active_window = 0; 
uint32_t system_ticks = 0;

int mouse_x = 512; 
int mouse_y = 384; 
uint8_t mouse_cycle = 0;
int8_t mouse_byte[3];

// ====================================================================
// 2. GELİŞMİŞ PENCERE YÖNETİCİSİ (WINDOW MANAGER) YAPISI
// ====================================================================
typedef struct {
    int x;
    int y;
    int width;
    int height;
    int is_dragging;   
    int drag_offset_x; 
    int drag_offset_y;
} Window;

// Ana uygulama penceremiz
Window app_window = {262, 184, 500, 350, 0, 0, 0};

// ====================================================================
// 3. DOUBLE BUFFERING (ÇİFT TAMPON) MOTORU
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
// 4. MODERN WINDOWS 11 STİLİ ARAYÜZ BİLEŞENLERİ
// ====================================================================

void gui_render_wallpaper(void) {
    for (int y = 0; y < SCREEN_HEIGHT; y++) {
        for (int x = 0; x < SCREEN_WIDTH; x++) {
            uint8_t r = 15 + (y / 32);  
            uint8_t g = 20 + (x / 40) + (y / 40);  
            uint8_t b = 50 + (x / 16); 
            back_buffer[y * SCREEN_WIDTH + x] = (r << 16) | (g << 8) | b;
        }
    }
}

void gui_draw_icons(void) {
    gui_draw_rounded_window(40, 40, 40, 40, 8, 0x0078D4, 255);
    gui_draw_rect(50, 52, 20, 16, 0xFFFFFF); 
    
    gui_draw_rounded_window(40, 105, 40, 40, 8, 0xA8193D, 255); 
    gui_draw_rect(54, 113, 12, 24, 0xE0E0E0); 
    
    gui_draw_rounded_window(40, 170, 40, 40, 8, 0x8B5CF6, 255); 
    gui_draw_rect(52, 182, 16, 16, 0x1E1B4B); 
}

void gui_draw_window_frame(Window *win) {
    gui_draw_rect_alpha(win->x + 6, win->y + 6, win->width, win->height, 0x000000, 70);
    gui_draw_rounded_window(win->x, win->y, win->width, win->height, 12, COLOR_WIN_BG, 240); 
    gui_draw_rounded_window(win->x, win->y, win->width, 36, 12, COLOR_WIN_TITLE, 255);
    gui_draw_rect(win->x, win->y + 24, win->width, 12, COLOR_WIN_TITLE); 
    gui_draw_rounded_window(win->x + win->width - 44, win->y + 6, 36, 24, 6, COLOR_MODERN_RED, 255);
}

void gui_render_taskbar(uint32_t tick_count) {
    gui_draw_rounded_window(12, 714, SCREEN_WIDTH - 24, 46, 14, COLOR_TASKBAR, 210);
    
    int search_bar_x = (SCREEN_WIDTH / 2) - 100; 
    gui_draw_rounded_window(search_bar_x, 720, 200, 34, 8, 0xFFFFFF, 255);
    
    int start_icon_x = search_bar_x - 140;
    gui_draw_rounded_window(start_icon_x, 721, 32, 32, 8, COLOR_ACCENT, 255);      
    gui_draw_rounded_window(start_icon_x + 40, 721, 32, 32, 8, 0xFFB900, 255); 
    gui_draw_rounded_window(start_icon_x + 80, 721, 32, 32, 8, 0x107C41, 255); 

    if ((tick_count / 30) % 2 == 0) {
        gui_draw_rounded_window(980, 728, 16, 16, 8, 0x00E676, 255); 
    } else {
        gui_draw_rounded_window(980, 728, 16, 16, 8, 0x00B0FF, 255); 
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
// 5. MODÜLER ALTSİSTEM İÇERİK RENDERLARI
// ====================================================================

void run_exe_subsystem(int win_x, int win_y) {
    gui_draw_rect(win_x + 15, win_y + 50, 470, 285, 0xF8FAFC);
    gui_draw_rounded_window(win_x + 40, win_y + 80, 80, 60, 6, 0x38BDF8, 255);
    gui_draw_rounded_window(win_x + 140, win_y + 80, 80, 60, 6, 0x34D399, 255);
}

void run_deb_subsystem(int win_x, int win_y) {
    gui_draw_rect(win_x + 15, win_y + 50, 470, 285, 0x0F172A); 
    gui_draw_rect(win_x + 35, win_y + 70, 20, 4, 0x38BDF8);    
}

void run_sky_subsystem(int win_x, int win_y, uint32_t current_tick) {
    gui_draw_rect(win_x + 15, win_y + 50, 470, 285, 0x050515);
    int pulse = (current_tick / 4) % 10;
    gui_draw_rect_alpha(win_x + 30, win_y + 120, 440, 160, 0x1E1B4B, 150); 
    for (int i = 0; i < 7; i++) {
        int h = 30 + (i * pulse) % 70;
        gui_draw_rounded_window(win_x + 55 + (i * 60), win_y + 250 - h, 20, h, 4, 0x818CF8, 255);
    }
}

void gui_refresh_desktop(int mx, int my, uint32_t tick) {
    gui_render_wallpaper();
    gui_draw_icons();
    
    if (active_window != 0) {
        gui_draw_window_frame(&app_window);
        if (active_window == 1)      run_exe_subsystem(app_window.x, app_window.y);
        else if (active_window == 2) run_deb_subsystem(app_window.x, app_window.y);
        else if (active_window == 3) run_sky_subsystem(app_window.x, app_window.y, tick);
    }
    
    gui_render_taskbar(tick);
    gui_draw_mouse(mx, my);
    vga_flush();
}

// ====================================================================
// 6. SÜREKLİ SÜRÜKLEME VE TIKLAMA MANTIĞI
// ====================================================================

void handle_desktop_click(int is_pressed) {
    if (active_window != 0) {
        // Kapatma butonu (X) alanı
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

        // Başlık çubuğu tıklama (Sürükleme başlatma)
        if (mouse_x >= app_window.x && mouse_x <= (app_window.x + app_window.width) &&
            mouse_y >= app_window.y && mouse_y <= (app_window.y + 36)) {
            if (is_pressed && !app_window.is_dragging) {
                app_window.is_dragging = 1;
                app_window.drag_offset_x = mouse_x - app_window.x;
                app_window.drag_offset_y = mouse_y - app_window.y;
            }
        }
    }

    if (!is_pressed) {
        app_window.is_dragging = 0;
    }

    if (active_window == 0 && is_pressed) {
        if (mouse_x >= 40 && mouse_x <= 80) {
            if (mouse_y >= 40 && mouse_y <= 80) active_window = 1; 
            else if (mouse_y >= 105 && mouse_y <= 145) active_window = 2; 
            else if (mouse_y >= 170 && mouse_y <= 210) active_window = 3; 
            gui_refresh_desktop(mouse_x, mouse_y, system_ticks);
        }
    }
}

// ====================================================================
// 7. DONANIM SÜRÜCÜLERİ VE ÇEKİRDEK LOOPU
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
            
            int left_button = (mouse_byte[0] & 0x01);
            int8_t move_x = mouse_byte[1];
            int8_t move_y = mouse_byte[2];
            
            mouse_x += move_x;
            mouse_y -= move_y; 
            
            if (mouse_x < 0) mouse_x = 0;
            if (mouse_x > SCREEN_WIDTH - 6) mouse_x = SCREEN_WIDTH - 6;
            if (mouse_y < 0) mouse_y = 0;
            if (mouse_y > SCREEN_HEIGHT - 6) mouse_y = SCREEN_HEIGHT - 6;
            
            if (app_window.is_dragging && left_button) {
                app_window.x = mouse_x - app_window.drag_offset_x;
                app_window.y = mouse_y - app_window.drag_offset_y;
                
                if (app_window.x < 0) app_window.x = 0;
                if (app_window.y < 0) app_window.y = 0;
                if (app_window.x + app_window.width > SCREEN_WIDTH) app_window.x = SCREEN_WIDTH - app_window.width;
            } else {
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
        
        if (system_ticks % 250 == 0) { 
            gui_refresh_desktop(mouse_x, mouse_y, system_ticks);
        }
        
        sky_kernel_delay(1); 
    }
}
