#include "game.h"
#include "enemy.h"
#include "entity.h"
#include "raylib.h"
#include "utility.h"
#include <stdio.h>
#include <stdlib.h>

int ENEMY_ROWS = 5;
#define MAX_PLAYER_SHOTS 4

bool run = true;

WindowInfo gameWindow = {0};
GameState gameState = {0};

EnemyRow **enemyRows = NULL;
Shot playerShots[MAX_PLAYER_SHOTS];
float PLAYER_COOLDOWN = 0.5f;
float playerShotTimer = 0.0f;
Vector3 playerPosition = {0.0f, -6.0f, 0.0f};
Vector3 barrierPosition = {2.0f, -2.5f, 0.0f};
Vector3 groundPosition = {0.0f, -7.0f, 0.0f};
Vector2 groundSize = {40.0f, 20.0f};

Model playerModel;
Model enemyModel;
Model barrierModel;

void drawEntity(Entity *e) { DrawModel(*e->model, e->pos, e->scale, e->tint); }

void drawEnemies() {
  if (enemyRows == NULL)
    return;
  for (int i = 0; i < ENEMY_ROWS; i++) {
    EnemyRow *er = enemyRows[i];
    if (!er->enabled)
      continue;
    for (int j = 0; j < er->enemyCount; j++) {
      Enemy e = er->enemies[j];
      if (e.enabled)
        drawEntity(er->enemies[j].entity);
    }
  }
}

void drawBackground() { DrawPlane(groundPosition, groundSize, DARKGREEN); }

void drawBarriers() { DrawModel(barrierModel, barrierPosition, 1.0f, WHITE); }

void drawPlayer() { DrawModel(playerModel, playerPosition, 1.0f, WHITE); }

void drawShots() {
  for (int i = 0; i < MAX_PLAYER_SHOTS; i++) {
    if (playerShots[i].enabled) {
      DrawSphere(playerShots[i].pos, playerShots[i].radius,
                 playerShots[i].color);
    }
  }
}

void draw3dContent() {
  BeginMode3D(gameWindow.camera);
  drawBackground();
  drawBarriers();
  drawEnemies();
  drawPlayer();
  drawShots();
  EndMode3D();
}

void drawUI() {
  char score[30];
  switch (gameState.state) {
  case Welcome:
    DrawTextCentered("SPACE INVADERS!", gameWindow.width / 2,
                     gameWindow.height / 2, 45, WHITE);
    break;
  case Paused:
    DrawTextCentered("PAUSED", gameWindow.width / 2, gameWindow.height / 2, 40,
                     WHITE);
  default:
    snprintf(score, 29, "SCORE: %d", gameState.score);
    DrawText(score, 0, 0, 20, WHITE);
  }
}

void drawFrame() {
  BeginDrawing();
  ClearBackground((Color){30, 30, 30, 255});
  draw3dContent();
  drawUI();
  EndDrawing();
}

void playerShoot() {
  if (playerShotTimer <= 0.0) {
    for (int i = 0; i < MAX_PLAYER_SHOTS; i++) {
      if (playerShots[i].enabled)
        continue;

      playerShots[i].enabled = true;
      playerShots[i].pos = (Vector3){playerPosition.x, playerPosition.y + 1.0f,
                                     playerPosition.z};
      playerShots[i].vel = (Vector3){0, 10.0f, 0};
      playerShots[i].radius = 0.1f;
      playerShots[i].color = RED;
      printf("Fire shot!\n");
      playerShotTimer = PLAYER_COOLDOWN;
      break;
    }
  }
}

bool handleInput() {
  if (IsKeyReleased(KEY_Q))
    return false;

  if (gameState.state == Playing) {
    if (IsKeyReleased(KEY_TAB)) {
      gameState.state = Paused;
    }
    if (IsKeyDown(KEY_LEFT)) {
      playerPosition.x -= 0.1; // FIX - need dT
    }
    if (IsKeyDown(KEY_RIGHT)) {
      playerPosition.x += 0.1;
    }
    if (IsKeyDown(KEY_SPACE)) {
      playerShoot();
    }

  } else {
    if (IsKeyReleased(KEY_SPACE) || IsKeyReleased(KEY_TAB)) {
      gameState.state = Playing;
    }
  }

  return true;
}

void playerCollisionDetection(float dT) {}
void movePlayerShots(float dT) {
  for (int i = 0; i < MAX_PLAYER_SHOTS; i++) {
    Shot p = playerShots[i];
    if (p.enabled) {
      p.pos.x += p.vel.x * dT;
      p.pos.y += p.vel.y * dT;
      p.pos.z += p.vel.z * dT;
      // TODO - find correct end states for a shot:
      // Out of frame
      // Barrier collision?
      // Enemy collision happens when enemies are updated
      if (p.pos.y > 9.0f)
        p.enabled = false;
      playerShots[i] = p;
    }
  }
}

bool checkShotCollision(Shot s, Entity *e) {
  bool hit = CheckCollisionSpheres(e->pos, e->scale / 2, s.pos, s.radius);
  return hit;
}

void checkShotsCollisions(Enemy *e) {
  for (int k = 0; k < MAX_PLAYER_SHOTS; k++) {
    if (playerShots[k].enabled &&
        checkShotCollision(playerShots[k], e->entity)) {
      playerShots[k].enabled = false;
      // er->enemies[j].enabled = false;
      e->enabled = false;
    }
  }
}

void moveEnemy(Enemy *e, Vector3 vel, float dT) {
  e->entity->pos.x += vel.x * dT;
  e->entity->pos.y += vel.y * dT;
  e->entity->pos.z += vel.z * dT;
}

void updateEnemyRowState(EnemyRow *er) {
  switch (er->mode) {
  case Stationary:
    er->mode = LeftMarch;
    er->vel = ENEMY_MOVE_LEFT;
  case LeftMarch:
    if (er->enemies[0].entity->pos.x < -10) {
      er->mode = RightMarch;
      er->vel = ENEMY_MOVE_RIGHT;
    }
    break;
  case RightMarch:
    if (er->enemies[0].entity->pos.x > -8.5) {
      er->mode = LeftMarch;
      er->vel = ENEMY_MOVE_LEFT;
    }
    break;
  case Advancing:

  default:
    break;
  }
}

void updateEnemyState(float dT) {
  if (enemyRows == NULL)
    return;
  for (int i = 0; i < ENEMY_ROWS; i++) {
    EnemyRow *er = enemyRows[i];
    if (er->enabled) {
      updateEnemyRowState(er);
      for (int j = 0; j < er->enemyCount; j++) {
        Enemy *e = &er->enemies[j];
        if (e->enabled) {
          moveEnemy(e, er->vel, dT);
          checkShotsCollisions(e);
        }
      }
    }
  }
}

void updateState(float dT) {
  if (playerShotTimer > 0) {
    playerShotTimer -= dT;
  }
  // move enemy shots & player collision detection
  playerCollisionDetection(dT);
  // move player shots
  movePlayerShots(dT);
  // update enemy state & enemy collision detection
  updateEnemyState(dT);
}

bool gameLoop(float dT) {
  if (!handleInput())
    return false;

  if (gameState.state == Playing) {
    updateState(dT);
  }

  drawFrame();
  return true;
}

void loadModels() {
  playerModel = LoadModel("resources/models/player.glb");
  enemyModel = LoadModel("resources/models/enemy.glb");
  barrierModel = LoadModel("resources/models/barrier.glb");
}

void unloadModels() {
  UnloadModel(playerModel);
  UnloadModel(enemyModel);
  UnloadModel(barrierModel);
}

Camera3D initCamera3d() {
  Camera3D camera = {0};
  camera.position = (Vector3){0.0f, 0.0f, 20.0f};
  camera.target = (Vector3){0.0f, 0.0f, 0.0f};
  camera.up = (Vector3){0.0f, 1.0f, 0.0f};
  camera.fovy = 45.0f;
  camera.projection = CAMERA_PERSPECTIVE;
  return camera;
}

void setupScreen(const char *title, int width, int height, bool fullScreen) {
  gameWindow.width = width;
  gameWindow.height = height;
  gameWindow.fullScreen = fullScreen;
  gameWindow.camera = initCamera3d();
  InitWindow(width, height, title);
  SetTargetFPS(60);
  if (fullScreen) {
    int monitor = GetCurrentMonitor();
    gameWindow.width = GetMonitorWidth(monitor);
    gameWindow.height = GetMonitorHeight(monitor);
    SetWindowSize(gameWindow.width, gameWindow.height);
    ToggleFullscreen();
  }
}

void setupEnemies() {
  float y = 7.0f;
  enemyRows = calloc(ENEMY_ROWS, sizeof(EnemyRow *));
  for (int i = 0; i < ENEMY_ROWS; i++) {
    EnemyRow *enemyRow = enemyRows[i];
    CreateEnemyRow(y -= 1.5f, 11, &enemyModel, &enemyRow);
    enemyRows[i] = enemyRow;
  }
}

void resetGame() {
  gameState.state = Welcome;
  gameState.score = 0;
  setupEnemies();
}

int InitGame(char *title, int width, int height) {
  bool run = true;
  setupScreen(title, width, height, false);
  loadModels();
  resetGame();
  return 0;
}

int RunGame() {
  while (!WindowShouldClose() && run) {
    run = gameLoop(GetFrameTime());
  }

  unloadModels();
  CloseWindow();
  return 0;
}
