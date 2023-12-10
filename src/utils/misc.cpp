#include<SDL2/SDL.h>
#include "colors.hpp"


int clearRenderer(SDL_Renderer *renderer, Color color) {
  SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b,
                                color.a);
    return SDL_RenderClear(renderer);
}
