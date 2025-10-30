#ifndef INVADER_ENEMY
#define INVADER_ENEMY

#include "entity.h"
#include "raylib.h"

enum EnemyRowMode { Advancing, LeftMarch, RightMarch, Stationary };

typedef struct enemy_t {
  bool enabled;
  Entity *entity;
} Enemy;

typedef struct enemyRow_t {
  bool enabled;
  int enemyCount;
  Enemy *enemies;
  Vector2 vel;
  enum EnemyRowMode mode;
} EnemyRow;

int CreateEnemyRow(float y, int count, Model *model, EnemyRow **row);

#endif
