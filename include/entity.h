#ifndef INVADER_ENTITY
#define INVADER_ENTITY
#include "raylib.h"

typedef struct entity_t {
  bool enabled;
  Model *model;
  Vector3 pos;
  float scale;
  Color tint;
} Entity;

typedef struct shot_t {
  bool enabled;
  Vector3 pos;
  Vector3 vel;
  float radius;
  Color color;
} Shot;

void EntityMove(Entity e, Vector3 d);

#endif
