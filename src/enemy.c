#include "enemy.h"
#include "entity.h"
#include "game.h"
#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>

int CreateEnemyRow(float y, int count, Model *model, EnemyRow **row) {
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

    e->enabled = true;
    e->entity = malloc(sizeof(Entity));
    e->entity->enabled = true;
    e->entity->model = model;
    e->entity->pos = (Vector3){-9.625 + (i * 1.75), y, -10.0f};
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

void updateEnemyRowState(EnemyRow *er) {
  switch (er->mode) {
  case Arriving:
    if (er->enemies[0].entity->pos.z >= er->target.z) {
      er->prevMode = Arriving;
      er->mode = LeftMarch;
      er->vel = ENEMY_MOVE_LEFT;
      er->target.x = -10;
    }
    break;
  case LeftMarch:
    if (er->enemies[0].entity->pos.x < er->target.x) {
      er->prevMode = LeftMarch;
      er->mode = RightMarch;
      er->vel = ENEMY_MOVE_RIGHT;
      er->target.x = -8.5;
    }
    break;
  case RightMarch:
    if (er->enemies[0].entity->pos.x > er->target.x) {
      er->prevMode = RightMarch;
      er->mode = Advancing;
      er->vel = ENEMY_MOVE_DOWN;
      er->target.y = er->enemies[0].entity->pos.y - 1.0;
    }
    break;
  case Advancing:
    if (er->enemies[0].entity->pos.y < er->target.y) {
      er->prevMode = Advancing;
      er->mode = LeftMarch;
      er->vel = ENEMY_MOVE_LEFT;
      er->target.x = -10;
    }
    break;
  default:
    break;
  }
}

int UpdateEnemyState(EnemyRow **rows, int rowCount, float dT) {
  int enemiesAlive = 0;
  if (rows == NULL)
    return 0;
  for (int i = 0; i < rowCount; i++) {
    EnemyRow *er = rows[i];
    int eir = 0;
    if (er->enabled) {
      updateEnemyRowState(er);
      for (int j = 0; j < er->enemyCount; j++) {
        Enemy *e = &er->enemies[j];
        moveEnemy(e, er->vel, dT);
        if (e->enabled) {
          if (checkShotsCollisions(e))
            eir++;
          // TODO fix player collision check
          if (checkPlayerCollision(e->entity->pos, e->entity->scale / 2)) {
            gameState.lives = 0;
            gameState.state = GameOver;
          }
        }
      }
      if (eir == 0)
        rows[i]->enabled = false;
    }
    enemiesAlive += eir;
  }
  return enemiesAlive;
}
