//
// Created by helen on 21/08/22.
//

#ifndef RAYCASTING_GEOMETRY_H
#define RAYCASTING_GEOMETRY_H


#include <SDL2/SDL.h>

/*
 * Elements to display on the screen
 * */

typedef struct SDL_vector2D
{
    int dx;
    int dy;
} SDL_vector2D;

typedef struct SDL_edge
{
    SDL_Point vertex_1;
    SDL_Point vertex_2;
} SDL_edge;

typedef struct SDL_ray
{
    SDL_Point point;
    SDL_vector2D direction;
} SDL_ray ;

typedef struct SDL_linked_drawing
{
    SDL_Rect rect;
    SDL_edge edge;
    struct SDL_linked_drawing *next;
} SDL_linked_drawing;


/*
 * Global variables
 * */

extern SDL_Point mouse_pos;
extern SDL_Scancode drawing_type;

/*
 * Functions for rectangles & edges
 * */

extern void add_rect_to_list(SDL_Event event, SDL_Renderer *renderer, void *user_param);

extern void add_edge_to_list(SDL_Event event, SDL_Renderer *renderer, void *user_param);

extern void draw_chain(SDL_Renderer *rend);

extern bool is_ray_intersect_edge(SDL_edge edge, SDL_ray ray);

#endif //RAYCASTING_GEOMETRY_H
