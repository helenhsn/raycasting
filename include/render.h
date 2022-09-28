//
// Created by helen on 28/08/22.
//

#ifndef RAYCASTING_RENDER_H
#define RAYCASTING_RENDER_H

#include <SDL2/SDL.h>
#include <stdint.h>

extern int w_width;
extern int w_height;
extern int panel_width;
extern int points_w_h;


extern void render_rays(SDL_Event *event, SDL_Renderer *rend, void *param);

extern void init_drawing(SDL_Renderer *renderer);

extern void render_edge(SDL_Renderer *renderer, SDL_drawing drawing);

extern void render_rect(SDL_Renderer *renderer, SDL_drawing drawing);

extern void render_curves(SDL_Renderer *renderer);

extern void render_objects(SDL_Renderer *renderer);

extern void render_text(SDL_Renderer *renderer);

#endif //RAYCASTING_RENDER_H
