#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include "events.h"

/*--------------------------------------------*/

bool QUIT         = false;

/*---------------------------------------------*/



void update_window(SDL_Renderer *rend)
{
    while (!QUIT)
    {
        SDL_Event current_event;
        while (SDL_PollEvent(&current_event))
            handle_event(current_event, rend);
        SDL_RenderPresent(rend);

    }
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
    bind_event(SDL_KEYDOWN, quit_callback, NULL);

    //window update loop
    update_window(renderer);


    if ((!window)|(!renderer)) {
        SDL_DestroyWindow(window);
        SDL_Quit();
        return EXIT_FAILURE;
    }

    unbind_event(SDL_MOUSEBUTTONDOWN, push_button_callback, NULL);
    unbind_event(SDL_KEYDOWN, quit_callback, NULL);

    SDL_DestroyWindow(window);
    SDL_Quit();
    return EXIT_SUCCESS;
}