#ifndef MOUSE_H
#define MOUSE_H

#include <stdint.h>

// Ekran çözünürlük tanımları (Kernel ile senkronize)
#define SCREEN_WIDTH     800
#define SCREEN_HEIGHT    600

// Dışarıya aktarılan global fare koordinatları
extern int mouse_x;
extern int mouse_y;

// Fonksiyon prototipleri
void init_mouse(void);
void handle_mouse_polling(void);

// Fare okunun 12x19 matrisi (Sprite)
extern const uint8_t mouse_pointer_sprite[19][12];

#endif
