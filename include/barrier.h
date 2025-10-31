#ifndef INVADER_BARRIER
#define INVADER_BARRIER
#include "entity.h"
#include "raylib.h"

typedef struct barrier_t {
  Entity *entity;
  int hitPoints;
} Barrier;

typedef struct barrierData_t {
  Barrier *barriers;
  int count;
} BarrierData;

void InitBarriers(BarrierData *data, int count, int hitPoints, float y,
                  Model *barrierModel);

#endif
