#ifndef maploader
#define maploader
#include "raylib.h"
#include "iostream"
#include <fstream>
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <map>
#include "json.hpp"

using namespace std;
#define max_tiles 1000
#define tile_size 40
#define screenheight 720
using json = nlohmann::json;

class tile
{

public:
    Rectangle placement;
    string type;
    Texture2D look;
    int variant;
    int breakable;
    bool used;
    string ongrid;
    tile()
    {
        used = false;
    }
};

class mapset
{

public:
    map<int, tile> tiles;
    int tilenumber = 0;
    map<int, tile> copy_tiles;

    void load_map(map<string, map<int, Texture2D>> textures, string name)
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
        json map_data;
        map_data = json::parse(map_file);

        tilenumber = map_data["tile_number"];
        for (int i = 0; i < map_data["tile_number"]; i++)
        {
            if (map_data[to_string(i)][0] != "mob")
            {
                tiles[i];
                tiles[i].type = map_data[to_string(i)][0];
                tiles[i].variant = map_data[to_string(i)][1];
                tiles[i].placement.x = map_data[to_string(i)][2][0];
                tiles[i].placement.y = map_data[to_string(i)][2][1];
                tiles[i].placement.height = map_data[to_string(i)][2][2];
                tiles[i].placement.width = map_data[to_string(i)][2][3];
                tiles[i].ongrid = map_data[to_string(i)][2][4];
                tiles[i].used = true;
                tiles[i].look = textures[tiles[i].type][tiles[i].variant];
            }
            else
            {
                enemy new_en(Rectangle{map_data[to_string(i)][2][0],
                                       map_data[to_string(i)][2][1],
                                       map_data[to_string(i)][2][2], 
                                       map_data[to_string(i)][2][3]},
                             textures[map_data[to_string(i)][0]][map_data[to_string(i)][1]]);
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
        maps.open("../assets/maps/" + name+".json", ios::out);
        all_map.open("../assets/maps/maps.txt", ios::out);

        if (!maps)
        {
            cout << "error opening file";
        }
        json map_data;

        for (int i = 0; i < max_tiles; i++)
        {
            if (tiles[i].used)
            {

                map_data[to_string(tilenumber)] = {tiles[i].type, tiles[i].variant, {tiles[i].placement.x, tiles[i].placement.y, tiles[i].placement.height, tiles[i].placement.width, tiles[i].ongrid}};
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
    void autosort(map<string, map<int, Texture2D>> textures)
    {

        for (int i = 0; i < max_tiles; i++)
        {
            bool tile_left = false;
            bool tile_right = false;
            bool tile_up = false;
            bool tile_down = false;

            if (tiles[i].used)
            {
                int grid_x = (int)tiles[i].placement.x / 40;
                int grid_y = (int)tiles[i].placement.y / 40;

                for (int j = 0; j < max_tiles; j++)
                {
                    if (tiles[j].used && i != j)
                    {
                        int grid_x_compare = (int)tiles[j].placement.x / 40;
                        int grid_y_compare = (int)tiles[j].placement.y / 40;

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

                tiles[i].look = textures[tiles[i].type][tiles[i].variant];
            }
        }
    }

    void drawmap()
    {
        for (int i = 0; i < tilenumber; i++)
        {
            if (tiles[i].ongrid == "true")
            {
                DrawTexture(tiles[i].look, tiles[i].placement.x, tiles[i].placement.y, RAYWHITE);
            }
        
        }
        for (int i = 0; i < tilenumber; i++)
        {
            if (tiles[i].ongrid == "false")
            {
                DrawTexture(tiles[i].look, tiles[i].placement.x, tiles[i].placement.y, RAYWHITE);
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
        json map_data;
        map_data = json::parse(map_file);

        tilenumber = map_data["tile_number"];
        for (int i = 0; i < map_data["tile_number"]; i++)
        {
            {
                tiles[i];
                tiles[i].type = map_data[to_string(i)][0];
                tiles[i].variant = map_data[to_string(i)][1];
                tiles[i].placement.x = map_data[to_string(i)][2][0];
                tiles[i].placement.y = map_data[to_string(i)][2][1];
                tiles[i].placement.height = map_data[to_string(i)][2][2];
                tiles[i].placement.width = map_data[to_string(i)][2][3];
                tiles[i].ongrid = map_data[to_string(i)][2][4];
                tiles[i].used = true;
                tiles[i].look = textures[tiles[i].type][tiles[i].variant];
            }
        }
    }
    int get_tile_size(tile tile_in[])
    {
        int num = 0;
        for (int i = 0; i < max_tiles; i++)
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
class map_select
{
public:
    map<int, string> maps;
    map<int, Rectangle> buttons;
    Color looks[4] = {RED, BLUE, GREEN, YELLOW};
    int current_map = 0;
    map_select()
    {

        fstream map_names("assets/maps/maps.txt", ios::in);
        if (!map_names)
        {
            cout << "not opened";
        }
        int counter = 0;

        // loop not loading all the maps  issue with this part;

        while (getline(map_names, maps[counter]))
        {
            counter++;
        }

        for (int i = 0; i < maps.size(); i++)
        {
            buttons[i].height = 100;
            buttons[i].width = 500;
            buttons[i].x = 680 / 2;
            buttons[i].y = screenheight / 2;
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
            map_to_load = maps[current_map];
        }

        return map_to_load;
    }

    void render()
    {

        DrawRectangleRec(buttons[current_map], looks[current_map]);
        DrawText(maps[current_map].c_str(), screenheight / 2, screenheight / 2, 15, BLACK);
    }
};

#endif