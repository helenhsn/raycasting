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
int panel_height = 100;

void render_rays(SDL_Event *event, SDL_Renderer *rend, void *param)
{
        if (is_mouse_in_panel())
                return;
        for (uint16_t i = 0; i < nb_ray; i++)
        {
                rays_array[i].start_point.x = mouse_pos.x;
                rays_array[i].start_point.y = mouse_pos.y;
                rays_array[i].end_point.x = mouse_pos.x + cos(-i*2*M_PI/nb_ray);
                rays_array[i].end_point.y = mouse_pos.y + sin(-i*2*M_PI/nb_ray);
                SDL_FPoint last_endpoint = {-1,-1};
                SDL_FPoint intersect_pt = {-2, -2};
                SDL_linked_drawing *current_edge = chain_drawings;

                while (current_edge)
                {
                        if (is_ray_intersect_edge(&intersect_pt, current_edge->drawing.edge, rays_array[i]))
                        {
                                if ( (last_endpoint.x == -1 && last_endpoint.y == -1) || distance(&mouse_pos, &intersect_pt) < distance(&mouse_pos, &last_endpoint))
                                {
                                        last_endpoint.x = intersect_pt.x;
                                        last_endpoint.y = intersect_pt.y;
                                }
                        }

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