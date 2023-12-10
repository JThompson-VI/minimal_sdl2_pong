#include "colors.hpp"
#include "SDL2/SDL_render.h"

const Color WHITE = {255, 255, 255, 255};

const Color BLACK = {0, 0, 0, 255};

const Color PLAYER_1COLOR = {255, 0, 0, 255};

const Color PLAYER_2COLOR = {0, 255, 0, 255};

int setDrawColor(SDL_Renderer *renderer, Color color) {
  return SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b,
                                color.a);
}
