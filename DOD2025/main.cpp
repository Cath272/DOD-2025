
#define SDL_MAIN_USE_CALLBACKS 1  /* use the callbacks instead of main() */
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_surface.h>
#include <stdio.h>
#include <inttypes.h>
#include<box2d/types.h>
#include<box2d/box2d.h>
#include<box2d/base.h>
#include<box2d/collision.h>
#include<box2d/id.h>
#include<box2d/math_functions.h>
#include<IOSTREAM>
#include<SDL3/SDL_rect.h>





//b2WorldDef worldDef = b2DefaultWorldDef();


static SDL_Window* window = NULL;
static SDL_Renderer* renderer = NULL;


//Window size 
int w = 0, h = 0;
float scale = 2.0f;
float renderScale = 2.0f;

bool changedscale1 = false;
bool changedscale2 = false;
bool changedscale3 = false;




float playerPosY = 0.0f;
float playerPosX = 0.0f;


Uint32 totalFrameTicks = 0;
Uint32 totalFrames = 0;
Uint32 totalSecondFrames = 0;
Uint32 lastframeavg = 0;

bool moveUp, runUp, moveDown, runDown, moveLeft, runLeft, moveRight, runRight;


//Screen size for windowed mode
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

SDL_FRect rectangle;

#define SIZE 100000
#define SPAWN_SIZE 20000






float xEntity[SIZE], yEntity[SIZE];
int entityTexture[SIZE];
bool entityDestroy[SIZE];

float playerSize = 1.;

b2Polygon entityCollisionBox[SIZE] ;
b2Polygon playerCollisionBox;




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
    bool stopDown = 0;
    bool stopRight = 0;
    bool stopLeft = 0;

    moveUp = 0;
    runUp = 0;
    moveDown = 0;
    runDown = 0;
    moveRight = 0;
    runRight = 0;
    moveLeft = 0;
    runLeft = 0;


    
    for (int i = 0; i < SIZE; i++) {

        if (entityDestroy[i]) {
            continue;
        }

        //doamne fereste
        if (playerPosY <= yEntity[i]+ world.yPos +textures[entityTexture[i]].texture_height +15.0f && playerPosY + textures[2].texture_width * playerSize >= yEntity[i] + world.yPos && (playerPosX + textures[2].texture_width * playerSize >= xEntity[i] + world.xPos && playerPosX<= xEntity[i] + world.xPos + textures[entityTexture[i]].texture_width)) {
            //stopUp = 1;
            entityDestroy[i] = true;
            playerSize = playerSize + 0.01f;
        }

        if (playerPosY + textures[2].texture_width * playerSize + 15.0f >= yEntity[i]+ world.yPos  && playerPosY  <= yEntity[i] + world.yPos + textures[entityTexture[i]].texture_height  && (playerPosX + textures[2].texture_width * playerSize >= xEntity[i] + world.xPos && playerPosX <= xEntity[i] + world.xPos + textures[entityTexture[i]].texture_width)) {
            //stopDown = 1;
            entityDestroy[i] = true;
            playerSize = playerSize + 0.01f;

        }


        if (playerPosY <= yEntity[i] + world.yPos + textures[entityTexture[i]].texture_height  && playerPosY + textures[2].texture_width * playerSize >= yEntity[i] + world.yPos && (playerPosX + textures[2].texture_width * playerSize +10.0f>= xEntity[i] + world.xPos && playerPosX <= xEntity[i] + world.xPos + textures[entityTexture[i]].texture_width)) {
            //stopRight = 1;
            entityDestroy[i] = true;
            playerSize = playerSize + 0.01f;


        }

        if (playerPosY <= yEntity[i] + world.yPos + textures[entityTexture[i]].texture_height && playerPosY + textures[2].texture_width * playerSize >= yEntity[i] + world.yPos && (playerPosX -10.0f  <= xEntity[i] + world.xPos + textures[entityTexture[i]].texture_width && playerPosX + textures[2].texture_width * playerSize >= xEntity[i] + world.xPos )) {
            //stopLeft = 1;
            entityDestroy[i] = true;
            playerSize = playerSize + 0.01f;


        }
    }

    
    if (key_states[SDL_SCANCODE_W] && !stopUp) {
        if (key_states[SDL_SCANCODE_LSHIFT]) {
            runUp = 1;
        }
        else {
            moveUp = 1;
        }
    }

    if (key_states[SDL_SCANCODE_S] && !stopDown) {
        if (key_states[SDL_SCANCODE_LSHIFT]) {
            runDown = 1;
        }
        else {
            moveDown = 1;

        }

    }

    if (key_states[SDL_SCANCODE_D] && !stopRight) {
        if (key_states[SDL_SCANCODE_LSHIFT]) {
            runRight = 1;
        }
        else {
            moveRight = 1;
        }

    }

    if (key_states[SDL_SCANCODE_A] && !stopLeft) {
        if (key_states[SDL_SCANCODE_LSHIFT]) {
            runLeft = 1;
        }
        else {
            moveLeft = 1;
        }

    }

    if (moveUp) {
        world.yPos += +5.0f;
    }
    else if (runUp) {
        world.yPos += +10.0f;
    }

    if (moveDown) {
        world.yPos += -5.0f;
    }
    else if (runDown) {
        world.yPos += -10.0f;
    }

    if (moveRight) {
        world.xPos += -5.0f;
    }
    else if (runRight) {
        world.xPos += -10.0f;
    }

    if (moveLeft) {
        world.xPos += +5.0f;
    }
    else if (runLeft) {
        world.xPos += +10.0f;
    }
    
};




struct Complex_textures loadTexture( const char* textureName) {
    struct Complex_textures texture_local ;
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
    localObject.x = playerPosX;
    localObject.y = playerPosY;
    
    //printf(" \n player x:%f player y:%f \n", localObject.x, localObject.y);
    localObject.w = (float)localObjectTexture.texture_width * playerSize;
    localObject.h = (float)localObjectTexture.texture_height * playerSize;
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

    //worldDef.gravity = (b2Vec2){ 0.0f, -10.0f };
    //b2WorldId worldId = b2CreateWorld(&worldDef);

    

    textures[0] = loadTexture("prince_1.png");
    textures[1] = loadTexture("prince_2.png");
    textures[2] = loadTexture("prince_3.png");

    for (int i = 0; i < 3; i++) {
        textures[i].texture_height = textures[i].texture_height * 0.2f;
        textures[i].texture_width = textures[i].texture_width * 0.2f;

    }

    

    for (int i = 0; i < SIZE; i++) {

        xEntity[i] = (float)SDL_rand(SPAWN_SIZE) - SPAWN_SIZE/2;
        yEntity[i] = (float)SDL_rand(SPAWN_SIZE) - SPAWN_SIZE / 2;
        entityTexture[i] = SDL_rand(3) ;

        entityCollisionBox[i] = b2MakeSquare(textures[entityTexture[i]].texture_height * 0.5f);
        entityCollisionBox[i].centroid = { xEntity[i] + textures[entityTexture[i]].texture_width / 2, yEntity[i] - textures[entityTexture[i]].texture_height / 2 };
        
       
        
        //printf("x:%f y:%f entitytexture:%d \n", xEntity[i], yEntity[i], entityTexture[i]);
    }

    

    world.yPos = 0.0f;
    world.xPos = 0.0f;

    SDL_GetRenderOutputSize(renderer, &w, &h);
    SDL_SetRenderScale(renderer, scale, scale);

    playerPosY = (float)((h / scale) - textures[2].texture_height) / 2;
    playerPosX = (float)((w / scale) - textures[2].texture_width) / 2;
    playerCollisionBox = b2MakeSquare(textures[2].texture_height * 0.5f);
    playerCollisionBox.centroid = { playerPosX + textures[entityTexture[2]].texture_width/2, playerPosY + textures[entityTexture[2]].texture_height/2 };
    
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


void changeScale(float scale) {
    SDL_SetRenderScale(renderer, scale, scale);
    SDL_GetRenderOutputSize(renderer, &w, &h);
    
    
}


/* This function runs once per frame, and is the heart of the program. */
SDL_AppResult SDL_AppIterate(void* appstate)
{
    
        if(playerSize > 1.9 && playerSize<2.1 && !changedscale1){
            scale = 1.5f;
            changeScale(1.5f);
            
            changedscale1 = true;
            
        }
        else if (playerSize > 6 && !changedscale2) {
            changeScale(1.f);
            scale = 1.f;
            changedscale2 = true;

        } if (playerSize > 10 && !changedscale3) {
            changeScale(1.f);
            scale = 1.f;
            changedscale3 = true;

        }

        playerPosY = (float)((h / scale) - textures[2].texture_height * playerSize) / 2;
        playerPosX = (float)((w / scale) - textures[2].texture_width * playerSize) / 2;
        
    
    
    


    char* posText = NULL;
    totalFrames++;
    totalSecondFrames++;
    Uint64 startTicks = SDL_GetTicks();
    Uint64 startPerf = SDL_GetPerformanceCounter();


 
    SDL_FRect dst_rect, rect2, rect3;


    


    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 0);
    for (int i = 0; i < SIZE; i++) {

        if (entityDestroy[i]) {
            continue;
        }

        renderObject(textures[entityTexture[i]], 0.2f, xEntity[i], yEntity[i], textures[entityTexture[i]].texture_width, textures[entityTexture[i]].texture_height);
        entityCollisionBox[i].centroid = { xEntity[i] + textures[entityTexture[i]].texture_width/2, yEntity[i] + textures[2].texture_height/2  };

       

        
        //SDL_RenderPoint(renderer, entityCollisionBox[i].centroid.x + world.xPos, entityCollisionBox[i].centroid.y + world.yPos);
        
        //SDL_FRect a = { entityCollisionBox[i].centroid.x + entityCollisionBox[i].vertices[0].x + world.xPos, entityCollisionBox[i].centroid.y + entityCollisionBox[i].vertices[0].y + world.yPos, textures[entityTexture[i]].texture_width, textures[entityTexture[i]].texture_height };
        //SDL_RenderRect(renderer, &a);
        
        
        

    }

    

    SDL_SetRenderDrawColor(renderer, 255,255,255,0);


    SDL_asprintf(&posText, "x: %f y: %f", world.xPos, world.yPos);
    renderText(0.0f, 0.0f, posText, 255, 255, 255, 255);

    
   
    updatePos();
   

    

    

    renderPlayer(textures[2], 0.2f);
    //std::cout << "X:" << playerCollisionBox.vertices[0].x << " Y:" << playerCollisionBox.vertices[0].y << std::endl;

    SDL_RenderPoint(renderer, playerCollisionBox.centroid.x, playerCollisionBox.centroid.y);
    /*
    for (int i = 0; i < 4; i++) {
        SDL_RenderPoint(renderer, playerCollisionBox.centroid.x + playerCollisionBox.vertices[i].x * playerSize, playerCollisionBox.centroid.y + playerCollisionBox.vertices[i].y * playerSize);
    }
    */

    


    
    

    

    Uint64 endTicks = SDL_GetTicks();
    Uint64 endPerf = SDL_GetPerformanceCounter();

    Uint64 framePerf = endPerf - startPerf;
    float frameTime = (endTicks - startTicks) / 1000.0f;
    
    if (totalFrameTicks > 1000.0f) {
        lastframeavg = (1000.0f / ((float)totalFrameTicks / totalSecondFrames));
        totalFrameTicks = 0.0f;
        totalSecondFrames = 0.0f;
    }
    else {
        totalFrameTicks += endTicks - startTicks;
    }
   

    // Strings to display
    char* fps;
    char* avg = NULL;
    char* min= NULL;
    char* perf = NULL;
    float ff = (float)totalFrameTicks / totalFrames;
    SDL_asprintf(&fps, "Current FPS: %f",  (1.0f / frameTime));
    
    if (totalSecondFrames == 0) {
        SDL_asprintf(&avg, "Average FPS: %f", (float)lastframeavg);
    }
    else {
        SDL_asprintf(&avg, "Average FPS: %f", (1000.0f / ((float)totalFrameTicks / totalSecondFrames)));
    }


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
