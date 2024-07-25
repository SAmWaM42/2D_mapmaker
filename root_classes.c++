#ifndef root_classes
#define root_classes
#include "raylib.h"
#include "iostream"
#include <fstream>
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <map>
#include "json.hpp"
using namespace std;
#define screenheight 720
#define screenwidth 960

class world_object
{
public:
    Vector2 position;
    string name;
};

class button
{
public:
    Rectangle size;
    Texture2D texture;
    Color Color;
};
class tile
{
public:
    Rectangle position;
    Texture2D texture;
    string type;
    int variant;
    int breakable;
    bool used;
    string ongrid;
    tile()
    {
        used = false;
    }
};
class physics_entity : public world_object
{
public:
    Vector2 velocity = {0, 0};
    Vector2 dimensions{0, 0};
    Texture2D skin;
    int fram_counter;
    int speed;
    void fall()
    {
        velocity.y += 1;
        if (velocity.y >= 7)
        {
            velocity.y = 7;
        }
    }
};

class enemy : public physics_entity
{
public:
    void pathfind(Rectangle p, map<int, tile> set)
    {
        Vector2 velocity;
        int distance_x = p.x - position.x;
        int distance_y = p.y - position.y;

        if ((distance_x <= 300 && distance_x >= -300) && (distance_y <= 300 && distance_y >= -300))
        {

            if ((distance_x <= 200 && distance_x >= -200) && (distance_y <= 120 && distance_y >= -120))
            {

                if (distance_x > 0)
                {
                    velocity.x = 1;
                }
                else if (distance_x < 0)
                {
                    velocity.x = -1;
                }
                else
                {
                    velocity.x = 0;
                }
                position.x += velocity.x;
            }

            for (int i = 0; i < set.size(); i++)
            {
                if (set[i].position.x <= position.x + 60 && set[i].position.x >= position.x - 60 && set[i].position.y <= position.y + 60 && set[i].position.y >= position.y - 60)
                {
                    if (set[i].used && (set[i].type == "grass" || set[i].type == "stone"))
                    {

                        bool collide = CheckCollisionRecs(set[i].position, Rectangle{position.x, position.y, dimensions.x, dimensions.y});
                        if (collide)
                        {

                            if (velocity.x > 0)
                            {
                                position.x = set[i].position.x - dimensions.x;
                            }
                            else
                            {
                                position.x = set[i].position.x + set[i].position.width;
                            }
                        }
                    }
                }
            }

            fall();
            position.y += velocity.y;

            for (int i = 0; i < set.size(); i++)
            {
                if (set[i].position.x <= position.x + 60 && set[i].position.x >= position.x - 60 && set[i].position.y <= position.y + 60 && set[i].position.y >= position.y - 60)
                {
                    if (set[i].used && (set[i].type == "grass" || set[i].type == "stone"))
                    {
                        bool collide = CheckCollisionRecs(set[i].position, Rectangle{position.x, position.y, dimensions.x, dimensions.y});

                        if (collide)
                        {

                            position.y = set[i].position.y - dimensions.x;
                        }
                    }
                }
            }
        }
    }
    void drawself()
    {
        DrawTexture(skin, position.x, position.y, RAYWHITE);
    }
    void act(Rectangle p, map<int, tile> set)
    {
        pathfind(p, set);
        drawself();
    }
};
class tool : public world_object
{
};
class mod_cam
{

public:
    Camera2D camera;
    Vector2 velocity;
    int velocity_x = 0;
    int velocity_y = 0;
    mod_cam(Vector2 target, Vector2 offset, float zoom, int rotation)
    {
        camera.target = target;
        camera.offset = offset;
        camera.rotation = rotation;
        camera.zoom = zoom;
    }
    mod_cam()
    {
    }

    void update_position(Vector2 play, Vector2 velocity)
    {
        int distance_x = play.x - camera.target.x;
        int distance_y = play.y - camera.target.y;

        if (distance_x >= 200 || distance_x <= -200)
        {
            if (distance_x != 0)
            {
                if (distance_x > 0)
                {
                    camera.target.x += 2;
                }
                else
                {
                    camera.target.x += -2;
                }
            }
        }
        if (distance_y > 100 || distance_y < -100)
        {


            if (distance_y > 0)
            {
                camera.target.y += velocity.y;
            }
            else
            {

                camera.target.y -= 2;
            }
        }
    }

    void move_cam()
    {

        if (IsKeyDown(KEY_A))
        {
            velocity_x = -1;
            camera.target.x -= 1;
        }
        else if (IsKeyDown(KEY_D))
        {
            velocity_x = 1;
            camera.target.x += 1;
        }
        if (IsKeyDown(KEY_S))
        {
            velocity_y = 1;
            camera.target.y += 1;
        }
        else if (IsKeyDown(KEY_W))
        {
            velocity_y = -1;
            camera.target.y -= 1;
        }
    }
};

class mapset
{
public:
    map<int, tile> tiles;
    map<int, tile> copy_tiles;
    int tilenumber = 0;
    map<int, enemy> actors;
    map<int, tool> items;
    void loadmap(map<string, map<int, Texture2D>> textures, string name)
    {
        fstream map_file;
        try
        {
            map_file.open("assets/maps/" + name + ".json", ios::in);
        }
        catch (exception e)
        {
            cout << "filed to load file";
        }

        nlohmann::json map_data;
        map_data = nlohmann::json::parse(map_file);

        tilenumber = map_data["tile_number"];
        for (int i = 0; i < map_data["tile_number"]; i++)
        {
            if (map_data[to_string(i)][0] == "mob")
            {
                Rectangle pos = Rectangle{map_data[to_string(i)][2][0],
                                          map_data[to_string(i)][2][1],
                                          map_data[to_string(i)][2][3],
                                          map_data[to_string(i)][2][2]};
                enemy temp;
                actors[i];
                temp.position = {pos.x, pos.y};
                temp.dimensions = {pos.width, pos.height};
                temp.skin = textures[map_data[to_string(i)][0]][map_data[to_string(i)][1]];
                actors[i] = temp;
            }
            else if (map_data[to_string(i)][0] == "weapon")
            {

                Rectangle pos = Rectangle{map_data[to_string(i)][2][0],
                                          map_data[to_string(i)][2][1],
                                          map_data[to_string(i)][2][3],
                                          map_data[to_string(i)][2][2]};
            }
            else
            {
                tiles[i];
                tiles[i].type = map_data[to_string(i)][0];
                tiles[i].variant = map_data[to_string(i)][1];
                tiles[i].position.x = map_data[to_string(i)][2][0];
                tiles[i].position.y = map_data[to_string(i)][2][1];
                tiles[i].position.height = map_data[to_string(i)][2][2];
                tiles[i].position.width = map_data[to_string(i)][2][3];
                tiles[i].ongrid = map_data[to_string(i)][2][4];
                tiles[i].used = true;
                tiles[i].texture = textures[tiles[i].type][tiles[i].variant];
            }
        }
    }

    void save_file()
    {
        string name;
        cout << "enter map_name";
        cin >> name;
        fstream maps;
        fstream all_map;
        maps.open("../assets/maps/" + name + ".json", ios::out);
        all_map.open("../assets/maps/maps.txt", ios::out);

        if (!maps)
        {
            cout << "error opening file";
        }
        nlohmann::json map_data;

        for (int i = 0; i < tiles.size(); i++)
        {
            if (tiles[i].used)
            {

                map_data[to_string(tilenumber)] = {tiles[i].type, tiles[i].variant, {tiles[i].position.x, tiles[i].position.y, tiles[i].position.height, tiles[i].position.width, tiles[i].ongrid}};
                tilenumber++;
            }
        }
        map_data["tile_number"] = tilenumber;
        map_data["map_name"] = name;

        maps << map_data.dump();
        all_map << "\n" + name;
        all_map.close();
        maps.close();
    }
    void autosort(map<string, map<int, Texture2D>> textures, int tilenumber)
    {

        for (int i = 0; i < tilenumber; i++)
        {
            bool tile_left = false;
            bool tile_right = false;
            bool tile_up = false;
            bool tile_down = false;

            if (tiles[i].used)
            {
                int grid_x = (int)tiles[i].position.x / 40;
                int grid_y = (int)tiles[i].position.y / 40;

                for (int j = 0; j < tilenumber; j++)
                {
                    if (tiles[j].used && i != j)
                    {
                        int grid_x_compare = (int)tiles[j].position.x / 40;
                        int grid_y_compare = (int)tiles[j].position.y / 40;

                        if (grid_x_compare == grid_x - 1)
                        {
                            tile_left = true;
                        }
                        if (grid_x_compare == grid_x + 1)
                        {
                            tile_right = true;
                        }
                        if (grid_y_compare == (grid_y - 1))
                        {
                            tile_up = true;
                        }
                        if (grid_y_compare == grid_y + 1)
                        {
                            tile_down = true;
                        }
                    }
                }

                if (tile_up == false && tile_down == true && tile_left == false && tile_right == true)
                {
                    // left upper tile
                    tiles[i].variant = 0;
                }
                if (tile_up == false && tile_down == true && tile_left == true && tile_right == true)
                {

                    // middle upper tile
                    tiles[i].variant = 1;
                }
                if (tile_up == false && tile_down == true && tile_left == true && tile_right == false)
                {
                    // upper right tile
                    tiles[i].variant = 2;
                }
                if (tile_up == true && tile_down == true && tile_left == false && tile_right == true)
                { // middle left tile
                    tiles[i].variant = 3;
                }
                if (tile_up == true && tile_down == true && tile_left == true && tile_right == true)
                {
                    // true centre tile
                    tiles[i].variant = 4;
                }
                if (tile_up == true && tile_down == true && tile_left == true && tile_right == false)
                {
                    // middle right tile
                    tiles[i].variant = 5;
                }
                if (tile_up == true && tile_down == false && tile_left == false && tile_right == true)
                { // bottom left tile
                    tiles[i].variant = 3;
                }
                if (tile_up == true && tile_down == false && tile_left == true && tile_right == true)
                {
                    // bottom centre
                    tiles[i].variant = 4;
                }
                if (tile_up == true && tile_down == false && tile_left == true && tile_right == false)
                { // bottom right tile
                    tiles[i].variant = 5;
                }

                tiles[i].texture = textures[tiles[i].type][tiles[i].variant];
            }
        }
    }

    void drawmap()
    {
        for (int i = 0; i < tilenumber; i++)
        {
            if (tiles[i].ongrid == "true")
            {
                DrawTexture(tiles[i].texture, tiles[i].position.x, tiles[i].position.y, RAYWHITE);
            }
        }
        for (int i = 0; i < tilenumber; i++)
        {
            if (tiles[i].ongrid == "false")
            {
                DrawTexture(tiles[i].texture, tiles[i].position.x, tiles[i].position.y, RAYWHITE);
            }
        }
    }
    void edit_map(string file_name, map<string, map<int, Texture2D>> textures)
    {

        fstream map_file;
        try
        {
            map_file.open("../assets/maps/" + file_name + ".json", ios::in);
        }
        catch (exception e)
        {
            cout << "filed to load file";
        }
        nlohmann::json map_data;
        map_data = nlohmann::json::parse(map_file);

        tilenumber = map_data["tile_number"];
        for (int i = 0; i < map_data["tile_number"]; i++)
        {
            {
                tiles[i];
                tiles[i].type = map_data[to_string(i)][0];
                tiles[i].variant = map_data[to_string(i)][1];
                tiles[i].position.x = map_data[to_string(i)][2][0];
                tiles[i].position.y = map_data[to_string(i)][2][1];
                tiles[i].position.height = map_data[to_string(i)][2][2];
                tiles[i].position.width = map_data[to_string(i)][2][3];
                tiles[i].ongrid = map_data[to_string(i)][2][4];
                tiles[i].used = true;
                tiles[i].texture = textures[tiles[i].type][tiles[i].variant];
            }
        }
    }
    int get_tile_size(tile tile_in[])
    {
        int num = 0;
        for (int i = 0; i < 1000; i++)
        {

            if (tile_in[i].used == true)
            {
                num++;
            }
        }

        return num;
    }
    int extract(string name, int var, bool keep)
    {
        int max_tiles = tiles.size();
        map<int, tile> new_tile;
        int j = 0;
        for (int i = 0; i < max_tiles; i++)
        {
            if (keep == true)
            {
                if (tiles[i].type == name && tiles[i].variant == var)
                {

                    copy_tiles[j] = tiles[i];
                    j++;
                }
            }
            else
            {
                new_tile[j] = tiles[i];
                j++;
            }
        }

        if (keep == false)
        {
            for (int i = 0; i < j; i++)
            {
                tiles[i] = new_tile[i];
            }
            for (int i = 0; i < max_tiles; i++)
            {
                if (i > j)
                {
                    tiles[i].used = false;
                }
            }
        }

        return j;
    }
};
class player : public physics_entity
{
    bool isjumping = false;
    int delay_timer;
   

public:

    player(float x, float y, float width, float height, Texture2D texture)
    {
        position = {x, y};
        dimensions = {width, height};
        skin = texture;
        speed=2;
    }
    player()
    {
        speed=2;
    }

    void command_player(mapset set)
    {

        if (IsKeyDown(KEY_A))
        {
            velocity.x = -speed;
            
        }
        else if (IsKeyDown(KEY_D))
        {
            velocity.x = speed;
        }
        else
        {
            velocity.x = 0;
        }
        position.x += velocity.x;
      for (int i = 0; i<1000; i++)
        {
            if (set.tiles[i].used && (set.tiles[i].type == "grass" || set.tiles[i].type == "stone"))
            {
                Rectangle temp = {position.x, position.y, dimensions.x, dimensions.y};
                bool collide = CheckCollisionRecs(set.tiles[i].position, temp);
                if (collide)
                {
                    if (velocity.x > 0)
                    {
                        position.x = set.tiles[i].position.x - temp.width;
                    }
                    else
                    {
                        position.x = set.tiles[i].position.x + set.tiles[i].position.width;
                    }
                }
            }
        }

        if (IsKeyDown(KEY_W) && !isjumping)
        {
            velocity.y = -15;
            position.y += velocity.y;
            isjumping = true;
        }
        fall();
        position.y += velocity.y;
       for (int i = 0; i < 1000; i++)
        {

            if (set.tiles[i].used && (set.tiles[i].type == "grass" || set.tiles[i].type == "stone"))
            {
                Rectangle temp = {position.x, position.y, dimensions.x, dimensions.y};
                bool collide = CheckCollisionRecs(set.tiles[i].position, temp);

                if (collide)
                {
                    if (velocity.y > 0)
                    {
                        position.y = set.tiles[i].position.y - temp.height;
                        isjumping = false;
                    }
                    else
                    {
                        position.y = set.tiles[i].position.y + set.tiles[i].position.height;
                    }
                }
            }
        }
        if (position.y > 800)
        {
            position.y *= -1;
        }

        if (IsKeyPressed(KEY_SPACE))
        {
        }
    }
    void drawplayer()
    {
        DrawRectangle(position.x, position.y, dimensions.x, dimensions.y, RED);
    }
    void act(mapset set)
    {
        command_player(set);
        drawplayer();
    }
};
class main_menu
{
public:
    button buttons[3];
    const char text1[10] = "NEW GAME";
    const char text2[10] = "LOAD GAME";
    const char text3[10] = "QUIT";
    main_menu()
    {
        for (int i = 0; i < 3; i++)
        {
            buttons[i].size.width = 300;
            buttons[i].size.height = 70;
            buttons[i].size.x = screenheight / 2;
            buttons[i].size.y = i * (buttons[i].size.height + 20);
            buttons[i].Color = BLACK;
        }
    }
    void draw_menu()
    {

        for (int i = 0; i < 3; i++)
        {

            DrawRectangleRec(buttons[i].size, buttons[i].Color);
        }
        DrawText(text1, (buttons[0].size.x), (buttons[0].size.y), 50, WHITE);
        DrawText(text2, (buttons[1].size.x), (buttons[1].size.y), 50, WHITE);
        DrawText(text3, (buttons[2].size.x), (buttons[2].size.y), 50, WHITE);
    }
    void check_menu(int *world_state, mod_cam cam)
    {
        for (int i = 0; i < 3; i++)
        {
            bool click = CheckCollisionPointRec(GetScreenToWorld2D(GetMousePosition(), cam.camera), buttons[i].size);
            if (click)
            {
                buttons[i].Color = RED;
            }
            else
            {
                buttons[i].Color = BLACK;
            }
            if (click && IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) && i == 0)
            {
                *world_state = 3;
            }
            if (click && IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) && i == 1)
            {
                CloseWindow();
            }
            if (click && IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) && i == 2)
            {
                CloseWindow();
            }
        }
    }
};

class world
{
public:
    player Player;
    map<int, mapset> maps;
    mapset working_mapset;
    map<int, string> map_name;
    map<string, map<int, Texture2D>> textures;
    map<string, tool> item_list;
    mod_cam cam;
    int current_map = 0;
    int world_state = 0;
    int *state = &world_state;
    main_menu menu;
    world(player p, mod_cam cam)
    {
        Player = p;
        this->cam = cam;
    }
    void LoadTextures()
    {
        map<int, Texture2D> innermap;
        textures["grass"][0] = LoadTexture("assets/grass/grass4.png");
        textures["grass"][1] = LoadTexture("assets/grass/grass1.png");
        textures["grass"][2] = LoadTexture("assets/grass/grass3.png");
        textures["grass"][3] = LoadTexture("assets/grass/grass9.png");
        textures["grass"][4] = LoadTexture("assets/grass/grass2.png");
        textures["grass"][5] = LoadTexture("assets/grass/grass8.png");
        textures["grass"][6] = LoadTexture("assets/grass/grass6.png");
        textures["grass"][7] = LoadTexture("assets/grass/grass7.png");
        textures["grass"][8] = LoadTexture("assets/grass/grass5.png");
        textures["stone"][0] = LoadTexture("assets/stone/stone3.png");
        textures["stone"][1] = LoadTexture("assets/stone/stone1.png");
        textures["stone"][2] = LoadTexture("assets/stone/stone4.png");
        textures["stone"][3] = LoadTexture("assets/stone/stone8.png");
        textures["stone"][4] = LoadTexture("assets/stone/stone2.png");
        textures["stone"][5] = LoadTexture("assets/stone/stone9.png");
        textures["stone"][6] = LoadTexture("assets/stone/stone7.png");
        textures["stone"][7] = LoadTexture("assets/stone/stone5.png");
        textures["stone"][8] = LoadTexture("assets/stone/stone6.png");
        textures["tree"][0] = LoadTexture("assets/trees/mytree.png");
        textures["particles"][0] = LoadTexture("assets/particles/leaf.png");
        textures["mob"][0] = LoadTexture("assets/mob/slime.png");
        textures["weapon"][0] = LoadTexture("assets/weapons/sword1.png");
    }
    void prepare_maps()
    {
        fstream map_names("assets/maps/maps.txt", ios::in);
        if (!map_names)
        {
            cout << "not opened";
        }
        int counter = 0;

        // loop not loading all the maps  issue with this part;

        while (getline(map_names, map_name[counter]))
        {
            counter++;
        }

        for (int i = 0; i < map_name.size(); i++)
        {
            if (map_name[i] != "")
            {
                maps[i].loadmap(textures, map_name[i]);
            }
        }

        
    }
    string select_map()
    {
        string map_to_load = "";

        if (IsKeyPressed(KEY_RIGHT))
        {
            current_map = (current_map + 1) % maps.size();
        }

        if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
        {
            map_to_load = to_string(current_map);
        }

        return map_to_load;
    }

    void main_menu()
    {
       
        menu.check_menu(state, cam);
    }
    void draw_menu()
    {
        menu.draw_menu();

    }
    void pause_menu()
    {
        Rectangle board = {0, 0, 600, 400};
        Color but_col[2] = {RED, RED};
        Rectangle buttons = {0, 0, 60, 20};
        Rectangle quit = {0, 0, 60, 20};

        board.x = cam.camera.target.x - (board.width / 2);
        board.y = cam.camera.target.y - (board.width / 2);
        buttons.x =
            buttons.y = board.y + 250;
        quit.x = board.x + 400;
        quit.y = board.y + 250;

        DrawRectangleRec(board, BLACK);
        DrawRectangleRec(buttons, but_col[0]);
        DrawRectangleRec(quit, but_col[1]);
        DrawText("PAUSED", board.x + 60, board.y + 60, 60, RED);
        DrawText("Continue", buttons.x, buttons.y, 15, WHITE);
        DrawText("Quit", quit.x, quit.y, 15, WHITE);

        if (CheckCollisionPointRec(GetScreenToWorld2D(GetMousePosition(), cam.camera), buttons))
        {
            but_col[0] = GREEN;
            if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
            {
                world_state = 1;
            }
        }
        else
        {
            but_col[0] = RED;
        }

        if (CheckCollisionPointRec(GetScreenToWorld2D(GetMousePosition(), cam.camera), quit))
        {
            but_col[1] = GREEN;
            if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
            {
                CloseWindow();
            }
        }
        else
        {
            but_col[1] = RED;
        }
    }
    void run_game()
    {
        cam.update_position(Player.position, Player.velocity);

        Player.act(working_mapset);

        for (int i = 0; i <  working_mapset.actors.size(); i++)
        {
            working_mapset.actors[i].act({Player.position.x, Player.position.y, Player.dimensions.x, Player.dimensions.y}, working_mapset.tiles);
        }

    }
    void draw_game()
    {
        working_mapset.drawmap();

        for (int i = 0; i < maps[current_map].actors.size(); i++)
        {
            working_mapset.actors[i].drawself();
        }
        Player.drawplayer();
    }
    void run_world()
    {
        if (world_state == 0)
        {
            main_menu();
            draw_menu();
        }
        else if (world_state == 1)
        {
            run_game();
            draw_game();
        }
        else if (world_state == 2)
        {
            pause_menu();
        }
        else if (world_state == 3)
        {
            if (select_map() != "")
            {
                current_map = stoi(select_map());
                cout<<current_map;
                working_mapset=maps[current_map];
                world_state = 1;
            }
            DrawRectangleRec({screenheight / 2, screenheight / 2 ,400, 300}, RED);
            DrawText(map_name[current_map].c_str(), screenheight / 2, screenheight / 2, 15, BLACK);
        }
    }
};

#endif