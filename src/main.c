#include "raylib.h"
#include <stdio.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define GAME_TITLE "Space Invaders!"

typedef struct {
  int width;
  int height;
  bool fullScreen;
  Camera3D camera;
} windowInfo;

windowInfo gameWindow = {0};
Vector3 playerPosition = {0.0f, -6.0f, 0.0f};
Vector3 barrierPosition = {2.0f, -2.5f, 0.0f};
Vector3 enemyPosition = {2.0f, 2.5f, 0.0f};
Vector3 groundPosition = {0.0f, -7.0f, 0.0f};
Vector2 groundSize = {40.0f, 20.0f};
Model playerModel;
Model enemyModel;
Model barrierModel;

void draw3dContent() {
  BeginMode3D(gameWindow.camera);
  DrawModel(playerModel, playerPosition, 1.0f, WHITE);
  DrawModel(barrierModel, barrierPosition, 1.0f, WHITE);
  DrawModel(enemyModel, enemyPosition, 1.0f, WHITE);
  DrawPlane(groundPosition, groundSize, DARKGREEN);
  // DrawCube(cubePosition, 0.5f, 0.5f, 0.5f, RED);
  // DrawCubeWires(cubePosition, 2.0f, 2.0f, 2.0f, MAROON);
  EndMode3D();
}

void drawFrame() {
  BeginDrawing();
  ClearBackground(BLACK);
  draw3dContent();
  DrawText("Sample", 0, gameWindow.height / 2, 20, WHITE);
  EndDrawing();
}

bool handleInput() {
  if (IsKeyReleased(KEY_Q))
    return false;

  if (IsKeyDown(KEY_LEFT)) {
    playerPosition.x -= 0.1;
  } else if (IsKeyDown(KEY_RIGHT)) {
    playerPosition.x += 0.1;
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

int main(int argc, char *argv[]) {
  bool run = true;
  setupScreen(GAME_TITLE, WINDOW_WIDTH, WINDOW_HEIGHT, false);
  loadModels();
  while (!WindowShouldClose() && run) {
    run = gameLoop();
  }

  unloadModels();
  CloseWindow();

  return 0;
}
