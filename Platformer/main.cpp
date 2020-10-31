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

#include "Entity.h"
#include "Map.h"
#include "Util.h"
#include "Scene.h"
#include "Menu.h"
#include "Level1.h"
#include "Level2.h"
#include "Level3.h"
//#define PLATFORM_COUNT 11


GLuint fontTextureID;

Mix_Music *music;
Mix_Chunk *gameOver;
Mix_Chunk *success;

SDL_Window* displayWindow;
bool gameIsRunning = true;
bool endGame = false;

ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;

Scene *currentScene;
Scene *sceneList[4];

void SwitchToScene(Scene *scene, int life) {
    currentScene = scene;
    currentScene->Initialize();
    if (currentScene->state.level != MENU){
        currentScene->state.player->life = life;
    }
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
    displayWindow = SDL_CreateWindow("Superb Mario 3", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
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
    
    fontTextureID = Util::LoadTexture("font.png");
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
    
    sceneList[0] = new Menu();
    sceneList[1] = new Level1();
    sceneList[2] = new Level2();
    sceneList[3] = new Level3();
    SwitchToScene(sceneList[0],3);
}

void ProcessInput() {
    
    currentScene->state.player->movement = glm::vec3(0);
    
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
                        if (currentScene->state.level != MENU && currentScene->state.endGame != true){
                            if (currentScene->state.player->collidedBottom) {
                                currentScene->state.player->jump = true;
                            }
                            break;
                        }
                    case SDLK_RETURN:
                        if(currentScene->state.level == MENU){
                            currentScene->state.nextScene = 1;
                        }
                        break;
                }
                break; // SDL_KEYDOWN
        }
    }
    const Uint8 *keys = SDL_GetKeyboardState(NULL);
    if (currentScene->state.endGame == false) {
        if (keys[SDL_SCANCODE_LEFT]) {
            currentScene->state.player->movement.x = -1.0f;
        }
        else if (keys[SDL_SCANCODE_RIGHT]) {
            currentScene->state.player->movement.x = 1.0f;
        }
        if (glm::length(currentScene->state.player->movement) > 1.0f) {
            currentScene->state.player->movement = glm::normalize(currentScene->state.player->movement);
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
    while (deltaTime >= FIXED_TIMESTEP && currentScene->state.endGame == false) {
        currentScene->Update(FIXED_TIMESTEP);
        deltaTime -= FIXED_TIMESTEP;
    }
    accumulator = deltaTime;
    
    //following the player under condition
    viewMatrix = glm::mat4(1.0f);
    if (currentScene->state.player->position.x > 5) {
        viewMatrix = glm::translate(viewMatrix,
                                    glm::vec3(-currentScene->state.player->position.x, 3.75, 0));
    } else {
        viewMatrix = glm::translate(viewMatrix, glm::vec3(-5, 3.75, 0));
    }
}


void Render() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    program.SetViewMatrix(viewMatrix);
    currentScene->Render(&program);
    
    if (currentScene->state.level == MENU){
        Util::DrawText(&program, currentScene->state.font->textureID, "Superb Mario 3", 0.6f, -0.1f, glm::vec3(1.9f, -2.0f, 0));
        Util::DrawText(&program, currentScene->state.font->textureID, "Avoid Enemies to Survive", 0.5f, -0.3f, glm::vec3(3.0f, -2.9f, 0));
        Util::DrawText(&program, currentScene->state.font->textureID, "or Risk a life for every Kill", 0.5f, -0.3f, glm::vec3(2.5f, -3.5f, 0));
        Util::DrawText(&program, currentScene->state.font->textureID, "(Press Enter to begin)", 0.5f, -0.3f, glm::vec3(3.1f, -4.9f, 0));
        
    }
    
    if (currentScene->state.level != MENU){
        if (currentScene->state.player->life == 3 && currentScene->state.endGame != true){
            Util::DrawText(&program, currentScene->state.font->textureID, "Lives left: 2", 0.5f, -0.3f, glm::vec3(0.6f, -0.3f, 0));
        }
        else if (currentScene->state.player->life == 2 && currentScene->state.endGame != true){
            Util::DrawText(&program, currentScene->state.font->textureID, "Lives left: 1", 0.5f, -0.3f, glm::vec3(0.5f, -0.3f, 0));
        }
        else if (currentScene->state.player->life == 1 && currentScene->state.endGame != true){
            Util::DrawText(&program, currentScene->state.font->textureID, "Lives left: 0", 0.5f, -0.3f, glm::vec3(0.5f, -0.3f, 0));
        }
        else if (currentScene->state.player->life == 0 && currentScene->state.endGame == true){
            Util::DrawText(&program, currentScene->state.font->textureID, "You're dead. Game's Over", 0.5f, -0.2f, glm::vec3(1.9f, -2.0f, 0));
            Mix_PlayChannel(-1, gameOver, 0);
        }
    }
    if (currentScene->state.level == LEVEL3 && currentScene->state.player->life != 0 && currentScene->state.endGame == true){
        Util::DrawText(&program, currentScene->state.font->textureID, "Level Complete", 0.5f, -0.2f, glm::vec3(28.0f, -3.0f, 0));
        Mix_PlayChannel(-1, success, 0);
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
        
        if (currentScene->state.nextScene >= 0) SwitchToScene(sceneList[currentScene->state.nextScene], currentScene->state.player->life);
        Render();
    }
    
    Shutdown();
    return 0;
}

