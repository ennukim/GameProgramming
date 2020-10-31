#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1

#include <vector>
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"

#include "Util.h"
#include "Entity.h"

SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;

glm::mat4 uiViewMatrix, uiProjectionMatrix;
GLuint fontTextureID;
GLuint heartTextureID;

#define OBJECT_COUNT 10
#define ENEMY_COUNT 1

struct GameState {
    Entity *player;
    Entity *objects;
    Entity *enemies;
};

GameState state;

void Initialize() {
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("3D!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    glViewport(0, 0, 1280, 720);
    
    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");
    
    uiViewMatrix = glm::mat4(1.0);
    uiProjectionMatrix = glm::ortho(-6.4f, 6.4f, -3.6f, 3.6f, -1.0f, 1.0f);
    fontTextureID = Util::LoadTexture("font1.png");
    heartTextureID = Util::LoadTexture("platformPack_item017.png");
    
    viewMatrix = glm::mat4(1.0f);
    modelMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::perspective(glm::radians(45.0f), 1.777f, 0.1f, 100.0f);
    
    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    program.SetColor(1.0f, 1.0f, 1.0f, 1.0f);
    
    glUseProgram(program.programID);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LEQUAL);
    
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    
    state.player = new Entity();
    state.player->entityType = PLAYER;
    state.player->position = glm::vec3(0, 1.0f, 2.0f);
    state.player->acceleration = glm::vec3(0, -9.81f, 0);
    state.player->speed = 1.0f;
    state.player->jumpPower = 8.0f;
    state.objects = new Entity[OBJECT_COUNT];
    
    GLuint floorTextureID = Util::LoadTexture("floor.JPG");
    Mesh *cubeMesh = new Mesh();
    cubeMesh->LoadOBJ("cube.obj", 10);
    
    state.objects[0].textureID = floorTextureID;
    state.objects[0].mesh = cubeMesh;
    state.objects[0].position = glm::vec3(0,-0.25f,0);
    state.objects[0].rotation = glm::vec3(0,0,0);
    state.objects[0].acceleration = glm::vec3(0,0,0);
    state.objects[0].scale = glm::vec3(20, 0.5f, 20);
    state.objects[0].entityType = FLOOR;
    
    GLuint crateTextureID = Util::LoadTexture("crate1_diffuse.png");
    Mesh *crateMesh = new Mesh();
    crateMesh->LoadOBJ("cube.obj", 1);
    
    state.objects[1].mesh = crateMesh;
    state.objects[1].textureID = crateTextureID;
    state.objects[1].position = glm::vec3(0, 0.5f, -2);
    state.objects[1].rotation = glm::vec3(0, 0, 0);
    state.objects[1].acceleration = glm::vec3(0, 0, 0);
    state.objects[1].scale = glm::vec3(1);
    state.objects[1].entityType = CRATE;
    
    state.objects[2].mesh = crateMesh;
    state.objects[2].textureID = crateTextureID;
    state.objects[2].position = glm::vec3(-1, 1.5f, -3);
    state.objects[2].rotation = glm::vec3(0, 0, 0);
    state.objects[2].acceleration = glm::vec3(0, 0, 0);
    state.objects[2].scale = glm::vec3(1);
    state.objects[2].entityType = CRATE;
    
    state.objects[3].mesh = crateMesh;
    state.objects[3].textureID = crateTextureID;
    state.objects[3].position = glm::vec3(0, 2.5f, -4);
    state.objects[3].rotation = glm::vec3(0, 0, 0);
    state.objects[3].acceleration = glm::vec3(0, 0, 0);
    state.objects[3].scale = glm::vec3(1);
    state.objects[3].entityType = CRATE;
    
    state.objects[4].mesh = crateMesh;
    state.objects[4].textureID = crateTextureID;
    state.objects[4].position = glm::vec3(1, 3.5f, -5);
    state.objects[4].rotation = glm::vec3(0, 0, 0);
    state.objects[4].acceleration = glm::vec3(0, 0, 0);
    state.objects[4].scale = glm::vec3(1);
    state.objects[4].entityType = CRATE;
    
    state.objects[5].mesh = crateMesh;
    state.objects[5].textureID = crateTextureID;
    state.objects[5].position = glm::vec3(2, 4.5f, -6);
    state.objects[5].rotation = glm::vec3(0, 0, 0);
    state.objects[5].acceleration = glm::vec3(0, 0, 0);
    state.objects[5].scale = glm::vec3(1);
    state.objects[5].entityType = CRATE;
    
    state.objects[6].mesh = crateMesh;
    state.objects[6].textureID = crateTextureID;
    state.objects[6].position = glm::vec3(1, 5.5f, -7);
    state.objects[6].rotation = glm::vec3(0, 0, 0);
    state.objects[6].acceleration = glm::vec3(0, 0, 0);
    state.objects[6].scale = glm::vec3(1);
    state.objects[6].entityType = CRATE;
    
    state.objects[7].mesh = crateMesh;
    state.objects[7].textureID = crateTextureID;
    state.objects[7].position = glm::vec3(0, 6.5f, -8);
    state.objects[7].rotation = glm::vec3(0, 0, 0);
    state.objects[7].acceleration = glm::vec3(0, 0, 0);
    state.objects[7].scale = glm::vec3(1);
    state.objects[7].entityType = CRATE;
    
    state.objects[8].mesh = crateMesh;
    state.objects[8].textureID = crateTextureID;
    state.objects[8].position = glm::vec3(-1, 7.5f, -9);
    state.objects[8].rotation = glm::vec3(0, 0, 0);
    state.objects[8].acceleration = glm::vec3(0, 0, 0);
    state.objects[8].scale = glm::vec3(1);
    state.objects[8].entityType = CRATE;
    
    state.objects[9].mesh = crateMesh;
    state.objects[9].textureID = crateTextureID;
    state.objects[9].position = glm::vec3(-2, 8.5f, -10);
    state.objects[9].rotation = glm::vec3(0, 0, 0);
    state.objects[9].acceleration = glm::vec3(0, 0, 0);
    state.objects[9].scale = glm::vec3(1);
    state.objects[9].entityType = CRATE;
    
    state.enemies = new Entity[ENEMY_COUNT];
    
    GLuint enemyTextureID = Util::LoadTexture("ghost.png");
    for (int i = 0; i < ENEMY_COUNT; i++) {
        state.enemies[i].billboard = true;
        state.enemies[i].textureID = enemyTextureID;
        state.enemies[i].position = glm::vec3(0, 0.5f, 0);
//        state.enemies[i].position = glm::vec3(0, 0.5f, 0);
//        state.enemies[i].position = glm::vec3(rand() % 20 - 10, 0.5, rand() % 20 - 10);
        state.enemies[i].rotation = glm::vec3(0, 0, 0);
        state.enemies[i].acceleration = glm::vec3(0, 0, 0);
        state.enemies[i].velocity.x = -1.0f;
//        state.enemies[i].velocity.z = -1.0f;
    }
}

void ProcessInput() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
            case SDL_WINDOWEVENT_CLOSE:
                gameIsRunning = false;
                break;
                
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_SPACE:
                        if (state.player->position.y == 1) {
                            state.player->jump = true;
                            break;
                        }
//                        state.player->lastCollision == CRATE && //if condition
                        else if (state.player->collidedBottom == true){
                            state.player->jump = true;
                        }
                }
                break;
        }
    }
    
    const Uint8 *keys = SDL_GetKeyboardState(NULL);
    if (keys[SDL_SCANCODE_A]) {
        state.player->rotation.y += 1.0f;
    } else if (keys[SDL_SCANCODE_D]) {
        state.player->rotation.y -= 1.0f;
    }
    
    if (keys[SDL_SCANCODE_W]) {
        state.player->rotation.x += 1.0f;
    }
    else if (keys[SDL_SCANCODE_S]) {
        state.player->rotation.x -= 1.0f;
    }
    
    state.player->velocity.x = 0;
    state.player->velocity.z = 0;
    
    if (keys[SDL_SCANCODE_UP]) {
        state.player->velocity.z = cos(glm::radians(state.player->rotation.y)) * -2.0f;
        state.player->velocity.x = sin(glm::radians(state.player->rotation.y)) * -2.0f;
    }
    else if (keys[SDL_SCANCODE_DOWN]) {
        state.player->velocity.z = cos(glm::radians(state.player->rotation.y)) * 2.0f;
        state.player->velocity.x = sin(glm::radians(state.player->rotation.y)) * 2.0f;
    }
    
//    if (keys[SDL_SCANCODE_W]) {
//        state.player->velocity.z = cos(glm::radians(state.player->rotation.y)) * -2.0f;
//        state.player->velocity.x = sin(glm::radians(state.player->rotation.y)) * -2.0f;
//    }
//    else if (keys[SDL_SCANCODE_S]) {
//        state.player->velocity.z = cos(glm::radians(state.player->rotation.y)) * 2.0f;
//        state.player->velocity.x = sin(glm::radians(state.player->rotation.y)) * 2.0f;
//    }
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
    
    while (deltaTime >= FIXED_TIMESTEP) {
        state.player->Update(FIXED_TIMESTEP, state.player, state.objects, OBJECT_COUNT, state.enemies, ENEMY_COUNT);
        for (int i = 0; i < OBJECT_COUNT; i++){
            state.objects[i].Update(FIXED_TIMESTEP,state.player, state.objects, OBJECT_COUNT, state.enemies, ENEMY_COUNT);
        }
        for (int i = 0; i < ENEMY_COUNT; i++){
            state.enemies[i].Update(FIXED_TIMESTEP,state.player, state.objects, OBJECT_COUNT, state.enemies, ENEMY_COUNT);
        }
        deltaTime -= FIXED_TIMESTEP;
    }
    
    accumulator = deltaTime;
    
    viewMatrix = glm::mat4(1.0f);
    viewMatrix = glm::rotate(viewMatrix, glm::radians(state.player->rotation.y), glm::vec3(0, -1.0f, 0));
    viewMatrix = glm::translate(viewMatrix, -state.player->position);

}


void Render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    
    //state.player->Render(&program);
    
    for (int i = 0; i < OBJECT_COUNT; i++){
        state.objects[i].Render(&program);
    }
    for (int i = 0; i < ENEMY_COUNT; i++){
        state.enemies[i].Render(&program);
    }
    program.SetProjectionMatrix(uiProjectionMatrix);
    program.SetViewMatrix(uiViewMatrix);
    
    Util::DrawText(&program, fontTextureID, "Lives: 3", 0.5, -0.3f, glm::vec3(-6, 3.2, 0));
    
    for (int i = 0; i < 3; i++)
    {
        Util::DrawIcon(&program, heartTextureID, glm::vec3(5 + (i * 0.5f), 3.2, 0));
    }
    
    SDL_GL_SwapWindow(displayWindow);
}

void Shutdown() {
    SDL_Quit();
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
