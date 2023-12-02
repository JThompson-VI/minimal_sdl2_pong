#include "colors.h"


int clearRenderer(SDL_Renderer *renderer, Color color) {
  SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b,
                                color.opacity);
    return SDL_RenderClear(renderer);
}
