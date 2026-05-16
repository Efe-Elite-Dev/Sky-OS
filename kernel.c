#include "sky_subsystem.h"
#include "mouse.h"
#include "gui.h"

extern int ai_predict_hardware_load(int mouse_delta_x, int loop_count);
extern uint32_t* vbe_vram; 
extern uint32_t  vbe_pitch;

static inline void io_wait(void) {
    asm volatile ("outb %%al, $0x80" : : "a"(0));
}

// Güvenli ana giriş kapısı
void kernel_main(struct multiboot_info* mboot) {
    
    // KORUMA: Eğer Stack köprüsü başarıyla kurulduysa ve GRUB adresi pasladıysa
    if (mboot != 0 && (mboot->flags & (1 << 11))) {
        vbe_vram = (uint32_t*)(uintptr_t)mboot->framebuffer_addr;
        vbe_pitch = mboot->framebuffer_pitch;
    } else {
        // Fallback: Eğer donanım adresi bozuk gelirse çökme yapmaması için güvenli adres tanımla
        vbe_vram = (uint32_t*)0xFD000000; // Standart sanal VRAM başlangıç tahmini
        vbe_pitch = 800 * 4;
    }

    // Donanım Sürücüleri ve Grafik Başlatma (Senin mevcut kodların)
    init_mouse();
    gui_refresh_desktop();

    uint32_t refresh_counter = 0;
    int current_wait_cycles = 30;

    // === YAPAY ZEKA DESTEKLİ ANA DÖNGÜN (Aynen Korunuyor) ===
    while (1) {
        handle_mouse_polling();
        
        refresh_counter++;
        if (refresh_counter >= 150) {
            gui_refresh_desktop();
            refresh_counter = 0;
        }
        
        int system_stress = ai_predict_hardware_load(10, refresh_counter);
        
        if (system_stress == 1) {
            current_wait_cycles = 60;
        } else {
            current_wait_cycles = 15;
        }

        for (volatile int i = 0; i < current_wait_cycles; i++) {
            io_wait();
        }
    }
}
