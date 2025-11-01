#ifndef INVADERS_GAME
#define INVADERS_GAME
#include "barrier.h"
#include "enemy.h"
#include "entity.h"
#include "raylib.h"

#define MAX_PLAYER_SHOTS 4
#define MAX_ENEMY_SHOTS 4

enum GameStates { Welcome, Paused, Playing, GameOver };

typedef struct {
  int width;
  int xCenter;
  int height;
  int yCenter;
  int fontSmallSize;
  int fontLargeSize;
  bool fullScreen;
  Camera3D camera;
} WindowInfo;

typedef struct {
  int score;
  int highScore;
  enum GameStates state;
  int lives;
} GameState;

int InitGame(char *title, int width, int height);
int RunGame();
bool checkShotsToEnemy(Enemy *e);
bool checkEnemyRowWin(Vector3 v, float radius);
void enemyShoot(Vector3 pos);

extern GameState gameState;
extern WindowInfo gameWindow;
extern Entity player;
extern BarrierData barrierData;
extern EnemyData enemyData;
extern Shot playerShots[];
extern Shot enemyShots[];
extern Vector3 groundPosition;
extern Vector2 groundSize;
extern bool showFps;

#endif
