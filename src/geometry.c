#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include "geometry.h"
#include "events.h"
#include "render.h"

SDL_FPoint mouse_pos;
SDL_linked_drawing *chain_drawings          = NULL;
static int rect_size                            = 5;
SDL_ray *rays_array = NULL;
int nb_ray = 200;


/*--------------- TOOLS ----------------*/

SDL_FPoint affine_ratio(float a, SDL_FPoint pt1, float b, SDL_FPoint pt2)
{
        //a+b = 1
        SDL_FPoint result = {a*pt1.x + b*pt2.x, a*pt1.y + b*pt2.y};
        return result;
}

inline bool is_point_in_rect(SDL_Rect rect, SDL_FPoint point)
{
        return point.x <= rect.x + rect.w && point.x >= rect.x && point.y >= rect.y && point.y <= rect.y + rect.h;
}

inline float dot_product(SDL_FPoint a, SDL_FPoint b)
{
        return a.x*b.x + a.y*b.y;
}


/*----------- DRAWINGS ----------*/
void init_drawing(SDL_Renderer *renderer)
{
        SDL_edge left_edge = {0,0,0,w_height};
        add_edge_to_list(NULL, renderer, &left_edge);

        SDL_edge top_edge = {0,0,w_width - panel_width, 0};
        add_edge_to_list(NULL, renderer, &top_edge);

        SDL_edge right_edge = {w_width-1 -panel_width, 0, w_width-1 - panel_width, w_height};
        add_edge_to_list(NULL, renderer, &right_edge);

        SDL_edge bottom_edge = {0, w_height-1,  w_width - panel_width, w_height-1};
        add_edge_to_list(NULL, renderer, &bottom_edge);
}

void init_rays()
{
        rays_array = calloc(nb_ray, sizeof(SDL_ray));
}

void add_rect_to_list(SDL_Event *event, SDL_Renderer *renderer, void *user_param)
{
        if (mouse_pos.x + rect_size/2 > w_width - panel_width)
                return;
        SDL_Rect new_rect           = {mouse_pos.x - rect_size/2, mouse_pos.y - rect_size/2, rect_size, rect_size};
        SDL_linked_drawing **first_drawing     = &chain_drawings;
        SDL_linked_drawing *drawing_to_add     = malloc(sizeof(SDL_linked_drawing));
        drawing_to_add->drawing.rect = new_rect;
        drawing_to_add->renderfunc = render_rect;
        drawing_to_add->next        = chain_drawings;
        *first_drawing = drawing_to_add;

}

void add_edge_to_list(SDL_Event *event, SDL_Renderer *renderer, void *user_param)
{
        SDL_linked_drawing **first_drawing = &chain_drawings;
        SDL_linked_drawing  *drawing_to_add = malloc(sizeof(SDL_linked_drawing));
        drawing_to_add->drawing.edge = * ((SDL_edge *)user_param);
        drawing_to_add->renderfunc = render_edge;
        drawing_to_add->next = chain_drawings;
        *first_drawing = drawing_to_add;

        first_button_down = true;


}


/*-------------- OTHERS ------------------*/

void free_chain_drawings()
{

        //free rects & edges
        SDL_linked_drawing *current_drawing = chain_drawings;
        while (current_drawing)
        {
                SDL_linked_drawing *drawing_to_free = current_drawing;
                current_drawing = current_drawing->next;
                free(drawing_to_free);
        }
        chain_drawings = NULL;

        //free rays
        free(rays_array);

}
