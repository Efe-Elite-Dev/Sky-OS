#ifndef GUI_H
#define GUI_H

#include <stdint.h>

#define VBE_START_ADDR  0xE0000000 
#define SCREEN_WIDTH    1024
#define SCREEN_HEIGHT   768

#define COLOR_TASKBAR          0xCCF2F7FF  
#define COLOR_SEARCH_BAR       0xFFFFFFFF  
#define COLOR_MODERN_CURSOR    0x0067C0    
#define COLOR_WIN_TITLE        0x0F172A    
#define COLOR_MODERN_RED       0xE81123    

extern int active_window;

static inline void gui_put_pixel(int x, int y, uint32_t color) {
    if (x < 0 || x >= SCREEN_WIDTH || y < 0 || y >= SCREEN_HEIGHT) return;
    uint32_t *vbe_buffer = (uint32_t*)VBE_START_ADDR;
    vbe_buffer[y * SCREEN_WIDTH + x] = color;
}

static inline uint32_t mix_colors(uint32_t bg, uint32_t fg, uint8_t alpha) {
    uint8_t bg_r = (bg >> 16) & 0xFF; uint8_t bg_g = (bg >> 8) & 0xFF; uint8_t bg_b = bg & 0xFF;
    uint8_t fg_r = (fg >> 16) & 0xFF; uint8_t fg_g = (fg >> 8) & 0xFF; uint8_t fg_b = fg & 0xFF;

    uint8_t res_r = ((fg_r * alpha) + (bg_r * (255 - alpha))) / 255;
    uint8_t res_g = ((fg_g * alpha) + (bg_g * (255 - alpha))) / 255;
    uint8_t res_b = ((fg_b * alpha) + (bg_b * (255 - alpha))) / 255;

    return (res_r << 16) | (res_g << 8) | res_b;
}

static inline void gui_draw_rect_alpha(int start_x, int start_y, int width, int height, uint32_t color, uint8_t alpha) {
    uint32_t *vbe_buffer = (uint32_t*)VBE_START_ADDR;
    for (int y = start_y; y < start_y + height; y++) {
        for (int x = start_x; x < start_x + width; x++) {
            if (x >= 0 && x < SCREEN_WIDTH && y >= 0 && y < SCREEN_HEIGHT) {
                uint32_t bg_color = vbe_buffer[y * SCREEN_WIDTH + x];
                vbe_buffer[y * SCREEN_WIDTH + x] = mix_colors(bg_color, color, alpha);
            }
        }
    }
}

// Alt sistemlerin çağırabilmesi için düz inline yapıyoruz
static inline void gui_draw_rect(int start_x, int start_y, int width, int height, uint32_t color) {
    for (int y = start_y; y < start_y + height; y++) {
        for (int x = start_x; x < start_x + width; x++) {
            gui_put_pixel(x, y, color);
        }
    }
}

// ALT SİSTEMLERİ BURADA İÇERİ ALIYORUZ (Artık gui_draw_rect fonksiyonunu şak diye tanıyacaklar)
#include "exe_subsystem.h"
#include "deb_subsystem.h"
#include "sky_subsystem.h"

// ... (Dosyanın geri kalan gui_draw_rounded_window vs. fonksiyonları aynen kalıyor)
