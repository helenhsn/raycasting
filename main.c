#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL2/SDL.h>


/* struct to compute the collision between
 * the rays and edges (squares) */
typedef struct SDL_line
{
    SDL_Point point;
    float direction;
} SDL_line ;

typedef struct SDL_edge
{
    SDL_Point vertex_1;
    SDL_Point vertex_2;
} SDL_edge;

/*
 * Linked chain of rectangles to display on the screen
 * */
typedef struct SDL_linked_Rect
{
    SDL_Rect rect;
    struct SDL_linked_Rect *next;
} SDL_linked_Rect;

/*
 * Elements for events handling in the main loop
 * */

typedef bool (*SDL_callback) (SDL_Event event, SDL_Renderer *renderer, void* param);

typedef struct SDL_linked_event
{
    SDL_EventType type;
    SDL_callback callback;
    void *param;
    struct SDL_linked_event *next;

}SDL_linked_event;

/*--------------------------------------------*/

static SDL_Point mouse_pos;
static int rect_size    = 16;
static int QUIT         = false;
static SDL_linked_Rect *chain_rects = NULL;
static SDL_linked_event *chain_events = NULL;

/*---------------------------------------------*/

void draw_chain_rects(SDL_Renderer *rend)
{
    SDL_linked_Rect *current_rect = chain_rects;
    while (current_rect)
    {
        SDL_RenderDrawRect(rend, &current_rect->rect);
        current_rect = current_rect->next;
    }
    SDL_RenderPresent(rend);
}

void add_rect_to_list(SDL_Event event, SDL_Renderer *renderer, void *user_param)
{
    SDL_GetMouseState(&mouse_pos.x, &mouse_pos.y); //update mouse coordinates
    SDL_Rect new_rect = {mouse_pos.x, mouse_pos.y, rect_size, rect_size};
    SDL_linked_Rect **first_rect_ad = &chain_rects;
    SDL_linked_Rect *rect_to_add = malloc(sizeof(SDL_linked_Rect));
    rect_to_add->rect = new_rect;
    rect_to_add->next = chain_rects;
    *first_rect_ad = rect_to_add;

    draw_chain_rects(renderer);
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

void update_window(SDL_Renderer *rend)
{
    while (!QUIT)
    {
        SDL_Event current_event;
        SDL_WaitEvent(&current_event);
        handle_event(current_event, rend);
    }
}

void app_quit(void)
{
    QUIT = true;
}

int main()
{
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return EXIT_FAILURE;
    }
    printf("Hello, World!\n");

    //creating window
    char *title = "Raycasting project";
    SDL_Window *window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED, 800,
                                          800, 0);
    //renderer for the window
    uint32_t  render_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
    SDL_Renderer  *renderer = SDL_CreateRenderer(window, -1, render_flags);


    SDL_RenderClear(renderer); //clear window

    //color of the rects
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);

    //bind initial events
    bind_event(SDL_MOUSEBUTTONDOWN, push_button_callback, NULL);
    //window update loop
    update_window(renderer);


    if ((!window)|(!renderer)) {
        SDL_DestroyWindow(window);
        SDL_Quit();
        return EXIT_FAILURE;
    }

    unbind_event(SDL_MOUSEBUTTONDOWN, push_button_callback, NULL);
    //wait till shutdown
    SDL_Delay(30000);

    SDL_DestroyWindow(window);
    SDL_Quit();
    return EXIT_SUCCESS;
}