//
// Created by helen on 21/08/22.
//
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include "events.h"
#include "geometry.h"

static Uint32 button_pressed;
static SDL_linked_event *chain_events           = NULL;
bool first_button_down                          = true;
static SDL_Point start_point;
static SDL_Point end_point;
SDL_Scancode drawing_type = SDL_SCANCODE_D;

static void free_chain_events()
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


void app_free()
{
        free_chain_events();
        free_chain_drawings();
}

void app_quit()
{
        QUIT=true;
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
void callback_release(SDL_Event event, SDL_Renderer *renderer, void *param)
{
    if ( (button_pressed & SDL_BUTTON_LMASK) != 0)
    {
            if (drawing_type == SDL_SCANCODE_E)
                    unbind_event(SDL_MOUSEMOTION, add_rect_to_list, NULL);
            unbind_event(SDL_MOUSEBUTTONUP, callback_release, NULL);
    }
}

void push_button_callback(SDL_Event event, SDL_Renderer *renderer, void *param)
{

    button_pressed = SDL_GetMouseState(&mouse_pos.x, &mouse_pos.y);
    SDL_Log("Mouse button is pressed at %u %u", mouse_pos.x, mouse_pos.y);

    if ( (button_pressed & SDL_BUTTON_LMASK) != 0 )
    {
            if (drawing_type == SDL_SCANCODE_E) //rects = eraser
            {
                    fprintf(stderr, "ok1");
                    add_rect_to_list(event, renderer, param);
                    bind_event(SDL_MOUSEMOTION, add_rect_to_list, NULL);
            }
            else //edges = drawing
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
    bind_event(SDL_MOUSEBUTTONUP, callback_release, NULL);
}

static void change_color(SDL_Renderer *renderer, Uint8 r, Uint8 g, Uint8 b,
                   Uint8 a)
{
        free_chain_drawings();
        SDL_SetRenderDrawColor(renderer, r, g, b, a);
}

void keydown_callback(SDL_Event event, SDL_Renderer *renderer, void *param)
{
    SDL_Scancode key_pressed = event.key.keysym.scancode;
    if (key_pressed == SDL_SCANCODE_ESCAPE)
        app_quit();

    if (key_pressed == SDL_SCANCODE_C) // C : casts rays
            CASTING_RAYS = true;

    if (key_pressed != drawing_type) // D : draws / E : erase :-)
    {
            if ((key_pressed == SDL_SCANCODE_D)&&(drawing_type == SDL_SCANCODE_E))
                    change_color(renderer, 255, 0, 0, 255);
            if ((key_pressed == SDL_SCANCODE_E)&&(drawing_type==SDL_SCANCODE_D)) //we don't want both types of drawings on the screen
                    change_color(renderer, 0, 0, 0, 255);
            fprintf(stderr, "\nkey selected : %i (8 : E / 7 : D)", key_pressed);
            drawing_type = key_pressed;
    }

}

void handle_event(SDL_Event event, SDL_Renderer *rend)
{
        SDL_linked_event  *current_linked_event = chain_events;
        while ((current_linked_event)&&(!QUIT))
        {
                if (current_linked_event->type == event.type)
                {
                        if (current_linked_event->type == SDL_MOUSEBUTTONDOWN)
                                fprintf(stderr, "\nbuttondown in handle event");
                        current_linked_event->callback(event, rend, current_linked_event->param);
                }

        current_linked_event = current_linked_event->next;
        }

}