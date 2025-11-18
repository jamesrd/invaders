#include "utility.h"

void DrawTextCentered(const char *text, int posX, int posY, int fontSize,
                      Color color) {
  int dPosX = MeasureText(text, fontSize) / 2;
  DrawText(text, posX - dPosX, posY - fontSize / 2, fontSize, color);
}
