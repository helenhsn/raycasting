//
// Created by helen on 29/09/22.
//
#include <SDL2/SDL.h>
#include "geometry.h"
#include "bezier.h"
#include <stdbool.h>
#include <gsl/gsl_poly.h>
#include "render.h"

static void get_intersection(SDL_FPoint *intersection_pt,
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
                (intersection_pt)->x = x1;
                (intersection_pt)->y = x1 * a2 + b2 ;

        }
        else if (x3==x4) //vertical ray
        {
                (intersection_pt)->x = x3;
                (intersection_pt)->y = x3 * a1 + b1 ;
        }

        else //non verticals edge & rays
        {
                (intersection_pt)->x = (b1 - b2)/(a2 - a1);
                (intersection_pt)->y =  a2 * (intersection_pt)->x + b2;
        }

        //check if the intersection is in the ray's direction
        SDL_FPoint start_vector;
        start_vector.x = (intersection_pt)->x - x1;
        start_vector.y = (intersection_pt)->y - y1;

        SDL_FPoint end_vector;
        end_vector.x = (intersection_pt)->x - x2;
        end_vector.y = (intersection_pt)->y - y2;

        SDL_FPoint ray_vector;
        ray_vector.x = x4 - x3;
        ray_vector.y = y4 - y3;

        SDL_FPoint intersect_vector;
        intersect_vector.x = (intersection_pt)->x - x3;
        intersect_vector.y = (intersection_pt)->y - y3;


        if (dot_product(start_vector, end_vector) >= 0
            || dot_product(intersect_vector, ray_vector) <= 0)
        {
                (intersection_pt)->x = -1;
                (intersection_pt)->y = -1;
        }



}

static bool is_ray_intersect_edge(SDL_FPoint *intersection, SDL_edge edge, SDL_ray ray)
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


        get_intersection(intersection,
                         x1, y1,
                         x2, y2,
                         x3, y3,
                         x4, y4);

        if ((intersection->x<0) && (intersection->y<0))
                return false;
        return true;
}

void draw_intersection_ray_edges(SDL_Renderer *renderer, SDL_ray ray, SDL_FPoint *last_endpoint)
{
        SDL_FPoint intersect_pt = {-2, -2};
        SDL_linked_drawing *current_edge = chain_drawings;

        while (current_edge)
        {
                if (is_ray_intersect_edge(&intersect_pt, current_edge->drawing.edge, ray))
                {
                        if ( (last_endpoint->x == 50 && last_endpoint->y == 50) || distance(&mouse_pos, &intersect_pt) < distance(&mouse_pos, last_endpoint))
                        {
                                last_endpoint->x = intersect_pt.x;
                                last_endpoint->y = intersect_pt.y;
                        }
                }

                current_edge = current_edge->next;
        }
}
static SDL_FPoint get_closest_intersect_point(SDL_FPoint *intersections, int nb_intersections, SDL_FPoint *start_point)
{
        float min_dist = 4*w_width*w_width;
        SDL_FPoint intersect_pt;
        for(uint8_t i=0; i<nb_intersections; i++)
        {
                float new_dist = distance(&intersections[i], start_point);
                if (new_dist < min_dist)
                {
                        min_dist = new_dist;
                        intersect_pt = intersections[i];
                }
        }
        return intersect_pt;

}

static bool same_direction_ray(SDL_ray ray, SDL_FPoint intersection_pt)
{
        SDL_FPoint ray_vector;
        ray_vector.x = ray.end_point.x - ray.start_point.x;
        ray_vector.y = ray.end_point.y - ray.start_point.y;

        SDL_FPoint intersect_vector;
        intersect_vector.x = intersection_pt.x - ray.start_point.x;
        intersect_vector.y = intersection_pt.y - ray.start_point.y;

        if (dot_product(intersect_vector, ray_vector) > 0)
                return true;
        return false;
}

static bool is_ray_intersect_curve(SDL_FPoint *intersection_list, SDL_FPoint *ctrl_pts, SDL_ray ray, int *nb_intersections)
{
        float pt1_x = ray.start_point.x;
        float pt1_y = ray.start_point.y;
        float pt2_x = ray.end_point.x;
        float pt2_y = ray.end_point.y;

        SDL_FPoint v = {pt2_y - pt1_y, pt1_x - pt2_x};

        float second_member = dot_product(ray.start_point, v);

        SDL_FPoint Q = {-ctrl_pts[0].x + 3*ctrl_pts[1].x - 3*ctrl_pts[2].x + ctrl_pts[3].x,
                        -ctrl_pts[0].y + 3*ctrl_pts[1].y - 3*ctrl_pts[2].y + ctrl_pts[3].y};
        SDL_FPoint R = {3*ctrl_pts[0].x -6*ctrl_pts[1].x +3*ctrl_pts[2].x,
                        3*ctrl_pts[0].y -6*ctrl_pts[1].y +3*ctrl_pts[2].y};
        SDL_FPoint S = {-3*ctrl_pts[0].x + 3*ctrl_pts[1].x,
                        -3*ctrl_pts[0].y + 3*ctrl_pts[1].y};

        float a = dot_product(Q, v);
        float b = dot_product(R, v)/a;
        float c = dot_product(S, v)/a;
        float d = (dot_product(ctrl_pts[0], v) - second_member)/a;

        double roots[3] = {0, 0, 0};
        gsl_poly_solve_cubic(b, c, d, &roots[0], &roots[1], &roots[2]);

        for (uint8_t i=0; i<3; i++)
        {
                if (roots[i] > 0 && roots[i] <1)
                {
                        SDL_FPoint temp_point;
                        compute_bezier_point((float) roots[i], &temp_point, ctrl_pts);
                        if (same_direction_ray(ray, temp_point))
                        {
                                intersection_list[*nb_intersections] = temp_point;
                                (*nb_intersections)++;
                        }
                }
        }
        if (*nb_intersections > 1)
                return true;
        return false;
}

void draw_intersection_ray_curve(SDL_Renderer *renderer, SDL_ray ray, SDL_FPoint *last_endpoint) {
        if (!splines)
                return;

        SDL_linked_tab *current_bezier = splines;
        while (current_bezier)
        {
                SDL_FPoint intersections_list[4]; //max 4 intersections
                intersections_list[0] = *last_endpoint;
                SDL_FPoint intersect_pt;
                int nb_intersections = 1;
                if (is_ray_intersect_curve(intersections_list, current_bezier->control_pts, ray, &nb_intersections))
                {
                        intersect_pt = get_closest_intersect_point(intersections_list, nb_intersections, &ray.start_point);
                        last_endpoint->x = intersect_pt.x;
                        last_endpoint->y = intersect_pt.y;
                }
                current_bezier = current_bezier->next;
        }
}