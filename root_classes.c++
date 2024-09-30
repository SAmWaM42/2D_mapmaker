#ifndef root_classes
#define root_classes
#include "raylib.h"
#include <math.h>
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
int durability;
    Vector2 position;
    string name;
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
class Itemtype
{
    public:
    enum type
    {
        WEAPON,SPELL
    };
   
};


class Item : public world_object
{
    public:
    Itemtype type;
    int stack_size;
    Texture2D skin;
    int range;
    int knockback;
    int value_changed;
    int delay;
    bool active;
 
    Rectangle  attack(Rectangle position,int facing )
    {  
    
    }
    
    
};
class Weapon : public Item
{

public:
    enum variant
    {
        RANGED,
        MELEE
    };
    Weapon()
    {
        type.WEAPON;
    }
};
class physics_entity : public world_object
{
public:
    Vector2 velocity = {0, 0};
    Vector2 dimensions{0, 0};
    map<int,Item> inventory;
    Texture2D skin;
    int fram_counter;
    int speed;
    int facing;
    bool alive;
    void fall()
    {
        velocity.y += 1;
        if (velocity.y >= 7)
        {
            velocity.y = 7;
        }
    }
    void check_standing( map<int, tile> set)
    {
       //logic for tile collision here
    }
};
 float find_distance(Vector2 p,Vector2 position)
  {
     float dist_squared=pow(position.x-p.x,2)+pow(position.x-p.x,2);

     return sqrt(dist_squared);

  }
   
class enemy : public physics_entity
{
public:
bool idle=true;
  //enemy movement logic here
    void drawself()
    {
        DrawTexture(skin, position.x, position.y, RAYWHITE);
    }
   
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
    map<int, Item> items;
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





#endif