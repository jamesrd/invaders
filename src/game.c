#include "game.h"
#include "barrier.h"
#include "draw.h"
#include "enemy.h"
#include "entity.h"
#include "raylib.h"

void resetGame();
#define ENEMY_ROWS 5
#define ENEMIES_PER_ROW 11
#define PLAYER_COOLDOWN 0.5f
#define PLAYER_X_VEL 10.0
#define PLAYER_FADE_RATE 500.0f
#define TARGET_FPS 60

bool run = true;
bool showFps = false;

WindowInfo gameWindow = {0};
GameState gameState = {0};
EnemyData enemyData;
BarrierData barrierData;

Shot playerShots[MAX_PLAYER_SHOTS];
Shot enemyShots[MAX_ENEMY_SHOTS];
float playerShotTimer = 0.0f;
Entity player = {0};
enum EntityState playerState = Active;
Vector3 playerVel = {0};
Vector3 groundPosition = {0.0f, -7.0f, 0.0f};
Vector2 groundSize = {40.0f, 20.0f};

Model playerModel;
Model enemyModel;
Model barrierModel;
Model bossModel;

void updateScreenHelpers() {
  gameWindow.xCenter = gameWindow.width / 2;
  gameWindow.yCenter = gameWindow.height / 2;
  gameWindow.fontSmallSize = gameWindow.height / 30;
  gameWindow.fontLargeSize = gameWindow.height / 15;
}

void playerShoot() {
  if (playerShotTimer <= 0.0) {
    for (int i = 0; i < MAX_PLAYER_SHOTS; i++) {
      if (playerShots[i].enabled)
        continue;

      playerShots[i].enabled = true;
      playerShots[i].pos =
          (Vector3){player.pos.x, player.pos.y + 1.0f, player.pos.z};
      playerShots[i].vel = (Vector3){0, 10.0f, 0};
      playerShots[i].radius = 0.1f;
      playerShots[i].color = RED;
      playerShotTimer = PLAYER_COOLDOWN;
      break;
    }
  }
}

void enemyShoot(Vector3 pos) {
  for (int i = 0; i < MAX_ENEMY_SHOTS; i++) {
    if (enemyShots[i].enabled)
      continue;

    enemyShots[i].enabled = true;
    enemyShots[i].pos = (Vector3){pos.x, pos.y - 1.0f, 0};
    enemyShots[i].vel = (Vector3){0, -10.0f, 0};
    enemyShots[i].radius = 0.1f;
    enemyShots[i].color = RED;
    enemyData.enemyShotTimer = ENEMY_SHOT_COOLDOWN;
    break;
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
  updateScreenHelpers();
}

bool handleInput() {
  if (IsKeyReleased(KEY_Q))
    return false;

  if (IsKeyReleased(KEY_ONE))
    showFps = !showFps;

  if (IsKeyReleased(KEY_F)) {
    toggleFullscreen();
  }

  if (gameState.state == Playing) {
    playerVel.x = 0.0;
    if (IsKeyReleased(KEY_TAB)) {
      gameState.state = Paused;
    }
    if (IsKeyDown(KEY_LEFT)) {
      playerVel.x = -PLAYER_X_VEL;
    }
    if (IsKeyDown(KEY_RIGHT)) {
      playerVel.x = PLAYER_X_VEL;
    }
    if (IsKeyDown(KEY_SPACE)) {
      playerShoot();
    }

  } else if (gameState.state == Paused || gameState.state == Welcome) {
    if (IsKeyReleased(KEY_ENTER) || IsKeyReleased(KEY_TAB)) {
      gameState.state = Playing;
    } else if (IsKeyReleased(KEY_N)) {
      resetGame();
    }
  } else if (gameState.state == GameOver) {
    if (IsKeyReleased(KEY_ENTER) || IsKeyReleased(KEY_TAB) ||
        IsKeyReleased(KEY_N)) {
      resetGame();
    }
  }

  return true;
}

bool checkShotCollision(Shot s, Entity *e) {
  bool hit = CheckCollisionSpheres(e->pos, e->scale / 2, s.pos, s.radius);
  return hit;
}

void playerCollisionDetection(float dT) {
  if (playerState == Destroyed)
    return;
  for (int i = 0; i < MAX_ENEMY_SHOTS; i++) {
    if (enemyShots[i].enabled) {
      if (checkShotCollision(enemyShots[i], &player)) {
        playerState = Destroyed;
        enemyShots[i].enabled = false;
        gameState.lives--;
        if (gameState.lives == 0) {
          gameState.state = GameOver;
        }
      }
    }
  }
}

bool checkShotsToEnemy(Enemy *e) {
  for (int k = 0; k < MAX_PLAYER_SHOTS; k++) {
    if (playerShots[k].enabled &&
        checkShotCollision(playerShots[k], e->entity)) {
      playerShots[k].enabled = false;
      e->state = Destroyed;
      gameState.score += e->scoreValue;
    }
  }
  return e->state == Active;
}

bool checkShotHitsBarrier(Shot *s) {
  for (int i = 0; i < barrierData.count; i++) {
    Barrier barrier = barrierData.barriers[i];
    if (barrier.hitPoints > 0) {
      BoundingBox b = GetModelBoundingBox(*barrier.entity->model);
      b.max.x += barrier.entity->pos.x;
      b.max.y += barrier.entity->pos.y;
      b.max.z += barrier.entity->pos.z;
      b.min.x += barrier.entity->pos.x;
      b.min.y += barrier.entity->pos.y;
      b.min.z += barrier.entity->pos.z;
      if (CheckCollisionBoxSphere(b, s->pos, s->radius)) {
        barrierData.barriers[i].hitPoints--;
        barrierData.barriers[i].entity->tint.a *= 0.5;
        return true;
      }
    }
  }
  return false;
}

bool checkEnemyRowWin(Vector3 v, float radius) {
  return v.y - radius < player.pos.y + 0.5;
}

void movePlayerShots(float dT) {
  for (int i = 0; i < MAX_PLAYER_SHOTS; i++) {
    Shot p = playerShots[i];
    if (p.enabled) {
      p.pos.x += p.vel.x * dT;
      p.pos.y += p.vel.y * dT;
      p.pos.z += p.vel.z * dT;

      // TODO - find correct end states for a shot:
      p.enabled = p.pos.y < gameWindow.camera.position.z / 2 &&
                  !checkShotHitsBarrier(&p);
      // Enemy collision happens when enemies are updated

      playerShots[i] = p;
    }
  }
}

void moveEnemyShots(float dT) {
  for (int i = 0; i < MAX_PLAYER_SHOTS; i++) {
    Shot p = enemyShots[i];
    if (p.enabled) {
      p.pos.x += p.vel.x * dT;
      p.pos.y += p.vel.y * dT;
      p.pos.z += p.vel.z * dT;

      p.enabled = p.pos.y > groundPosition.y && !checkShotHitsBarrier(&p);

      enemyShots[i] = p;
    }
  }
}

void updateState(float dT) {
  moveEnemyShots(dT);
  if (playerState == Active) {
    player.pos.x += playerVel.x * dT;
    player.pos.y += playerVel.y * dT;
    player.pos.z += playerVel.z * dT;
    if (playerShotTimer > 0) {
      playerShotTimer -= dT;
    }
    playerCollisionDetection(dT);
  } else if (playerState == Destroyed) {
    player.tint.a -= PLAYER_FADE_RATE * dT;
    if (player.tint.a <= 0.0) {
      player.tint.a = 255;
      playerState = Active;
    }
  }
  // move player shots
  movePlayerShots(dT);
  // update enemy state & enemy collision detection
  int alive = UpdateEnemyState(&enemyData, enemyData.rowCount, dT);
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
  bossModel = LoadModel("resources/models/boss.glb");
  barrierModel = LoadModel("resources/models/barrier.glb");
}

void unloadModels() {
  UnloadModel(playerModel);
  UnloadModel(enemyModel);
  UnloadModel(barrierModel);
}

Camera3D initCamera3d() {
  Camera3D camera = {0};
  camera.position = (Vector3){0.0f, 0.0f, 25.0f};
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
  SetTargetFPS(TARGET_FPS);
  if (fullScreen) {
    int monitor = GetCurrentMonitor();
    gameWindow.width = GetMonitorWidth(monitor);
    gameWindow.height = GetMonitorHeight(monitor);
    SetWindowSize(gameWindow.width, gameWindow.height);
    ToggleFullscreen();
  }
  updateScreenHelpers();
}

void resetGame() {
  gameState.state = Welcome;
  if (gameState.score > gameState.highScore) {
    gameState.highScore = gameState.score;
  }
  gameState.score = 0;
  gameState.lives = 3;
  playerState = Active;
  player.enabled = true;
  player.scale = 1.0;
  player.tint = WHITE;
  player.model = &playerModel;
  player.pos.x = 0;
  player.pos.z = 0;
  player.pos.y = gameWindow.camera.position.z * -0.30;
  groundPosition.y = player.pos.y - 1;
  groundSize.x = gameWindow.camera.position.z * 1.5;
  groundSize.y = gameWindow.camera.position.z / 1;
  InitEnemies(&enemyData, ENEMIES_PER_ROW, &enemyModel, &bossModel);
  InitBarriers(&barrierData, 4, 4, player.pos.y + 2, &barrierModel);
  for (int i = 0; i < MAX_PLAYER_SHOTS; i++) {
    playerShots[i].enabled = false;
  }
  for (int i = 0; i < MAX_ENEMY_SHOTS; i++) {
    enemyShots[i].enabled = false;
  }
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
