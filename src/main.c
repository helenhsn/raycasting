#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include "events.h"
#include "geometry.h"
#include "render.h"

/*--------------------------------------------*/

bool QUIT         = false;

/*---------------------------------------------*/


void update_window(SDL_Renderer *rend)
{
        while (!QUIT)
        {
                //clearing renderer
                SDL_SetRenderDrawColor(rend, 0, 2, 0, 255);
                SDL_RenderClear(rend);

                //events handling
                SDL_Event current_event;
                SDL_GetMouseState(&mouse_pos.x, &mouse_pos.y);
                while (SDL_PollEvent(&current_event))
                        handle_event(current_event, rend);

                //rendering
                render_objects(rend);
                SDL_RenderPresent(rend);

                //waiting before repeating (or not)

        }
}

static void app_free()
{
        free_chain_events();
        free_chain_drawings();
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
                                          SDL_WINDOWPOS_CENTERED, w_width,
                                          w_height, 0);
        //renderer for the window
        uint32_t  render_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
        SDL_Renderer  *renderer = SDL_CreateRenderer(window, -1, render_flags);


        SDL_RenderClear(renderer); //clear window

        //bind initial events
        bind_event(SDL_MOUSEBUTTONDOWN, push_button_callback, NULL);
        bind_event(SDL_KEYDOWN, keydown_callback, NULL);

        //initialize drawing chain with the window's edges
        init_drawing(renderer);

        //window update loop
        update_window(renderer);


        if ((!window)|(!renderer)) {
                SDL_DestroyWindow(window);
                SDL_Quit();
                return EXIT_FAILURE;
        }

        unbind_event(SDL_MOUSEBUTTONDOWN, push_button_callback, NULL);
        unbind_event(SDL_KEYDOWN, keydown_callback, NULL);

        app_free();
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return EXIT_SUCCESS;
}