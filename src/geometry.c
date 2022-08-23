#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include "geometry.h"
#include "events.h"


SDL_Point mouse_pos;
static SDL_linked_drawing *chain_drawings          = NULL;
static SDL_edge null_edge                       = {0,0, 0, 0};
static SDL_Rect null_rect                       = {0,0,0,0};
static int rect_size                            = 16;

bool is_ray_intersect_edge(SDL_edge edge, SDL_ray ray)
{

}

void add_rect_to_list(SDL_Event event, SDL_Renderer *renderer, void *user_param)
{
        SDL_GetMouseState(&mouse_pos.x, &mouse_pos.y); //update mouse coordinates
        SDL_Rect new_rect           = {mouse_pos.x, mouse_pos.y, rect_size, rect_size};
        SDL_linked_drawing **first_drawing     = &chain_drawings;
        SDL_linked_drawing *drawing_to_add     = malloc(sizeof(SDL_linked_drawing));
        drawing_to_add->rect        = new_rect;
        drawing_to_add->edge        = null_edge;
        drawing_to_add->next        = chain_drawings;
        *first_drawing = drawing_to_add;

        draw_chain(renderer);
}

void add_edge_to_list(SDL_Event event, SDL_Renderer *renderer, void *user_param)
{
        SDL_linked_drawing **first_drawing = &chain_drawings;
        SDL_linked_drawing  *drawing_to_add = malloc(sizeof(SDL_linked_drawing));
        drawing_to_add->rect = null_rect;
        drawing_to_add->edge = * ((SDL_edge *)user_param);
        drawing_to_add->next = chain_drawings;
        *first_drawing = drawing_to_add;

        first_button_down = true;
        draw_chain(renderer);


}

void draw_chain(SDL_Renderer *rend)
{
        SDL_linked_drawing *current_drawing = chain_drawings;
        while (current_drawing)
        {
                if (drawing_type == SDL_SCANCODE_R)
                        SDL_RenderDrawRect(rend, &current_drawing->rect);
                else
                        SDL_RenderDrawLine(rend,
                                           current_drawing->edge.vertex_1.x,
                                           current_drawing->edge.vertex_1.y,
                                           current_drawing->edge.vertex_2.x,
                                           current_drawing->edge.vertex_2.y);
                current_drawing = current_drawing->next;
        }
}