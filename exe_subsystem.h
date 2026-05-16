#ifndef EXE_SUBSYSTEM_H
#define EXE_SUBSYSTEM_H

#include <stdint.h>

void gui_draw_rect(int start_x, int start_y, int width, int height, uint32_t color);

static inline void run_exe_subsystem(int win_x, int win_y) {
    // İç panel arka planı
    gui_draw_rect(win_x + 15, win_y + 45, 470, 290, 0xF3F3F3);
    
    // Windows Logosunu Temsil Eden Modern Dörtlü Mavi Kareler
    gui_draw_rect(win_x + 50, win_y + 80, 22, 22, 0x0078D4); 
    gui_draw_rect(win_x + 75, win_y + 80, 22, 22, 0x0078D4); 
    gui_draw_rect(win_x + 50, win_y + 105, 22, 22, 0x0078D4); 
    gui_draw_rect(win_x + 75, win_y + 105, 22, 22, 0x0078D4); 
}

#endif
