#include "Level3.h"

#define LEVEL3_WIDTH 32
#define LEVEL3_HEIGHT 8

#define LEVEL3_ENEMY_COUNT 6
#define METEOR_COUNT 3
#define DRAGON_COUNT 1
#define ENEMY1_COUNT 2

unsigned int level3_data[] =
{
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,0, 0, 0, 2,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 2, 2, 0,0, 0, 0, 2,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0,0, 0, 0, 2,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 2, 2, 2, 0,0, 0, 0, 2,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 1,0, 0, 0, 2,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 2,0, 0, 0, 2,
    3, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 0, 0, 0, 1, 1, 0, 0, 2, 2, 1, 1, 1, 1, 1, 1, 2,0, 3, 3, 2,
    3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 2, 2, 0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2,0, 3, 3, 2
};

void Level3::Initialize() {
    
    state.nextScene = -1;
    state.level = LEVEL3;
    state.endGame = false;
    
    GLuint mapTextureID = Util::LoadTexture("tileset.png");
    state.map = new Map(LEVEL3_WIDTH, LEVEL3_HEIGHT, level3_data, mapTextureID, 1.0f, 4, 1);
    
    
    // Initialize Player
    state.player = new Entity();
    state.player->entityType = PLAYER;
    state.player->position = glm::vec3(5,0,0);
    state.player->movement = glm::vec3(0);
    state.player->acceleration = glm::vec3(0, -9.81, 0); //Setting Acceleration to Gravity
    state.player->speed = 2.5f;
    state.player->textureID = Util::LoadTexture("player.png");
    state.player->height = 0.8f;
    state.player->width = 0.6f;
    state.player->jumpPower = 6.0f;
    state.player->life = 3;
    
    // Initialize Enemy
    state.enemies = new Entity[LEVEL3_ENEMY_COUNT];
    GLuint enemy1textureID = Util::LoadTexture("enemy1.png");
    
    for (int i = 0; i < LEVEL3_ENEMY_COUNT; i++) {
        state.enemies[i].entityType = ENEMY;
        state.enemies[i].aiType = WALKER;
        state.enemies[i].aiState = PATROLLING;
        state.enemies[i].textureID = enemy1textureID;
        state.enemies[i].speed = 1.0f;
        state.enemies[i].acceleration = glm::vec3(0, -9.81f, 0);
        state.enemies[i].width = 0.8f;
        state.enemies[i].movement = glm::vec3(1);
    }
    state.enemies[0].position = glm::vec3(10.0f, 0, 0);
    state.enemies[1].position = glm::vec3(9.0f, 0, 0);
    state.enemies[2].position = glm::vec3(10.0f, 0, 0);
    state.enemies[3].position = glm::vec3(15.0f, 0, 0);

    GLuint dragonTextureID = Util::LoadTexture("dragon.png");
    state.enemies->entityType = ENEMY;
    state.enemies->aiType = JUMPER;
    state.enemies->aiState = WALKING;
    state.enemies->textureID = dragonTextureID;
    state.enemies->speed = 1.0f;
    state.enemies->acceleration = glm::vec3(0, -9.81f, 0);
    state.enemies->width = 0.8f;
    state.enemies->position = glm::vec3(2.0f, -2.0f, 0);
    state.enemies->jumpPower = 2.0f;
    
    state.font = new Entity();
    GLuint textTextureID = Util::LoadTexture("font.png");
    state.font->textureID = textTextureID;
}
void Level3::Update(float deltaTime) {
    state.player->Update(deltaTime, state.player, state.map, state.enemies, LEVEL3_ENEMY_COUNT);
    for (int i = 0; i < LEVEL3_ENEMY_COUNT; i++) {
        state.enemies[i].Update(deltaTime, state.player,state.map, state.enemies, LEVEL3_ENEMY_COUNT);
    }
    if(state.player->life == 0){
        state.endGame = true;
    }
    if(state.player->position.x >= 30){
        state.endGame = true;
    }
}

void Level3::Render(ShaderProgram *program) {
    state.map->Render(program);
    state.player->Render(program);
    for (int i = 0; i < LEVEL3_ENEMY_COUNT; i++) {
        state.enemies[i].Render(program);
    }
}
