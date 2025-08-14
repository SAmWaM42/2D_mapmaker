#include "root_classes.h"
tile::tile()
{
    used = true;
}
map<string, map<int, Texture2D>> mapset::prepare_textures(map<string, map<int, Texture2D>> textures, string name, Texture2D set_text)
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
void mapset::loadmap(map<string, map<int, Texture2D>> textures, string name)
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
    cout << name;
    nlohmann::json map_data;

    map_data.dump();
    map_data = nlohmann::json::parse(map_file);

    nlohmann::json specific_data = map_data[name];
    specific_data.dump(4);

    for (auto const &tile_data : specific_data)
    {
        if (tile_data[0] == "slime_spawner")
        {

            Rectangle pos = Rectangle{tile_data[2][0],
                                      tile_data[2][1],
                                      tile_data[2][3],
                                      tile_data[2][2]};
            spawner temp;
            temp.prepare_spawner(tile_data[0], pos, 0);

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
                pair<int, int> new_pair = {(int(new_tile.position.x / t_size)), (int(new_tile.position.y / t_size))};
                tile_grid[new_pair] = true;
                tiles.push_back(new_tile);
            }
            catch (exception e)
            {
                cout << "blank tile : ghost tile present";
            }
        }
    }
    map_file.close();

    cout << tilenumber;
}

void mapset::save_file()
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
    nlohmann::json data_array = nlohmann::json::array();

    for (int i = 0; i < tiles.size(); i++)
    {
        if (tiles[i].used)
        {

            nlohmann::json tile_data = {
                tiles[i].type,
                tiles[i].variant,
                {tiles[i].position.x,
                 tiles[i].position.y,
                 tiles[i].position.height,
                 tiles[i].position.width,
                 tiles[i].ongrid}};
            data_array.push_back(tile_data);
        }
    }
    map_data[name] = data_array;

    maps << map_data.dump(4);
    all_map << "\n" + name;
    all_map.close();
    maps.close();
}
void mapset::autosort(map<string, map<int, Texture2D>> textures, int tilenumber)
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
                    if (grid_x_compare == grid_x + 1 && grid_y_compare == grid_y)
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

void mapset::drawmap()
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
void mapset::edit_map(string file_name, map<string, map<int, Texture2D>> textures)
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

    nlohmann::json specific_data = map_data[file_name];

    for (auto const &tile_data : specific_data)
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
            // ghost tiles
            cout << "error missing tiles";
        }
}

int mapset::get_tile_size(tile tile_in[])
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
int mapset::extract(string name, int var, bool keep)
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
