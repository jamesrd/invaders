#ifndef INVADER_ENEMY
#define INVADER_ENEMY

#include "entity.h"
#include "raylib.h"
#define ENEMY_MOVE_LEFT (Vector3){-0.2, 0, 0}
#define ENEMY_MOVE_RIGHT (Vector3){0.2, 0, 0}
#define ENEMY_MOVE_DOWN (Vector3){0, -1.0, 0}
#define ENEMY_MOVE_ARRIVE (Vector3){0, 0, 20}
#define ENEMY_FADE_RATE 500.0f

enum EnemyRowMode { Arriving, Advancing, LeftMarch, RightMarch, Stationary };
enum EnemyState { Active, Destroyed, Disabled };

typedef struct enemy_t {
  enum EnemyState state;
  Entity *entity;
  int scoreValue;
} Enemy;

typedef struct enemyRow_t {
  bool enabled;
  int enemyCount;
  Enemy *enemies;
  Vector3 vel;
  enum EnemyRowMode mode;
  enum EnemyRowMode prevMode;
  Vector3 target;
} EnemyRow;

typedef struct enemyData_t {
  int rowCount;
  EnemyRow **rows;
} EnemyData;

void InitEnemies(EnemyData *enemyData, int enemiesPerRow, Model *enemyModel);
int CreateEnemyRow(float y, int count, Model *model, EnemyRow **row);
int UpdateEnemyState(EnemyRow **rows, int rowCount, float dT);

#endif
