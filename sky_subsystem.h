#ifndef SKY_SUBSYSTEM_H
#define SKY_SUBSYSTEM_H

#include <stdint.h>

void gui_draw_rect(int start_x, int start_y, int width, int height, uint32_t color);

static inline void run_sky_subsystem(int win_x, int win_y, uint32_t current_tick) {
    // Derin Siber Mavi Arka Plan
    gui_draw_rect(win_x + 15, win_y + 45, 470, 290, 0x0F172A);
    
    // Sky-OS Yerel Altın Logosu
    gui_draw_rect(win_x + 35, win_y + 65, 30, 30, 0xFFB900);
    
    // Efe Tech MKU Matris Akışı Simülasyonu
    // Gelen zamanlayıcı (tick) değerine göre piksellerin yerini dinamik kaydırarak 
    // kodların ekranda aktığı hissini veriyoruz usta!
    int offset = (current_tick % 10);
    for(int i = 0; i < 5; i++) {
        int bar_y = win_y + 120 + (i * 30) + offset;
        if(bar_y < win_y + 300) {
            gui_draw_rect(win_x + 50 + (i * 15), bar_y, 120 - (i * 10), 8, 0x10B981); // Canlı Matrix Yeşili
        }
    }
}

#endif
