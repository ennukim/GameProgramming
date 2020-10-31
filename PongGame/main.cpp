
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

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define WINDOW_WIDTH 4.8f
#define WINDOW_HEIGHT 3.6f

SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, paddle1, paddle2, ball, projectionMatrix;

// Start at 0, 0, 0
glm::vec3 paddle1_position = glm::vec3(-5, 0, 0);
glm::vec3 paddle2_position = glm::vec3(5, 0, 0);
glm::vec3 ball_position = glm::vec3(0, 0, 0);
// Don’t go anywhere (yet).
glm::vec3 paddle1_movement = glm::vec3(0, 0, 0);
glm::vec3 paddle2_movement = glm::vec3(0, 0, 0);
glm::vec3 ball_movement = glm::vec3(0, 0, 0);

float player_speed = 1.5f;

void Initialize() {
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("Pong Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    glViewport(0, 0, 640, 480);
    
    program.Load("shaders/vertex.glsl", "shaders/fragment.glsl");
    
    viewMatrix = glm::mat4(1.0f);
    modelMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    
    paddle1 = glm::mat4(1.0f);
    paddle2 = glm::mat4(1.0f);
    ball = glm::mat4(1.0f);
    
    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    program.SetColor(1.0f, 1.0f, 0.0f, 1.0f);
    
    glUseProgram(program.programID);
    
    glClearColor(0.0f, 0.2f, 0.2f, 0.7f);
    glEnable(GL_BLEND);
    
    // Good setting for transparency
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    //playerTextureID = LoadTexture("paddle.png");
    //playerTextureID2 = LoadTexture("paddle.png");
}

void ProcessInput() {
    paddle1_movement = glm::vec3(0);
    paddle2_movement = glm::vec3(0);
    
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
                        //player_movement.x = 1.0f;
                        break;
                    case SDLK_SPACE:
                        // Some sort of action
                        ball_movement.x = 1.5f;
                        ball_movement.y = 1.5f;
                        break;
                }
                break; // SDL_KEYDOWN
        }
    }
    const Uint8 *keys = SDL_GetKeyboardState(NULL);

    if (keys[SDL_SCANCODE_UP]) {
        paddle2_movement.y = 1.5f;
    }
    else if (keys[SDL_SCANCODE_DOWN]) {
        paddle2_movement.y = -1.5f;
    }
    if (keys[SDL_SCANCODE_W]) {
        paddle1_movement.y = 1.5f;
    }
    else if (keys[SDL_SCANCODE_S]) {
        paddle1_movement.y = -1.5f;
    }
}

void Paddle(glm::mat4 modelMatrix){
    program.SetModelMatrix(modelMatrix);
    float vertices[] = { -0.3, -1, 0.3, -1, 0.3, 1, -0.3, -1, 0.3, 1, -0.3, 1 };
    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program.positionAttribute);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisableVertexAttribArray(program.positionAttribute);
}

void Ball(glm::mat4 modelMatrix){
    program.SetModelMatrix(modelMatrix);
    float vertices[] = { -0.2, -0.2, 0.2, -0.2, 0.2, 0.2, -0.2, -0.2, 0.2, 0.2, -0.2, 0.2 };
    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program.positionAttribute);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisableVertexAttribArray(program.positionAttribute);
}

GLboolean Collision(glm::vec3 paddle, glm::vec3 ball, float W1, float W2, float H1, float H2) {
    float xdiff = fabs(paddle.x - ball.x);
    float ydiff = fabs(paddle.y - ball.y);
    
    float xdist = xdiff - ((W1 + W2) / 2.0f);
    float ydist = ydiff - ((H1 + H2) / 2.0f);
    if (xdist < 0 && ydist < 0) {return true;}
    else {return false;}
}

float lastTicks = 0.0f;

void Update() {
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastTicks;
    lastTicks = ticks;
    // Add (direction * units per second * elapsed time)
    
    paddle1_position += paddle1_movement * player_speed * deltaTime;
    paddle2_position += paddle2_movement * player_speed * deltaTime;
    ball_position += ball_movement * player_speed * deltaTime;
    
    modelMatrix = glm::mat4(1.0f);
    paddle1 = glm::translate(modelMatrix, paddle1_position);
    paddle2 = glm::translate(modelMatrix, paddle2_position);
    ball = glm::translate(modelMatrix, ball_position);
    
    if (Collision(paddle1_position, ball_position, 0.5, 0.5, 0.5, 1)) {
        ball_movement.x = -ball_movement.x;
    }
    if (Collision(paddle2_position, ball_position, 0.5, 0.5, 0.5, 1)) {
        ball_movement.x = -ball_movement.x;
    }
    if (fabs(ball_position.x) >= WINDOW_WIDTH) {
        ball_movement = glm::vec3(0);
    }
    if (fabs(ball_position.y) >= WINDOW_HEIGHT) {
        ball_movement.y = -ball_movement.y;
    }
}

void Render() {
    glClear(GL_COLOR_BUFFER_BIT);
    Paddle(paddle1);
    Paddle(paddle2);
    Ball(ball);
    
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
