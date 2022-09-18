//
// Created by helen on 28/08/22.
//

#include <SDL2/SDL.h>
#include <math.h>
#include "geometry.h"
#include "commands.h"

int w_width = 1000;
int w_height = 800;
int panel_width = 200;
int panel_height = 200;
int nb_ray = 100;

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
        for (int32_t i = 0; i < nb_ray; i++)
        {
                rays_array[i].start_point = mouse_pos;
                rays_array[i].direction.dx = -i*2*M_PI/nb_ray;
                rays_array[i].direction.dy = -i*2*M_PI/nb_ray; //direction is a const for each ray
                rays_array[i].end_point.x = (float) mouse_pos.x + cos(-i*2*M_PI/nb_ray);
                rays_array[i].end_point.y = (float) mouse_pos.y + sin(-i*2*M_PI/nb_ray);
        }
}
void render_rays(SDL_Event *event, SDL_Renderer *rend, void *param)
{
        if (is_mouse_in_panel())
                return;

        for (int8_t i = 0; i < nb_ray; i++)
        {
                rays_array[i].start_point = mouse_pos;
                rays_array[i].end_point.x = (float) mouse_pos.x + cos(-i*2*M_PI/nb_ray);
                rays_array[i].end_point.y = (float) mouse_pos.y + sin(-i*2*M_PI/nb_ray);
                SDL_FPoint last_endpoint = {0,0};
                SDL_linked_drawing *current_edge = chain_drawings;

                while (current_edge)
                {
                        SDL_FPoint *intersect_pt = is_ray_intersect_edge(current_edge->drawing.edge, rays_array[i]);
                        if ((intersect_pt)
                        &&((distance(mouse_pos, *intersect_pt) < distance(mouse_pos, last_endpoint))
                        || (last_endpoint.x == 0 && last_endpoint.y == 0)))
                                last_endpoint = *intersect_pt;

                        current_edge = current_edge->next;
                }

                //draw the endpoint with the smallest distance from the mouse (the other ones are stopped by the edge)
                SDL_SetRenderDrawColor(rend, 255, 255, 255, 255);
                SDL_RenderDrawLine(rend,
                                   rays_array[i].start_point.x,
                                   rays_array[i].start_point.y,
                                   last_endpoint.x,
                                   last_endpoint.y);
        }


}

void render_rect(SDL_Renderer *rend, SDL_drawing drawing)
{
        SDL_SetRenderDrawColor(rend, 0, 255, 0, 255);
        SDL_RenderFillRect(rend, &drawing.rect);
        SDL_RenderDrawRect(rend, &drawing.rect);
}
void render_edge(SDL_Renderer *renderer, SDL_drawing drawing)
{
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderDrawLine(renderer,
                           drawing.edge.vertex_1.x,
                           drawing.edge.vertex_1.y,
                           drawing.edge.vertex_2.x,
                           drawing.edge.vertex_2.y);
}

void render_objects(SDL_Renderer *renderer) // draw all objects (edges or rects) onto the screen
{
        SDL_linked_drawing *current_drawing = chain_drawings;
        while (current_drawing)
        {
                current_drawing->renderfunc(renderer, current_drawing->drawing);
                current_drawing = current_drawing->next;
        }
}

void render_text(SDL_Renderer *renderer)
{
        SDL_text_frame *current_frame = chain_text;
        while (current_frame)
        {
                SDL_SetRenderDrawColor(renderer,
                                       current_frame->text_color.r,
                                       current_frame->text_color.g,
                                       current_frame->text_color.b,
                                       current_frame->text_color.a
                                       );
                SDL_RenderCopy(renderer, current_frame->text_texture, NULL, &current_frame->text_rect);
                SDL_SetRenderDrawColor(renderer,
                                       current_frame->button.button_color.r,
                                       current_frame->button.button_color.g,
                                       current_frame->button.button_color.b,
                                       current_frame->button.button_color.a);
                SDL_RenderDrawRect(renderer, &current_frame->button.rect);
                current_frame = current_frame->next;
        }
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderDrawRect(renderer, &selector);
}