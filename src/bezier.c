//
// Created by helen on 22/09/22.
//
#include <SDL2/SDL.h>
#include "bezier.h"
#include <math.h>

static const float step = 0.05;
const int nb_values = (int) 1/step + 1;
SDL_FPoint *control_points = NULL;
SDL_linked_FPoint *bezier_smoothed_values = NULL;

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

static void compute_bezier_point(float time, SDL_FPoint *result)
{
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
                bezier_values[k].point.x = 0;
                bezier_values[k].point.y = 0;
                compute_bezier_point(t_k, &bezier_values[k].point);
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
        {
                fprintf(stderr, "\nbinary search r1");
                return (float) target_index;
        }
        else //linear interpolation between two points
        {
                float length_before = bezier_not_smoothed[target_index].distance;
                float length_after = bezier_not_smoothed[target_index+1].distance;
                float segment = length_after - length_before;

                float fraction_segment = (target_distance - length_before)/segment; //where does the new point stand on the segment

                fprintf(stderr, "\nbinary search r1");
                return (float) target_index + fraction_segment;
        }
}

static SDL_FPoint new_point_equidistant(SDL_bezier_FPoint *bezier_not_smoothed, float point_distance, SDL_FPoint result_point)
{
        //binary search followed by linear interpolation to find the index t so that the arc length in P(t) = point_distance
        float t = larger_dist_smaller(bezier_not_smoothed, point_distance) / ((float) nb_values - 1);
        compute_bezier_point( t, &result_point);
}

static void add_point_to_list(SDL_FPoint point)
{
        SDL_linked_FPoint *first_point = bezier_smoothed_values;
        SDL_linked_FPoint *new_point = malloc(sizeof(SDL_linked_FPoint));
        new_point->point = point;
        new_point->next = bezier_smoothed_values;
        bezier_smoothed_values = new_point;
}


void smooth_bezier_curve(SDL_Renderer *renderer)
{
        SDL_bezier_FPoint bezier_values[nb_values];

        /*fetching P(t_k) where t_k is in the uniform subdivision of [0,1]
         * and the distance² from the start point for each P(t).*/
        cubic_bezier_curve(bezier_values);

        //target arc length of the curve given percentage parameter
        float length_curve = bezier_values[nb_values-1].distance;
        float delta_arc = length_curve*(1/ ((float)nb_values));
        int counter = 1;
        add_point_to_list(bezier_values[0].point);

        while (counter < nb_values)
        {
                /*
                new_point_equidistant(bezier_values,
                                      delta_arc*counter,
                                      bezier_smoothed_values[counter]);
                counter++;*/
                add_point_to_list(bezier_values[counter].point);
                counter++;
        }

        add_point_to_list(bezier_values[nb_values-1].point);
}

SDL_FPoint *create_tab_control_points()
{
        control_points = calloc(4, sizeof(SDL_FPoint));
}

void free_curves()
{
        free(control_points);
        SDL_linked_FPoint *current_point = bezier_smoothed_values;
        while (current_point)
        {
                SDL_linked_FPoint *point_to_free = current_point;
                current_point = current_point->next;
                free(point_to_free);
        }
        bezier_smoothed_values = NULL;
}






