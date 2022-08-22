#include <stdio.h>
#include
#include <stdbool.h>
#include <SDL2/SDL.h>
#include "geometry.h"


SDL_Point mouse_pos;
static SDL_linked_Rect *chain_rects    = NULL;
static int rect_size                   = 16;

bool is_ray_intersect_edge(SDL_edge edge, SDL_ray ray)
{

}

void add_rect_to_list(SDL_Event event, SDL_Renderer *renderer, void *user_param)
{
    SDL_GetMouseState(&mouse_pos.x, &mouse_pos.y); //update mouse coordinates
    SDL_Rect new_rect = {mouse_pos.x, mouse_pos.y, rect_size, rect_size};
    SDL_linked_Rect **first_rect_ad = &chain_rects;
    SDL_linked_Rect *rect_to_add = malloc(sizeof(SDL_linked_Rect));
    rect_to_add->rect = new_rect;
    rect_to_add->next = chain_rects;
    *first_rect_ad = rect_to_add;

    draw_chain_rects(renderer);
}

void draw_chain_rects(SDL_Renderer *rend)
{
    SDL_linked_Rect *current_rect = chain_rects;
    while (current_rect)
    {
        SDL_RenderDrawRect(rend, &current_rect->rect);
        current_rect = current_rect->next;
    }
}