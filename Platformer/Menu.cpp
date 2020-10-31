#include "Menu.h"
#include "Util.h"

#define MENU_WIDTH 11
#define MENU_HEIGHT 8
#define MENU_ENEMY_COUNT 1

unsigned int menu_data[] =
{
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
};

void Menu::Initialize() {
    
    state.nextScene = -1;
    state.level = MENU;
    GLuint mapTextureID = Util::LoadTexture("tileset.png");
    state.map = new Map(MENU_WIDTH, MENU_HEIGHT, menu_data, mapTextureID, 1.0f, 4, 1);
    
    // Initialize Player
    state.player = new Entity();
    state.player->isActive = false;

    state.font = new Entity();
    GLuint textTextureID = Util::LoadTexture("font.png");
    state.font->textureID = textTextureID;
    
    state.endGame = false;
}

void Menu::Update(float deltaTime) {
    state.player->Update(deltaTime, state.player, state.map, state.enemies, MENU_ENEMY_COUNT);
    if(state.player->position.x >= 12){
        state.nextScene = 1;
    }
}

void Menu::Render(ShaderProgram *program) {
    state.map->Render(program);
    state.player->Render(program);
}
