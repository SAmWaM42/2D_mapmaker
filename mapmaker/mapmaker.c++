#ifndef mapmaker
#define mapmaker
#include "../raylib.h"

#include "../root_classes.c++"
#include "../json.hpp"
#include <iostream>
#include <map>
using namespace std;
#define RENDER_SCALE 2.0
using json = nlohmann::json;

int grid_tiles = 100;
int text_length = 16;
int tile_size = 40;
class tileeditor
{

public:
    void run_editor()
    {

        bool loaded = false;
        int zoom = 1;
        int rotation = 0;
        int x = grid_tiles;
        int y = grid_tiles;
        Vector2 offset = {screenwidth / 2, screenheight / 2};
        Vector2 target = {screenwidth / 2, screenheight / 2};
        mod_cam camera(target, offset, zoom, rotation);
        Vector2 grid[x][y];
        for (int i = 0; i < grid_tiles; i++)
        {
            for (int j = 0; j < grid_tiles; j++)
            {
                grid[i][j].x = j * 40;
                grid[i][j].y = i * 40;
            }
        }

        InitWindow(screenwidth, screenheight, "indigoV2");
        map<string, map<int, Texture2D>> textures; 
        mapset mapset;
        Texture2D grass = LoadTexture("../assets/grass/grass.png");
        Texture2D tree = LoadTexture("../assets/trees/mytree.png");
        Texture2D slime_spawner = LoadTexture("../assets/mob/slime_spawner.png");
        Texture2D slime = LoadTexture("../assets/mob/slime.png");
       
        textures = mapset.prepare_textures(textures, "grass", grass);
        textures = mapset.prepare_textures(textures, "slime_spawner", slime_spawner);
        textures=mapset.prepare_textures(textures, "slime", slime);
    
        string tile_name[textures.size()];
         cout<<textures.size();
        tile_name[0] = "grass";
        tile_name[1] = "slime_spawner";
        tile_name[2] = "slime";

        Texture2D current_tile_img;
        int current_image = 0;
        int current_tile = 0;
        int variant = 0;
        bool tile_shift = false;
        bool mode_shift = false;
        bool grid_on = false;
        int time;

        tile temp_tile;

        SetTargetFPS(60);
        while (!WindowShouldClose())
        {

            current_tile_img = textures[tile_name[current_image]][variant];
            temp_tile.variant = variant;
            temp_tile.texture = current_tile_img;
            temp_tile.type = tile_name[current_image];

            if ((camera.camera.target.x - camera.camera.offset.x) < 0)
            {
                camera.camera.target.x += 1;
            }
            if ((camera.camera.target.y - camera.camera.offset.y) < 0)
            {
                camera.camera.target.y += 1;
            }
            camera.move_cam();

            // fix camera movement and improve block positioning ans assignment

            if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && !mode_shift)
            {
                

               temp_tile.position.width = current_tile_img.width;
               temp_tile.position.height = current_tile_img.height;
               temp_tile.used = true;
               temp_tile.ongrid = "true";
                int x = (int)(GetScreenToWorld2D(GetMousePosition(), camera.camera).x / 40);
                int y = (int)(GetScreenToWorld2D(GetMousePosition(), camera.camera).y / 40);

               temp_tile.position.x = grid[x][y].y;
               temp_tile.position.y = grid[x][y].x + 40 -temp_tile.position.height;
                bool layered = false;
                bool same_tile=false;
                if (current_tile > 0)
                {
                    for (int i = 0; i < mapset.tiles.size(); i++)
                    {
                        for (int j = 0; j < mapset.tiles.size(); j++)
                        {
                            if (mapset.tiles[i].used && mapset.tiles[i].ongrid == "true" && mapset.tiles[j].ongrid == "true")
                            {
                                
                                if (i != j)
                                {
                                    layered = CheckCollisionRecs(mapset.tiles[i].position, mapset.tiles[j].position);

                                    if (layered)
                                    {

                                        mapset.tiles.erase(mapset.tiles.begin()+j);
                                        same_tile=true;
                                    }
                                }
                            }
                        }
                    }
                }
                if (!same_tile)
                {
                                   mapset.tiles.push_back(temp_tile);
                                    current_tile++;

                }
                else
                {
                    same_tile=false;
                }
            }
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && mode_shift)
            {

               temp_tile.position.width = current_tile_img.width;
               temp_tile.position.height = current_tile_img.height;
               temp_tile.used = true;
               temp_tile.ongrid = "false";
                int x = (int)(GetScreenToWorld2D(GetMousePosition(), camera.camera).x / 40);
                int y = (int)(GetScreenToWorld2D(GetMousePosition(), camera.camera).y / 40);

               temp_tile.position.x = grid[x][y].y;
               temp_tile.position.y = grid[x][y].x + 40 -temp_tile.position.height;
               mapset.tiles.push_back(temp_tile);
               current_tile++;

               
            }

            if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
            {
                for (int i = 0; i < mapset.tiles.size(); i++)
                {
                    if (mapset.tiles[i].used)
                    {

                        if (CheckCollisionPointRec(GetScreenToWorld2D(GetMousePosition(), camera.camera), mapset.tiles[i].position))
                        {

                            mapset.tiles.erase((mapset.tiles.begin()+i));
                             current_tile--;
                            break;
                        }
                    }
                }
            }
            if (IsKeyPressed(KEY_RIGHT_SHIFT))
            {
                tile_shift = !tile_shift;
            }
            if (IsKeyPressed(KEY_LEFT_SHIFT))
            {
                mode_shift = !mode_shift;
            }

            if (IsKeyPressed(KEY_N) && tile_shift)
            {
                // changing tile type
                cout<<current_image<<"\n";
                cout<<textures.size();
                

                current_image = ((current_image + 1) % (textures.size()));
               
            
            }
            if (IsKeyPressed(KEY_N) && !tile_shift)
            {
                // changing the variant of that specific tile type
                 variant = ((variant + 1) % textures[tile_name[current_image]].size());
                if(variant>=textures[tile_name[current_image]].size())
                {
                    variant=0;
                }
                
            }
            if (IsKeyPressed(KEY_O))
            {
                mapset.save_file();
            }
            if (IsKeyPressed(KEY_G))
            {
                // the grid to assist in tile placement
                grid_on = !grid_on;
            }
            if (IsKeyPressed(KEY_X))
            {
                // primitive tile sorting on placement
                mapset.autosort(textures, current_tile);
            }
            if (IsKeyPressed(KEY_R))
            {
                cout << "enter file to be removed";
                string filename;
                cin >> filename;
                string name = "../assets/maps/" + filename;
                cout << name;
                if (remove(name.c_str()) != 0)
                {
                    cout << "file not deleted";
                }
            }
            if (IsKeyPressed(KEY_E))
            {
                string name;
                cout << "enter map name";
                cin >> name;
            mapset.edit_map(name, textures);
                
            }

            BeginDrawing();
            ClearBackground(WHITE);
            BeginMode2D(camera.camera);
            if (grid_on)
            {
                for (int i = 0; i < grid_tiles; i++)
                {
                    for (int j = 0; j < grid_tiles; j++)
                    {
                        DrawRectangle(grid[i][j].x, grid[i][j].y, 39, 39, BLACK);
                    }
                }
            }

            DrawTexture(textures[tile_name[current_image]][variant], camera.camera.target.x - camera.camera.offset.x, camera.camera.target.y - camera.camera.offset.y, RAYWHITE);

            for (int i = 0; i < mapset.tiles.size(); i++)
            {
                if (mapset.tiles[i].used == true)
                {
                    if (mapset.tiles[i].ongrid == "true")
                    {
                        DrawTexture(mapset.tiles[i].texture, mapset.tiles[i].position.x, mapset.tiles[i].position.y, RAYWHITE);
                    }
                }
            }
            for (int i = 0; i < mapset.tiles.size(); i++)
            {
                if (mapset.tiles[i].used == true)
                {
                    if (mapset.tiles[i].ongrid == "false")
                    {
                        DrawTexture(mapset.tiles[i].texture, mapset.tiles[i].position.x, mapset.tiles[i].position.y, RAYWHITE);
                        
                    }
                }
            }

            EndMode2D();
            EndDrawing();
        }
        CloseWindow();
    }
};
int main()
{
    tileeditor editor;
    editor.run_editor();
}
    

#endif