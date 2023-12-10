#include <SDL2/SDL.h>

#ifndef JT_COLORS_H
#define JT_COLORS_H
struct Color {
  int r;
  int g;
  int b;
  int a;
};

extern const Color WHITE;
extern const Color BLACK;
extern const Color PLAYER_1COLOR;
extern const Color PLAYER_2COLOR;

int setDrawColor(SDL_Renderer *renderer, Color color);

#endif

