//
// Created by helen on 21/08/22.
//
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include "events.h"
#include "geometry.h"
#include "commands.h"
#include "render.h"
#include "bezier.h"



static SDL_button *current_button = NULL;
SDL_linked_event *chain_events    = NULL;
bool first_button_down            = true;
static int last_id;
static SDL_FPoint start_point;
SDL_FPoint switch_point;
SDL_Rect selector;

/*--------------- CALLBACKS ---------------------*/

static void cancel_current_button()
{
        current_button->sunken = false;
        unbind_event(SDL_MOUSEBUTTONDOWN, current_button->callback, current_button);
        if (current_button->id == 1)
                unbind_event(SDL_MOUSEMOTION, render_rays, NULL);
}

static void update_current_button(SDL_Event *event, SDL_Renderer *renderer, void *param, SDL_button *button)
{
        current_button = button;
        current_button->sunken = true;
        switch (button->id) {
                case 1: case 5:
                        if (last_id ==3)
                                free(control_points);
                        button->callback(event, renderer, button);
                        break;
                case 3:
                        nb_control_points = 0;
                        create_tab_control_points();
                        bind_event(SDL_MOUSEBUTTONDOWN, button->callback, button);
                        break;
                default:
                        if (last_id == 3)
                                free(control_points);
                        bind_event(SDL_MOUSEBUTTONDOWN, button->callback, button);
        }

        selector.x = button->rect.x + button->rect.w + 10;
        selector.y =  button->rect.y + button->rect.h/4 + 5;
        selector.w = 10;
        selector.h = 10;

}
void free_all_objects()
{
        free_chain_objects();
        free_curves();
}
void clear_callback(SDL_Event *event, SDL_Renderer *renderer, void *param)
{
        free_all_objects();
        init_drawing(renderer);
        init_rays();
        SDL_button *button = (SDL_button *)param;
        button->sunken = false;
        current_button = NULL;
}

static void update_control_points()
{
        switch_point = mouse_pos;
        SDL_FPoint computed_point = affine_ratio(2, control_points[3], -1, switch_point);
        control_points[2] = computed_point;
}

static void move_curve_callback(SDL_Event *event, SDL_Renderer *renderer, void *param)
{
        update_control_points();
        smooth_bezier_curve(renderer, (SDL_linked_tab *)param);
}

static void release_move_curve_callback(SDL_Event *event, SDL_Renderer *renderer, void *param)
{
        update_control_points();
        smooth_bezier_curve(renderer, (SDL_linked_tab *) param);
        add_final_control_points((SDL_linked_tab *) param);
        control_points[0] = control_points[3];
        control_points[1] = switch_point;
        unbind_event(SDL_MOUSEMOTION, move_curve_callback, param);
        unbind_event(SDL_MOUSEBUTTONUP, release_move_curve_callback, param);

}

void curves_callback(SDL_Event *event, SDL_Renderer *renderer, void *param)
{
        if (is_mouse_in_panel())
                return;
        control_points[nb_control_points] = mouse_pos;
        nb_control_points++;

        if (nb_control_points == 4) //end of the cubic bézier
        {
                control_points[3] = control_points[2];
                SDL_linked_tab *new_bezier = create_bezier_tab(); //creating a new bézier curve
                add_tab_to_spline(new_bezier); //adding the bézier to the others
                bind_event(SDL_MOUSEMOTION, move_curve_callback, new_bezier);
                bind_event(SDL_MOUSEBUTTONUP, release_move_curve_callback, new_bezier);
                nb_control_points = 2;
        }
        return;
}


void rects_callback(SDL_Event *event, SDL_Renderer *renderer, void *param)
{
        add_rect_to_list(event, renderer, param);
        bind_event(SDL_MOUSEMOTION, add_rect_to_list, NULL);
        bind_event(SDL_MOUSEBUTTONUP, callback_release, NULL);
}

void edges_callback(SDL_Event *event, SDL_Renderer *renderer, void *param)
{
        if (is_mouse_in_panel())
        {
                first_button_down = true;
                return;
        }
        if (first_button_down)
        {
                start_point = mouse_pos;
                first_button_down = false;
        }
        else
        {
                first_button_down = true;
                SDL_edge edge_to_draw = {start_point, mouse_pos};
                add_edge_to_list(event, renderer, &edge_to_draw);
        }
}

void rays_callback(SDL_Event *event, SDL_Renderer *renderer, void *param)
{
        bind_event(SDL_MOUSEMOTION, render_rays, NULL);
}

void callback_release(SDL_Event *event, SDL_Renderer *renderer, void *param)
{

        unbind_event(SDL_MOUSEMOTION, add_rect_to_list, NULL);
        unbind_event(SDL_MOUSEBUTTONUP, callback_release, NULL);
}


void button_panel_callback(SDL_Event *event, SDL_Renderer *renderer, void *param)
{
        if (!is_mouse_in_panel())
                return;

        SDL_text_frame *current_frame = chain_text;
        while (current_frame)
        {
                if (is_point_in_rect(current_frame->button.rect, mouse_pos) && current_frame->pushable)
                        break;
                current_frame = current_frame->next;
        }
        if (current_frame &&(!current_frame->button.sunken))
        {
                last_id = current_frame->button.id;
                if (!current_button)
                        update_current_button(event, renderer, param, &current_frame->button);

                else if (current_frame->button.id != current_button->id) //change drawings
                {
                        cancel_current_button();
                        update_current_button(event, renderer, param,&current_frame->button);
                }
        }
}


void quit_callback(SDL_Event *event, SDL_Renderer *renderer, void *param)
{
    if ((*event).window.event == SDL_WINDOWEVENT_CLOSE)
        QUIT = true;
}

/*-------------- EVENTS MANAGEMENT -------------------*/

void free_chain_events()
{
        SDL_linked_event *current_event = chain_events;
        while (current_event)
        {
                SDL_linked_event  *event_to_free = current_event;
                current_event = current_event->next;
                free(event_to_free);
        }
        chain_events = NULL;
}

void bind_event(SDL_EventType event_type, SDL_callback callback, void *param)
{
        SDL_linked_event *new_linked_event = malloc(sizeof(SDL_linked_event));
        new_linked_event->type = event_type;
        new_linked_event->callback = callback;
        new_linked_event->param = param;
        new_linked_event->next = chain_events;
        chain_events = new_linked_event;

}

void unbind_event(SDL_EventType event_type, SDL_callback callback, void *param)
{
        SDL_linked_event sentinel_event = {SDL_FIRSTEVENT, NULL, NULL, chain_events};
        SDL_linked_event *current_event = &sentinel_event;


        while ((current_event->next)
               &&
               !((current_event->next->type == event_type)
               && (current_event->next->callback == callback)
               && (current_event->next->param == param))
               )
        {
                current_event = current_event->next;
        }
        if (current_event->next)
        {
                SDL_linked_event *event_to_free = current_event->next;
                current_event->next = event_to_free->next;

                SDL_linked_event **ptr_event_to_free = &event_to_free;
                free(*ptr_event_to_free);
                *ptr_event_to_free = NULL;
        }
        chain_events = sentinel_event.next;
}

void handle_event(SDL_Event event, SDL_Renderer *rend)
{
        SDL_linked_event  *current_linked_event = chain_events;
        while ((current_linked_event)&&(!QUIT))
        {
                if (current_linked_event->type == event.type)
                        current_linked_event->callback(&event, rend, current_linked_event->param);
                current_linked_event = current_linked_event->next;
        }
}