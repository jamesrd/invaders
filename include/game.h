#ifndef INVADERS_GAME
#define INVADERS_GAME
#include "enemy.h"
#include "entity.h"
#include "raylib.h"

enum GameStates { Welcome, Paused, Playing, GameOver };

typedef struct {
  int width;
  int height;
  bool fullScreen;
  Camera3D camera;
} WindowInfo;

typedef struct {
  int score;
  enum GameStates state;
  int lives;
} GameState;

int InitGame(char *title, int width, int height);
int RunGame();
bool checkShotsToEnemy(Enemy *e);
bool checkPlayerCollision(Vector3 v, float radius);

extern GameState gameState;

#endif
