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

#include "Mesh.h"

enum EntityType { PLAYER, PLATFORM, ENEMY, CUBE, SHIP, FLOOR, CRATE};

class Entity {
public:
    EntityType entityType;
    
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec3 acceleration;
    glm::vec3 rotation;
    glm::vec3 scale;
    
    float speed;
    
    bool billboard;
    float width;
    float height;
    float depth;
    
    bool jump = false;
    float jumpPower = 0;
    
    bool collidedTop = false;
    bool collidedBottom = false;
    bool collidedRight = false;
    bool collidedLeft = false;
//    bool collidedFront = false;
//    bool collidedBack = false;
    EntityType lastCollision;
    
    GLuint textureID;
    Mesh *mesh;
    
    glm::mat4 modelMatrix;
    
    Entity();
    
    bool CheckCollision(Entity *other);
    void Update(float deltaTime, Entity *player, Entity *objects, int objectCount, Entity *enemies, int enemyCount);
    void DrawBillboard(ShaderProgram *program);
    void Render(ShaderProgram *program);
    
    bool CheckCollisionsY(Entity *objects, int objectCount);
    bool CheckCollisionsX(Entity *objects, int objectCount);
    bool CheckCollisionsZ(Entity *objects, int objectCount);
};



