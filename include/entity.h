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

#endif
