/*
  Copyright (C) 1997-2025 Sam Lantinga <slouken@libsdl.org>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely.
*/
#define SDL_MAIN_USE_CALLBACKS 1  /* use the callbacks instead of main() */
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_surface.h>
#include <stdio.h>

static SDL_Window* window = NULL;
static SDL_Renderer* renderer = NULL;


static SDL_Texture* texture = NULL;
static int texture_width = 0;
static int texture_height = 0;

#define WIDTH 800
#define HEIGHT 600

SDL_FRect rectangle;




/* This function runs once at startup. */
SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[])
{

    SDL_Surface* surface = NULL;
    char* png_path = NULL;

    /* Create the window */
    if (!SDL_CreateWindowAndRenderer("Hello World", WIDTH, HEIGHT, SDL_WINDOW_FULLSCREEN, &window, &renderer)) {
        SDL_Log("Couldn't create window and renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    printf("%s", SDL_GetBasePath());
    
    
    SDL_asprintf(&png_path, "%stextures/prince_1.png", SDL_GetBasePath());  /* allocate a string of the full file path */
    
    surface = SDL_LoadPNG(png_path);
    if (!surface) {
        SDL_Log("Couldn't load bitmap: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    SDL_free(png_path);  /* done with this, the file is loaded. */
    
    texture_width = surface->w;
    texture_height = surface->h;

    texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        SDL_Log("Couldn't create static texture: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    SDL_DestroySurface(surface);  /* done with this, the texture has a copy of the pixels now. */
    
    
    return SDL_APP_CONTINUE;
}

/* This function runs when a new event (mouse input, keypresses, etc) occurs. */
SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event)
{
    if (event->type == SDL_EVENT_KEY_DOWN ||
        event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;  /* end the program, reporting success to the OS. */
    }
    return SDL_APP_CONTINUE;
}

/* This function runs once per frame, and is the heart of the program. */
SDL_AppResult SDL_AppIterate(void* appstate)
{
    const char* message = "Hello World!";
    int w = 0, h = 0;
    float x, y;
    const float scale = 4.0f;
    SDL_FRect dst_rect;


    int ScreenWidth;
    int ScreenHeight;
    SDL_GetWindowSize(window, &ScreenWidth, &ScreenHeight);


    


    /* Center the message and scale it up */
    SDL_GetRenderOutputSize(renderer, &w, &h);
    SDL_SetRenderScale(renderer, scale, scale);
    x = ((w / scale) - SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE * SDL_strlen(message)) / 2;
    y = ((h / scale) - SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE) / 2;

    //printf("%d %d \n", ScreenHeight, ScreenWidth);

    rectangle.w = 100;
    rectangle.h = 50;
    rectangle.x = ((w / scale) - rectangle.w) / 2;//(ScreenWidth  - (rectangle.w))/ 2;
    rectangle.y = ((h / scale) - rectangle.h) / 2; //(ScreenHeight - (rectangle.h)) / 2;

    printf("%s", SDL_GetBasePath());

    //printf(" Rectangle coord x: %f y: %f \n", rectangle.x, rectangle.y);
    //printf("Text coord x:%f y:%f \n", x, y);


    /* Draw the message */
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 191, 64, 191, 255);
    SDL_RenderFillRect(renderer, &rectangle);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDebugText(renderer, x, y, message);




    dst_rect.x = (float)(WIDTH - texture_width);
    dst_rect.y = (float)(HEIGHT - texture_height);
    dst_rect.w = (float)texture_width/5;
    dst_rect.h = (float)texture_height/5;
    SDL_RenderTexture(renderer, texture, NULL, &dst_rect);

    printf("2nd rect coord x:%f y:%f \n", dst_rect.x, dst_rect.y);






    SDL_RenderPresent(renderer);

    return SDL_APP_CONTINUE;
}

/* This function runs once at shutdown. */
void SDL_AppQuit(void* appstate, SDL_AppResult result)
{
}
