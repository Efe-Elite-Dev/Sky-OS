#include "screen.h"
#include "keyboard.h"
#include "idt.h"

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
