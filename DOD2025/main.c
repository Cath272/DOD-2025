
#define SDL_MAIN_USE_CALLBACKS 1  /* use the callbacks instead of main() */
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_surface.h>
#include <stdio.h>
#include <inttypes.h>

static SDL_Window* window = NULL;
static SDL_Renderer* renderer = NULL;


//Window size 
int w = 0, h = 0;
float scale = 2.0f;

float playerPosY = 0.0f;
float playerPosX = 0.0f;

Uint32 totalFrameTicks = 0;
Uint32 totalFrames = 0;


//Screen size for windowed mode
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

SDL_FRect rectangle;

#define SIZE 10000
#define SPAWN_SIZE 10000




float xEntity[SIZE], yEntity[SIZE];
float wEntity[SIZE], hEntity[SIZE];
Uint8 entityTexture[SIZE];



struct Complex_textures {
    SDL_Texture* texture;
    int texture_width;
    int texture_height;
};

typedef struct  {
    float xPos;
    float yPos;
} Vec2f;

Vec2f world;
struct Complex_textures textures[4];


void updatePos(){
    const bool* key_states = SDL_GetKeyboardState(NULL);
    bool stopUp = 0;

    for (int i = 0; i < SIZE; i++) {
        //doamne fereste
        if (playerPosY >= yEntity[i]+ world.yPos +textures[entityTexture[i]].texture_height && playerPosX + textures[2].texture_width <= xEntity[i] + world.xPos && playerPosX >= xEntity[i] + world.xPos + textures[entityTexture[i]].texture_width) {
            stopUp = 1;
        }
    }

    
    if (key_states[SDL_SCANCODE_W] && !stopUp) {
        if (key_states[SDL_SCANCODE_LSHIFT]) {
            world.yPos += +4.0f;
        }
        else {
            world.yPos += +2.0f;
        }
    }

    if (key_states[SDL_SCANCODE_S]) {
        if (key_states[SDL_SCANCODE_LSHIFT]) {
            world.yPos += -4.0f;
        }
        else {
            world.yPos += -2.0f;
        }

    }

    if (key_states[SDL_SCANCODE_D]) {
        if (key_states[SDL_SCANCODE_LSHIFT]) {
            world.xPos += -4.0f;
        }
        else {
            world.xPos += -2.0f;
        }

    }

    if (key_states[SDL_SCANCODE_A]) {
        if (key_states[SDL_SCANCODE_LSHIFT]) {
            world.xPos += +4.0f;
        }
        else {
            world.xPos += +2.0f;
        }

    }
    
};




struct Complex_textures loadTexture( char* textureName) {
    struct Complex_textures texture_local;
    SDL_Surface* surface = NULL;
    char* png_path = NULL;
    SDL_asprintf(&png_path,"%stextures/%s", SDL_GetBasePath(), textureName);  

    surface = SDL_LoadPNG(png_path);
    if (!surface) {
        SDL_Log("Couldn't load bitmap: %s", SDL_GetError());
        //return SDL_APP_FAILURE;
    }

    SDL_free(png_path);  /* done with this, the file is loaded. */

    if (surface != NULL)
    {
        texture_local.texture_width = surface->w ;
        texture_local.texture_height = surface->h;
    }
   

    //printf("%d %d ", texture_local.texture_width, texture_local.texture_height);

    

    texture_local.texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture_local.texture) {
        SDL_Log("Couldn't create static texture: %s", SDL_GetError());
        //return SDL_APP_FAILURE;
    }

    SDL_DestroySurface(surface);  
    return texture_local;
}

void renderObject(struct Complex_textures localObjectTexture, float scalingFactor, float xPos, float yPos, float wTexture, float hTexture ) {
    //track time
    SDL_FRect localObject;
    localObject.x = (float)(xPos) + world.xPos;
    localObject.y = (float)(yPos) + world.yPos;

    localObject.w = wTexture;
    localObject.h = hTexture;

    SDL_RenderTexture(renderer, localObjectTexture.texture, NULL, &localObject);
}

void renderText(float x, float y, char* message , uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    SDL_SetRenderDrawColor(renderer, r, g, b, a);
    SDL_RenderDebugText(renderer, x, y, message);
}

void renderPlayer(struct Complex_textures localObjectTexture, float scalingFactor) {
    
    SDL_FRect localObject;
    //(screen dimension/screen scaling) - textureWidth + texturecalingFactor
    localObject.x = (float)((w/ scale) - localObjectTexture.texture_width ) / 2;
    localObject.y = (float)((h/ scale) - localObjectTexture.texture_height ) / 2;
    //printf(" \n player x:%f player y:%f \n", localObject.x, localObject.y);
    localObject.w = (float)localObjectTexture.texture_width;
    localObject.h = (float)localObjectTexture.texture_height ;
    SDL_RenderTexture(renderer, localObjectTexture.texture, NULL, &localObject);
}



/* This function runs once at startup. */
SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[])
{
    /* Create the window */
    if (!SDL_CreateWindowAndRenderer("Hello World", SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_FULLSCREEN, &window, &renderer)) {
        SDL_Log("Couldn't create window and renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    for (int i = 0; i < SIZE; i++) {
        xEntity[i] = (float)SDL_rand(SPAWN_SIZE) - SPAWN_SIZE/2;
        yEntity[i] = (float)SDL_rand(SPAWN_SIZE) - SPAWN_SIZE / 2;
        entityTexture[i] = SDL_rand(3) + 1;
        //printf("x:%f y:%f entitytexture:%d \n", xEntity[i], yEntity[i], entityTexture[i]);
    }

    textures[0] = loadTexture("prince_1.png");
    textures[1] = loadTexture("prince_2.png");
    textures[2] = loadTexture("prince_3.png");

    for (int i = 0; i < 3; i++){
        textures[i].texture_height = textures[i].texture_height * 0.2f;
        textures[i].texture_width = textures[i].texture_width * 0.2f;

    }

    world.yPos = 0.0f;
    world.xPos = 0.0f;

  
    return SDL_APP_CONTINUE;
}

/* This function runs when a new event (mouse input, keypresses, etc) occurs. */
SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event)
{
    if ((event->type == SDL_EVENT_KEY_DOWN && event->key.key == SDLK_ESCAPE) ||
        event->type == SDL_EVENT_QUIT ) {
        return SDL_APP_SUCCESS;  /* end the program, reporting success to the OS. */
    }
    return SDL_APP_CONTINUE;
}

bool checkCollision(float playerX, float playerY, float playerWidth, float playerHeight, float objX, float objY, float objWidth, float objHeight)
{

    /*
    * The lines don't do anything anymore
    * //The sides of the rectangles
    int leftA, leftB;
    int rightA, rightB;
    int topA, topB;
    int bottomA, bottomB;

    //Calculate the sides of rect A
    leftA = playerX;
    rightA = playerX +playerWidth;
    topA = playerY;
    bottomA = playerY + playerHeight;

    //Calculate the sides of rect B
    leftB = objX;
    rightB = objX + objWidth;
    topB = objY;
    bottomB = objY + objHeight;
    */
    

    

     //If any of the sides from A are outside of B
     if (playerY + playerHeight <= objY)
     {
            return false;
     }

    if (playerY >= objY + objHeight)
    {
        return false;
    }

    if (playerX + playerWidth <= objX)
    {
        return false;
    }

    if (playerX >= objX + objWidth)
    {
        return false;
    }

    //If none of the sides from A are outside B
    return true;
}




/* This function runs once per frame, and is the heart of the program. */
SDL_AppResult SDL_AppIterate(void* appstate)
{

    char* posText = NULL;
    totalFrames++;
    Uint64 startTicks = SDL_GetTicks();
    Uint64 startPerf = SDL_GetPerformanceCounter();


 
    SDL_FRect dst_rect, rect2, rect3;


    SDL_GetRenderOutputSize(renderer, &w, &h);
    SDL_SetRenderScale(renderer, scale, scale);


    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    


    for (int i = 0; i < SIZE; i++) {
        renderObject(textures[entityTexture[i]], 0.2f, xEntity[i], yEntity[i], textures[entityTexture[i]].texture_width, textures[entityTexture[i]].texture_height);
    }

    
    
    playerPosY = (float)((h / scale) - textures[2].texture_height) / 2;
    playerPosX = (float)((w / scale) - textures[2].texture_width) / 2;
    

     
   
    SDL_asprintf(&posText, "x: %f y: %f", world.xPos, world.yPos);
    renderText(0.0f, 0.0f, posText, 255, 255, 255, 255);
   
    
    updatePos();
    //printf("\nxpos:%f ypos:%f", world.xPos, world.yPos);
    renderPlayer(textures[2], 0.2f);

    Uint32 endTicks = SDL_GetTicks();
    Uint64 endPerf = SDL_GetPerformanceCounter();
    Uint64 framePerf = endPerf - startPerf;
    float frameTime = (endTicks - startTicks) / 1000.0f;
    totalFrameTicks += endTicks - startTicks;
   

    // Strings to display
    char* fps;
    char* avg = NULL;
    char* perf = NULL;
    float ff = (float)totalFrameTicks / totalFrames;
    SDL_asprintf(&fps, "Current FPS: %f",  (1.0f / frameTime));
    SDL_asprintf(&avg, "Average FPS: %f",  (1000.0f / ((float)totalFrameTicks / totalFrames)));
    SDL_asprintf(&perf, "Current Perf: %" PRIu64 , framePerf);

    



    //// Display strings
    SDL_Rect dest = { 10, 10, 0, 0 };
    renderText(dest.x, dest.y, fps, 255, 255, 255, 255);
    dest.y += 24;
    renderText(dest.x, dest.y, avg, 255, 255, 255, 255);
    dest.y += 24;
    renderText(dest.x, dest.y, perf, 255, 255, 255, 255);

  

    SDL_RenderPresent(renderer);

    return SDL_APP_CONTINUE;
}

/* This function runs once at shutdown. */
void SDL_AppQuit(void* appstate, SDL_AppResult result)
{
}
