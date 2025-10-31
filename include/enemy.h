#ifndef INVADER_ENEMY
#define INVADER_ENEMY

#include "entity.h"
#include "raylib.h"
#define ENEMY_MOVE_LEFT (Vector3){-1.0, 0, 0}
#define ENEMY_MOVE_RIGHT (Vector3){1.0, 0, 0}
#define ENEMY_MOVE_DOWN (Vector3){0, 1.0, 0}

enum EnemyRowMode { Advancing, LeftMarch, RightMarch, Stationary };

typedef struct enemy_t {
  bool enabled;
  Entity *entity;
  int scoreValue;
} Enemy;

typedef struct enemyRow_t {
  bool enabled;
  int enemyCount;
  Enemy *enemies;
  Vector3 vel;
  enum EnemyRowMode mode;
} EnemyRow;

int CreateEnemyRow(float y, int count, Model *model, EnemyRow **row);

#endif
