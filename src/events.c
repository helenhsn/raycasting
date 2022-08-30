//
// Created by helen on 21/08/22.
//
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include "events.h"
#include "geometry.h"
#include "render.h"


static Uint32 button_pressed;
static SDL_linked_event *chain_events           = NULL;
bool first_button_down                          = true;
static SDL_Point start_point;
static SDL_Point end_point;
SDL_Scancode drawing_type = SDL_SCANCODE_E;

void free_chain_events()
{
        SDL_linked_event *current_event = chain_events;
        while (current_event)
        {
                SDL_linked_event  **ptr_event_to_free = &current_event;
                current_event = current_event->next;
                free(*ptr_event_to_free);
                *ptr_event_to_free = NULL;
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
           &&(current_event->next->type != event_type)
           &&(current_event->next->callback != callback)
           &&(current_event->next->param != param))
    {
        current_event = current_event->next;
    }
    if (current_event->next)
    {
            SDL_linked_event *event_to_free = current_event->next;
            current_event->next = event_to_free->next;

            SDL_linked_event **ptr_event_free = &event_to_free;
            free(*ptr_event_free);
            *ptr_event_free = NULL;
    }

    chain_events = sentinel_event.next;


}
void callback_release(SDL_Event *event, SDL_Renderer *renderer, void *param)
{
    if ( (button_pressed & SDL_BUTTON_LMASK) != 0)
    {
            if (drawing_type == SDL_SCANCODE_R)
                    unbind_event(SDL_MOUSEMOTION, add_rect_to_list, NULL);
            unbind_event(SDL_MOUSEBUTTONUP, callback_release, NULL);
    }
}

void push_button_callback(SDL_Event *event, SDL_Renderer *renderer, void *param)
{

    button_pressed = SDL_GetMouseState(&mouse_pos.x, &mouse_pos.y);
    SDL_Log("Mouse button is pressed at %u %u", mouse_pos.x, mouse_pos.y);

    if ( (button_pressed & SDL_BUTTON_LMASK) != 0 )
    {
            if (drawing_type == SDL_SCANCODE_R) //rects = eraser
            {
                    fprintf(stderr, "ok1");
                    add_rect_to_list(event, renderer, param);
                    bind_event(SDL_MOUSEMOTION, add_rect_to_list, NULL);
            }
            else //edges = drawing
            {
                if (first_button_down)
                {
                        start_point = mouse_pos;
                        first_button_down = false;
                }
                else
                {
                        end_point = mouse_pos;
                        first_button_down = true;
                        SDL_edge edge_to_draw = {start_point, end_point};
                        add_edge_to_list(event, renderer, &edge_to_draw);
                }
            }
    }
    bind_event(SDL_MOUSEBUTTONUP, callback_release, NULL);
}


void keydown_callback(SDL_Event *event, SDL_Renderer *renderer, void *param)
{
    SDL_Scancode key_pressed = (*event).key.keysym.scancode;
    if (key_pressed == SDL_SCANCODE_ESCAPE)
        QUIT = true;

    else if (key_pressed == SDL_SCANCODE_L) // L : casts rays (light!)
    {
            if (!CASTING_RAYS) //first time key pressed : turn on lights
            {
                    CASTING_RAYS = true;
                    init_rays();
                    render_rays(NULL, renderer, NULL);
                    bind_event(SDL_MOUSEMOTION, render_rays, NULL);
            }
            else //second time key pressed : turn off lights
            {
                    CASTING_RAYS = false;
                    unbind_event(SDL_MOUSEMOTION, render_rays, NULL);
            }
    }

    else if (key_pressed == SDL_SCANCODE_C)// C : clears drawings
    {
        free_chain_drawings();
        //init_drawing
        fprintf(stderr, "\n pbbb  ??? \n");
        return;
    }

    else if (key_pressed != drawing_type) // E : edges / R : rects :-)
    {
            free_chain_drawings();
            fprintf(stderr, "\nkey selected : %i (8 : E / 7 : D / 6 : C)", key_pressed);
            drawing_type = key_pressed;
    }

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