#include "sky_subsystem.h"
#include "mouse.h"
#include "gui.h"

// Alt sistemlerin ve yapay zekanın fonksiyon köprüleri
extern int ai_predict_hardware_load(int mouse_delta_x, int loop_count);
extern uint32_t* vbe_vram; 
extern uint32_t  vbe_pitch;

static inline void io_wait(void) {
    asm volatile ("outb %%al, $0x80" : : "a"(0));
}

/**
 * 🚀 KUSURSUZ x86 ÇEKİRDEK GİRİŞ KAPISI
 */
void kernel_main(struct multiboot_info* mboot) {
    
    // x86 GÜVENLİ GRAPHICS KONTROLÜ
    if (mboot != 0 && (mboot->framebuffer_addr != 0)) {
        vbe_vram = (uint32_t*)(uintptr_t)mboot->framebuffer_addr;
        vbe_pitch = mboot->framebuffer_pitch;
    } else {
        // FALLBACK: Adres bozuk gelirse VirtualBox varsayılan LFB alanı
        vbe_vram = (uint32_t*)0xE0000000; 
        vbe_pitch = 800 * 4;
    }

    // === SÜRÜCÜLER VE GRAFİK BAŞLATMA ===
    init_mouse();
    gui_refresh_desktop();

    uint32_t refresh_counter = 0;
    int current_wait_cycles = 30;

    // === YAPAY ZEKA DESTEKLİ ANA DÖNGÜ ===
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
