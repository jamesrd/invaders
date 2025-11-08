#ifndef INVADER_ENEMY
#define INVADER_ENEMY

#include "entity.h"
#include "raylib.h"
#define ENEMY_MOVE_LEFT (Vector3){-1.0, 0, 0}
#define ENEMY_MOVE_RIGHT (Vector3){1.0, 0, 0}
#define ENEMY_MOVE_DOWN (Vector3){0, -1.0, 0}
#define ENEMY_MOVE_ARRIVE (Vector3){0, 0, 30}
#define ENEMY_SHOT_COOLDOWN 1.0f
#define ENEMY_START_Z -15
#define ENEMY_FADE_RATE 500.0f
#define BOSS_COUNTDOWN 0.0f
#define BOSS_SPEED 2.0

enum EnemyRowMode { Arriving, Advancing, LeftMarch, RightMarch, Stationary };

typedef struct enemy_t {
  enum EntityState state;
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
  bool canShoot;
} EnemyRow;

typedef struct enemyData_t {
  int rowCount;
  EnemyRow **rows;
  float xTargetLeft;
  float xTargetRight;
  float yAdvance;
  Enemy *boss;
  float timeToBoss;
  bool bossActive;
  float enemyShotTimer;
} EnemyData;

void InitEnemies(EnemyData *enemyData, int enemiesPerRow, Model *shooterModel,
                 Model *infantryModel, Model *bossModel);
int CreateEnemyRow(float y, int count, bool canShoot, int score, Model *model,
                   EnemyRow **row);
int UpdateEnemyState(EnemyData *ed, int rowCount, float dT);

#endif
