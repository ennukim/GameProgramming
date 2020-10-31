#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include <vector>


#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "Entity.h"

#define PLATFORM_COUNT 11
#define ENEMY1_COUNT 2
#define METEOR_COUNT 3
#define DRAGON_COUNT 1

struct GameState {
    Entity *player;
    Entity *platforms;
    Entity *enemy1;
    Entity *meteor;
    Entity *dragon;
};
GLuint fontTextureID;
GameState state;

Mix_Music *music;
Mix_Chunk *gameOver;
Mix_Chunk *success;

SDL_Window* displayWindow;
bool gameIsRunning = true;
bool endGame = false;

ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;

GLuint LoadTexture(const char* filePath) {
    int w, h, n;
    unsigned char* image = stbi_load(filePath, &w, &h, &n, STBI_rgb_alpha);
    
    if (image == NULL) {
        std::cout << "Unable to load image. Make sure the path is correct\n";
        assert(false);
    }
    
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    stbi_image_free(image);
    return textureID;
}

void DrawText(ShaderProgram* program, GLuint fontTextureID, std::string text, float size, float spacing, glm::vec3 position) {
    float width = 1.0f / 16.0f;
    float height = 1.0f / 16.0f;
    std::vector<float> vertices;
    std::vector<float> texCoords;
    for (int i = 0; i < text.size(); i++) {
        int index = (int)text[i];
        float offset = (size + spacing) * i;
        float u = (float)(index % 16) / 16.0f;
        float v = (float)(index / 16) / 16.0f;
        vertices.insert(vertices.end(), {
            offset + (-0.5f * size), 0.5f * size,
            offset + (-0.5f * size), -0.5f * size,
            offset + (0.5f * size), 0.5f * size,
            offset + (0.5f * size), -0.5f * size,
            offset + (0.5f * size), 0.5f * size,
            offset + (-0.5f * size), -0.5f * size,
        });
        texCoords.insert(texCoords.end(), {
            u, v,
            u, v + height,
            u + width, v,
            u + width, v + height,
            u + width, v,
            u, v + height,
        });
    } // end of for loop
    
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
    program->SetModelMatrix(modelMatrix);
    
    glUseProgram(program->programID);
    
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices.data());
    glEnableVertexAttribArray(program->positionAttribute);
    
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords.data());
    glEnableVertexAttribArray(program->texCoordAttribute);
    
    glBindTexture(GL_TEXTURE_2D, fontTextureID);
    glDrawArrays(GL_TRIANGLES, 0, (int)(text.size() * 6));
    
    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}

void Initialize() {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    displayWindow = SDL_CreateWindow("Rise of the AI", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    glViewport(0, 0, 640, 480);
    
    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");
    
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4996);
    music = Mix_LoadMUS("crypto.mp3");
    Mix_PlayMusic(music, -1);
    Mix_VolumeMusic(MIX_MAX_VOLUME / 4);
    
    gameOver = Mix_LoadWAV("gameover.wav");
    success = Mix_LoadWAV("success.wav");
    
    fontTextureID = LoadTexture("font.png");
    viewMatrix = glm::mat4(1.0f);
    modelMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    
    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    
    glUseProgram(program.programID);
    
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glEnable(GL_BLEND);
    
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Initialize Game Objects
    
    // Initialize Player
    state.player = new Entity();
    state.player->entityType = PLAYER;
    state.player->position = glm::vec3(-4,0,0);
    state.player->movement = glm::vec3(0);
    state.player->acceleration = glm::vec3(0, -9.81, 0); //Setting Acceleration to Gravity
    state.player->speed = 1.5f;
    state.player->textureID = LoadTexture("player.png");
    state.player->height = 0.8f;
    state.player->width = 0.6f;
    state.player->jumpPower = 5.0f;
    
    // Initialize Platforms
    state.platforms = new Entity[PLATFORM_COUNT];
    
    GLuint platformTextureID = LoadTexture("platform.png");
    
    for (int i = 0; i < PLATFORM_COUNT; i++) {
        state.platforms[i].textureID = platformTextureID;
        state.platforms[i].entityType = PLATFORM;
        state.platforms[i].position = glm::vec3(-5 +i, -3.25f, 0);
    }
    
    for (int i = 0; i < PLATFORM_COUNT; i++) {
        state.platforms[i].Update(0, NULL, NULL, 0, NULL, 0, NULL, 0, NULL);
    }
    
    // Initialize Enemy
    state.enemy1 = new Entity[ENEMY1_COUNT];
    GLuint enemy1textureID = LoadTexture("enemy1.png");
    
    for (int i = 0; i < ENEMY1_COUNT; i++) {
        state.enemy1[i].entityType = ENEMY1;
        state.enemy1[i].aiType = WALKER;
        state.enemy1[i].aiState = PATROLLING;
        state.enemy1[i].textureID = enemy1textureID;
        state.enemy1[i].speed = 1.0f;
        state.enemy1[i].acceleration = glm::vec3(0, -9.81f, 0);
        state.enemy1[i].width = 0.8f;
        state.enemy1[i].movement = glm::vec3(1);
        
    }
    state.enemy1[0].position = glm::vec3(-2.25f, 0, 0);
    state.enemy1[1].position = glm::vec3(3.0f, 0, 0);
    
    //METEOR
    state.meteor = new Entity[METEOR_COUNT];
    GLuint meteorTextureID = LoadTexture("meteor.png");
    
    for (int i = 0; i < METEOR_COUNT; i++) {
        state.meteor[i].entityType = METEOR;
        state.meteor[i].aiType = FALLER;
        state.meteor[i].aiState = IDLE;
        state.meteor[i].textureID = meteorTextureID;
        state.meteor[i].speed = 0.7f;
        state.meteor[i].acceleration = glm::vec3(0, -5.0, 0);
    }
    state.meteor[0].position = glm::vec3(-1.5f, 25.0f, 0);
    state.meteor[1].position = glm::vec3(2.5f, 20.0f, 0);
    state.meteor[2].position = glm::vec3(0.5f, 15.0f, 0);

    // Dragon
    state.dragon = new Entity();
    GLuint dragonTextureID = LoadTexture("dragon.png");
    state.dragon->entityType = DRAGON;
    state.dragon->aiType = JUMPER;
    state.dragon->aiState = WALKING;
    state.dragon->textureID = dragonTextureID;
    state.dragon->speed = 1.0f;
    state.dragon->acceleration = glm::vec3(0, -9.81f, 0);
    state.dragon->width = 0.8f;
    state.dragon->position = glm::vec3(-4.0f, -2.0f, 0);
    state.dragon->isActive = false;
    state.dragon->jumpPower = 2.0f;
}

void ProcessInput() {
    
    state.player->movement = glm::vec3(0);
    
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
            case SDL_WINDOWEVENT_CLOSE:
                gameIsRunning = false;
                break;
                
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_LEFT:
                        // Move the player left
                        break;
                        
                    case SDLK_RIGHT:
                        // Move the player right
                        break;
                        
                    case SDLK_SPACE:
                        if (state.player->collidedBottom) {
                            state.player->jump = true;
                        }
                        break;
                }
                break; // SDL_KEYDOWN
        }
    }
    const Uint8 *keys = SDL_GetKeyboardState(NULL);
    if (endGame == false) {
        if (keys[SDL_SCANCODE_LEFT]) {
            state.player->movement.x = -1.0f;
        }
        else if (keys[SDL_SCANCODE_RIGHT]) {
            state.player->movement.x = 1.0f;
        }
        if (glm::length(state.player->movement) > 1.0f) {
            state.player->movement = glm::normalize(state.player->movement);
        }
    }
}

#define FIXED_TIMESTEP 0.0166666f
float lastTicks = 0;
float accumulator = 0.0f;

void Update() {
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastTicks;
    lastTicks = ticks;
    
    deltaTime += accumulator;
    if (deltaTime < FIXED_TIMESTEP) {
        accumulator = deltaTime;
        return;
    }
    while (deltaTime >= FIXED_TIMESTEP && endGame == false) {
        
        state.player->Update(FIXED_TIMESTEP, state.player, state.platforms, PLATFORM_COUNT, state.enemy1, ENEMY1_COUNT, state.meteor, METEOR_COUNT, state.dragon);
        
        for (int i = 0; i < ENEMY1_COUNT; i++) {
            state.enemy1[i].Update(FIXED_TIMESTEP, state.player, state.platforms, PLATFORM_COUNT, state.enemy1, ENEMY1_COUNT, state.meteor, METEOR_COUNT, state.dragon);
        }
        for (int i = 0; i < METEOR_COUNT; i++) {
            state.meteor[i].Update(FIXED_TIMESTEP, state.player, state.platforms, PLATFORM_COUNT, state.enemy1, ENEMY1_COUNT, state.meteor, METEOR_COUNT, state.dragon);
        }
        for (int i = 0; i < DRAGON_COUNT; i++) {
            state.dragon->Update(FIXED_TIMESTEP, state.player, state.platforms, PLATFORM_COUNT, state.enemy1, ENEMY1_COUNT, state.meteor, METEOR_COUNT, state.dragon);
        }
        
        if (state.player->lastCollision == DRAGON && state.player->collidedBottom == true) {
            state.player->jump = true;
            state.dragon->aiState = DEAD;
        }
        
        deltaTime -= FIXED_TIMESTEP;
    }
    accumulator = deltaTime;
}


void Render() {
    glClear(GL_COLOR_BUFFER_BIT);
    state.player->Render(&program);
    state.dragon->Render(&program);
    
    if ((state.player->lastCollision == ENEMY1 || state.player->lastCollision == METEOR || state.player->lastCollision == DRAGON) && (state.player->collidedLeft == true || state.player->collidedRight == true)) {
        DrawText(&program, fontTextureID, "GAME OVER", 0.5f, -0.2f, glm::vec3(-1.1f, 2.0f, 0));
        Mix_PlayChannel(-1, gameOver, 0);
        endGame = true;
    }
    else if (state.enemy1->aiState == DEAD &&  state.meteor->aiState == DEAD && state.dragon->aiState == DEAD) {
        state.dragon->isActive = false;
        DrawText(&program, fontTextureID, "YOU WON", 0.5f, -0.2f, glm::vec3(-1.0f, 1.0f, 0));
        Mix_PlayChannel(-1, success, 0);
        endGame = true;
    }
    for (int i = 0; i < PLATFORM_COUNT; i++) {
        state.platforms[i].Render(&program);
    }
    for (int i = 0; i < ENEMY1_COUNT; i++) {
        state.enemy1[i].Render(&program);
    }
    for (int i = 0; i < METEOR_COUNT; i++) {
        state.meteor[i].Render(&program);
    }
    
    SDL_GL_SwapWindow(displayWindow);
}

void Shutdown() {
    SDL_Quit();
    Mix_FreeChunk(success);
    Mix_FreeChunk(gameOver);
    Mix_FreeMusic(music);
}

int main(int argc, char* argv[]) {
    Initialize();
    
    while (gameIsRunning) {
        ProcessInput();
        Update();
        Render();
    }
    
    Shutdown();
    return 0;
}

