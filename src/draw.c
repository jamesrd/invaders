#include "game.h"
#include "raylib.h"
#include "raymath.h"
#include "utility.h"
#include <stdio.h>

#define SCORE_STRING "SCORE: %d ^(;,;)^ HIGH SCORE: %d"
#define TEXT_FORMAT_SCORE                                                      \
  TextFormat(SCORE_STRING, gameState.score, gameState.highScore)

void drawEntity(Entity *e) {
  DrawModel(*e->model, e->pos, e->scale, e->tint);
  if (drawDebug) {
    BoundingBox bb = GetModelBoundingBox(*e->model);
    bb.max = Vector3Scale(bb.max, e->scale);
    bb.min = Vector3Scale(bb.min, e->scale);
    bb.max = Vector3Add(bb.max, e->pos);
    bb.min = Vector3Add(bb.min, e->pos);
    DrawBoundingBox(bb, RED);
  }
}

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
  if (enemyData.boss->state != Disabled) {
    drawEntity(enemyData.boss->entity);
  }
}

void drawBackground() { DrawPlane(groundPosition, groundSize, DARKGREEN); }

void drawBarriers() {
  for (int i = 0; i < barrierData.count; i++) {
    Barrier b = barrierData.barriers[i];
    if (b.hitPoints > 0) {
      drawEntity(b.entity);
    }
  }
}

void drawPlayer() { drawEntity(&player); }

void drawShots() {
  for (int i = 0; i < MAX_PLAYER_SHOTS; i++) {
    if (playerShots[i].enabled) {
      DrawSphere(playerShots[i].pos, playerShots[i].radius,
                 playerShots[i].color);
    }
  }
  for (int i = 0; i < MAX_ENEMY_SHOTS; i++) {
    if (enemyShots[i].enabled) {
      DrawSphere(enemyShots[i].pos, enemyShots[i].radius, enemyShots[i].color);
    }
  }
}

void drawLives() {
  Vector3 lp =
      (Vector3){-gameWindow.camera.position.z / 2.5, player.pos.y, 4.0f};
  for (int i = 0; i < gameState.lives - 1; i++) {
    DrawModel(*player.model, lp, player.scale, WHITE);
    lp.x += player.scale * 1.5;
  }
}

void draw3dContent() {
  BeginMode3D(gameWindow.camera);
  drawBackground();
  drawEnemies();
  drawBarriers();
  drawPlayer();
  drawShots();
  if (gameState.state != Welcome) {
    drawLives();
  }
  EndMode3D();
}

void drawWelcomeScreen() {
  DrawTextCentered("SPACE INVADERS!", gameWindow.xCenter, gameWindow.yCenter,
                   gameWindow.fontTitleSize, WHITE);
  // add instructions
}

void drawUI() {
  const char *message = NULL;
  if (showFps)
    DrawFPS(10, 10);

  switch (gameState.state) {
  case Welcome:
    drawWelcomeScreen();
    return;
  case Paused:
    message = "PAUSED";
    break;
  case GameOver:
    message = gameState.lives > 0 ? "YOU WIN!" : "YOU LOSE!";
    break;
  case Playing:
    message = NULL;
    break;
  }
  if (message != NULL) {
    DrawTextCentered(message, gameWindow.xCenter, gameWindow.yCenter,
                     gameWindow.fontLargeSize, WHITE);
  }
  DrawTextCentered(TEXT_FORMAT_SCORE, gameWindow.xCenter,
                   gameWindow.fontSmallSize, gameWindow.fontSmallSize, WHITE);
}

void drawFrame() {
  BeginDrawing();
  ClearBackground((Color){50, 50, 50, 255});
  draw3dContent();
  drawUI();
  EndDrawing();
}
