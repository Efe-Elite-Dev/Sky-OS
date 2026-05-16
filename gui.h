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

// Alt sistemlerin prototiplerini de buraya paslıyoruz
void run_exe_subsystem(int win_x, int win_y);
void run_deb_subsystem(int win_x, int win_y);
void run_sky_subsystem(int win_x, int win_y, uint32_t current_tick);

// Fonksiyon tanımlamaları (Hafıza şişmesini önlemek için inline kaldırıldı)
void gui_put_pixel(int x, int y, uint32_t color);
uint32_t mix_colors(uint32_t bg, uint32_t fg, uint8_t alpha);
void gui_draw_rect_alpha(int start_x, int start_y, int width, int height, uint32_t color, uint8_t alpha);
void gui_draw_rect(int start_x, int start_y, int width, int height, uint32_t color);
void gui_draw_rounded_window(int sx, int sy, int w, int h, int r, uint32_t color, uint8_t alpha);
void gui_render_wallpaper(void);
void gui_draw_icons(void);
void gui_draw_window_frame(int win_x, int win_y, int win_w, int win_h);
void gui_render_taskbar(uint32_t tick_count);
void gui_draw_mouse(int mx, int my);
void gui_refresh_desktop(int mx, int my, uint32_t tick);

#endif
