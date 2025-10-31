#include "enemy.h"
#include "entity.h"
#include "game.h"
#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>

int CreateEnemyRow(float y, int count, Model *model, EnemyRow **row) {
  float xStart = -(gameWindow.camera.position.z / 2);
  float xGap = (11 * 1.75) / count;
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
    e->scoreValue = 10;
  }

  er->enabled = true;
  er->enemyCount = count;
  er->enemies = ea;
  er->vel = ENEMY_MOVE_ARRIVE;
  er->mode = Arriving;
  er->prevMode = Stationary;
  er->target = (Vector3){-10, y, 0};

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

int UpdateEnemyState(EnemyData *data, int rowCount, float dT) {
  int enemiesAlive = 0;
  if (data == NULL || data->rows == NULL)
    return 0;
  for (int i = 0; i < rowCount; i++) {
    EnemyRow *er = data->rows[i];
    int eir = 0;
    if (er->enabled) {
      updateEnemyRowState(data, er);

      for (int j = 0; j < er->enemyCount; j++) {
        Enemy *e = &er->enemies[j];
        moveEnemy(e, er->vel, dT);
        if (e->state == Active) {
          if (checkShotsToEnemy(e))
            eir++;
          if (checkEnemyRowWin(e->entity->pos, e->entity->scale / 2)) {
            gameState.lives = 0;
            gameState.state = GameOver;
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
  return enemiesAlive;
}

void setupEnemies(EnemyData *enemyData, int enemyCount, Model *model) {
  float y = gameWindow.camera.position.z / 3;
  float yGap = (5 * 1.5) / enemyData->rowCount;
  if (enemyData->rows == NULL) {
    enemyData->rows = calloc(enemyData->rowCount, sizeof(EnemyRow *));
  } else {
    enemyData->rows =
        realloc(enemyData->rows, sizeof(EnemyRow *) * enemyData->rowCount);
  }
  for (int i = 0; i < enemyData->rowCount; i++) {
    EnemyRow *enemyRow = enemyData->rows[i];
    CreateEnemyRow(y -= yGap, enemyCount, model, &enemyRow);
    enemyData->rows[i] = enemyRow;
  }
}

void InitEnemies(EnemyData *enemyData, int enemiesPerRow, Model *enemyModel) {
  enemyData->xTargetLeft = -(gameWindow.camera.position.z / 2);
  float xGap = gameWindow.camera.position.z / 2 - 17.5;
  enemyData->xTargetRight = xGap;
  enemyData->yAdvance = -1.0;
  setupEnemies(enemyData, enemiesPerRow, enemyModel);
}
