#ifndef GUI_H
#define GUI_H

#include <stdint.h>

#define VBE_START_ADDR  0xE0000000 
#define SCREEN_WIDTH    1024
#define SCREEN_HEIGHT   768

// Modern Renk Paleti Tanımlamaları
#define COLOR_TASKBAR          0xEEF2F7  
#define COLOR_SEARCH_BAR       0xFFFFFF  
#define COLOR_ICON_BLUE        0x0078D4  
#define COLOR_ICON_GREEN       0x107C41  
#define COLOR_MODERN_CURSOR    0x0067C0  

// Ham Piksel Basıcı
static inline void gui_put_pixel(int x, int y, uint32_t color) {
    if (x < 0 || x >= SCREEN_WIDTH || y < 0 || y >= SCREEN_HEIGHT) return;
    uint32_t *vbe_buffer = (uint32_t*)VBE_START_ADDR;
    vbe_buffer[y * SCREEN_WIDTH + x] = color;
}

// Dikdörtgen Alan Çizici
static inline void gui_draw_rect(int start_x, int start_y, int width, int height, uint32_t color) {
    for (int y = start_y; y < start_y + height; y++) {
        for (int x = start_x; x < start_x + width; x++) {
            gui_put_pixel(x, y, color);
        }
    }
}

// Mor-Mavi Dalgalı Modern Duvar Kağıdı Motoru
static inline void gui_render_wallpaper(void) {
    for (int y = 0; y < SCREEN_HEIGHT; y++) {
        for (int x = 0; x < SCREEN_WIDTH; x++) {
            uint8_t r = 30 + ((x * 40) / SCREEN_WIDTH) + ((y * 60) / SCREEN_HEIGHT);  
            uint8_t g = 50 + ((x * 20) / SCREEN_WIDTH) + ((y * 30) / SCREEN_HEIGHT);  
            uint8_t b = 120 + ((x * 100) / SCREEN_WIDTH) - ((y * 20) / SCREEN_HEIGHT); 
            
            // Merkezdeki yumuşak ışık patlaması
            if (x > 300 && x < 700 && y > 200 && y < 550) {
                r += 40; g += 25; b += 30;
            }
            gui_put_pixel(x, y, (r << 16) | (g << 8) | b);
        }
    }
}

// Masaüstü İkonları Gridi
static inline void gui_draw_icons(void) {
    // Geri Dönüşüm Kutusu
    gui_draw_rect(40, 40, 32, 32, COLOR_ICON_BLUE);
    gui_draw_rect(48, 48, 16, 16, 0xFFFFFF); 
    
    // Sky-Görüntü Merkezi (NVIDIA stili)
    gui_draw_rect(40, 100, 32, 32, COLOR_ICON_GREEN);
    gui_draw_rect(52, 106, 8, 20, 0xFFFFFF);
    
    // Sky-Hava Durumu
    gui_draw_rect(40, 160, 32, 32, 0x00B7F1); 
}

// Ortalanmış Görev Çubuğu (Taskbar)
static inline void gui_render_taskbar(void) {
    gui_draw_rect(0, 718, SCREEN_WIDTH, 50, COLOR_TASKBAR);
    
    // Ortalı Arama Çubuğu
    int search_bar_x = (SCREEN_WIDTH / 2) - 150; 
    gui_draw_rect(search_bar_x, 724, 300, 36, COLOR_SEARCH_BAR);
    
    // Uygulama İkon Blokları (Başlat, Dosyalar, Store)
    int start_icon_x = search_bar_x - 100;
    gui_draw_rect(start_icon_x, 726, 32, 32, 0x0078D4);      
    gui_draw_rect(start_icon_x + 40, 726, 32, 32, 0xFFB900); 
    gui_draw_rect(start_icon_x + 80, 726, 32, 32, 0x107C41); 
}

// İçi Beyaz Kenarı Mavi Şık Ok İmleci
static inline void gui_draw_mouse(int mx, int my) {
    uint32_t c = COLOR_MODERN_CURSOR;
    uint32_t w = 0xFFFFFF; 
    
    gui_put_pixel(mx, my, c);
    gui_put_pixel(mx, my + 1, c);   gui_put_pixel(mx + 1, my + 1, c);
    gui_put_pixel(mx, my + 2, c);   gui_put_pixel(mx + 1, my + 2, w); gui_put_pixel(mx + 2, my + 2, c);
    gui_put_pixel(mx, my + 3, c);   gui_put_pixel(mx + 1, my + 3, w); gui_put_pixel(mx + 2, my + 3, w); gui_put_pixel(mx + 3, my + 3, c);
    gui_put_pixel(mx, my + 4, c);   gui_put_pixel(mx + 1, my + 4, w); gui_put_pixel(mx + 2, my + 4, c);
    gui_put_pixel(mx, my + 5, c);   gui_put_pixel(mx + 1, my + 5, c);
}

// Tüm Masaüstünü Tek Seferde Tetikleyen Ana Fonksiyon
static inline void gui_refresh_desktop(int mx, int my) {
    gui_render_wallpaper();
    gui_draw_icons();
    gui_render_taskbar();
    gui_draw_mouse(mx, my); // Fareyi en son katman olarak basıyoruz
}

#endif
