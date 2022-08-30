#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include "geometry.h"
#include "events.h"


SDL_Point mouse_pos;
SDL_linked_drawing *chain_drawings          = NULL;
static int rect_size                            = 30;
bool CASTING_RAYS = false;
SDL_ray *rays_array = NULL;

static bool same_direction(SDL_edge a, SDL_edge b)
{
        int scalar_product = a.vertex_1.x*b.vertex_1
}
int distance(SDL_Point pt1, SDL_Point pt2)
{
        return (pt1.x-pt2.x)*(pt1.x-pt2.x) + (pt1.y - pt2.y)*(pt1.y - pt2.y);
}
static int min(int a, int b)
{
        return a<b? a : b;
}

static int max(int a, int b)
{
        return a>b? a : b;
}

static void get_intersection(SDL_Point **intersection_pt,
                                   int x1, int y1,
                                   int x2, int y2,
                                   int x3, int y3,
                                   int x4, int y4,
                                   float a1, float a2,
                                   float b1, float b2)

{
        if (((x2==x1) && (x3 == x4)) || a1 == a2)
        {
                *intersection_pt = NULL;
                return;
        }


        else if (x1==x2)
        {
                (*intersection_pt)->x = x1;
                (*intersection_pt)->y = x1 * a2 + b2 ;

        }
        else if (x3==x4)
        {
                (*intersection_pt)->x = x3;
                (*intersection_pt)->y = x3 * a1 + b1 ;
        }
        else
        {
                (*intersection_pt)->x = (int) (b1 - b2)/(a2 - a1);
                (*intersection_pt)->y =  (int) a2 * (*intersection_pt)->x + b2;
        }
        if
}

SDL_Point *is_ray_intersect_edge(SDL_edge edge, SDL_ray ray)
{

        //edge points
        int x1 = edge.vertex_1.x;
        int y1 = edge.vertex_1.y;
        int x2 = edge.vertex_2.x;
        int y2 = edge.vertex_2.y;

        //ray points
        int x3 = ray.start_point.x;
        int y3 = ray.start_point.y;
        int x4 = ray.end_point.x;
        int y4 = ray.end_point.y;


        float a1 = (float) (y2-y1)/(x2-x1);
        float a2 = (float) (y4-y3)/(x4-x3);

        float b1 = (float) y1 - a1*x1;
        float b2 = (float) y3 - a2*x3;

        SDL_Point null_point = {0,0};
        SDL_Point *intersection = &null_point;
        get_intersection(&intersection,
                          x1, y1,
                          x2, y2,
                          x3, y3,
                          x4, y4,
                          a1, a2,
                          b1, b2);

        fprintf(stderr, "\ntesting edge [(%i,%i), (%i,%i)] and ray [(%i,%i), (%i,%i)]", x1, y1, x2, y2, x3, y3, x4, y4);

        fprintf(stderr, "\n coef EDGE : (%f,%f) // RAY : (%f, %f)\n", a1, b1, a2, b2);

        if (!intersection)
        {
                fprintf(stderr, "eeeeeeeee\n\n");
                return NULL;
        }

        //if the intersection point lies inside the edge
        if ((min(x1, x2)<= intersection->x)
                && (intersection->x <= max(x1, x2))
                && (min(y1, y2)<= intersection->y)
                && (intersection->y <= max(y1, y2))
                )
                {
                        fprintf(stderr, "------> YEEEEEEEEEEEEEEEEES intersection : (%i, %i)\n\n", intersection->x, intersection->y);
                        return intersection;
                }
        fprintf(stderr, "eeeeeeeee\n\n");
        return  NULL;


        /*
        float denom = (float) ((x1-x2)*(y3-y4) - (y1-y2)*(x3-x4));

        if (denom==0)
                return NULL;

        float t = (float) ((x1 - x3)*(y3-y4)- (y1-y3)*(x3-x4))/denom;

        float u = (float) ((x1-x3)*(y1-y2) - (y1-y3)*(x1-x2))/denom;

        if ((0 <= t && t <=1))
        {
                intersection->x = x1 + t*(x2-x1);
                intersection->y = y1 + t*(y2-y1);
                fprintf(stderr, "------> YEEEEEEEEEEEEEEEEES intersection : (");
                if (intersection->x)
                {
                        fprintf(stderr, "gngn");
                }
                return intersection;
        }
        return NULL;
         */

}

void add_rect_to_list(SDL_Event *event, SDL_Renderer *renderer, void *user_param)
{
        SDL_Rect new_rect           = {mouse_pos.x - rect_size/2, mouse_pos.y - rect_size/2, rect_size, rect_size};
        SDL_linked_drawing **first_drawing     = &chain_drawings;
        SDL_linked_drawing *drawing_to_add     = malloc(sizeof(SDL_linked_drawing));
        drawing_to_add->drawing.rect = new_rect;
        drawing_to_add->next        = chain_drawings;
        *first_drawing = drawing_to_add;

}

void add_edge_to_list(SDL_Event *event, SDL_Renderer *renderer, void *user_param)
{
        SDL_linked_drawing **first_drawing = &chain_drawings;
        SDL_linked_drawing  *drawing_to_add = malloc(sizeof(SDL_linked_drawing));
        drawing_to_add->drawing.edge = * ((SDL_edge *)user_param);
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