#ifndef Evolve
#define Evolve

#include "Classes/mapset.c++";
#include "Classes/mobs.c++";
#include "Classes/managers.c++";



using namespace std;
#define screenheight 720
#define screenwidth 960
#define RENDER_SCALE 2.0
using json = nlohmann::json;
class game
{
    map<string, map<int, Texture2D>> textures;
    world_manager manager;

public:
    void run()
    {
        InitWindow(screenwidth, screenheight, "Evolve");
        SetTargetFPS(60);
        textures = manager.set.prepare_textures(textures, "grass", LoadTexture("assets/grass/grass.png"));
        textures = manager.set.prepare_textures(textures, "slime", LoadTexture("assets/mob/slime.png"));
        manager.current_State = manager.loading;
        while (!WindowShouldClose())
        {
            BeginDrawing();
            ClearBackground(BLUE);
            BeginMode2D(manager.cam.camera);

            manager.manage(textures);
            EndMode2D();
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