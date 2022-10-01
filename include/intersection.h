//
// Created by helen on 29/09/22.
//

#ifndef RAYCASTING_INTERSECTION_H
#define RAYCASTING_INTERSECTION_H
#include "geometry.h"
#include <SDL2/SDL.h>

extern void draw_intersection_ray_edges(SDL_Renderer *renderer, SDL_ray ray, SDL_FPoint *last_endpoint);

extern void draw_intersection_ray_curve(SDL_Renderer *renderer, SDL_ray ray, SDL_FPoint *last_endpoint);

#endif //RAYCASTING_INTERSECTION_H
