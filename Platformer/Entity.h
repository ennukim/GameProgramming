#pragma once
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
#include <SDL_mixer.h>
#include "Map.h"

enum EntityType { PLAYER, PLATFORM, ENEMY };
enum AIType { WALKER, JUMPER };
enum AIState { IDLE, WALKING, PATROLLING, JUMPING, DEAD };

class Entity {
public:
    
    EntityType entityType;
    AIType aiType;
    AIState aiState;
    
    glm::vec3 position;
    glm::vec3 movement;
    glm::vec3 acceleration;
    glm::vec3 velocity;
    
    float width = 1;
    float height = 1;
    
    bool jump = false;
    float jumpPower = 0;
    
    float speed;
    
    GLuint textureID;
    
    glm::mat4 modelMatrix;

    bool isActive = true;
    int life = 3;
    
    bool collidedTop = false;
    bool collidedBottom = false;
    bool collidedLeft = false;
    bool collidedRight = false;
    
    EntityType lastCollision;
    
    Entity();
    
    void CheckCollisionsY(Entity* objects, int objectCount);
    void CheckCollisionsX(Entity* objects, int objectCount);
    void CheckCollisionsX(Map* map);
    void CheckCollisionsY(Map* map);
    
    bool CheckCollision(Entity* other);
    void Update(float deltaTime, Entity* player, Map* map, Entity* objects, int objectCount);
    void Render(ShaderProgram *program);
    void DrawSpriteFromTextureAtlas(ShaderProgram *program, GLuint textureID, int index);
    
    void AI(Entity* player, Map* map);
    void AIWalker(Entity* player, Map* map);
    void AIJumper(Entity* player, Map* map);
    
};

