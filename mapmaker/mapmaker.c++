#ifndef mapmaker
#define mapmaker
#include "../raylib.h"

#include "../root_classes.c++"
#include "../json.hpp"
#include <iostream>
#include <map>
using namespace std;
#define screenheight 720
#define screenwidth 960
#define RENDER_SCALE 2.0
using json = nlohmann::json;

int grid_tiles = 100;
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
        textures["grass"][0] = LoadTexture("../assets/grass/grass4.png");
        textures["grass"][1] = LoadTexture("../assets/grass/grass1.png");
        textures["grass"][2] = LoadTexture("../assets/grass/grass3.png");
        textures["grass"][3] = LoadTexture("../assets/grass/grass9.png");
        textures["grass"][4] = LoadTexture("../assets/grass/grass2.png");
        textures["grass"][5] = LoadTexture("../assets/grass/grass8.png");
        textures["grass"][6] = LoadTexture("../assets/grass/grass6.png");
        textures["grass"][7] = LoadTexture("../assets/grass/grass7.png");
        textures["grass"][8] = LoadTexture("../assets/grass/grass5.png");
        textures["stone"][0] = LoadTexture("../assets/stone/stone3.png");
        textures["stone"][1] = LoadTexture("../assets/stone/stone1.png");
        textures["stone"][2] = LoadTexture("../assets/stone/stone4.png");
        textures["stone"][3] = LoadTexture("../assets/stone/stone8.png");
        textures["stone"][4] = LoadTexture("../assets/stone/stone2.png");
        textures["stone"][5] = LoadTexture("../assets/stone/stone9.png");
        textures["stone"][6] = LoadTexture("../assets/stone/stone7.png");
        textures["stone"][7] = LoadTexture("../assets/stone/stone5.png");
        textures["stone"][8] = LoadTexture("../assets/stone/stone6.png");
        textures["tree"][0] = LoadTexture("../assets/trees/mytree.png");
        textures["mob"][0]=LoadTexture("../assets/mob/slime.png");
        textures["weapon"][0]=LoadTexture("../assets/weapons/sword1.png");

        mapset mapset;
        string tile_name[textures.size()];
        tile_name[0] = "grass";
        tile_name[1] = "stone";
        tile_name[2] = "tree";
        tile_name[3]="mob";
          tile_name[4]="weapon";



        Texture2D current_tile_img;
        int current_image = 0;
        int current_tile = 0;
        int variant = 0;
        bool tile_shift = false;
        bool mode_shift = false;
        bool grid_on = false;
        int time;

        SetTargetFPS(60);
        while (!WindowShouldClose())
        {

            current_tile_img = textures[tile_name[current_image]][variant];

            mapset.tiles[current_tile].variant = variant;
            mapset.tiles[current_tile].texture = current_tile_img;
            mapset.tiles[current_tile].type = tile_name[current_image];

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

            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !mode_shift)
            {

                mapset.tiles[current_tile].position.width = current_tile_img.width;
                mapset.tiles[current_tile].position.height = current_tile_img.height;
                mapset.tiles[current_tile].used = true;
                mapset.tiles[current_tile].ongrid = "true";
                int x = (int)(GetScreenToWorld2D(GetMousePosition(), camera.camera).x / 40);
                int y = (int)(GetScreenToWorld2D(GetMousePosition(), camera.camera).y / 40);

                mapset.tiles[current_tile].position.x = grid[x][y].y;
                mapset.tiles[current_tile].position.y = grid[x][y].x + 40 - mapset.tiles[current_tile].position.height;
                if (current_tile > 0)
                {
                    for (int i = 0; i < mapset.tiles.size(); i++)
                    {
                        for (int j = 0; j < mapset.tiles.size(); j++)
                        {
                            if (mapset.tiles[i].used)
                            {
                                if (i != j)
                                {
                                    bool layered = CheckCollisionRecs(mapset.tiles[i].position
                , mapset.tiles[j].position
                );

                                    if (layered)
                                    {

                                        mapset.tiles[j].position
                    .x = mapset.tiles[i].position
                    .x + 40;
                                    }
                                }
                            }
                        }
                    }
                }

                current_tile = (current_tile + 1);
            }
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && mode_shift)
            {

                mapset.tiles[current_tile].position.width = current_tile_img.width;
                mapset.tiles[current_tile].position.height = current_tile_img.height;
                mapset.tiles[current_tile].used = true;
                mapset.tiles[current_tile].ongrid = "false";
                mapset.tiles[current_tile].position.x = GetScreenToWorld2D(GetMousePosition(), camera.camera).x;

                mapset.tiles[current_tile].position.y = GetScreenToWorld2D(GetMousePosition(), camera.camera).y;
                current_tile = (current_tile += 1);
            }

            if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
            {
                for (int i = 0; i < mapset.tiles.size(); i++)
                {
                    if (mapset.tiles[i].used)
                    {

                        if (CheckCollisionPointRec(GetScreenToWorld2D(GetMousePosition(), camera.camera), mapset.tiles[i].position
    ))
                        {

                            mapset.tiles.extract(i);
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
                current_image = (current_image + 1) % textures.size();
            }
            if (IsKeyPressed(KEY_N) && !tile_shift)
            {

                variant = (variant + 1) % textures[tile_name[current_image]].size();
            }
            if (IsKeyPressed(KEY_O))
            {
                mapset.save_file();
            }
            if (IsKeyPressed(KEY_G))
            {
                grid_on = !grid_on;
            }
            if (IsKeyPressed(KEY_X))
            {
                mapset.autosort(textures,mapset.tilenumber);
            }
            if(IsKeyPressed(KEY_R))
            {
                cout<<"enter file to be removed";
                string filename;
                cin>>filename;
                 string name="../assets/maps/"+filename;
                 cout<<name;
                 if(remove(name.c_str())!=0)
                 {
                    cout<<"file not deleted";
                 }

               
            }
            if (IsKeyPressed(KEY_E))
            {
                string name;
                cout<<"enter map name";
                cin>>name;
                mapset.edit_map(name,textures);
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
                        DrawTexture(mapset.tiles[i].texture, mapset.tiles[i].position
    .x, mapset.tiles[i].position
    .y, RAYWHITE);
                    }
                }
            }
            for (int i = 0; i < sizeof(mapset.tiles) / sizeof(tile); i++)
            {
                if (mapset.tiles[i].used == true)
                {
                    if (mapset.tiles[i].ongrid == "false")
                    {
                        DrawTexture(mapset.tiles[i].texture, mapset.tiles[i].position
    .x, mapset.tiles[i].position
    .y, RAYWHITE);
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