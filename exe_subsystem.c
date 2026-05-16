#include "sky_subsystem.h"
#include "gui.h"

int setup_stage = 0; // Kurulum aşaması takibi

/**
 * 💻 SKY-OS YAPAY ZEKA DESTEKLİ KURULUM SİHİRBAZI
 */
void run_exe_subsystem(void) {
    // 1. ANA KURULUM PENCERESİ TABAĞI
    gui_draw_rect(150, 100, 500, 400, 0xDCDCDC); // Şık Açık Gri Arka Plan
    gui_draw_rect(150, 100, 500, 30, 0x4A69BD);  // Kurumsal Mavi Başlık Çubuğu

    // Siber Gölgelendirme (Pencereye 3D derinlik katıyoruz)
    gui_draw_rect(150, 495, 500, 5, 0x888888);
    gui_draw_rect(645, 100, 5, 400, 0x888888);

    // 2. AŞAMA KONTROLLERİ
    if (setup_stage == 0) {
        // Hoş Geldiniz Ekranı Metin Kutusu
        gui_draw_rect(180, 160, 440, 180, 0xFFFFFF); 
        
        // Aktif "İleri (Next)" Butonu
        gui_draw_rect(540, 440, 80, 30, 0x1E3799); 
    } 
    else if (setup_stage == 1) {
        // İlerleme Çubuğu (Disk Biçimlendirme ve Kurulum Simülasyonu)
        gui_draw_rect(180, 220, 440, 30, 0xEEF2F3); // Boş Çubuk
        gui_draw_rect(180, 220, 250, 30, 0x78E08F); // Dolan Bar (%60 Yeşil)
        
        // Bekleme Durumunda Pasif Buton
        gui_draw_rect(540, 440, 80, 30, 0x95A5A6); 
    }
    else if (setup_stage == 2) {
        // Kurulum Başarı Alanı
        gui_draw_rect(180, 160, 440, 180, 0x78E08F); 
        
        // "Sistemi Başlat (Finish)" Butonu
        gui_draw_rect(520, 440, 100, 30, 0x218C74); 
    }
}
