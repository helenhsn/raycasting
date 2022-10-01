//
// Created by helen on 28/08/22.
//

#include <SDL2/SDL.h>
#include <math.h>
#include "geometry.h"
#include "commands.h"
#include "bezier.h"
#include "intersection.h"

int w_width = 1000;
int w_height = 800;
int panel_width = 200;
int points_w_h = 10;


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

                SDL_FPoint last_endpoint = {50,50};
                draw_intersection_ray_edges(rend, rays_array[i], &last_endpoint);
                draw_intersection_ray_curve(rend, rays_array[i], &last_endpoint);
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
        SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255);
        SDL_RenderDrawLine(renderer,
                           drawing.edge.vertex_1.x,
                           drawing.edge.vertex_1.y,
                           drawing.edge.vertex_2.x,
                           drawing.edge.vertex_2.y);
}
static void draw_static_control_points(SDL_Renderer *renderer)
{
        for (uint8_t i=0; i<nb_control_points; i++)
        {
                SDL_Rect point_symbol = {control_points[i].x - points_w_h/2,
                                         control_points[i].y - points_w_h/2,
                                         points_w_h,
                                         points_w_h};
                SDL_SetRenderDrawColor(renderer, 255, 0, 255, 255);
                SDL_RenderDrawRect(renderer, &point_symbol);
                SDL_RenderFillRect(renderer, &point_symbol);
        }
}

static void draw_var_control_points(SDL_Renderer *renderer)
{
        SDL_Rect point_symbol = {switch_point.x - points_w_h/2,
                                 switch_point.y - points_w_h/2,
                                 points_w_h,
                                 points_w_h};
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderDrawRect(renderer, &point_symbol);
        SDL_RenderFillRect(renderer, &point_symbol);

        point_symbol.x = control_points[2].x- points_w_h/2;
        point_symbol.y = control_points[2].y- points_w_h/2;
        SDL_RenderDrawRect(renderer, &point_symbol);
        SDL_RenderFillRect(renderer, &point_symbol);
}

void render_curves(SDL_Renderer *renderer)
{
        if (control_points)
                draw_static_control_points(renderer);
        if (!splines) {
                return;
        }
        SDL_linked_tab *current_bezier = splines;
        while(current_bezier)
        {
                SDL_FPoint *current_tab_bezier = (current_bezier->tab);
                uint32_t last_i = 0;
                for(uint32_t current_i = 1; current_i<nb_values; current_i++)
                {
                        SDL_SetRenderDrawColor(renderer, 255, 0, 255, 255);
                        SDL_RenderDrawLineF(renderer,
                                            current_tab_bezier[last_i].x,
                                            current_tab_bezier[last_i].y,
                                            current_tab_bezier[current_i].x,
                                            current_tab_bezier[current_i].y);
                        last_i++;
                }
                current_bezier = current_bezier->next;
        }
        draw_var_control_points(renderer);
}

void render_objects(SDL_Renderer *renderer) // draw all objects (edges or rects) onto the screen
{
        SDL_linked_drawing *current_drawing = chain_objects;
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
        SDL_RenderFillRect(renderer, &selector);
}