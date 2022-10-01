//
// Created by helen on 22/09/22.
//
#include <SDL2/SDL.h>
#include "bezier.h"
#include <math.h>

static const float step = 0.01;
const int nb_values = (int) 1/step + 1;
SDL_FPoint *control_points = NULL;
SDL_linked_tab *splines = NULL;
int nb_control_points = 0;
int nb_beziers = 0;

static int factorial(int n)
{
        if (n==0)
                return 1;
        return n* factorial(n-1);
}

static inline int binomial_coef(int k, int n)
{
        return (factorial(n))/(factorial(n-k) * factorial(k));
}

static inline float bernstein_polynomial(float t_k, int i, int n)
{
        return binomial_coef(i, n) * powf(t_k, (float) i) * pow(1-t_k, (float) n-i) ;
}

inline float distance(SDL_FPoint *pt1, SDL_FPoint *pt2)
{
        return (pt1->x-pt2->x)*(pt1->x-pt2->x) + (pt1->y - pt2->y)*(pt1->y - pt2->y);
}

void compute_bezier_point(float time, SDL_FPoint *result, SDL_FPoint *control_points)
{
        result->x = 0;
        result->y = 0;
        for (uint8_t i=0; i<4; i++)
        {
                result->x += bernstein_polynomial(time, i, 3)*control_points[i].x;
                result->y += bernstein_polynomial(time, i,3)*control_points[i].y;
        }
}

void cubic_bezier_curve(SDL_bezier_FPoint *bezier_values)
{
        // control_points : array of 4 points used to calculate cubic Bézier curve

        //each control point is added to the head of the chain

        //uniform subdivision of [0,1] in which we compute the Bézier polynomial curve
        float t_0 = 0;
        float t_n = 1;
        int k = 0;
        float t_k = t_0;

        float distance_from_start = 0;

        while(k < nb_values)
        {
                compute_bezier_point(t_k, &bezier_values[k].point, control_points);
                if (k!=0)
                        distance_from_start += sqrtf(distance(&bezier_values[k].point, &bezier_values[k-1].point));
                bezier_values[k].distance = distance_from_start;
                t_k = t_k + step;
                k++;
        }
}
static float larger_dist_smaller(SDL_bezier_FPoint *bezier_not_smoothed, float target_distance)
{
        //bezier_not_smoothed : contains an array of sorted distances
        float index_high = nb_values - 1;
        float index_low = 0;
        int target_index = (index_low + index_high)/2;
        while (!(bezier_not_smoothed[target_index].distance<= target_distance
        && bezier_not_smoothed[target_index+1].distance>target_distance))
        {
                if (bezier_not_smoothed[target_index].distance < target_distance) // right side
                        index_low = target_index +1;
                else if (bezier_not_smoothed[target_index].distance > target_distance)
                        index_high = target_index -1;
                target_index = (index_low + index_high)/2;
        }
        if (bezier_not_smoothed[target_index].distance == target_distance )
                return (float) target_index;
        else //linear interpolation between two points
        {
                float length_before = bezier_not_smoothed[target_index].distance;
                float length_after = bezier_not_smoothed[target_index+1].distance;
                float segment = length_after - length_before;

                float fraction_segment = (target_distance - length_before)/segment; //where does the new point stand on the segment
                return (float) target_index + fraction_segment;
        }
}

static SDL_FPoint new_point_equidistant(SDL_bezier_FPoint *bezier_not_smoothed, float point_distance)
{
        SDL_FPoint result_point={0,0};
        //binary search followed by linear interpolation to find the index t so that the arc length in P(t) = point_distance
        float t = larger_dist_smaller(bezier_not_smoothed, point_distance) / ((float) nb_values - 1);
        compute_bezier_point( t, &result_point, control_points);
        return result_point;
}


void smooth_bezier_curve(SDL_Renderer *renderer, SDL_linked_tab *new_bezier_tab)
{
        SDL_bezier_FPoint bezier_values[nb_values];

        /*fetching P(t_k) where t_k is in the uniform subdivision of [0,1]
         * and the distance² from the start point for each P(t).*/
        cubic_bezier_curve(bezier_values);

       //target arc length of the curve given percentage parameter
        float length_curve = bezier_values[nb_values-1].distance;
        float delta_arc = length_curve*(1/ ((float)nb_values));
        int counter = 1;
        (new_bezier_tab->tab)[0] = bezier_values[0].point;
        while (counter < nb_values)
        {
                (new_bezier_tab->tab)[counter] = new_point_equidistant(bezier_values,
                                      delta_arc*counter);
                counter++;
        }

        (new_bezier_tab->tab)[nb_values-1]=bezier_values[nb_values-1].point;
}

void add_tab_to_spline(SDL_linked_tab *new_bezier_tab)
{
        SDL_linked_tab **first_tab = &splines;
        new_bezier_tab->id = nb_beziers;
        new_bezier_tab->next = splines;
        *first_tab = new_bezier_tab;
        nb_beziers++;
}
void add_final_control_points(SDL_linked_tab *bezier_tab) {
        for (uint8_t i=0; i<4; i++)
                bezier_tab->control_pts[i] = control_points[i];
}


SDL_linked_tab *create_bezier_tab()
{
        SDL_linked_tab *bezier_tab = malloc(sizeof(SDL_linked_tab));
        bezier_tab->tab = calloc(nb_values, sizeof(SDL_FPoint));
        bezier_tab->control_pts = calloc(4, sizeof(SDL_FPoint));
        bezier_tab->next = NULL;
        return bezier_tab;
}
void create_tab_control_points()
{
        control_points = calloc(4, sizeof(SDL_FPoint));
}



void free_curves()
{
        //cleaning control_points
        free(control_points);

        SDL_linked_FPoint  *current_point = chain_control_points;
        while (current_point)
        {
                SDL_linked_FPoint *point_to_free = current_point;
                current_point = current_point->next;
                free(point_to_free);
        }
        chain_control_points = NULL;
        nb_control_points = 0;

        //cleaning splines
        SDL_linked_tab *current_bezier = splines;
        while (current_bezier)
        {
                SDL_linked_tab *bezier_to_free = current_bezier;
                current_bezier = current_bezier->next;
                free(bezier_to_free->tab);
                free(bezier_to_free->control_pts);
                free(bezier_to_free);
        }
        splines=NULL;

}






