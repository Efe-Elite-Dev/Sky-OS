#include "sky_subsystem.h"
#include "gui.h"

// Kurulum ekranının aşamalarını takip eden siber durum değişkeni
int setup_stage = 0; 

/**
 * 💻 SKY-OS YAPAY ZEKA DESTEKLİ KURULUM SİHİRBAZI
 */
void run_exe_subsystem(void) {
    // 1. ANA PENCERE (Kurulum Arka Plan Kutusu)
    // Gri şık bir kurulum penceresi tabanı çiziyoruz
    gui_draw_rect(150, 100, 500, 400, 0xDCDCDC); // Açık Gri Pencere
    gui_draw_rect(150, 100, 500, 30, 0x4A69BD);  // Mavi Başlık Çubuğu (Title Bar)

    // Pencereye siber gölgelendirme efekti verelim
    gui_draw_rect(150, 495, 500, 5, 0x888888);
    gui_draw_rect(645, 100, 5, 400, 0x888888);

    // 2. AŞAMALARA GÖRE KURULUM İÇERİĞİ
    if (setup_stage == 0) {
        // Hoş Geldiniz Ekranı Kutuları
        gui_draw_rect(180, 160, 440, 180, 0xFFFFFF); // Beyaz metin alanı simülasyonu
        
        // Sağ Alttaki "İleri (Next)" Butonu (Aktif ve Tıklanabilir Durumda)
        gui_draw_rect(540, 440, 80, 30, 0x1E3799); // Koyu Mavi Buton
    } 
    else if (setup_stage == 1) {
        // "Yükleniyor ve Disk Biçimlendiriliyor" Aşaması (İlerleme Çubuğu / Progress Bar)
        gui_draw_rect(180, 220, 440, 30, 0xEEF2F3); // Bar Arka Planı
        gui_draw_rect(180, 220, 250, 30, 0x78E08F); // Yeşil İlerleme Durumu (%60)
        
        // Alt Kısımdaki Buton (Pasif Durumda)
        gui_draw_rect(540, 440, 80, 30, 0x95A5A6); // Gri Buton
    }
    else if (setup_stage == 2) {
        // "Kurulum Başarıyla Tamamlandı" Ekranı
        gui_draw_rect(180, 160, 440, 180, 0x78E08F); // Başarılı Yeşili Alan
        
        // "Sistemi Başlat (Finish)" Butonu
        gui_draw_rect(520, 440, 100, 30, 0x218C74); // Koyu Yeşil Buton
    }
}
