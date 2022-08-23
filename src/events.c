//
// Created by helen on 21/08/22.
//
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include "events.h"
#include "geometry.h"

static SDL_linked_event *chain_events           = NULL;
bool first_button_down                          = true;
static SDL_Point start_point;
static SDL_Point end_point;
SDL_Scancode drawing_type = SDL_SCANCODE_R;

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

    if (!current_event->next) //if the chain is empty
        return;

    while ((current_event->next)
           &&(current_event->next->type != event_type)
           &&(current_event->next->callback != callback)
           &&(current_event->next->param != param))
    {
        current_event = current_event->next;
    }
    SDL_linked_event *event_to_free = current_event->next;
    current_event->next = event_to_free->next;

    SDL_linked_event **ptr_event_free = &event_to_free;
    free(*ptr_event_free);
    *ptr_event_free = NULL;

    chain_events = sentinel_event.next;


}
void callback_release(SDL_Event event, SDL_Renderer *renderer, void *param)
{
    if (((*(int *) param) & SDL_BUTTON_LMASK) != 0)
    {
            if (drawing_type == SDL_SCANCODE_R)
                    unbind_event(SDL_MOUSEMOTION, add_rect_to_list, NULL);
            else
            {
                    if (!first_button_down)
                    {
                            unbind_event(SDL_MOUSEBUTTONDOWN, add_edge_to_list, &start_point);
                    }
            }
            unbind_event(SDL_MOUSEBUTTONUP, callback_release, param);
    }
}

void push_button_callback(SDL_Event event, SDL_Renderer *renderer, void *param)
{

    Uint32 button_pressed = SDL_GetMouseState(&mouse_pos.x, &mouse_pos.y);
    SDL_Log("Mouse button is pressed at %u %u", mouse_pos.x, mouse_pos.y);

    if ( (button_pressed & SDL_BUTTON_LMASK) != 0 )
    {
            if (drawing_type == SDL_SCANCODE_R) //current drawing = rect
            {
                    fprintf(stderr, "ok1");
                    add_rect_to_list(event, renderer, param);
                    bind_event(SDL_MOUSEMOTION, add_rect_to_list, NULL);
            }
            else //current drawing = line
            {
                if (first_button_down)
                {
                        fprintf(stderr, "ok2");
                        start_point = mouse_pos;
                        first_button_down = false;
                }
                else
                {
                        fprintf(stderr, "ok3");
                        end_point = mouse_pos;
                        first_button_down = true;
                        SDL_edge edge_to_draw = {start_point, end_point};
                        add_edge_to_list(event, renderer, &edge_to_draw);
                }
            }

    }

    bind_event(SDL_MOUSEBUTTONUP, callback_release, &button_pressed);
}

void keydown_callback(SDL_Event event, SDL_Renderer *renderer, void *param)
{
    SDL_Scancode key_pressed = event.key.keysym.scancode;
    if (key_pressed == SDL_SCANCODE_ESCAPE)
        QUIT = true;
    if (key_pressed == SDL_SCANCODE_R || key_pressed == SDL_SCANCODE_L)
    {
            fprintf(stderr, "\nkey selected : %i (15 : L / 21 : R)", key_pressed);
            drawing_type = key_pressed;
    }
}

void handle_event(SDL_Event event, SDL_Renderer *rend)
{
    SDL_linked_event  *current_linked_event = chain_events;
    while (current_linked_event)
    {
        if (current_linked_event->type == event.type)
            current_linked_event->callback(event, rend, current_linked_event->param);

        current_linked_event = current_linked_event->next;
    }

}