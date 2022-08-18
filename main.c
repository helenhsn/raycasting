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

static int rect_size    = 16;
static int QUIT         = false;
static SDL_linked_Rect *chain_rects = NULL;

void add_rect_to_list(SDL_Rect rect)
{
    SDL_linked_Rect *rect_to_add = malloc(sizeof(SDL_linked_Rect));
    rect_to_add->rect = rect;
    rect_to_add->next = chain_rects;
}

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
void update_window(SDL_Renderer *rend)
{
    while (!QUIT)
    {
        SDL_Event current_event;
        SDL_WaitEvent(&current_event);
        if (current_event.type==SDL_MOUSEBUTTONDOWN)
        {
            SDL_Point mouse_pos;
            Uint32 button_pressed = SDL_GetMouseState(&mouse_pos.x, &mouse_pos.y);
            SDL_Log("Mouse button is pressed at %u %u", mouse_pos.x, mouse_pos.y);
            if ( (button_pressed & SDL_BUTTON_LMASK) != 0 )
            {
                fprintf(stderr, "ok1\n");
                SDL_Rect new_rect = {mouse_pos.x, mouse_pos.y, rect_size, rect_size};
                add_rect_to_list(new_rect);
            }
            draw_chain_rects(rend);
        }
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

    //window update loop
    update_window(renderer);
    /*
    //EVENTS LOOP

    uint8_t nb_rects = 4;
    SDL_Rect *rect_list = calloc(nb_rects, sizeof(SDL_Rect));
    for (uint8_t i=0; i<nb_rects; i++)
    {
        rect_list[i].x=20+i*60;
        rect_list[i].y=20+i*30;
        rect_list[i].h=50+i*20;
        rect_list[i].w=10+i*15;
    }

    //rect rendering
    SDL_RenderDrawRects(renderer, rect_list, nb_rects);

    //render the rect onto the screen
    SDL_RenderPresent(renderer);
    */

    if ((!window)|(!renderer)) {
        SDL_DestroyWindow(window);
        SDL_Quit();
        return EXIT_FAILURE;
    }

    //wait till shutdown
    SDL_Delay(30000);

    SDL_DestroyWindow(window);
    SDL_Quit();
    return EXIT_SUCCESS;
}