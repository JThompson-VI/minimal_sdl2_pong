#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <time.h>

#include "utils/colors.h"
#include "utils/misc.h"

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

const int PLAYER_WIDTH = 20;
const int PLAYER_HEIGHT = 75;
const int PLAYER_MARGIN = 10;
const float PLAYER_MOVE_SPEED = 150.0f;
bool isServed = false;

Ball ball;
Player player1;
Player player2;

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
void mainLoop(SDL_Event);
void updateScore(int player, int points);
void serveBall(Ball *ball);

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;

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
  ball = makeBall(BALL_SIZE);
  player1 = makePlayer();
  player2 = makePlayer();
  return true;
}

void update(float elapsed) {
  updateBall(&ball, elapsed);
  updatePlayer(elapsed);
}

void draw(void) {
  clearRenderer(renderer, BLACK);

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

int main(void) {
  srand(time(NULL));
  atexit(shutdown);
  if (initialize() == false) {
    exit(1);
  }

  bool quit = false;
  Uint32 lastTick = SDL_GetTicks();

  while (!quit) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        quit = true;
      }
    }

    Uint32 curTick = SDL_GetTicks();
    float elapsed = (curTick - lastTick) / 1000.0f;
    update(elapsed);
    draw();
    lastTick = curTick;
  }
}

bool coinFlip(void) { return rand() % 2 == 1 ? true : false; }

Ball makeBall(int size) {
  const float speed = 120;
  Ball ball = {
      .x = (float)WIDTH / 2 - (float)BALL_SIZE / 2,
      .y = (float)HEIGHT / 2 - (float)BALL_SIZE / 2,
      .size = size,
      .xSpeed = speed * (coinFlip() ? 1 : -1),
      .ySpeed = speed * (coinFlip() ? 1 : -1),
  };
  return ball;
}

void renderBall(const Ball *ball) {
  int size = ball->size;
  int halfSize = size / 2;
  SDL_Rect rect = {
      .x = ball->x - halfSize,
      .y = ball->y - halfSize,
      .w = size,
      .h = size,
  };
  setDrawColor(renderer, WHITE);
  SDL_RenderFillRect(renderer, &rect);
}

void updateBall(Ball *ball, float elapsed) {
  if (!isServed) {
    ball->x = (float)WIDTH / 2;
    ball->y = (float)HEIGHT / 2;
    return;
  }
  ball->x += ball->xSpeed * elapsed;
  ball->y += ball->ySpeed * elapsed;

  if (ball->x < (float)BALL_SIZE / 2) {
    // player2 score
    updateScore(2, 1);
    ball->x = (float)WIDTH / 2 - (float)BALL_SIZE / 2;
    ball->y = (float)HEIGHT / 2 - (float)BALL_SIZE / 2;
    isServed = false;
  } else if (ball->x > WIDTH - (float)BALL_SIZE / 2) {
    // player1 score
    updateScore(1, 1);
    ball->x = (float)WIDTH / 2 - (float)BALL_SIZE / 2;
    ball->y = (float)HEIGHT / 2 - (float)BALL_SIZE / 2;
    isServed = false;
  }

  if (ball->y < (float)BALL_SIZE / 2) {
    ball->ySpeed = fabs(ball->ySpeed);
  } else if (ball->y > HEIGHT - (float)BALL_SIZE / 2) {
    ball->ySpeed = -fabs(ball->ySpeed);
  }
}
Player makePlayer(void) {
  Player player = {.yPosition = (float)HEIGHT / 2};
  return player;
}

void updatePlayer(float elapsed) {
  const Uint8 *keyboardState = SDL_GetKeyboardState(NULL);

  if (keyboardState[SDL_SCANCODE_SPACE]) {
    if (!isServed) {
      serveBall(&ball);
    }
  }
  if (keyboardState[SDL_SCANCODE_W] &&
      player1.yPosition - (float)PLAYER_HEIGHT / 2 > 0) {
    player1.yPosition -= PLAYER_MOVE_SPEED * elapsed;
  }
  if (keyboardState[SDL_SCANCODE_S] &&
      player1.yPosition < HEIGHT - (float)PLAYER_HEIGHT / 2) {
    player1.yPosition += PLAYER_MOVE_SPEED * elapsed;
  }
  if (keyboardState[SDL_SCANCODE_UP] &&
      player2.yPosition - (float)PLAYER_HEIGHT / 2 > 0) {
    player2.yPosition -= PLAYER_MOVE_SPEED * elapsed;
  }
  if (keyboardState[SDL_SCANCODE_DOWN] &&
      player2.yPosition < HEIGHT - (float)PLAYER_HEIGHT / 2) {
    player2.yPosition += PLAYER_MOVE_SPEED * elapsed;
  }

  SDL_Rect ballRect = {
      .x = ball.x - (float)ball.size / 2,
      .y = ball.y - (float)ball.size / 2,
      .w = ball.size,
      .h = ball.size,
  };

  SDL_Rect player1paddle = {
      .x = PLAYER_MARGIN,
      .y = (int)player1.yPosition - PLAYER_HEIGHT / 2,
      .w = PLAYER_WIDTH,
      .h = PLAYER_HEIGHT,
  };
  SDL_Rect player2paddle = {
      .x = WIDTH - PLAYER_MARGIN - PLAYER_WIDTH,
      .y = (int)player2.yPosition - PLAYER_HEIGHT / 2,
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
      .y = (int)player1.yPosition - PLAYER_HEIGHT / 2,
      .w = PLAYER_WIDTH,
      .h = PLAYER_HEIGHT,
  };
  SDL_RenderFillRect(renderer, &player1paddle);

  setDrawColor(renderer, PLAYER_2COLOR);
  SDL_Rect player2paddle = {
      .x = WIDTH - PLAYER_MARGIN - PLAYER_WIDTH,
      .y = (int)player2.yPosition - PLAYER_HEIGHT / 2,
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
  const char *fmt = "PLAYER 1: %d | PLAYER 2: %d";
  int len = snprintf(NULL, 0, fmt, player1.score, player2.score);
  char scoreLine[len + 1];
  snprintf(scoreLine, len + 1, fmt, player1.score, player2.score);
  SDL_SetWindowTitle(window, scoreLine);
}

void serveBall(Ball *ball) {
  ball->xSpeed = ball->xSpeed * (coinFlip() ? 1 : -1);
  ball->ySpeed = ball->ySpeed * (coinFlip() ? 1 : -1);
  isServed = true;
}
