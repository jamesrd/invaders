#ifndef INVADER_UTILITY
#define INVADER_UTILITY
#include "raylib.h"

typedef struct animData_t {
  int frameCount;
  int frame;
  float frameRate;
  ModelAnimation *frames;
} AnimData;

void DrawTextCentered(const char *text, int posX, int posY, int fontSize,
                      Color color);

#endif
