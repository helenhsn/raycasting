//
// Created by helen on 21/08/22.
//

#ifndef RAYCASTING_GEOMETRY_H
#define RAYCASTING_GEOMETRY_H


#include <SDL2/SDL.h>

/*
 * Elements to display on the screen
 * */

typedef struct SDL_linked_Rect
{
    SDL_Rect rect;
    struct SDL_linked_Rect *next;
} SDL_linked_Rect;

typedef struct SDL_ray
{
    SDL_Point point;
    float direction;
} SDL_ray ;

typedef struct SDL_edge
{
    SDL_Point vertex_1;
    SDL_Point vertex_2;
} SDL_edge;


/*
 * Global variables
 * */

extern SDL_Point mouse_pos;

/*
 * Functions for rectangles & edges
 * */

extern void add_rect_to_list(SDL_Event event, SDL_Renderer *renderer, void *user_param);

extern void draw_chain_rects(SDL_Renderer *rend);

extern bool is_ray_intersect_edge(SDL_edge edge, SDL_ray ray);

#endif //RAYCASTING_GEOMETRY_H
