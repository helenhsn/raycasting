//
// Created by helen on 21/08/22.
//
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include "events.h"
#include "geometry.h"

static SDL_linked_event *chain_events = NULL;

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
    if (((int) param & SDL_BUTTON_LMASK) != 0)
        unbind_event(SDL_MOUSEMOTION, add_rect_to_list, NULL);
    unbind_event(SDL_MOUSEBUTTONUP, callback_release, param);
}

void push_button_callback(SDL_Event event, SDL_Renderer *renderer, void *param)
{

    Uint32 button_pressed = SDL_GetMouseState(&mouse_pos.x, &mouse_pos.y);
    SDL_Log("Mouse button is pressed at %u %u", mouse_pos.x, mouse_pos.y);

    if ( (button_pressed & SDL_BUTTON_LMASK) != 0 )
        add_rect_to_list(event, renderer, param);
    bind_event(SDL_MOUSEMOTION, add_rect_to_list, NULL);

    bind_event(SDL_MOUSEBUTTONUP, callback_release, button_pressed);
}

void quit_callback(SDL_Event event, SDL_Renderer *renderer, void *param)
{
    if (event.key.keysym.sym == SDLK_ESCAPE)
        QUIT = true;
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