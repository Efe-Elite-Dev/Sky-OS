#include "sky_subsystem.h"
#include "mouse.h"
#include "gui.h"

// Alt sistemlerin, DEB'in ve yapay zekanın fonksiyon köprüleri
extern int ai_predict_hardware_load(int mouse_delta_x, int loop_count);
extern void run_exe_subsystem(void);
extern void init_idt(void); 

// Orijinal değişkenleri sky_subsystem'den çekiyoruz, linker çakışması çözüldü!
extern uint32_t* vbe_vram; 
extern uint32_t  vbe_pitch;

static inline void io_wait(void) {
    asm volatile ("outb %%al, $0x80" : : "a"(0));
}

/**
 * 🚀 TÜM ALT SİSTEMLERİ BARINDIRAN ULTIMATE X86 ÇEKİRDEĞİ
 */
void kernel_main(struct multiboot_info* mboot) {
    
    // 1. DONANIMSAL GRAFİK ADRESİNİ YAKALA
    if (mboot != 0 && (mboot->framebuffer_addr != 0)) {
        vbe_vram = (uint32_t*)(uintptr_t)mboot->framebuffer_addr;
        vbe_pitch = mboot->framebuffer_pitch;
    } else {
        vbe_vram = (uint32_t*)0xE0000000; // Standart LFB Fallback
        vbe_pitch = 800 * 4;
    }

    // 2. DONANIM SÜRÜCÜLERİNİ VE İŞLEMCİ KESMELERİNİ BAŞLAT
    init_idt();   // Klavye ve IDT yüklemesi
    init_mouse(); // Fare altyapısı
    
    // 3. İLK EKRAN TAZELEMESİ
    gui_refresh_desktop();

    uint32_t refresh_counter = 0;
    int current_wait_cycles = 30;

    // === YAPAY ZEKA DESTEKLİ VE KURULUM EKRANLI ANA DÖNGÜ ===
    while (1) {
        handle_mouse_polling(); // Fare hareket kontrolü
        
        refresh_counter++;
        if (refresh_counter >= 15) { 
            gui_refresh_desktop();  // Arka planı (Gece Mavisi) temizler
            run_exe_subsystem();    // Üzerine kurulum pencerelerini basar!
            refresh_counter = 0;
        }
        
        // YAPAY ZEKA DESTEKLİ CPU OPTİMİZASYONU
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
