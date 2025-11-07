#include "barrier.h"
#include "entity.h"
#include "game.h"
#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>

// TODO these could be parameters to InitBarriers
#define BARRIER_WIDTH 5
#define BARRIER_HEIGHT 3
const unsigned short BMASK = 0b101011111111011;

void caculateBarriers(int count, float mWidth, float *xStart, float *xGap) {
  float bWidth = mWidth * BARRIER_WIDTH;
  float x = gameWindow.worldTopLeft.x / 1.5;
  *xGap = (x * -2) / (count - 1);
  *xStart = x - bWidth / 2;
}

void InitBarriers(BarrierData *data, int count, int hitPoints, float y,
                  Model *barrierModel) {
  int totalBarriers = count * BARRIER_WIDTH * BARRIER_HEIGHT;

  if (data->barriers != NULL) {
    data->barriers = realloc(data->barriers, totalBarriers * sizeof(Barrier));
  } else {
    data->barriers = calloc(totalBarriers, sizeof(Barrier));
  }
  data->count = totalBarriers;

  float xStart, xGap, mWidth, mHeight;
  BoundingBox bb = GetModelBoundingBox(*barrierModel);
  mWidth = bb.max.x - bb.min.x;
  mHeight = bb.max.y - bb.min.y;
  caculateBarriers(count, mWidth, &xStart, &xGap);

  int i = 0;
  for (int bc = 0; bc < count; bc++) {
    float bcX = xStart + (bc * xGap);
    float bcY = y;

    for (int br = 0; br < BARRIER_HEIGHT; br++) {
      float bsY = mHeight * br;
      for (int bc = 0; bc < BARRIER_WIDTH; bc++) {
        int pos = br * BARRIER_WIDTH + bc;
        int bvalue = (BMASK & (1 << pos)) == 0 ? 0 : 1;
        float bsX = mWidth * bc;
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
}
