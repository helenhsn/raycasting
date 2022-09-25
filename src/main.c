#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "events.h"
#include "geometry.h"
#include "commands.h"
#include "render.h"
#include "bezier.h"

/*--------------------------------------------*/

bool QUIT         = false;

/*---------------------------------------------*/


static void init_rendering(SDL_Window **window,
                           SDL_Renderer **renderer)
{
        uint32_t  render_flags = SDL_RENDERER_ACCELERATED
                                | SDL_RENDERER_PRESENTVSYNC
                                | SDL_RENDERER_TARGETTEXTURE;



        //CREATING MAIN WINDOW
        char *title_project = "Raycasting project";
        *window = SDL_CreateWindow(title_project, SDL_WINDOWPOS_CENTERED,
                                              SDL_WINDOWPOS_CENTERED, w_width,
                                              w_height, 0);
        *renderer = SDL_CreateRenderer(*window, -1, render_flags);

        if ((!*window) | (!*renderer))
                QUIT = true;

        //clearing windows
        SDL_RenderClear(*renderer);
}


static void update_window(SDL_Renderer *rend)
{
        uint32_t FPS = 60;
        uint32_t start_time;
        while (!QUIT)
        {
                start_time = SDL_GetTicks();
                //clearing renderer
                SDL_SetRenderDrawColor(rend, 0, 2, 0, 255);
                SDL_RenderClear(rend);

                SDL_Point int_mouse_pos;
                SDL_GetMouseState(&int_mouse_pos.x, &int_mouse_pos.y);
                mouse_pos.x = (float) int_mouse_pos.x;
                mouse_pos.y = (float) int_mouse_pos.y;

                //events handling
                SDL_Event current_event;
                while (SDL_PollEvent(&current_event)) {
                        handle_event(current_event, rend);
                }

                //rendering main window
                render_objects(rend);
                render_text(rend);
                render_curves(rend);
                SDL_RenderPresent(rend);

                //limiting to 60fps
                if (1000/60 > SDL_GetTicks() - start_time)
                        SDL_Delay(1000/60 - (SDL_GetTicks() - start_time));
        }
}

static void app_free()
{
        free_chain_events();
        free_chain_drawings();
        free_curves();
        free_panel();
}

static void app_quit(SDL_Renderer *rend,
                     SDL_Window *window) {
        app_free();
        TTF_CloseFont(font_title);
        TTF_CloseFont(font_buttons);
        TTF_Quit();
        SDL_DestroyRenderer(rend);
        SDL_DestroyWindow(window);
        SDL_Quit();

}

int main()
{
        if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
        {
                SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
                return EXIT_FAILURE;
        }
        printf("Hello, World!\n");

        //creating renderers
        SDL_Renderer *renderer;
        SDL_Window *window;

        init_rendering(&window, &renderer);
        init_rays();
        init_command_panel(renderer);

        //bind initial events
        bind_event(SDL_MOUSEBUTTONDOWN, button_panel_callback, NULL);
        bind_event(SDL_KEYDOWN, quit_callback, NULL);

        //initialize drawing chain with the window's edges
        init_drawing(renderer);

        //window update loop
        update_window(renderer);


        unbind_event(SDL_MOUSEBUTTONDOWN, button_panel_callback, NULL);
        unbind_event(SDL_KEYDOWN, quit_callback, NULL);


        app_quit(renderer, window);
        return EXIT_SUCCESS;
}