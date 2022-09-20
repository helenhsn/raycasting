//
// Created by helen on 04/09/22.
//
#include "commands.h"
#include "render.h"
#include "events.h"
#include <SDL2/SDL.h>
#include <SDL_ttf.h>

TTF_Font *font_title = NULL;
TTF_Font *font_buttons = NULL;
static int padding_text = 5;
SDL_text_frame *chain_text = NULL;

typedef struct frame_info {
    int id;
    SDL_callback callback;
    char *text;
} frame_info;


bool is_mouse_in_panel()
{
        return (int) mouse_pos.x > w_width - panel_width;
}

static SDL_Texture *create_texture(SDL_Renderer *renderer, SDL_text_frame *frame)
{
        TTF_Font *font = frame->pushable ? font_buttons : font_title;
        SDL_Surface *surface = TTF_RenderText_Solid(font, frame->text, frame->text_color);
        int margin = 6*padding_text;
        if (!frame->pushable)
                margin = 2*padding_text;
        frame->text_rect.x = w_width - surface->w - margin;
        frame->text_rect.y = 2*padding_text;
        frame->text_rect.w = surface->w;
        frame->text_rect.h = surface->h;

        SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
        return texture;
}

static SDL_button create_button(SDL_text_frame *text_frame, SDL_Color button_color, frame_info info)
{
        SDL_button new_button = {info.id,
                                 text_frame->text_rect.x - padding_text,
                                text_frame->text_rect.y - padding_text,
                                text_frame->text_rect.w + 2*padding_text,
                                text_frame->text_rect.h + 2*padding_text,
                                       button_color,
                                       info.callback,
                                false
        };
        return new_button;
}

static void add_to_text_list(SDL_Renderer *renderer,
                      frame_info info,
                      SDL_Color color,
                      SDL_Color button_color,
                      bool pushable)
{
        //creating the text frame
        SDL_text_frame *new_frame = malloc(sizeof(SDL_text_frame));
        new_frame->text_color = color;
        new_frame->text = info.text;
        new_frame->pushable = pushable;
        new_frame->text_texture = create_texture(renderer, new_frame);
        new_frame->next = chain_text;
        new_frame->button = create_button(new_frame, button_color, info);
        new_frame->button.sunken = false;
        if (chain_text) {
                SDL_text_frame *last_frame = chain_text;
                int margin_buttons = 30;
                if (last_frame->pushable==false)
                        margin_buttons = 100;
                new_frame->button.rect.y = last_frame->button.rect.y + last_frame->button.rect.h + margin_buttons;
                new_frame->text_rect.y = new_frame->button.rect.y + padding_text;
        }
        chain_text = new_frame;

}

void init_command_panel(SDL_Renderer *renderer)
{
        //font
        TTF_Init();
        font_title = TTF_OpenFont("../fonts/NotoSansMono-Bold.ttf", 24);
        font_buttons = TTF_OpenFont("../fonts/NotoSansMono-Regular.ttf", 18);
        if ((!font_title) || (!font_buttons))
                printf("TTF_OpenFont : %s \n", TTF_GetError());

        //adding buttons
        SDL_Color text_color = {255, 120, 120};
        SDL_Color button_color = {255, 255, 255};

        //create title
        frame_info title_info = {0, NULL, "COMMAND PANEL"};
        add_to_text_list(renderer, title_info, text_color, button_color, false);

        //button casting rays
        frame_info button_strings[] = {{1, rays_callback, "CAST RAYS"},
                                  {2, edges_callback, "EDGES"},
                                  {3, rects_callback, "RECTANGLES"},
                                  {4, curves_callback, "CURVES"},
                                  {5, clear_callback, "CLEAR DRAWINGS"}};

        for (uint8_t i=0; i<5; i++)
                add_to_text_list(renderer, button_strings[i], text_color, button_color, true);
}

void free_panel()
{
        SDL_text_frame *current_frame = chain_text;
        while (current_frame)
        {
                SDL_text_frame *frame_to_free = current_frame;
                current_frame = current_frame->next;
                free(frame_to_free);
        }
        chain_text = NULL;
}