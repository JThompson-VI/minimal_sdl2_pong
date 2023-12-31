#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sys/wait.h>
#include <time.h>
#include <vector>

#include "utils/colors.hpp"
#include "utils/misc.hpp"

const int WIDTH = 640;
const int HEIGHT = 400;
const int BALL_SIZE = 10;

typedef struct Ball {
  float x;
  float y;
  float xSpeed;
  float ySpeed;
  int size;
} Ball;

typedef struct Player {
  int score;
  float yPosition;
} Player;

static const int PLAYER_WIDTH = 10;
static const int PLAYER_HEIGHT = 75;
static const int PLAYER_MARGIN = 10;
static const float PLAYER_MOVE_SPEED = 300.0f;
static bool isServed = false;

static Ball ball;
static Player player1;
static Player player2;

bool initialize(void);
void update(float);
void draw(void);
void shutdown(void);
Ball makeBall(int size);
void updateBall(Ball *ball, float elapsed);
Player makePlayer(void);
void updatePlayer(float);
void renderPlayers(void);
void renderBall(const Ball *ball);
void mainLoop(void);
void updateScore(int player, int points);
void serveBall(Ball *ball);
void resetBall(Ball *ball);
// TODO: void renderScore(Player *player1, Player *player2);

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;

bool initialize(void) {
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    fprintf(stderr, "Failed to initialize SDL:%s\n", SDL_GetError());
    return false;
  }

  window =
      SDL_CreateWindow("pong", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                       WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
  if (window == NULL) {
    fprintf(stderr, "Failed to initialize SDL window:%s\n", SDL_GetError());
    return false;
  }
  renderer = SDL_CreateRenderer(
      window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if (renderer == NULL) {
    fprintf(stderr, "Failed to initialize SDL renderer:%s\n", SDL_GetError());
    return false;
  }

  srand(time(NULL));
  ball = makeBall(BALL_SIZE);
  player1 = makePlayer();
  player2 = makePlayer();
  return true;
}

void update(float elapsed) {
  updateBall(&ball, elapsed);
  updatePlayer(elapsed);
}

void drawCenterLine(void) {
  const int width = 3;
  const int lineSegHeight = 20;

  Color centerLineColor = {255, 255, 255, 64};

  setDrawColor(renderer, centerLineColor);
  SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
  int drawPosition = 0;
  int rectsLength = HEIGHT / lineSegHeight / 2 + 1;
  std::vector<SDL_Rect> rects;
  while (drawPosition < HEIGHT) {
    SDL_Rect lineSegment = {
        .x = WIDTH / 2 - width / 2,
        .y = drawPosition,
        .w = width,
        .h = lineSegHeight,
    };
    rects.push_back(lineSegment);
    drawPosition += 2 * lineSegHeight;
  }
  SDL_RenderFillRects(renderer, rects.data(), rectsLength);
}

void draw(void) {
  clearRenderer(renderer, BLACK);
  drawCenterLine();

  renderBall(&ball);
  renderPlayers();

  SDL_RenderPresent(renderer);
}

void shutdown(void) {
  if (renderer != NULL) {
    SDL_DestroyRenderer(renderer);
  }
  if (window != NULL) {
    SDL_DestroyWindow(window);
  }
  SDL_Quit();
}

void mainLoop(void) {
  bool quit = false;
  Uint32 lastTick = SDL_GetTicks();

  while (!quit) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
      case SDL_QUIT:
        quit = true;
        break;

      case SDL_KEYDOWN:
        break;
      }
    }

    Uint32 curTick = SDL_GetTicks();
    float elapsed = (curTick - lastTick) / 1000.0f;
    update(elapsed);
    draw();
    lastTick = curTick;
  }
}

int main(void) {
  atexit(shutdown);
  if (initialize() == false) {
    exit(1);
  }
  mainLoop();
}

bool coinFlip(void) { return rand() % 2 == 1 ? true : false; }

Ball makeBall(int size) {
  const float speed = 255;
  Ball ball = {
      .x = (float)WIDTH / 2 - (float)size / 2,
      .y = (float)HEIGHT / 2 - (float)size / 2,
      .xSpeed = speed * (coinFlip() ? 1 : -1),
      .ySpeed = speed * (coinFlip() ? 1 : -1),
      .size = size,
  };
  return ball;
}

void renderBall(const Ball *ball) {
  int size = ball->size;
  // int halfSize = size / 2;
  SDL_Rect rect = {
      .x = static_cast<int>(ball->x),
      .y = static_cast<int>(ball->y),
      .w = size,
      .h = size,
  };
  setDrawColor(renderer, WHITE);
  SDL_RenderFillRect(renderer, &rect);
}

void updateBall(Ball *ball, float elapsed) {
  if (!isServed) {
    ball->x = (float)WIDTH / 2 - (float)ball->size / 2;
    ball->y = (float)HEIGHT / 2 - (float)ball->size / 2;
    return;
  }
  ball->x += ball->xSpeed * elapsed;
  ball->y += ball->ySpeed * elapsed;

  if (ball->x <= 0) {
    // player2 score
    updateScore(2, 1);
    resetBall(ball);
    isServed = false;
  } else if (ball->x > WIDTH - ball->size) {
    // player1 score
    updateScore(1, 1);
    resetBall(ball);
    isServed = false;
  }

  if (ball->y <= 0) {
    ball->ySpeed = fabs(ball->ySpeed);
  } else if (ball->y > HEIGHT - ball->size) {
    ball->ySpeed = -fabs(ball->ySpeed);
  }
}

Player makePlayer(void) {
  Player player = {
      .yPosition = (float)HEIGHT / 2 - (float)PLAYER_HEIGHT / 2,
  };
  return player;
}

void updatePlayer(float elapsed) {
  const Uint8 *keyboardState = SDL_GetKeyboardState(NULL);

  if (keyboardState[SDL_SCANCODE_SPACE]) {
    if (!isServed) {
      serveBall(&ball);
    }
  }
  if (keyboardState[SDL_SCANCODE_W] && player1.yPosition > 0) {
    player1.yPosition -= PLAYER_MOVE_SPEED * elapsed;
  }
  if (keyboardState[SDL_SCANCODE_S] &&
      player1.yPosition < HEIGHT - PLAYER_HEIGHT) {
    player1.yPosition += PLAYER_MOVE_SPEED * elapsed;
  }
  if (keyboardState[SDL_SCANCODE_UP] && player2.yPosition > 0) {
    player2.yPosition -= PLAYER_MOVE_SPEED * elapsed;
  }
  if (keyboardState[SDL_SCANCODE_DOWN] &&
      player2.yPosition < HEIGHT - PLAYER_HEIGHT) {
    player2.yPosition += PLAYER_MOVE_SPEED * elapsed;
  }

  SDL_Rect ballRect = {
      .x = static_cast<int>(ball.x),
      .y = static_cast<int>(ball.y),
      .w = ball.size,
      .h = ball.size,
  };

  SDL_Rect player1paddle = {
      .x = PLAYER_MARGIN,
      .y = static_cast<int>(player1.yPosition),
      .w = PLAYER_WIDTH,
      .h = PLAYER_HEIGHT,
  };
  SDL_Rect player2paddle = {
      .x = WIDTH - PLAYER_MARGIN - PLAYER_WIDTH,
      .y = static_cast<int>(player2.yPosition),
      .w = PLAYER_WIDTH,
      .h = PLAYER_HEIGHT,
  };
  if (SDL_HasIntersection(&player1paddle, &ballRect)) {
    ball.xSpeed = fabs(ball.xSpeed);
  }

  if (SDL_HasIntersection(&player2paddle, &ballRect)) {
    ball.xSpeed = -fabs(ball.xSpeed);
  }
}

void renderPlayers(void) {
  setDrawColor(renderer, PLAYER_1COLOR);
  SDL_Rect player1paddle = {
      .x = PLAYER_MARGIN,
      .y = static_cast<int>(player1.yPosition),
      .w = PLAYER_WIDTH,
      .h = PLAYER_HEIGHT,
  };
  SDL_RenderFillRect(renderer, &player1paddle);

  setDrawColor(renderer, PLAYER_2COLOR);
  SDL_Rect player2paddle = {
      .x = WIDTH - PLAYER_MARGIN - PLAYER_WIDTH,
      .y = static_cast<int>(player2.yPosition),
      .w = PLAYER_WIDTH,
      .h = PLAYER_HEIGHT,
  };
  SDL_RenderFillRect(renderer, &player2paddle);
}

void updateScore(int player, int points) {
  if (player == 1) {
    player1.score += points;
  } else if (player == 2) {
    player2.score += points;
  }
  std::string scoreLine = "PLAYER 1: " + std::to_string(player1.score) +
                   " | PLAYER 2: " + std::to_string(player2.score);
  SDL_SetWindowTitle(window, scoreLine.data());
}

void serveBall(Ball *ball) {
  ball->xSpeed = ball->xSpeed * (coinFlip() ? 1 : -1);
  ball->ySpeed = ball->ySpeed * (coinFlip() ? 1 : -1);
  isServed = true;
}

void resetBall(Ball *ball) {
  ball->x = (float)WIDTH / 2 - (float)ball->size / 2;
  ball->y = (float)HEIGHT / 2 - (float)ball->size / 2;
}
