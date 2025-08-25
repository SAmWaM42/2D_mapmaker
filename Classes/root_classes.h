#ifndef root_classes
#define root_classes
#include "raylib.h"
#include <math.h>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <map>
#include <list>
#include "json.hpp"
using namespace std;
#define screenheight 720
#define screenwidth 960

#define t_size 40
#define t_length 16

#define g_tiles 100

struct pair_hash
{
    template <class T1, class T2>
    std::size_t operator()(const std::pair<T1, T2> &p) const
    {
        auto h1 = std::hash<T1>{}(p.first);
        auto h2 = std::hash<T2>{}(p.second);
        // Simple way to combine hashes
        return h1 ^ (h2 << 1);
    }
};
class animation
{
    map<int,Texture2D> frames;
    

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
    tile();
};
class world_object
{
public:
    Rectangle render_rec;
    Vector2 prev_pos;
    Texture2D texture;
    Rectangle collider;
    
    //to be removed if has no use later
    enum obj_type{
        static_object ,
        dynamic_object
    };
    obj_type my_type;
   

    void act();
    virtual ~world_object() = default;
};
class dynamic_obj:public world_object
{
    public:
     Vector2 force;

};
class static_obj:public world_object

{

};


class mob : public dynamic_obj
{
public:
    int max_wander_box;
    int idle_timer;
    int max_idle_time;
    float speed;
    Vector2 destination;

    enum states
    {
        wander,
        track,
        attack,
        idle
    };
    enum wander_states
    {
        target,
        relocate,
        change
    };

    states current_state;
    wander_states current;
    void set_self(
        Rectangle render_rec,
        Texture2D texture,
        Rectangle collider,
        int max_wander_box,
        int max_idle_time,
        float speed);
    void act(unordered_map<pair<int, int>, bool, pair_hash> grid);

    void drawself();

    Vector2 move(Vector2 destination);

    void move_state_machine(unordered_map<pair<int, int>, bool, pair_hash> grid);
   
};

class spawner
{

public:
    string spawn_type;
    Rectangle spawn_area;
    float spawn_interval;
    float spawn_timer;
    nlohmann::json mob_data;
    int mob_count;
    int max_mob_count;

    void prepare_spawner(string spawn_type, Rectangle spawn_area, float spawn_interval);
    void set_spawnable_mob(nlohmann::json mob_data);
   
};

class mapset
{
public:
    vector<tile> tiles;
    vector<tile> copy_tiles;
    unordered_map<std::pair<int, int>, bool, pair_hash> tile_grid;
    int tilenumber = 0;
    vector<world_object> actors;
    vector<spawner> spawners;
    int text_length = t_length;
    int tile_size = t_size;
    map<string, map<int, Texture2D>> prepare_textures(
        map<string, map<int, Texture2D>> textures,
        string name,
        Texture2D set_text);

    void loadmap(
        map<string, map<int, Texture2D>> textures,
        string name);

    void save_file();

    void autosort(
        map<string, map<int, Texture2D>> textures,
        int tilenumber);

    void drawmap();

    void edit_map(
        string file_name,
        map<string, map<int,
                        Texture2D>>
            texture);

    int get_tile_size(tile tile_in[]);

    int extract(
        string name,
        int var,
        bool keep);
};
class mod_cam
{

public:
    Camera2D camera;
    Vector2 velocity;
    int velocity_x = 0;
    int velocity_y = 0;
    mod_cam(
        Vector2 target,
        Vector2 offset,
        float zoom,
        int rotation);

    mod_cam();

    void update_position(
        Vector2 play,
        Vector2 velocity);

    void move_cam();
};

class collision_detector{
    int grid_size=t_size;
    public:
    map<int,map<int,vector<world_object*>>> entity_grid;
    vector<pair<world_object*,world_object*>> potential_collisions;
    void set_entity_grid(vector<world_object> entities);
    void update(world_object* object);
    void check_adjacent(world_object* object);


};
class collision_manager
{
    public:
    void resolve_collisions(vector<pair<world_object*,world_object*>> potential_collisions);

};
class spawn_manager
{

public:
    vector<std::unique_ptr<mob>> mobs;
    int base_idle = 500;
    int idle_modifier = 75;
    nlohmann::json entity_data;
    spawn_manager();

    spawner prepare_spawner(spawner spawners);

    spawner update(
        spawner spawners,
        map<string, map<int, Texture2D>> textures);

    void update_mobs(
        unordered_map<pair<int, int>,
                      bool, pair_hash>
            grid,collision_detector* detect);

    void draw_mobs();
};
class world_manager
{
public:
    mapset set;
    spawn_manager mob_manager;
    collision_detector col_detector;
    collision_manager col_manager;
    int zoom = 1;
    int rotation = 0;

    Vector2 offset = {screenwidth / 2, screenheight / 2};
    Vector2 target = {screenwidth / 2, screenheight / 2};
    mod_cam cam{target, offset, zoom, rotation};

    enum world_State
    {
        loading,
        menu,
        playing,
        paused

    };
    enum world_State current_State;

    void manage(
        map<string,
            map<int, Texture2D>>
            textures);
};
class world_renderer
{
};

#endif