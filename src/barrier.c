#include "barrier.h"
#include "entity.h"
#include "game.h"
#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>

#define BARRIER_SEGMENTS 8
const char BMASK = 0b10011111;

void caculateBarriers(int count, float *xStart, float *xGap) {
  *xStart = gameWindow.worldTopLeft.x / 1.5;
  *xGap = (*xStart * -2) / (count - 1);
}

void InitBarriers(BarrierData *data, int count, int hitPoints, float y,
                  Model *barrierModel) {
  int totalBarriers = count * BARRIER_SEGMENTS;
  int rowBreak = BARRIER_SEGMENTS / 2;

  if (data->barriers != NULL) {
    data->barriers = realloc(data->barriers, totalBarriers * sizeof(Barrier));
  } else {
    data->barriers = calloc(totalBarriers, sizeof(Barrier));
  }
  data->count = totalBarriers;

  float xStart, xGap, mWidth, mHeight;
  caculateBarriers(count, &xStart, &xGap);
  BoundingBox bb = GetModelBoundingBox(*barrierModel);
  mWidth = bb.max.x - bb.min.x;
  mHeight = bb.max.y - bb.min.y;

  int i = 0;
  for (int bc = 0; bc < count; bc++) {
    float bcX = xStart + (bc * xGap);
    float bcY = y;

    for (int bs = 0; bs < BARRIER_SEGMENTS; bs++) {
      int bvalue = (BMASK & (1 << bs)) >> bs;
      float bsX = mWidth * (bs % rowBreak);
      float bsY = mHeight * (bs >= rowBreak ? 1 : 0);
      Barrier *b = &data->barriers[i];
      b->hitPoints = hitPoints * bvalue;
      b->entity = malloc(sizeof(Entity));
      b->entity->enabled = true;
      b->entity->model = barrierModel;
      b->entity->pos = (Vector3){bcX + bsX, y + bsY, 0};
      b->entity->scale = 1.0f;
      b->entity->tint = WHITE;
      i++;
    }
  }
}
