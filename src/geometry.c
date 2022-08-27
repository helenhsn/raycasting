#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <math.h>
#include "geometry.h"
#include "events.h"


SDL_Point mouse_pos;
SDL_linked_drawing *chain_drawings          = NULL;
static SDL_edge null_edge                       = {0,0, 0, 0};
static SDL_Rect null_rect                       = {0,0,0,0};
static int rect_size                            = 30;
bool CASTING_RAYS = false;
SDL_ray *rays_array = NULL;

static void init_rays()
{
        rays_array = calloc(360, sizeof(SDL_ray));
        for (uint8_t i = 0; i < 360; i++)
        {
                rays_array[i].start_point = mouse_pos;
                rays_array[i].direction.dx = i*180/M_PI;
                rays_array[i].direction.dy = i*180/M_PI;
        }
}
void update_rays()
{

}

void is_ray_intersect_edge(SDL_edge edge, SDL_ray ray)
{
        //based on line-line intersection article in Wikipedia

        //edge points
        int x1 = edge.vertex_1.x;
        int y1 = edge.vertex_1.y;
        int x2 = edge.vertex_2.x;
        int y2 = edge.vertex_2.y;

        //ray points
        int x3 = ray.start_point.x;
        int y3 = ray.start_point.y;
        int x4 = ray.start_point.x + ray.direction.dx;
        int y4 = ray.start_point.y + ray.direction.dy;

        float denom = (float) ((x1-x2)*(y3-y4) - (y1-y2)*(x3-x4));

        if (denom==0)
                return;

        float t = (float) ((x1 - x3)*(y3-y4)- (y1-y3)*(x3-x4))/denom;

        float u = (float) ((x1-x3)*(y1-y2) - (y1-y3)*(x1-x2))/denom;

        if ((0 <= t <=1) && (0 <= u < 1))
        {

        }

}

void add_rect_to_list(SDL_Event event, SDL_Renderer *renderer, void *user_param)
{
        SDL_GetMouseState(&mouse_pos.x, &mouse_pos.y); //update mouse coordinates
        SDL_Rect new_rect           = {mouse_pos.x - rect_size/2, mouse_pos.y - rect_size/2, rect_size, rect_size};
        SDL_linked_drawing **first_drawing     = &chain_drawings;
        SDL_linked_drawing *drawing_to_add     = malloc(sizeof(SDL_linked_drawing));
        drawing_to_add->drawing.rect = new_rect;
        drawing_to_add->next        = chain_drawings;
        *first_drawing = drawing_to_add;

        draw_chain(renderer);
}

void add_edge_to_list(SDL_Event event, SDL_Renderer *renderer, void *user_param)
{
        SDL_linked_drawing **first_drawing = &chain_drawings;
        SDL_linked_drawing  *drawing_to_add = malloc(sizeof(SDL_linked_drawing));
        drawing_to_add->drawing.edge = * ((SDL_edge *)user_param);
        drawing_to_add->next = chain_drawings;
        *first_drawing = drawing_to_add;

        first_button_down = true;
        draw_chain(renderer);


}

void draw_chain(SDL_Renderer *rend) // draw all
{
        SDL_linked_drawing *current_drawing = chain_drawings;
        while (current_drawing)
        {
                if (drawing_type == SDL_SCANCODE_E)
                {
                        SDL_RenderFillRect(rend, &current_drawing->drawing.rect);
                        SDL_RenderDrawRect(rend, &current_drawing->drawing.rect);
                }

                else if (drawing_type == SDL_SCANCODE_D)
                        SDL_RenderDrawLine(rend,
                                           current_drawing->drawing.edge.vertex_1.x,
                                           current_drawing->drawing.edge.vertex_1.y,
                                           current_drawing->drawing.edge.vertex_2.x,
                                           current_drawing->drawing.edge.vertex_2.y);
                current_drawing = current_drawing->next;
        }
        if (CASTING_RAYS)
        {
                
        }
}

void free_chain_drawings()
{
        SDL_linked_drawing *current_drawing = chain_drawings;
        while (current_drawing)
        {
                SDL_linked_drawing *drawing_to_free = current_drawing;
                current_drawing = current_drawing->next;
                free(drawing_to_free);
                drawing_to_free = NULL;
        }
        chain_drawings = NULL;
}