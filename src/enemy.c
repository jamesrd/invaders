#include "enemy.h"
#include "entity.h"
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
