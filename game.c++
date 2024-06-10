#ifndef game
#define game
#include "maploader.hpp"
#include "charachters.c++"
#include <iostream>
#include "screens.hpp"
#include <map>
using namespace std;
#define screenheight 720
#define screenwidth 960

int main()
{

   int window_state = 0;
   int inner_windowstate = 0;
   player player(screenwidth / 2, screenheight / 2 - 500, 15, 30, RED);
   bool loaded = false;
   int zoom = 1;
   int rotation = 0;
   Vector2 offset = {screenwidth / 2, screenheight / 2};
   Vector2 target = {screenwidth / 2, screenheight / 2 - 200};
   mod_cam camera(target, offset, zoom, rotation);
   InitWindow(screenwidth, screenheight, "indigoV2");
   game_screen games;
   home_screen start;
   pause pause;
   map_select selector;
   string current_map;

   SetTargetFPS(60);
   while (!WindowShouldClose())
   {

      BeginDrawing();

      BeginMode2D(camera.camera);
      switch (window_state)
      {
      case 0:
         ClearBackground(DARKGREEN);
         start.run_screen(&window_state, camera.camera);
         start.draw_screen();

         break;

      case 1:
      ClearBackground(DARKPURPLE);
         if (inner_windowstate == 0)
         {  
            
            selector.render();
            current_map = selector.select_map();
   
          
            if(current_map!="")
            {
               games.set_game(camera, player, current_map,&inner_windowstate);
               
            }
         }
         else if (inner_windowstate == 1)
         {
            ClearBackground(BLUE);
            games.run_game();
            camera.update_position(games.play);
            games.pause_screen(&window_state);
          
         }

         break;
      case 2:
         pause.pause_on(camera);
         pause.pause_off(&window_state, camera);
         break;
      }

      EndMode2D();
      EndDrawing();
   }
   CloseWindow();
   return 0;
}

#endif