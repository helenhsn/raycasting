//
// Created by helen on 22/09/22.
//

#ifndef RAYCASTING_BEZIER_H
#define RAYCASTING_BEZIER_H

#include <SDL2/SDL.h>
typedef struct SDL_bezier_FPoint
{
    float distance;
    SDL_FPoint point;
}SDL_bezier_FPoint;

typedef struct SDL_linked_FPoint
{
    SDL_FPoint point;
    struct SDL_linked_FPoint *next;
}SDL_linked_FPoint;

typedef struct SDL_linked_tab
{
    SDL_FPoint *tab;
    struct SDL_linked_tab *next;
} SDL_linked_tab;

extern SDL_FPoint *control_points;
extern SDL_linked_tab *splines;
extern SDL_linked_FPoint *chain_control_points;
extern int nb_control_points;
extern const int nb_values;

extern float distance(SDL_FPoint *pt1, SDL_FPoint *pt2);

extern void smooth_bezier_curve(SDL_Renderer  *renderer, SDL_linked_tab *new_bezier_tab);

extern void free_curves();

extern SDL_linked_tab *create_bezier_tab();

extern void create_tab_control_points();

extern void add_tab_to_spline(SDL_linked_tab *new_bezier_tab);
#endif //RAYCASTING_BEZIER_H
