#include "screen.h"
#include "keyboard.h"

void kernel_main(void) {
    // Önce ekranı temizle ve başlat
    sky_clear_screen();
    
    // Üstte şık bir başlık barı yapalım
    sky_print("========================================================================\n");
    sky_print("             Sky-OS Safe AI Kernel - Surucu Katmani Yuklendi!          \n");
    sky_print("========================================================================\n\n");
    
    sky_print("[+] VGA Metin modu surucusu aktif edildi.\n");
    sky_print("[+] Donanim kesme tablosu (IDT) yukleniyor...\n");
    
    // Klavyeyi ve kesme motorunu başlatıyoruz
    init_keyboard();
    
    sky_print("[+] Klavye surucusu basariyla baglandi!\n");
    sky_print("[+] Sky-OS su an canli ve tus basimlarini dinliyor.\n\n");
    sky_print("Sistem Hazir> ");

    // İşlemciyi uyku modunda tut ama kesmelere açık bırak (sti)
    while (1) {
        __asm__ __volatile__("sti\n\thlt");
    }
}
