#include "colors.h"
#include "SDL2/SDL_render.h"

const Color WHITE = {
    .r = 255,
    .g = 255,
    .b = 255,
    .opacity = 255,
};

const Color BLACK = {
    .r = 0,
    .g = 0,
    .b = 0,
    .opacity = 255,
};

const Color PLAYER_1COLOR = {
    .r = 255,
    .b = 0,
    .g = 0,
    .opacity = 255,
};

const Color PLAYER_2COLOR = {
    .r = 0,
    .b = 255,
    .g = 0,
    .opacity = 255,
};

int setDrawColor(SDL_Renderer *renderer, Color color) {
  return SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b,
                                color.opacity);
}
