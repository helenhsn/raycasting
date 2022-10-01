//
// Created by helen on 21/08/22.
//

#ifndef RAYCASTING_GEOMETRY_H
#define RAYCASTING_GEOMETRY_H


#include <SDL2/SDL.h>
#include <stdbool.h>

/*
 * Elements to display on the screen
 * */

typedef struct SDL_edge
{
    SDL_FPoint vertex_1;
    SDL_FPoint vertex_2;
} SDL_edge;

typedef struct SDL_ray
{
    SDL_FPoint start_point;
    SDL_FPoint end_point; //used for rendering
} SDL_ray ;

typedef union SDL_drawing
{
    SDL_Rect rect;
    SDL_edge edge;
} SDL_drawing;

typedef void (*SDL_renderfunc) (SDL_Renderer *renderer, SDL_drawing drawing);

typedef struct SDL_linked_drawing
{
    SDL_drawing drawing;
    SDL_renderfunc renderfunc;
    struct SDL_linked_drawing *next;
} SDL_linked_drawing;


/*
 * Global variables
 * */

extern SDL_FPoint mouse_pos;
extern SDL_linked_drawing *chain_drawings;
extern SDL_ray *rays_array;
extern int nb_ray;

/*
 * Functions for rectangles & edges
 * */
extern SDL_FPoint affine_ratio(float a, SDL_FPoint pt1, float b, SDL_FPoint pt2);

extern bool is_point_in_rect(SDL_Rect rect, SDL_FPoint point);

extern float dot_product(SDL_FPoint a, SDL_FPoint b);

extern float distance(SDL_FPoint *pt1, SDL_FPoint *pt2);

extern void init_rays();

extern void init_drawing(SDL_Renderer *renderer);

extern void add_rect_to_list(SDL_Event *event, SDL_Renderer *renderer, void *user_param);

extern void add_edge_to_list(SDL_Event *event, SDL_Renderer *renderer, void *user_param);

extern void free_chain_drawings();


#endif //RAYCASTING_GEOMETRY_H
