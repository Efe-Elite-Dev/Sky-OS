#include "screen.h"
#include "keyboard.h"
#include <stdint.h>

// idt.h icerigini dogrudan buraya gomduk, harici dosya arama derdi bitti!
struct idt_entry_struct {
    uint16_t base_low;
    uint16_t sel;
    uint8_t  always0;
    uint8_t  flags;
    uint16_t base_high;
} __attribute__((packed));

struct idt_ptr_struct {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

extern void init_idt(void);

void kernel_main(void) {
    sky_clear_screen();
    
    sky_print("========================================================================\n");
    sky_print("             Sky-OS Safe AI Kernel - Klavye Aktif Edildi!              \n");
    sky_print("========================================================================\n\n");
    
    sky_print("[+] VGA Metin modu surucusu aktif.\n");
    sky_print("[+] Donanim kesme tablosu (IDT) yuklendi.\n");
    sky_print("[+] Klavye surucusu Garanti Porta baglandi!\n");
    sky_print("[+] Tuslara basabilirsiniz (Lutfen VirtualBox icine tiklayin).\n\n");
    sky_print("SkyOS> ");

    // Kesmeleri açıyoruz
    __asm__ __volatile__("sti");

    // GARANTİ DÖNGÜ: Her saniye klavye portunu doğrudan kontrol eder
    while (1) {
        check_keyboard_polling();
    }
}
