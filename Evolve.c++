#ifndef Evolve
#define Evolve

#include "root_classes.c++"

using namespace std;
#define screenheight 720
#define screenwidth 960
#define RENDER_SCALE 2.0
using json = nlohmann::json;

class game
{
    map<string,map<int,Texture2D>> textures;
    public:
    world_manager* manager=&world_manager::getInstance();
    void run()
    {
        InitWindow(screenwidth, screenheight, "Evolve");
        SetTargetFPS(60);
        textures=manager->set.prepare_textures(textures, "grass", LoadTexture("assets/grass/grass.png"));
         manager->set.loadmap(textures,"test_map");
        while (!WindowShouldClose())
        {
            BeginDrawing();
            ClearBackground(RAYWHITE);
            manager->manage();
            manager->set.drawmap();
            EndDrawing();
        }
        CloseWindow();
    }
 

};
int main()
{
    game Game;
    Game.run();

}







#endif