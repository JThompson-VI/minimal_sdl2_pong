#include <SDL2/SDL.h>

#ifndef JT_COLORS_H
#define JT_COLORS_H
typedef struct Color {
  int r;
  int g;
  int b;
  int opacity;
} Color;

const Color WHITE;
const Color BLACK;
const Color PLAYER_1COLOR;
const Color PLAYER_2COLOR;

int setDrawColor(SDL_Renderer *renderer, Color color);

#endif

