#include "screen.h"

// Basit string karşılaştırma fonksiyonu (Kendi strcmp'miz)
int sky_strcmp(const char* s1, const char* s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(unsigned char*)s1 - *(unsigned char*)s2;
}

// Komut satırı arayüzümüz (Sky-Shell)
void run_command(const char* cmd) {
    sky_print("\n");
    if (sky_strcmp(cmd, "help") == 0) {
        sky_print("--- Sky-OS Kullanilabilir Komutlar ---\n");
        sky_print("help     : Komut listesini gosterir.\n");
        sky_print("neofetch : Sistem ozelliklerini ekrana basar.\n");
        sky_print("clear    : Ekrani temizler.\n");
        sky_print("--------------------------------------\n");
    } else if (sky_strcmp(cmd, "neofetch") == 0) {
        sky_print("      .---.        Sky-OS [Safe AI Kernel]\n");
        sky_print("     /     \\       Mimari: x86 (32-bit) freestanding\n");
        sky_print("     \\     /       Surum: v0.3-Shell\n");
        sky_print("      `---'        Dil: %100 Saf C & Assembly\n");
        sky_print("                   Gelistirici: Efe-Elite-Dev\n");
    } else if (sky_strcmp(cmd, "clear") == 0) {
        sky_clear_screen();
    } else {
        sky_print("Hata: Bilinmeyen komut! 'help' yazarak listeyi gorebilirsiniz.\n");
    }
}

// Gecikme fonksiyonu (Sanal makinede akışı izleyebilmemiz için)
void sky_delay(int count) {
    volatile int i, j;
    for (i = 0; i < count; i++) {
        for (j = 0; j < 5000; j++) {
            // İşlemciyi hafifçe oyala
        }
    }
}

void kernel_main(void) {
    sky_clear_screen();
    
    sky_print("========================================================================\n");
    sky_print("             Sky-OS Safe AI Kernel - Sky-Shell Baslatildi!             \n");
    sky_print("========================================================================\n\n");
    
    sky_print("[+] VGA Metin modu ekran surucusu: STABIL\n");
    sky_print("[+] Sky-Shell komut interprets motoru yuklendi.\n");
    sky_print("[+] Sanal Klavye Simulatoru Devrede...\n\n");

    // 1. ADIM: Shell Hazır uyarısı
    sky_print("SkyOS> ");
    sky_delay(2000); // 2 saniye bekle

    // 2. ADIM: Otomatik 'help' komutunu simüle ediyoruz
    sky_print("help");
    sky_delay(1000);
    run_command("help");

    // 3. ADIM: Yeni komut satırı satırı aç
    sky_print("\nSkyOS> ");
    sky_delay(3000);

    // 4. ADIM: Otomatik 'neofetch' komutunu çalıştırıyoruz
    sky_print("neofetch");
    sky_delay(1000);
    run_command("neofetch");

    sky_print("\nSkyOS> ");

    // İşlemciyi çökertmeyen en güvenli kilit döngüsü
    while (1) {
        __asm__ __volatile__("hlt");
    }
}
