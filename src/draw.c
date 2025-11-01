#include "game.h"
#include "raylib.h"
#include "utility.h"
#include <stdio.h>

#define SCORE_STRING "LIVES: %d <> SCORE: %d <> HIGH SCORE: %d"

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

void draw3dContent() {
  BeginMode3D(gameWindow.camera);
  drawBackground();
  drawEnemies();
  drawBarriers();
  drawPlayer();
  drawShots();
  EndMode3D();
}

void drawUI() {
  char score[100];
  if (showFps)
    DrawFPS(10, 10);
  switch (gameState.state) {
  case Welcome:
    DrawTextCentered("SPACE INVADERS!", gameWindow.xCenter, gameWindow.yCenter,
                     45, WHITE);
    break;
  case Paused:
    DrawTextCentered("PAUSED", gameWindow.xCenter, gameWindow.yCenter,
                     gameWindow.fontLargeSize, WHITE);
    snprintf(score, 99, SCORE_STRING, gameState.lives, gameState.score,
             gameState.highScore);
    DrawTextCentered(score, gameWindow.xCenter, gameWindow.fontSmallSize,
                     gameWindow.fontSmallSize, WHITE);
    break;
  case GameOver:
    if (gameState.lives > 0)
      DrawTextCentered("YOU WIN!", gameWindow.xCenter, gameWindow.yCenter,
                       gameWindow.fontLargeSize, WHITE);
    else
      DrawTextCentered("YOU LOSE!", gameWindow.xCenter, gameWindow.yCenter,
                       gameWindow.fontLargeSize, WHITE);
    snprintf(score, 99, SCORE_STRING, gameState.lives, gameState.score,
             gameState.highScore);
    DrawTextCentered(score, gameWindow.xCenter, gameWindow.fontSmallSize,
                     gameWindow.fontSmallSize, WHITE);
  default:
    snprintf(score, 99, SCORE_STRING, gameState.lives, gameState.score,
             gameState.highScore);
    DrawTextCentered(score, gameWindow.xCenter, gameWindow.fontSmallSize,
                     gameWindow.fontSmallSize, WHITE);
  }
}

void drawFrame() {
  BeginDrawing();
  ClearBackground((Color){50, 50, 50, 255});
  draw3dContent();
  drawUI();
  EndDrawing();
}
