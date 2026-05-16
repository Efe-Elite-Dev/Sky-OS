#ifndef DEB_SUBSYSTEM_H
#define DEB_SUBSYSTEM_H

#include <stdint.h>

void gui_draw_rect(int start_x, int start_y, int width, int height, uint32_t color);

static inline void run_deb_subsystem(int win_x, int win_y) {
    // Koyu Linux Terminal Teması (Kömür Grisi)
    gui_draw_rect(win_x + 15, win_y + 45, 470, 290, 0x212121);
    
    // Debian Paket Simgesi (Kırmızı/Bordo Blok)
    gui_draw_rect(win_x + 40, win_y + 70, 40, 40, 0xD70A53);
    
    // Yükleme Çubuğu / Progress Bar Göstergesi (Sahte Paket Çözümleme)
    gui_draw_rect(win_x + 40, win_y + 260, 420, 12, 0x444444); // Bar Arka Planı
    gui_draw_rect(win_x + 40, win_y + 260, 280, 12, 0x2EA44F); // Dolmakta Olan Yeşil Bar (%65)
}

#endif
