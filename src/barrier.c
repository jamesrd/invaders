#include "barrier.h"
#include "entity.h"
#include "game.h"
#include "raylib.h"
#include <stdlib.h>

void caculateBarriers(int count, float *xStart, float *xGap) {
  *xStart = gameWindow.worldTopLeft.x / 1.5;
  *xGap = (*xStart * -2) / (count - 1);
}

void InitBarriers(BarrierData *data, int count, int hitPoints, float y,
                  Model *barrierModel) {
  if (data->barriers != NULL) {
    data->barriers = realloc(data->barriers, count * sizeof(Barrier));
  } else {
    data->barriers = calloc(count, sizeof(Barrier));
  }
  data->count = count;

  float xStart, xGap;
  caculateBarriers(count, &xStart, &xGap);

  for (int i = 0; i < data->count; i++) {
    Barrier *b = &data->barriers[i];
    b->hitPoints = hitPoints;
    b->entity = malloc(sizeof(Entity));
    b->entity->enabled = true;
    b->entity->model = barrierModel;
    b->entity->pos = (Vector3){xStart + (i * xGap), y, 0};
    b->entity->scale = 1.0f;
    b->entity->tint = WHITE;
  }
}
