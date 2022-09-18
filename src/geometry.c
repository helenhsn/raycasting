#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include "geometry.h"
#include "events.h"
#include "render.h"
#include "commands.h"


SDL_Point mouse_pos;
SDL_linked_drawing *chain_drawings          = NULL;
static int rect_size                            = 5;
SDL_ray *rays_array = NULL;

bool is_point_in_rect(SDL_Rect rect, SDL_Point point)
{
        return point.x <= rect.x + rect.w && point.x >= rect.x && point.y >= rect.y && point.y <= rect.y + rect.h;
}

static float dot_product(SDL_vector2D a, SDL_vector2D b)
{
        return a.dx*b.dx + a.dy*b.dy;
}

int distance(SDL_Point pt1, SDL_FPoint pt2)
{
        return (pt1.x-pt2.x)*(pt1.x-pt2.x) + (pt1.y - pt2.y)*(pt1.y - pt2.y);
}

static void get_intersection(SDL_FPoint **intersection_pt,
                             float x1, float y1,
                             float x2, float y2,
                             float x3, float y3,
                             float x4, float y4)

{

        if ((x1 == x2)&&(x3==x4)) //vertical lines
                return;

        float a1 = (y2-y1)/(x2-x1);
        float a2 = (y4-y3)/(x4-x3);
        float b1 = y1 - a1*x1;
        float b2 = y3 - a2*x3;

        if (a1==a2) //parallel lines
                return;

        else if (x1==x2) //vertical edge
        {
                (*intersection_pt)->x = x1;
                (*intersection_pt)->y = x1 * a2 + b2 ;

        }
        else if (x3==x4) //vertical ray
        {
                (*intersection_pt)->x = x3;
                (*intersection_pt)->y = x3 * a1 + b1 ;
        }
        else //non verticals edge & rays
        {
                (*intersection_pt)->x = (b1 - b2)/(a2 - a1);
                (*intersection_pt)->y =  a2 * (*intersection_pt)->x + b2;
        }

        //check if the intersection is in the ray's direction
        SDL_vector2D start_vector;
        start_vector.dx = (*intersection_pt)->x - x1;
        start_vector.dy = (*intersection_pt)->y - y1;

        SDL_vector2D end_vector;
        end_vector.dx = (*intersection_pt)->x - x2;
        end_vector.dy = (*intersection_pt)->y - y2;

        SDL_vector2D ray_vector;
        ray_vector.dx = x4 - x3;
        ray_vector.dy = y4 - y3;

        SDL_vector2D intersect_vector;
        intersect_vector.dx = (*intersection_pt)->x - x3;
        intersect_vector.dy = (*intersection_pt)->y - y3;


        if (dot_product(start_vector, end_vector) >= 0
        || dot_product(intersect_vector, ray_vector) <= 0)
        {
                (*intersection_pt)->x = -1;
                (*intersection_pt)->y = -1;
        }



}

SDL_FPoint *is_ray_intersect_edge(SDL_edge edge, SDL_ray ray)
{
        //edge points
        float x1 = edge.vertex_1.x;
        float y1 = edge.vertex_1.y;
        float x2 = edge.vertex_2.x;
        float y2 = edge.vertex_2.y;

        //ray points
        float x3 = ray.start_point.x;
        float y3 = ray.start_point.y;
        float x4 = ray.end_point.x;
        float y4 = ray.end_point.y;



        SDL_FPoint null_point = {-1,-1};
        SDL_FPoint *intersection = &null_point;

        get_intersection(&intersection,
                          x1, y1,
                          x2, y2,
                          x3, y3,
                          x4, y4);

        if (((int)intersection->x<0) && ((int) intersection->y<0))
                return NULL;
        return intersection;
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