//
// Created by helen on 28/08/22.
//

#ifndef RAYCASTING_RENDER_H
#define RAYCASTING_RENDER_H

#include <SDL2/SDL.h>
#include <stdint.h>

extern int w_width;
extern int w_height;

extern void init_rays();

extern void render_rays(SDL_Event *event, SDL_Renderer *rend, void *param);

extern void init_drawing(SDL_Renderer *renderer);

extern void render_objects(SDL_Renderer *rend);

#endif //RAYCASTING_RENDER_H