#ifndef MOUSE_H
#define MOUSE_H

#include <stdint.h>

// Ekran Sınırları
#define SCREEN_WIDTH     800
#define SCREEN_HEIGHT    600

// === PS/2 FARE DONANIMSAL OPTİMİZASYON VE HATA ÇÖZÜCÜ MAKROLARI ===
#define MOUSE_DATA_PORT   0x60
#define MOUSE_STATUS_PORT 0x64

#define MOUSE_BIT_SYNC    0x08  // Paket kaymasını önleyen nöbetçi bit (3. Bit her zaman 1 olmalı)
#define MOUSE_X_OVERFLOW  0x40  // X ekseni ani sıçrama engelleme filtresi
#define MOUSE_Y_OVERFLOW  0x80  // Y ekseni ani sıçrama engelleme filtresi
#define MOUSE_X_SIGN      0x10  // X ekseni negatif yön belirteci
#define MOUSE_Y_SIGN      0x20  // Y ekseni negatif yön belirteci

// mouse.c içindeki global fare verileri
extern int mouse_x;
extern int mouse_y;
extern int mouse_left_click;
extern int mouse_right_click;
extern int mouse_middle_click;

// Fare görsel matrisi
extern const uint8_t mouse_pointer_sprite[19][12];

// Fonksiyon prototipleri
void init_mouse(void);
void handle_mouse_polling(void);

// Donanımsal Port Köprüleri
unsigned char inb(unsigned short port);
void outb(unsigned short port, unsigned char data);

#endif // MOUSE_H
