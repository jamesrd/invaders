#include "enemy.h"
#include "entity.h"
#include "game.h"
#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>

// TODO make sure to recompute when fullscreen engaged
#define BOSS_END gameWindow.worldBottomRight.x + 2
#define BOSS_BEGIN gameWindow.worldTopLeft.x - 2
// 11 * 1.75 is the expected width of the enemy group
// TODO make it based on the model size + gap
#define ENEMY_ROW_WIDTH 11 * 1.75
//  5 * 1.5 is the height that the enemy grid covers
//  TODO may need to calculate based on model
#define ENEMY_ROW_GAP 5 * 1.5

int CreateEnemyRow(float y, int count, bool canShoot, int score, Model *model,
                   EnemyRow **row) {
  float xStart = gameWindow.worldTopLeft.x * 0.9;
  float xGap = (ENEMY_ROW_WIDTH) / count;
  Enemy *ea = calloc(count, sizeof(Enemy));
  if (ea == NULL) {
    perror("Enemy allocation problem");
    return 255;
  }
  EnemyRow *er = malloc(sizeof(EnemyRow));
  if (er == NULL) {
    perror("Enemy row allocation problem");
    return 255;
  }

  for (int i = 0; i < count; i++) {
    Enemy *e = &ea[i];

    e->state = Active;
    e->entity = malloc(sizeof(Entity));
    e->entity->enabled = true;
    e->entity->model = model;
    e->entity->pos = (Vector3){xStart + (i * xGap), y, ENEMY_START_Z};
    e->entity->scale = 1.0f;
    e->entity->tint = WHITE;
    e->scoreValue = score;
  }

  er->enabled = true;
  er->enemyCount = count;
  er->enemies = ea;
  er->vel = ENEMY_MOVE_ARRIVE;
  er->mode = Arriving;
  er->prevMode = Stationary;
  er->target = (Vector3){-10, y, 0};
  er->canShoot = canShoot;

  *row = er;
  return 0;
}

void moveEnemy(Enemy *e, Vector3 vel, float dT) {
  e->entity->pos.x += vel.x * dT;
  e->entity->pos.y += vel.y * dT;
  e->entity->pos.z += vel.z * dT;
}

void updateEnemyRowState(EnemyData *data, EnemyRow *er) {
  switch (er->mode) {
  case Arriving:
    if (er->enemies[0].entity->pos.z >= er->target.z) {
      for (int i = 0; i < er->enemyCount; i++) {
        er->enemies[i].entity->pos.z = er->target.z;
      }
      er->prevMode = Arriving;
      er->mode = RightMarch;
      er->vel = ENEMY_MOVE_RIGHT;
      er->target.x = data->xTargetRight;
    }
    break;
  case LeftMarch:
    if (er->enemies[0].entity->pos.x < er->target.x) {
      er->prevMode = LeftMarch;
      er->mode = Advancing;
      er->vel = ENEMY_MOVE_DOWN;
      er->target.y = er->enemies[0].entity->pos.y + data->yAdvance;
    }
    break;
  case RightMarch:
    if (er->enemies[0].entity->pos.x > er->target.x) {
      er->prevMode = RightMarch;
      er->mode = Advancing;
      er->vel = ENEMY_MOVE_DOWN;
      er->target.y = er->enemies[0].entity->pos.y + data->yAdvance;
    }
    break;
  case Advancing:
    if (er->enemies[0].entity->pos.y < er->target.y) {
      if (er->prevMode == RightMarch) {
        er->mode = LeftMarch;
        er->vel = ENEMY_MOVE_LEFT;
        er->target.x = data->xTargetLeft;
      } else if (er->prevMode == LeftMarch) {
        er->mode = RightMarch;
        er->vel = ENEMY_MOVE_RIGHT;
        er->target.x = data->xTargetRight;
      }
      er->prevMode = Advancing;
    }
    break;
  default:
    break;
  }
}

void updateBossState(EnemyData *data, float dT) {
  switch (data->boss->state) {
  case Active:
    data->boss->entity->pos.x += BOSS_SPEED * dT;
    if (data->boss->entity->pos.x > BOSS_END) {
      data->boss->state = Disabled;
      data->boss->entity->pos.x = BOSS_BEGIN;
      data->timeToBoss = BOSS_COUNTDOWN;
    } else if (!checkShotsToEnemy(data->boss)) {
      data->boss->state = Destroyed;
    }
    break;
  case Destroyed:
    data->boss->entity->tint.a -= (ENEMY_FADE_RATE * dT);
    if (data->boss->entity->tint.a <= 0.0) {
      data->boss->state = Disabled;
      data->boss->entity->pos.x = BOSS_BEGIN;
      data->timeToBoss = BOSS_COUNTDOWN;
      data->boss->entity->tint.a = 255;
    }
    break;
  case Disabled:
    data->timeToBoss -= dT;
    if (data->timeToBoss <= 0.0) {
      data->boss->state = Active;
    }
    break;
  }
}

int UpdateEnemyState(EnemyData *data, int rowCount, float dT) {
  int enemiesAlive = 0;
  if (data == NULL || data->rows == NULL)
    return 0;
  data->enemyShotTimer -= dT;
  for (int i = 0; i < rowCount; i++) {
    EnemyRow *er = data->rows[i];
    int eir = 0;
    if (er->enabled) {
      updateEnemyRowState(data, er);
      bool tryToShoot = er->canShoot && data->enemyShotTimer < 0;

      for (int j = 0; j < er->enemyCount; j++) {
        Enemy *e = &er->enemies[j];
        moveEnemy(e, er->vel, dT);
        if (e->state == Active) {
          bool enemyOk = checkShotsToEnemy(e);
          if (enemyOk)
            eir++;
          if (checkEnemyRowWin(e->entity->pos, e->entity->scale / 2)) {
            gameState.lives = 0;
            gameState.state = GameOver;
          } else if (tryToShoot && player.pos.x < e->entity->pos.x + 1.0f &&
                     player.pos.x > e->entity->pos.x - 1.0f) {

            enemyShoot(e->entity->pos);
            tryToShoot = false;
          }
        } else if (e->state == Destroyed) {
          e->entity->tint.a -= (ENEMY_FADE_RATE * dT);
          if (e->entity->tint.a <= 0.0)
            e->state = Disabled;
        }
      }
      if (eir == 0)
        data->rows[i]->enabled = false;
    }
    enemiesAlive += eir;
  }
  updateBossState(data, dT);
  return enemiesAlive;
}

void setupEnemies(EnemyData *enemyData, int enemyCount, Model *shooterModel,
                  Model *infantryModel, Model *bossModel) {
  float y = gameWindow.worldTopLeft.y * 0.7;
  float yGap = (ENEMY_ROW_GAP) / enemyData->rowCount;
  int score = 50;
  if (enemyData->rows == NULL) {
    enemyData->rows = calloc(enemyData->rowCount, sizeof(EnemyRow *));
  } else {
    enemyData->rows =
        realloc(enemyData->rows, sizeof(EnemyRow *) * enemyData->rowCount);
  }
  Model *rowModel = shooterModel;
  for (int i = 0; i < enemyData->rowCount; i++) {
    EnemyRow *enemyRow = enemyData->rows[i];
    CreateEnemyRow(y -= yGap, enemyCount, i == 0, score - (i * 10), rowModel,
                   &enemyRow);
    enemyData->rows[i] = enemyRow;
    rowModel = infantryModel;
  }
  enemyData->timeToBoss = BOSS_COUNTDOWN;
  if (enemyData->boss != NULL) {
    if (enemyData->boss->entity != NULL) {
      free(enemyData->boss->entity);
    }
    enemyData->boss = realloc(enemyData->boss, sizeof(Enemy));
  } else {
    enemyData->boss = malloc(sizeof(Enemy));
  }
  enemyData->boss->scoreValue = 100;
  enemyData->boss->state = Disabled;
  enemyData->boss->entity = malloc(sizeof(Entity));
  enemyData->boss->entity->enabled = true;
  enemyData->boss->entity->model = bossModel;
  enemyData->boss->entity->scale = 0.5f;
  enemyData->boss->entity->tint = WHITE;
  enemyData->boss->entity->pos =
      (Vector3){BOSS_BEGIN, gameWindow.worldTopLeft.y * 0.75, 0};
}

void InitEnemies(EnemyData *enemyData, int enemiesPerRow, Model *shooterModel,
                 Model *infantryModel, Model *bossModel) {
  enemyData->xTargetLeft = gameWindow.worldTopLeft.x * 0.9;
  float xGap = gameWindow.worldBottomRight.x - ENEMY_ROW_WIDTH;
  enemyData->xTargetRight = xGap;
  enemyData->yAdvance = -1.0;
  enemyData->enemyShotTimer = ENEMY_SHOT_COOLDOWN;
  setupEnemies(enemyData, enemiesPerRow, shooterModel, infantryModel,
               bossModel);
}
