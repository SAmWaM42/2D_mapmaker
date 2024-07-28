#ifndef game
#define game
#include "root_classes.c++"
using namespace std;



class new_game
{
 
   public:
    world gameworld;
      new_game(player p,mod_cam cam):gameworld(p,cam)
   {
     
   }
};



int main()
{


   player Player;
   Player.position={screenwidth / 2, screenheight / 2};
   Player.dimensions={30,50};
   float zoom = 1;
   int rotation = 0;
   Vector2 offset = {screenwidth / 2, screenheight / 2};
   Vector2 target = {screenwidth / 2, screenheight / 2 - 200};
   mod_cam camera(target, offset, zoom, rotation);
   new_game  begin_game(Player,camera);

   InitWindow(screenwidth, screenheight, "indigoV2");
    begin_game.gameworld.LoadTextures();
    begin_game.gameworld.prepare_maps();
\
   SetTargetFPS(60);
   while (!WindowShouldClose())
   {

      BeginDrawing();
      BeginMode2D(begin_game.gameworld.cam.camera);
      ClearBackground(GREEN);
      begin_game.gameworld.run_world();
   

    EndMode2D();
   EndDrawing();  
   }
  
   CloseWindow();
   return 0;
}

#endif