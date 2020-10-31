#include "Entity.h"

Entity::Entity()
{
    position = glm::vec3(0);
    movement = glm::vec3(0);
    acceleration = glm::vec3(0);
    velocity = glm::vec3(0);
    speed = 0;
    
    modelMatrix = glm::mat4(1.0f);
}

bool Entity::CheckCollision(Entity* other) {
    if (isActive == false || other->isActive == false) return false;
    
    float xdist = fabs(position.x - other->position.x) - ((width + other->width) / 2.0f);
    float ydist = fabs(position.y - other->position.y) - ((height + other->height) / 2.0f);
    
    if (xdist < 0 && ydist < 0) {
        lastCollision = other->entityType;
        return true;
    }
    return false;
}


void Entity::CheckCollisionsY(Entity* objects, int objectCount)
{
    for (int i = 0; i < objectCount; i++)
    {
        Entity* object = &objects[i];
        if (CheckCollision(object))
        {
            float ydist = fabs(position.y - object->position.y);
            float penetrationY = fabs(ydist - (height / 2.0f) - (object->height / 2.0f));
            if (velocity.y > 0) {
                position.y -= penetrationY;
                velocity.y = 0;
                collidedTop = true;
            }
            else if (velocity.y < 0) {
                position.y += penetrationY;
                velocity.y = 0;
                collidedBottom = true;
                if (this->entityType == PLAYER && (objects[i].entityType == ENEMY1 || objects[i].entityType == METEOR || objects[i].entityType == DRAGON)) {
                    objects[i].aiState = DEAD;
                }
            }
        }
    }
}

void Entity::CheckCollisionsX(Entity* objects, int objectCount)
{
    for (int i = 0; i < objectCount; i++)
    {
        Entity* object = &objects[i];
        if (CheckCollision(object))
        {
            float xdist = fabs(position.x - object->position.x);
            float penetrationX = fabs(xdist - (width / 2.0f) - (object->width / 2.0f));
            if (velocity.x > 0) {
                position.x -= penetrationX;
                velocity.x = 0;
                collidedRight = true;
            }
            else if (velocity.x < 0) {
                position.x += penetrationX;
                velocity.x = 0;
                collidedLeft = true;
            }
        }
    }
}

void Entity::AIWalker(Entity* player, Entity* platforms, int platformCount) {
    switch (aiState) {
        case IDLE:
            break;
        case WALKING:
            break;
        case PATROLLING:
                if (position.x >= 4.8f) {
                    movement.x = -1;
                }
                if (position.x <= -4.8f) {
                    movement.x = 1;
                }
                break;
        case JUMPING:
            break;
        case DEAD:
            isActive = false;
            break;
            
    }
}

void Entity::AIFaller(Entity* player, Entity* platforms, int platformCount) {
    switch (aiState) {
        case IDLE:
            if (glm::distance(position, player->position) < 4.0f) {
                aiState = WALKING;
            }
            break;
            
        case WALKING:
            if (player->position.x < position.x) {
                movement.x = -1;
            }
            else {
                movement.x = 1;
            }
            break;
        case PATROLLING:
            break;
        case JUMPING:
            break;
        case DEAD:
            isActive = false;
            break;
    }
}

void Entity::AIJumper(Entity* player, Entity* platforms, int platformCount) {
    switch (aiState) {
        case IDLE:
            break;
        case WALKING:
            if (glm::distance(position, player->position) < 6.0f) {
                aiState = JUMPING;
            }
            break;
            
        case JUMPING:
            if (player->position.x < position.x) {
                movement.x = 1;
            }
            else {
                movement.x = 1;
            }
            break;
        case PATROLLING:
            break;
        case DEAD:
            isActive = false;
            break;
            
    }
}

void Entity::AI(Entity* player, Entity* platforms, int platformCount) {
    switch (aiType) {
        case WALKER:
            AIWalker(player, platforms, platformCount);
            break;
            
        case FALLER:
            AIFaller(player, platforms, platformCount);
            break;
            
        case JUMPER:
            AIJumper(player, platforms, platformCount);
            break;
    }
}


void Entity::Update(float deltaTime, Entity* player, Entity* platforms, int platformCount, Entity* enemy1, int enemy1Count, Entity* meteor, int meteorCount, Entity* dragon)
{
    if (entityType == DRAGON) {
        int meteorDead = 0;
        for (int i = 0; i < meteorCount; i++) {
            if (meteor[i].aiState == DEAD) {
                meteorDead++;
            }
        }
        if (meteorDead == meteorCount) {
            isActive = true;
        }
    }
    
    if (isActive == false) return;
    
    collidedTop = false;
    collidedBottom = false;
    collidedLeft = false;
    collidedRight = false;
    
    if (entityType == ENEMY1 || entityType == METEOR || entityType == DRAGON ) {
        AI(player, platforms, platformCount);
    }
    
    if (jump) {
        jump = false;
        velocity.y += jumpPower;
    }
    
    velocity.x = movement.x * speed;
    velocity += acceleration * deltaTime;
    
    position.y += velocity.y * deltaTime; // Move on Y
    CheckCollisionsY(platforms, platformCount);// Fix if needed
    
    position.x += velocity.x * deltaTime; // Move on X
    CheckCollisionsX(platforms, platformCount);// Fix if needed
    
    if (entityType == DRAGON && collidedBottom == true) {
        jump = true;
    }
    
    if (entityType == PLAYER) {
        CheckCollisionsY(enemy1, enemy1Count);
        CheckCollisionsY(meteor, meteorCount);
        CheckCollisionsY(dragon, 1);
    }
    
    if (entityType == PLAYER) {
        CheckCollisionsX(enemy1, enemy1Count);
        CheckCollisionsX(meteor, meteorCount);
        CheckCollisionsX(dragon, 1);
    }
    
    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
}

void Entity::DrawSpriteFromTextureAtlas(ShaderProgram *program, GLuint textureID, int index)
{
    float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };
    
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->positionAttribute);
    
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program->texCoordAttribute);
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}

void Entity::Render(ShaderProgram *program) {
    
    if (isActive == false) return;
    
    program->SetModelMatrix(modelMatrix);
    
    float vertices[]  = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };
    
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->positionAttribute);
    
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program->texCoordAttribute);
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}



