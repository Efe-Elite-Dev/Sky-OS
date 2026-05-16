#include "sky_subsystem.h"
#include "gui.h"

// Donanımsal adresler (x86 LFB köprüleri)
extern uint32_t* vbe_vram;
extern uint32_t  vbe_pitch;

/**
 * 🎨 x86 STANDART DİKDÖRTGEN ÇİZME MOTORU
 */
void gui_draw_rect(int x, int y, int width, int height, uint32_t color) {
    if (vbe_vram == 0) return;

    // x86 standardında en güvenli satır genişliği hesabı:
    // Eğer donanım pitch değerini sıfır veya hatalı verdiyse, 
    // doğrudan 800 piksel standardına (800 * 4 byte) geri düş (Fallback).
    uint32_t width_pixels = (vbe_pitch > 0) ? (vbe_pitch / 4) : 800;

    for (int curr_y = y; curr_y < y + height; curr_y++) {
        // x86 Ekran Taşıma Koruması (600 dikey piksel sınırı)
        if (curr_y < 0 || curr_y >= 600) continue;

        for (int curr_x = x; curr_x < x + width; curr_x++) {
            // x86 Yatay Piksel Sınırı (800 yatay piksel sınırı)
            if (curr_x < 0 || curr_x >= 800) continue;

            // Tam x86 lineer framebuffer indeksleme formülü
            uint32_t pixel_index = (curr_y * width_pixels) + curr_x;
            vbe_vram[pixel_index] = color;
        }
    }
}

/**
 * 🚀 MASAÜSTÜ YENİLEME MOTORU
 */
void gui_refresh_desktop(void) {
    if (vbe_vram == 0) return;

    // Tüm ekranı saf x86 renk uzayında şık bir Gece Mavisine çekiyoruz
    gui_draw_rect(0, 0, 800, 600, 0x1A1A2E);

    // Tam ortaya beyaz amblemimizi çakıyoruz
    gui_draw_rect(350, 250, 100, 100, 0xFFFFFF);
}
