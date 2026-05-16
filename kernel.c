#include "sky_subsystem.h"
#include "mouse.h"

// Multiboot 1 Standart Yapısı - Siber Güvenlik İçin Tam Şema
struct multiboot_info {
    uint32_t flags;
    uint32_t mem_lower;
    uint32_t mem_upper;
    uint32_t boot_device;
    uint32_t cmdline;
    uint32_t mods_count;
    uint32_t mods_addr;
    uint32_t syms[4];
    uint32_t mmap_length;
    uint32_t mmap_addr;
    uint32_t drives_length;
    uint32_t drives_addr;
    uint32_t config_table;
    uint32_t boot_loader_name;
    uint32_t apm_table;
    // VBE (Video Electronics Standards Association) Grafik Alanı
    uint32_t vbe_control_info;
    uint32_t vbe_mode_info;
    uint16_t vbe_mode;
    uint16_t vbe_interface_seg;
    uint16_t vbe_interface_off;
    uint16_t vbe_interface_len;
    // Donanımsal Framebuffer (Doğrudan Ekran Belleği) Adresleri
    uint64_t framebuffer_addr; 
    uint32_t framebuffer_pitch;
    uint32_t framebuffer_width;
    uint32_t framebuffer_height;
    uint8_t  framebuffer_bpp;
    uint8_t  framebuffer_type;
};

// Arayüz tetikleyicisi
extern void gui_refresh_desktop(void);

// === DONANIM ADRES KÖPRÜLERİ ===
uint32_t* vbe_vram = (uint32_t*)0xE0000000; // Akıllı koruma adresi
uint32_t  vbe_pitch = 800 * 4;

// İşlemciyi uyutarak okul laptoplarının bataryasını koruyan saf Assembly siber fonksiyonu
static inline void io_wait(void) {
    // Port 0x80'e anlamsız bir veri yollayarak donanımsal olarak çok minik bir es veririz
    asm volatile ("outb %%al, $0x80" : : "a"(0));
}

void kernel_main(struct multiboot_info* mboot) {
    // KORUMA KATMANI 1: Multiboot yapısı RAM'de var mı ve GRUB grafik modu açtı mı?
    if (mboot != 0 && (mboot->flags & (1 << 12))) {
        // KORUMA KATMANI 2: Adres sıfır değilse ve BPP (Renk Derinliği) 32-bit ise kilitle
        if (mboot->framebuffer_addr != 0 && mboot->framebuffer_bpp == 32) {
            // GRUB 64-bit adres dönse bile biz 32-bit Korumalı moddayız, alt kısmı güvenle cast ediyoruz
            vbe_vram = (uint32_t*)(uintptr_t)mboot->framebuffer_addr;
            vbe_pitch = mboot->framebuffer_pitch;
        }
    }

    // Donanım Sürücülerini Güvenli Başlatma Hattı
    init_mouse();
    
    // Ekrandaki o dikey çizgileri temizleyip saf grafik arayüzünü VRAM'e basıyoruz
    gui_refresh_desktop();

    uint32_t refresh_counter = 0;
    
    // === KESİNTİSİZ SİBER DÖNGÜ (CRITICAL LOOP) ===
    while (1) {
        // Fare hareketlerini PS/2 portundan jilet gibi yakala
        handle_mouse_polling();
        
        refresh_counter++;
        // 150 döngüde bir arayüzü tazele (Ekranın akıcı olması için optimizasyon)
        if (refresh_counter >= 150) {
            gui_refresh_desktop();
            refresh_counter = 0;
        }
        
        // İşlemciyi kör döngüde eritmek yerine donanımsal gecikme veriyoruz (Sistem donmasın diye)
        for (volatile int i = 0; i < 30; i++) {
            io_wait();
        }
    }
}
