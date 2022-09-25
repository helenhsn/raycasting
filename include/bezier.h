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

typedef struct SDL_linked_FPoint{
    SDL_FPoint point;
    struct SDL_linked_FPoint *next;
}SDL_linked_FPoint;

extern SDL_FPoint *control_points;
extern SDL_linked_FPoint *bezier_smoothed_values;
extern const int nb_values;

extern float distance(SDL_FPoint *pt1, SDL_FPoint *pt2);

extern void smooth_bezier_curve(SDL_Renderer  *renderer);

extern void free_curves();

extern SDL_FPoint *create_tab_control_points();
#endif //RAYCASTING_BEZIER_H
