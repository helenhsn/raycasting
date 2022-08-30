//
// Created by helen on 21/08/22.
//

#ifndef RAYCASTING_EVENTS_H
#define RAYCASTING_EVENTS_H

#include <stdbool.h>
#include <SDL2/SDL.h>


/*
 * Structures used for events handling in the main file
 * */

typedef void (*SDL_callback) (SDL_Event *event, SDL_Renderer *renderer, void* param);

typedef struct SDL_linked_event
{
    SDL_EventType type;
    SDL_callback callback;
    void *param;
    struct SDL_linked_event *next;

} SDL_linked_event;

/*
 * Global variables used for events handling in the main file
 * */

extern bool QUIT;

extern bool first_button_down;

/*
 * Functions & callbacks used for events handling
 * */

extern void bind_event(SDL_EventType event_type, SDL_callback callback, void *param);

extern void unbind_event(SDL_EventType event_type, SDL_callback callback, void *param);

extern void callback_release(SDL_Event *event, SDL_Renderer *renderer, void *param);

extern void push_button_callback(SDL_Event *event, SDL_Renderer *renderer, void *param);

extern void free_chain_events();

extern void keydown_callback(SDL_Event *event, SDL_Renderer *renderer, void *param);

extern void handle_event(SDL_Event event, SDL_Renderer *rend);

#endif //RAYCASTING_EVENTS_H
