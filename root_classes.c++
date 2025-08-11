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
class world_object
{
public:
    Rectangle render_rec;
    Texture2D texture;
    Rectangle collider;
    Vector2 force;
    void act()
    {
    }
};
class mob : public world_object
{
public:
    void set_self(
        Rectangle render_rec,
        Texture2D texture,
        Rectangle collider)
    {
        this->render_rec = render_rec;
        this->texture = texture;
        this->collider = collider;
    }
    void act()
    {
    }
    void drawself()
    {
    }
};
class spawner
{

public:
    string spawn_type;
    Rectangle spawn_area;
    float spawn_interval;
    float spawn_timer;
    nlohmann::json mob_data;

    void prepare_spawner(string spawn_type, Rectangle spawn_area, float spawn_interval)
    {
        this->spawn_type = spawn_type;
        this->spawn_area = spawn_area;
        this->spawn_interval = spawn_interval;
        spawn_timer = 0;
    }
    void set_spawnable_mob(nlohmann::json mob_data)
    {
        this->mob_data = mob_data;
    }
};

class mapset
{
public:
    vector<tile> tiles;
    vector<tile> copy_tiles;
    int tilenumber = 0;
    vector<world_object> actors;
    vector<spawner> spawners;
    int text_length = 16;
    int tile_size = 40;
    map<string, map<int, Texture2D>> prepare_textures(map<string, map<int, Texture2D>> textures, string name, Texture2D set_text)
    {

        Rectangle bounds = {0, 0, text_length, text_length};
        Image grass_text = LoadImageFromTexture(set_text);
        int text_num = (grass_text.height / text_length) * (grass_text.height / text_length);
        cout << name << " " << text_num << "\n";
        for (int i = 0; i < text_num; i++)
        {

            Image gt_copy = ImageCopy(grass_text);
            Image *gt_pointer = &gt_copy;

            // not sure if i need this honestly
            if (text_num > 1)
            {
                ImageCrop(gt_pointer, bounds);
            }

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
        cout<<name;
        nlohmann::json map_data;

        map_data.dump();
        map_data = nlohmann::json::parse(map_file);

         nlohmann::json specific_data=map_data[name];
         specific_data.dump(4);

        for (auto const& tile_data : specific_data)
        {
            if (tile_data[0] == "slime_spawner")
            {
                
                Rectangle pos = Rectangle{tile_data[2][0],
                                          tile_data[2][1],
                                          tile_data[2][3],
                                          tile_data[2][2]};
                spawner temp;
                temp.prepare_spawner(tile_data[0], pos, (rand() % 100 + 1));

                spawners.push_back(temp);
            }

          
            else
            {

                try
                {

                    tile new_tile;
                    new_tile.type = tile_data[0];
                    new_tile.variant = tile_data[1];
                    new_tile.position.x = tile_data[2][0];
                    new_tile.position.y = tile_data[2][1];
                    new_tile.position.height = tile_data[2][2];
                    new_tile.position.width = tile_data[2][3];
                    new_tile.ongrid = tile_data[2][4];
                    new_tile.used = true;
                    new_tile.texture = textures[new_tile.type][new_tile.variant];

                    tiles.push_back(new_tile);
                }
                catch (exception e)
                {
                    cout << "blank tile : ghost tile present";
                   
                }
            }
        }
        map_file.close();

        /*

        for (int i = 0; i < tilenumber; i++)
        {
            if (tiles[i].used)
            {
                copy_tiles[copy_tiles.size()] = tiles[i];

            }
        }
        tilenumber = copy_tiles.size();
        tiles=copy_tiles;
        copy_tiles.clear();
        */
        cout << tilenumber;
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
        nlohmann::json data_array=nlohmann::json::array();

        for (int i = 0; i < tiles.size(); i++)
        {
            if (tiles[i].used)
            {

                nlohmann::json tile_data = {
                tiles[i].type,
                tiles[i].variant,
                {
                    tiles[i].position.x,
                    tiles[i].position.y,
                    tiles[i].position.height,
                    tiles[i].position.width,
                    tiles[i].ongrid
                }
            };
            data_array.push_back(tile_data);
            }
        }
          map_data[name] = data_array;
       

        maps << map_data.dump(4);
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
                        // reduce all other tiles to a  grid number  and set a boolean based on whether a tile is present or not
                        int grid_x_compare = (int)tiles[j].position.x / 40;
                        int grid_y_compare = (int)tiles[j].position.y / 40;

                        if (grid_x_compare == grid_x - 1 && grid_y_compare == grid_y)
                        {
                            tile_left = true;
                        }
                        if (grid_x_compare == grid_x + 1  && grid_y_compare == grid_y)
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
        for (int i = 0; i < tiles.size(); i++)
        {
            if (tiles[i].ongrid == "true")
            {
                DrawTexture(tiles[i].texture, tiles[i].position.x, tiles[i].position.y, RAYWHITE);
            }
        }
        for (int i = 0; i < tiles.size(); i++)
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

       nlohmann::json specific_data=map_data[file_name];

        for (auto const& tile_data : specific_data)
            try
            {
                    tile new_tile;
                    new_tile.type = tile_data[0];
                    new_tile.variant = tile_data[1];
                    new_tile.position.x = tile_data[2][0];
                    new_tile.position.y = tile_data[2][1];
                    new_tile.position.height = tile_data[2][2];
                    new_tile.position.width = tile_data[2][3];
                    new_tile.ongrid = tile_data[2][4];
                    new_tile.used = true;
                    new_tile.texture = textures[new_tile.type][new_tile.variant];

                    tiles.push_back(new_tile);
            }
            catch (exception e)
            {
               //ghost tiles
               cout<<"error missing tiles";
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
class spawn_manager
{

public:
    vector<mob> mobs;
    nlohmann::json entity_data;
    spawn_manager()
    {
        try
        {
            fstream entity_file;
            entity_file.open("assets/Entity_data/mob.json", ios::in);
            if (!entity_file.is_open())
            {
                throw runtime_error("Failed to open mob.json");
            }
            entity_data = nlohmann::json::parse(entity_file);
            cout << "entity data loaded" << "\n"
                 << entity_data;
        }
        catch (const exception &e)
        {
            cout << "Error loading mob data: " << e.what() << endl;
        }
    }
    spawner prepare_spawner(spawner spawners)
    {
        // add additional mob types here
      
        if (spawners.spawn_type == "slime_spawner")
        {
            std::cout << "Condition met. Setting slime data." << std::endl;
            spawners.set_spawnable_mob(entity_data["slime"]);
        }
        else
        {
            std::cout << "Condition failed for: " << spawners.spawn_type << std::endl;
        }

        return spawners;
    }
    spawner update(spawner spawners, map<string, map<int, Texture2D>> textures)
    {
        spawners.spawn_timer += 1;
        if (spawners.spawn_timer > spawners.spawn_interval)
        {
            mob temp;

            Rectangle rec = {
                             spawners.spawn_area.x, spawners.spawn_area.y,
                             spawners.mob_data["basic"]["height"], spawners.mob_data["basic"]["width"],};

            temp.set_self(rec, textures["slime"][spawners.mob_data["basic"]["texture"]], rec);

            mobs.push_back (temp);
            spawners.spawn_timer = 0;
            cout << "spawned mob here";
        }
        return spawners;
    }
    void update_mobs()
    {
         for (int i = 0; i < mobs.size(); i++)
        {
         mobs[i].collider.x+=1;
        }
    }
    void draw_mobs()
    {
        for (int i = 0; i < mobs.size(); i++)
        {
            DrawTexture(mobs[i].texture, mobs[i].collider.x, mobs[i].collider.y, RAYWHITE);
        }
    }
};

class world_manager
{
public:
    mapset set;
    map<int, map<int, map<int, world_object *>>> object_locate;
    spawn_manager mob_manager;

    enum world_State
    {
        loading,
        menu,
        playing,
        paused

    };
    enum world_State current_State;

    void manage(map<string, map<int, Texture2D>> textures)
    {
        switch (current_State)
        {

        case loading:
            set.loadmap(textures, "test_map");
            for (int i = 0; i < set.spawners.size(); i++)
            {
                set.spawners[i] = mob_manager.prepare_spawner(set.spawners[i]);
            }
            current_State = world_State::playing;
            cout<<"'loading ......";
            break;
        case menu:
            break;
        case playing:
             
            for (int i = 0; i < set.spawners.size(); i++)
            {
                set.spawners[i] = mob_manager.update(set.spawners[i], textures);
               
            }
            
            set.drawmap();
            mob_manager.update_mobs();
            mob_manager.draw_mobs();


            break;
        case paused:
            break;
        }
    }

    /*    update_object_locate();
        for(int i=0;i<set.spawners.size();i++)
        {
            spawn_manager.prepare_spawners(&set.spawners[i]);

        }
         for (int i = 0; i < set.spawners.size(); i++)
        {
            set.spawners[i].spawn();
        }
        for (int i = 0; i < set.spawners.size(); i++)
        {
            for(int j = 0; j < set.spawners[i].spawn_list.size(); j++)
            {

                set.spawners[i].spawn_list[j]->act();
                set.spawners[i].spawn_list[j]->drawself();
            }

        }

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
    */
};
class world_renderer
{
};

#endif