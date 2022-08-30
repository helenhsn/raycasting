//
// Created by helen on 28/08/22.
//

#include <SDL2/SDL.h>
#include <math.h>
#include "geometry.h"

int w_width = 1000;
int w_height = 800;

void init_rays()
{
        fprintf(stderr, "entering init rays");
        rays_array = calloc(360, sizeof(SDL_ray));
        for (uint32_t i = 0; i < 4; i++)
        {
                if (i==3)
                        fprintf(stderr, "\ninitrays last ray");
                rays_array[i].start_point = mouse_pos;
                rays_array[i].direction.dx = i*M_PI/2;
                rays_array[i].direction.dy = i*M_PI/2; //direction is a const for each ray
                rays_array[i].end_point.x = mouse_pos.x + cos(i*M_PI/2);
                rays_array[i].end_point.y = mouse_pos.y + sin(i*M_PI/2);
        }
}

void render_rays(SDL_Event *event, SDL_Renderer *rend, void *param)
{
        if (drawing_type != SDL_SCANCODE_E)
                return;

        for (uint8_t i = 0; i < 4; i++)
        {
                fprintf(stderr, "NOUVEAU RAYON \n ----------------------- ");

                rays_array[i].start_point = mouse_pos;
                rays_array[i].end_point.x = mouse_pos.x + 50*cos(i*M_PI/2);
                rays_array[i].end_point.y = mouse_pos.y + 50*sin(i*M_PI/2);
                SDL_Point last_endpoint = {0,0};


                SDL_linked_drawing *current_edge = chain_drawings;

                while (current_edge)
                {
                        SDL_Point *intersect_pt = is_ray_intersect_edge(current_edge->drawing.edge, rays_array[i]);
                        if ((intersect_pt)
                        &&((distance(mouse_pos, *intersect_pt) < distance(mouse_pos, last_endpoint))
                        || (last_endpoint.x == 0 && last_endpoint.y == 0)))
                                last_endpoint = *intersect_pt;
                        current_edge = current_edge->next;
                }

                //draw the endpoint with the smallest distance from the mouse (the other ones are stopped by the edge)
                fprintf(stderr, "RAY CORRECTED :ending point : (%i, %i) \n", last_endpoint.x, last_endpoint.y);
                SDL_SetRenderDrawColor(rend, 255, 255, 255, 255);
                SDL_RenderDrawLine(rend,
                                   rays_array[i].start_point.x,
                                   rays_array[i].start_point.y,
                                   last_endpoint.x,
                                   last_endpoint.y);
        }


}
void init_drawing(SDL_Renderer *renderer)
{
        SDL_edge left_edge = {0,0,0,w_height};
        add_edge_to_list(NULL, renderer, &left_edge);

        SDL_edge top_edge = {0,0,w_width, 0};
        add_edge_to_list(NULL, renderer, &top_edge);

        SDL_edge right_edge = {w_width-1, 0, w_width-1, w_height};
        add_edge_to_list(NULL, renderer, &right_edge);

        SDL_edge bottom_edge = {0, w_height-1, w_width, w_height-1};
        add_edge_to_list(NULL, renderer, &bottom_edge);
}

void render_objects(SDL_Renderer *rend) // draw all objects (edges or rects) onto the screen
{
        SDL_linked_drawing *current_drawing = chain_drawings;
        while (current_drawing)
        {
                if (drawing_type == SDL_SCANCODE_R)
                {
                        SDL_SetRenderDrawColor(rend, 0, 255, 0, 255);
                        SDL_RenderFillRect(rend, &current_drawing->drawing.rect);
                        SDL_RenderDrawRect(rend, &current_drawing->drawing.rect);
                }

                else if (drawing_type == SDL_SCANCODE_E)
                {
                        SDL_SetRenderDrawColor(rend, 255, 0, 0, 255);
                        SDL_RenderDrawLine(rend,
                                           current_drawing->drawing.edge.vertex_1.x,
                                           current_drawing->drawing.edge.vertex_1.y,
                                           current_drawing->drawing.edge.vertex_2.x,
                                           current_drawing->drawing.edge.vertex_2.y);
                }
                current_drawing = current_drawing->next;
        }
}