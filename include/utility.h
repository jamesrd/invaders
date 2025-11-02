#ifndef INVADER_UTILITY
#define INVADER_UTILITY
#include "raylib.h"

typedef struct animData_t {
  int frame;
  ModelAnimation *frames;
} AnimData;

void DrawTextCentered(const char *text, int posX, int posY, int fontSize,
                      Color color);

#endif
