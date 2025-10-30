#include "game.h"
#include "enemy.h"
#include "entity.h"
#include "raylib.h"
#include "utility.h"
#include <stdio.h>
#include <stdlib.h>

int ENEMY_ROWS = 5;

bool run = true;

WindowInfo gameWindow = {0};
GameState gameState = {0};

EnemyRow **enemyRows = NULL;
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
    for (int j = 0; j < er->enemyCount; j++) {
      drawEntity(er->enemies[j].entity);
    }
  }
}

void drawBackground() { DrawPlane(groundPosition, groundSize, DARKGREEN); }

void drawBarriers() { DrawModel(barrierModel, barrierPosition, 1.0f, WHITE); }

void drawPlayer() { DrawModel(playerModel, playerPosition, 1.0f, WHITE); }

void draw3dContent() {
  BeginMode3D(gameWindow.camera);
  drawBackground();
  drawBarriers();
  drawEnemies();
  drawPlayer();
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

bool handleInput() {
  if (IsKeyReleased(KEY_Q))
    return false;

  if (gameState.state == Playing) {
    if (IsKeyReleased(KEY_TAB)) {
      gameState.state = Paused;
    } else if (IsKeyDown(KEY_LEFT)) {
      playerPosition.x -= 0.1;
    } else if (IsKeyDown(KEY_RIGHT)) {
      playerPosition.x += 0.1;
    } else if (IsKeyDown(KEY_SPACE)) {
    }

  } else {
    if (IsKeyReleased(KEY_SPACE) || IsKeyReleased(KEY_TAB)) {
      gameState.state = Playing;
    }
  }

  return true;
}

bool gameLoop() {
  if (!handleInput())
    return false;

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
    run = gameLoop();
  }

  unloadModels();
  CloseWindow();
  return 0;
}
