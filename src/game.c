#include "game.h"
#include "enemy.h"
#include "entity.h"
#include "raylib.h"
#include "utility.h"
#include <stdio.h>
#include <stdlib.h>

#define MAX_PLAYER_SHOTS 4
#define ENEMY_ROWS 5
#define ENEMIES_PER_ROW 11

bool run = true;

WindowInfo gameWindow = {0};
GameState gameState = {0};
EnemyData enemyData;

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
  if (enemyData.rows == NULL)
    return;
  for (int i = 0; i < enemyData.rowCount; i++) {
    EnemyRow *er = enemyData.rows[i];
    if (!er->enabled)
      continue;
    for (int j = 0; j < er->enemyCount; j++) {
      Enemy e = er->enemies[j];
      if (e.state == Active || e.state == Destroyed)
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
    snprintf(score, 29, "SCORE: %d", gameState.score);
    DrawText(score, 0, 0, 20, WHITE);
    break;
  case GameOver:
    if (gameState.lives > 0)
      DrawTextCentered("YOU WIN!", gameWindow.width / 2, gameWindow.height / 2,
                       40, WHITE);
    else
      DrawTextCentered("YOU LOSE!", gameWindow.width / 2, gameWindow.height / 2,
                       40, WHITE);
    snprintf(score, 29, "SCORE: %d", gameState.score);
    DrawText(score, 0, 0, 20, WHITE);
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
      playerShotTimer = PLAYER_COOLDOWN;
      break;
    }
  }
}

void toggleFullscreen() {
  ToggleFullscreen();
  gameWindow.fullScreen = IsWindowFullscreen();
  if (gameWindow.fullScreen) {
    gameWindow.width = GetMonitorWidth(0);
    gameWindow.height = GetMonitorHeight(0);
  } else {
    gameWindow.width = GetScreenWidth();
    gameWindow.height = GetScreenHeight();
  }
}

bool handleInput() {
  if (IsKeyReleased(KEY_Q))
    return false;

  if (IsKeyReleased(KEY_F)) {
    toggleFullscreen();
  }

  if (gameState.state == Playing) {
    if (IsKeyReleased(KEY_TAB)) {
      gameState.state = Paused;
    }
    if (IsKeyDown(KEY_LEFT)) {
      playerPosition.x -= 0.1; // TODO - need dT
    }
    if (IsKeyDown(KEY_RIGHT)) {
      playerPosition.x += 0.1;
    }
    if (IsKeyDown(KEY_SPACE)) {
      playerShoot();
    }

  } else {
    if (IsKeyReleased(KEY_ENTER) || IsKeyReleased(KEY_TAB)) {
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

bool checkShotsCollisions(Enemy *e) {
  for (int k = 0; k < MAX_PLAYER_SHOTS; k++) {
    if (playerShots[k].enabled &&
        checkShotCollision(playerShots[k], e->entity)) {
      playerShots[k].enabled = false;
      // er->enemies[j].enabled = false;
      e->state = Destroyed;
      gameState.score += e->scoreValue;
    }
  }
  return e->state == Active;
}

bool checkPlayerCollision(Vector3 v, float radius) {
  return CheckCollisionSpheres(playerPosition, 0.5, v, radius);
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
  int alive = UpdateEnemyState(enemyData.rows, enemyData.rowCount, dT);
  if (alive == 0) {
    gameState.state = GameOver;
  }
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

void resetGame() {
  gameState.state = Welcome;
  gameState.score = 0;
  gameState.lives = 1;
  InitEnemies(&enemyData, ENEMIES_PER_ROW, &enemyModel);
}

int InitGame(char *title, int width, int height) {
  bool run = true;
  enemyData.rowCount = ENEMY_ROWS;
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
