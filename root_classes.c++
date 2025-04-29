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

#define t_size 40
#define g_tiles 100

class modifier
{
public:
    float value;
    float active_time;
    bool permanent = false;
    bool finished = false;
    int timer = 0;

    void track()
    {
        timer++;
        if (timer >= active_time && !permanent)
        {
            finished = true;
        }
    }
    void set_modifier(float value, float active_time, bool permanent)
    {
        this->value = value;
        this->active_time = active_time;
        this->permanent = permanent;
    }
};
class Stat
{
public:
    float base_value;
    map<int, modifier> modifiers;
    float applied_value = 0;

    void set_base(float val)
    {
        base_value = val;
    }
    void add_modifier(float value, float timer, bool permanent)
    {

        modifiers[modifiers.size()].set_modifier(value, timer, permanent);
    }
    void update_mods()
    {
        for (int i = 0; i < modifiers.size(); i++)
        {
            modifiers[i].track();
            if (modifiers[i].finished)
            {
                base_value += modifiers[i].value;
                modifiers.erase(i);
            }
        }
    }
    void set_current_value()
    {
        for (int i = 0; i < modifiers.size(); i++)
        {
            if (modifiers[i].permanent)
            {
                applied_value += modifiers[i].value;
            }
        }
    }
    void act()
    {
        update_mods();
        set_current_value();
    }
};

class durability : public Stat
{
};
class world_object
{
public:
    durability health;
    Vector2 position;

    string name;
    virtual void act()
    {
        health.act();
        return;
    }
    virtual void drawself()
    {
        return;
    }
};

class attack : public Stat
{
    void attack_target(world_object *target)
    {
        target->health.applied_value -= this->applied_value;
    }
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
class Static_entity : public world_object
{
public:
    Vector2 dimensions{0, 0};
    Rectangle collider;
    map<string, Texture2D> anim_frames;
    Texture2D current_frame;
    int fram_counter;
};

class physics_entity : public world_object
{
public:
    attack attack;
    Vector2 velocity = {0, 0};
    Vector2 dimensions{0, 0};
    Rectangle collider;
    map<string, Texture2D> anim_frames;
    Texture2D current_frame;
    int fram_counter;
    int speed;
    int facing;
    bool alive;
};

class mapset
{
public:
    map<int, tile> tiles;
    map<int, tile> copy_tiles;
    int tilenumber = 0;
    //decide how i am going to store entities in the game i.e. as physics objects and static objects
    map<int, world_object> actors;
    int text_length = 16;
    int tile_size = 40;
    map<string, map<int, Texture2D>> prepare_textures(map<string, map<int, Texture2D>> textures, string name, Texture2D set_text)
    {

        Rectangle bounds = {0, 0, text_length, text_length};
        Image grass_text = LoadImageFromTexture(set_text);
        int text_num = (grass_text.height / text_length) * (grass_text.height / text_length);
        for (int i = 0; i < text_num; i++)
        {

            Image gt_copy = ImageCopy(grass_text);
            Image *gt_pointer = &gt_copy;
            ImageCrop(gt_pointer, bounds);
            ImageResize(gt_pointer, tile_size, tile_size);
            textures[name][i] = LoadTextureFromImage(gt_copy);
            bounds.x += text_length;
            if (bounds.x >= grass_text.width)
            {
                bounds.x = 0;
                bounds.y += text_length;
            }

            UnloadImage(gt_copy);
        }
        UnloadImage(grass_text);

        cout << text_num;
        return textures;
    }
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
                physics_entity temp;
                actors[i];
                temp.position = {pos.x, pos.y};
                temp.dimensions = {pos.width, pos.height};
                temp.current_frame = textures[map_data[to_string(i)][0]][map_data[to_string(i)][1]];
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

                try
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
                catch (exception e)
                {
                    cout << "error loading tile" << i;
                }
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
                // reduce the position of the tile to a grid number
                int grid_x = (int)tiles[i].position.x / 40;
                int grid_y = (int)tiles[i].position.y / 40;

                for (int j = 0; j < tilenumber; j++)
                {

                    if (tiles[j].used && i != j)
                    {
                        // reduce all other tiles to a  grid number  and set a booleann based on whether a tile is present or not
                        int grid_x_compare = (int)tiles[j].position.x / 40;
                        int grid_y_compare = (int)tiles[j].position.y / 40;

                        if (grid_x_compare == grid_x - 1 && grid_y_compare == grid_y)
                        {
                            tile_left = true;
                        }
                        if (grid_x_compare == grid_x + 1 & 7 && grid_y_compare == grid_y)
                        {
                            tile_right = true;
                        }
                        if (grid_y_compare == (grid_y - 1) && grid_x_compare == grid_x)
                        {
                            tile_up = true;
                        }
                        if (grid_y_compare == grid_y + 1 && grid_x_compare == grid_x)
                        {
                            tile_down = true;
                        }
                    }
                }

                // behaviour upon finding a tile in the below positions
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
                    tiles[i].variant = 6;
                }
                if (tile_up == true && tile_down == false && tile_left == true && tile_right == true)
                {
                    // bottom centre
                    tiles[i].variant = 7;
                }
                if (tile_up == true && tile_down == false && tile_left == true && tile_right == false)
                { // bottom right tile
                    tiles[i].variant = 8;
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
    int edit_map(string file_name, map<string, map<int, Texture2D>> textures)
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
        return tilenumber-1;
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

class State
{
public:
    enum State_type
    {
        initiate,
        active,
        paused,
        dead
    };

    enum State_type current;
    void update_State()
    {
    }
};
class wander : public State
{
public:
    int speed;
    int max_grid;
    Vector2 position;
    Vector2 target;
    void set_vals(int speed, int max_grid, Vector2 position)
    {

        this->position = position;
        this->max_grid = max_grid;
        this->speed = speed;
        current = initiate;
    }
    void update_State()
    {
        if (current == initiate)
        {
            target.x = position.x + (rand() % max_grid) * t_size;
            target.y = position.y + (rand() % max_grid) * t_size;
            current = active;
        }
        if (current == active)
        {
            float dif_x = target.x - position.x;

            float dif_y = target.y - position.y;

            if (abs(dif_x) > 0.2 || abs(dif_y) > 0.2)
            {
                float move_x = 0;
                float move_y = 0;

                if (abs(dif_x) > abs(dif_y))
                {
                    move_x = sin(dif_x);
                }
                else if (abs(dif_x) < abs(dif_y))
                {
                    move_y = sin(dif_x);
                }
                else
                {
                    move_x = sin(dif_x);
                    move_y = sin(dif_y);
                }

                position.x += move_x * speed;
                position.y += move_y * speed;
            }
            else
            {
                current = initiate;
            }
        }
    }
};

class entity : public physics_entity
{
public:
    wander wander_State;

    void initiate(int speed, int facing, Vector2 position, Vector2 dimensions, int max_wander_grid)
    {
        this->position = position;
        this->dimensions = dimensions;
        this->speed = speed;
        this->facing = facing;
        wander_State.set_vals(speed, max_wander_grid, position);
        alive = true;
    }
    void act() override
    {
        wander_State.update_State();

        if (alive)
        {
            position.x = wander_State.position.x;
            position.y = wander_State.position.y;
        }
        wander_State.position = position;
    }
};

class player_controller : public physics_entity
{
    void act()
    {
    }
    player_controller()
    {
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

class world_manager
{
public:
    mapset set;
    map<int, map<int, map<int, world_object *>>> object_locate;
    enum world_State
    {
        loading,
        menu,
        playing,
        paused

    };
    enum world_State current_State;
    static world_manager &getInstance()
    {
        static world_manager instance;
        return instance;
    }
    void manage()
    {
        switch (current_State)
        {
        case loading:
            break;
        case menu:
            break;
        case playing:
            break;
        case paused:
            break;
        }

        update_object_locate();
        set.drawmap();
    }

    void update_object_locate()
    {
        if (object_locate.size() == 0)
        {
            object_locate.clear();
        }

        for (int i = 0; i < set.actors.size(); i++)
        {
            int a = set.actors[i].position.x / t_size;
            int b = set.actors[i].position.y / t_size;

            object_locate[a][b][object_locate[a][b].size()] = &set.actors[i];
        }
    }
    void update_world_objects()
    {

    }
    // add behaviour management and someform of renderer
};
class world_renderer
{
};

#endif