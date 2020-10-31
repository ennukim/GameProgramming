#include "Entity.h"

Entity::Entity()
{
    position = glm::vec3(0);
    acceleration = glm::vec3(0);
    velocity = glm::vec3(0);
    rotation = glm::vec3(0);
    scale = glm::vec3(1);
    
    modelMatrix = glm::mat4(1.0f);
    
    speed = 0.0f;
    billboard = false;
    
    width = 1.0f;
    height = 1.0f;
    depth = 1.0f;
}

bool Entity::CheckCollision(Entity *other)
{
    float xdist = fabs(position.x - other->position.x) - ((width + other->width) / 2.0f);
    float ydist = fabs(position.y - other->position.y) - ((height + other->height) / 2.0f);
    float zdist = fabs(position.z - other->position.z) - ((depth + other->depth) / 2.0f);
    if (xdist < 0 && ydist < 0 && zdist < 0) return true;
    return false;
}

bool Entity::CheckCollisionsY(Entity *objects, int objectCount)
{
    for (int i = 0; i < objectCount; i++)
    {
        Entity *object = &objects[i];
        if (CheckCollision(object))
        {
            float ydist = fabs(position.y - object->position.y);
            float penetrationY = fabs(ydist - (height / 2.0f) - (object->height / 2.0f));
            if (velocity.y > 0) {
                position.y -= penetrationY;
                velocity.y = 0;
                collidedTop = true;
                return true;
            }
            else if (velocity.y < 0) {
                position.y += penetrationY;
                velocity.y = 0;
                collidedBottom = true;
                return true;
            }
        }
        
    }
    return false;
}

bool Entity::CheckCollisionsX(Entity *objects, int objectCount)
{
    for (int i = 0; i < objectCount; i++)
    {
        Entity *object = &objects[i];
        if (CheckCollision(object))
        {
            float xdist = fabs(position.x - object->position.x);
            float penetrationX = fabs(xdist - (width / 2.0f) - (object->width / 2.0f));
            if (velocity.x > 0) {
                position.x -= penetrationX;
                velocity.x = 0;
                collidedRight = true;
                return true;
            }
            else if (velocity.x < 0) {
                position.x += penetrationX;
                velocity.x = 0;
                collidedLeft = true;
                return true;
            }
        }
    }
    return false;
}


void Entity::Update(float deltaTime, Entity *player, Entity *objects, int objectCount, Entity *enemies, int enemyCount){
    glm::vec3 previousPosition = position;
    
    if (billboard) {
        if (position.x >= 1.3f) {
            velocity.x = -1.0f;
        }
        if (position.x <= -1.3f) {
            velocity.x = 1.0f;
        }
//        float directionX = position.x - player->position.x;
//        float directionZ = position.z - player->position.z;
//        rotation.y = glm::degrees(atan2f(directionX, directionZ));
//
//        velocity.z = cos(glm::radians(rotation.y)) * -1.0f;
//        velocity.x = sin(glm::radians(rotation.y)) * -1.0f;
    }
    
    if (jump) {
        velocity.y += jumpPower;
        jump = false;
    }
    
    velocity += acceleration * deltaTime;
    position += velocity * deltaTime;
    
    //i don't care if the crate is hit by other thing than player
    if(entityType == PLAYER){
        if (player->position.y < 1) player->position.y = 1.0f;
        for (int i = 0; i < objectCount; i++)
        {
            // Ignore collisions with the floor
            if (objects[i].entityType == FLOOR) continue;
            
//            if (CheckCollision(&objects[i])) {
//                position = previousPosition;
//                break;
//            }
            
            if (CheckCollisionsX(&objects[i], objectCount)) {
                position.x = previousPosition.x;
                break;
            }
            if (CheckCollisionsY(&objects[i], objectCount)) {
                position.y = previousPosition.y;
//                jump = true;
                break;
            }
//            if (CheckCollisionsZ(&objects[i], objectCount)) {
//                position.z = previousPosition.z;
//                break;
//            }
            
        }
        for (int i = 0; i < enemyCount; i++)
        {
            if (CheckCollision(&enemies[i])) {
//                jump = true;
//                position = previousPosition;
                break;
            }
        }
    }
    
    if (entityType == CUBE){
        rotation.y += 45 * deltaTime;
        rotation.z += 45 * deltaTime;
    }
    else if(entityType == ENEMY){
        rotation.y += 30 * deltaTime;
    }
    
    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
    modelMatrix = glm::scale(modelMatrix, scale);
//    modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.y), glm::vec3(1.0f,0.0f,0.0f));
    modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.y), glm::vec3(0.0f,1.0f,0.0f));
    modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.z), glm::vec3(0.0f,0.0f,1.0f));
}

void Entity::Render(ShaderProgram *program) {
    program->SetModelMatrix(modelMatrix);

    glBindTexture(GL_TEXTURE_2D, textureID);
    
    if (billboard) {
        DrawBillboard(program);
    } else {
        mesh->Render(program);
    }
}

void Entity::DrawBillboard(ShaderProgram *program) {
    float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->positionAttribute);
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program->texCoordAttribute);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}

