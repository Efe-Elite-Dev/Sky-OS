#ifndef GUI_H
#define GUI_H

#include "sky_subsystem.h"

// Alt sistemlerin (EXE/DEB) erişebileceği grafik prototipleri
void gui_refresh_desktop(void);
void gui_draw_rect(int x, int y, int width, int height, uint32_t color);

#endif // GUI_H
