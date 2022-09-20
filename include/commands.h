//
// Created by helen on 04/09/22.
//

#ifndef RAYCASTING_COMMANDS_H
#define RAYCASTING_COMMANDS_H

#include <stdint.h>
#include <SDL2/SDL.h>
#include <stdbool.h>
#include <SDL_ttf.h>
#include "geometry.h"
#include "events.h"
#include "render.h"

typedef struct SDL_button {
    int id;
    SDL_Rect rect;
    SDL_Color button_color;
    SDL_callback callback;
    bool sunken;
} SDL_button;

typedef struct SDL_text_frame {
    SDL_Color text_color;
    char *text;
    SDL_Rect text_rect;
    SDL_Texture *text_texture;
    SDL_button button; //not always pushable (clickable)
    bool pushable;
    struct SDL_text_frame *next;
} SDL_text_frame;


extern TTF_Font *font_title;
extern TTF_Font *font_buttons;
extern SDL_text_frame *chain_text;

extern bool is_mouse_in_panel();
extern void init_command_panel(SDL_Renderer *renderer);
extern void free_panel();

#endif //RAYCASTING_COMMANDS_H
