#include "sky_subsystem.h"

// Dışarıdaki ekran değişkenlerine erişim köprüsü
extern uint32_t* vbe_vram;
extern uint32_t  vbe_pitch;

// Yapay zekanın siber hafızası
static int ai_error_history[10] = {0};
static int ai_self_repair_count = 0;

// Sinir ağı ağırlıkları
static const int ai_weights[2] = {3, -2}; 
static const int ai_bias = 5;

/**
 * MİKRO SİNİR AĞI: kernel.c'nin aradığı o fonksiyon!
 */
int ai_predict_hardware_load(int mouse_delta_x, int loop_count) {
    int score = (mouse_delta_x * ai_weights[0]) + (loop_count * ai_weights[1]) + ai_bias;
    if (score < 0) {
        return 1; // Sistem zorlanıyor
    }
    return 0; // Sistem rahat
}

/**
 * SELF-HEALING MOTORU: deb_subsystem.c'nin aradığı o fonksiyon!
 */
void ai_detect_and_self_repair(uint32_t fault_address, int error_code) {
    int log_index = ai_self_repair_count % 10;
    ai_error_history[log_index] = error_code;
    ai_self_repair_count++;

    if (error_code == 139 || error_code == -1) {
        uint8_t* repair_target = (uint8_t*)(uintptr_t)fault_address;
        if (repair_target != 0) {
            *repair_target = 0xC3; // Canlı x86 RET yama kodu yazıldı!
        }
        
        // Başarılı tamir sinyali olarak sol üst köşeye kırmızı nokta çiz
        if (vbe_vram != 0) {
            for (int y = 0; y < 20; y++) {
                for (int x = 0; x < 20; x++) {
                    vbe_vram[y * (vbe_pitch / 4) + x] = 0xFF0000;
                }
            }
        }
    }
}
