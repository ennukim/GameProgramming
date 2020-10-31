#include "Level2.h"

#define LEVEL2_WIDTH 32
#define LEVEL2_HEIGHT 8

#define LEVEL2_ENEMY_COUNT 3
#define METEOR_COUNT 3
#define DRAGON_COUNT 1
#define ENEMY1_COUNT 2

unsigned int level2_data[] =
{
    2, 0, 0, 0, 0, 0, 0, 0, 3, 3, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,0, 0, 0, 3,
    2, 0, 0, 0, 0, 0, 0, 0, 3, 3, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,0, 0, 0, 3,
    2, 0, 0, 0, 0, 0, 0, 0, 3, 3, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,0, 0, 0, 3,
    2, 0, 0, 0, 0, 0, 0, 0, 3, 3, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,0, 0, 0, 3,
    2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 3, 3, 3,3, 0, 0, 3,
    2, 0, 0, 0, 0, 0, 0, 0, 3, 3, 3, 3, 3, 3, 0, 0, 0, 0, 3, 0, 0, 3, 0, 0, 0, 0, 0, 0,0, 0, 0, 3,
    2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 2, 2, 0, 3, 3, 3, 2, 0, 0, 2, 3, 3, 3, 0, 0, 3,0, 3, 3, 3,
    2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 0, 2, 2, 2, 2, 0, 2, 2, 2, 2, 2, 1, 1, 2,0, 3, 3, 3
};

void Level2::Initialize() {
    
    state.nextScene = -1;
    state.level = LEVEL2;
    state.endGame = false;
    
    GLuint mapTextureID = Util::LoadTexture("tileset.png");
    state.map = new Map(LEVEL2_WIDTH, LEVEL2_HEIGHT, level2_data, mapTextureID, 1.0f, 4, 1);
    
    
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
    state.enemies = new Entity[LEVEL2_ENEMY_COUNT];
    GLuint enemy1textureID = Util::LoadTexture("enemy1.png");
     
    for (int i = 0; i < LEVEL2_ENEMY_COUNT; i++) {
        state.enemies[i].entityType = ENEMY;
        state.enemies[i].aiType = WALKER;
        state.enemies[i].aiState = PATROLLING;
        state.enemies[i].textureID = enemy1textureID;
        state.enemies[i].speed = 2.0f;
        state.enemies[i].acceleration = glm::vec3(0, -9.81f, 0);
        state.enemies[i].width = 0.8f;
        state.enemies[i].movement = glm::vec3(1);
    }
    state.enemies[0].position = glm::vec3(12.0f, -4, 0);
    state.enemies[1].position = glm::vec3(14.0f, -4, 0);
    state.enemies[2].position = glm::vec3(25.0f, 0, 0);
    
    state.font = new Entity();
    GLuint textTextureID = Util::LoadTexture("font.png");
    state.font->textureID = textTextureID;
}
void Level2::Update(float deltaTime) {
    state.player->Update(deltaTime, state.player, state.map, state.enemies, LEVEL2_ENEMY_COUNT);
    for (int i = 0; i < LEVEL2_ENEMY_COUNT; i++) {
        state.enemies[i].Update(deltaTime, state.player,state.map, state.enemies, LEVEL2_ENEMY_COUNT);
    }
    if(state.player->position.x >= 30){
        state.nextScene = 3;
    }
    if(state.player->life == 0){
        state.endGame = true;
    }
}

void Level2::Render(ShaderProgram *program) {
    state.map->Render(program);
    state.player->Render(program);
    for (int i = 0; i < LEVEL2_ENEMY_COUNT; i++) {
        state.enemies[i].Render(program);
        state.enemies[i].Render(program);
    }
}

