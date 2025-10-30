#include "game.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define GAME_TITLE "Space Invaders!"

int main(int argc, char *argv[]) {
  InitGame(GAME_TITLE, WINDOW_WIDTH, WINDOW_HEIGHT);
  RunGame();
  return 0;
}
