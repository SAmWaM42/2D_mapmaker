#include "root_classes.h"
#include "../json.hpp"
#include <iostream>
#include <map>
using namespace std;
#define RENDER_SCALE 2.0
using json = nlohmann::json;

int grid_tiles = 100;
int text_length = 16;
int tile_size = 40;

// spawn manager implementation
spawn_manager::spawn_manager()
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
spawner spawn_manager::prepare_spawner(spawner spawners)
{
    // add additional mob types here
    string mob_type = "";
    if (spawners.spawn_type == "slime_spawner")
    {
        std::cout << "Condition met. Setting mob data." << std::endl;
        mob_type = "slime";
    }
    else
    {
        std::cout << "Condition failed for: " << spawners.spawn_type << std::endl;
    }
    spawners.set_spawnable_mob(entity_data[mob_type]);
    spawners.spawn_interval = (GetRandomValue(1, entity_data[mob_type]["spawner_preset"]["spawn_interval"]));
    spawners.max_mob_count = entity_data[mob_type]["spawner_preset"]["max_mob_count"];

    return spawners;
}
void spawn_manager::despawn(std::unique_ptr<mob> &deadMob, int index)
{
    dead_pool[deadMob->typeId].push_back(move(deadMob));
    mobs.erase(mobs.begin() + index);
}
spawner spawn_manager::update(spawner spawners, map<string, map<int, Texture2D>> textures)
{
    if (spawners.mob_count >= spawners.max_mob_count)
    {

        return spawners;
    }
    spawners.spawn_timer += 1;
    if (spawners.spawn_timer > spawners.spawn_interval)
    {

        if (!dead_pool[spawners.mob_data["basic"]["typeId"]].empty())
        {
            auto temp = move(dead_pool[spawners.mob_data["basic"]["typeId"]].back());
            dead_pool[spawners.mob_data["basic"]["typeId"]].pop_back();
            mobs.push_back(std::move(temp));
            spawners.spawn_timer = 0;
            spawners.mob_count++;
            return spawners;
        }
        auto temp = make_unique<mob>();

        Rectangle rec = {
            spawners.spawn_area.x,
            spawners.spawn_area.y,
            spawners.mob_data["basic"]["height"],
            spawners.mob_data["basic"]["width"],
        };

        float speed = spawners.mob_data["basic"]["speed"];
        temp->set_self(
            rec, textures["slime"][spawners.mob_data["basic"]["texture"]],
            rec, spawners.mob_data["basic"]["observation"],
            (base_idle - (idle_modifier * speed)), speed);
        temp->typeId = spawners.mob_data["basic"]["typeId"];
        temp->collider_offset = spawners.mob_data["basic"]["damage_collider_offset"];
        temp->damage_collider.durability = spawners.mob_data["basic"]["health"];
        temp->damage_collider.collider.height = temp->collider.height;
        temp->damage_collider.collider.width = temp->collider.width;
        mobs.push_back(std::move(temp));

        spawners.spawn_timer = 0;
        spawners.mob_count++;
    }

    return spawners;
}
void spawn_manager::update_mobs(unordered_map<pair<int, int>, bool, pair_hash> grid, collision_detector *detect, collider_manager *box_manager)
{
    // where all the actualization of the individual mob logic occurs
    for (int i = 0; i < mobs.size(); i++)
    {
        mobs[i]->act(grid);
        if (mobs[i]->current_state == mobs[i]->attack)
        {
            box_manager->spawn_collider("bash", mobs[i].get());
            mobs[i]->current_state = mobs[i]->wander;
        }

        detect->update(mobs[i].get());
        if (mobs[i]->damage_collider.durability <= 0)
        {
            despawn(mobs[i], i);
        }
    }

    for (int i = 0; i < mobs.size(); i++)
    {

        detect->check_adjacent(mobs[i].get());
    }
}
void spawn_manager::draw_mobs()
{
    for (int i = 0; i < mobs.size(); i++)
    {
        mobs[i]->damage_collider.drawself();
        DrawTexture(mobs[i]->texture, mobs[i]->collider.x, mobs[i]->collider.y, RAYWHITE);
        // printf("collider values %f ,%f,%f,%f \n",mobs[i]->damage_collider.collider.x, mobs[i]->damage_collider.collider.y, mobs[i]->damage_collider.collider.width, mobs[i]->damage_collider.collider.height);
        DrawRectangleLines(mobs[i]->damage_collider.collider.x, mobs[i]->damage_collider.collider.y, mobs[i]->damage_collider.collider.width, mobs[i]->damage_collider.collider.height, BLACK);
    }
}
// camera manager implementation
mod_cam::mod_cam(Vector2 target, Vector2 offset, float zoom, int rotation)
{
    camera.target = target;
    camera.offset = offset;
    camera.rotation = rotation;
    camera.zoom = zoom;
}
mod_cam::mod_cam()
{
}

void mod_cam::update_position(Vector2 play, Vector2 velocity)
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

void mod_cam::move_cam()
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
    if (IsKeyDown(KEY_Z))
    {
        camera.zoom += 0.1;
    }
    if (IsKeyDown(KEY_B))
    {
        camera.zoom -= 0.1;
    }
    if (camera.zoom <= 0)
    {
        camera.zoom = 0;
    }
}
// collsion manager implementation

void collision_detector::set_entity_grid(vector<world_object> entities)
{
    for (auto &entity : entities)
    {
        entity_grid[int(entity.collider.x / grid_size)][int(entity.collider.y / grid_size)].push_back(&entity);
    }
}
void collision_detector::update(world_object *entity)
{
    int prev_x = (entity->prev_pos.x / grid_size);
    int prev_y = entity->prev_pos.y / grid_size;
    int x = entity->collider.x / grid_size;
    int y = entity->collider.y / grid_size;
    if (prev_x < 0)
    {
        prev_x = 0;
    }
    if (prev_y < 0)
    {
        prev_y = 0;
    }
    if (x < 0)
    {
        x = 0;
    }
    if (y < 0)
    {
        y = 0;
    }

    if (x == prev_x && y == prev_y)
    {
        return;
    }
    auto &grid_cell = entity_grid[prev_x][prev_y];

    // Find the entity's position in the vector
    for (int i = 0; i < grid_cell.size(); i++)
    {
        if (grid_cell[i] == entity)
        {
            // Swap with the last element and pop_back, which is faster
            grid_cell[i] = grid_cell.back();
            grid_cell.pop_back();
            break;
        }
    }

    // Add the entity to its new position

    dynamic_obj *dyn_obj_first = dynamic_cast<dynamic_obj *>(entity);
    entity_grid[int(entity->collider.x / grid_size)][int(entity->collider.y / grid_size)].push_back(entity);
}
void collision_detector::check_adjacent(world_object *entity)
{
    Vector2 checkable_positions[] =
        {
            {0, 0},
            {0, 1},
            {1, 1},
            {1, 0},
            {1, -1},
            {0, -1},
            {-1, -1},
            {-1, 0}};

    for (int i = 0; i < 8; i++)
    {
        int x = (entity->collider.x / grid_size) + checkable_positions[i].x;
        int y = (entity->collider.y / grid_size) + checkable_positions[i].y;
        if (x < 0 || y < 0)
        {
        }
        else
        {

            for (auto &obj : entity_grid[x][y])
            {

                if (obj != entity && obj)
                {

                    pair<world_object *, world_object *> temp = {entity, obj};
                    potential_collisions.push_back(temp);
                }
            }
        }
    }
}
void ::collision_manager::resolve_collisions(vector<pair<world_object *, world_object *>> potential_collisions)
{
    if (potential_collisions.size() == 0 || potential_collisions.empty())
    {
        return;
    }

    for (auto &entity_pair : potential_collisions)
    {

        if (CheckCollisionRecs(entity_pair.first->collider, entity_pair.second->collider))
        {

            dynamic_obj *dyn_obj_first = dynamic_cast<dynamic_obj *>(entity_pair.first);
            dynamic_obj *dyn_obj_sec = dynamic_cast<dynamic_obj *>(entity_pair.second);

            if (dyn_obj_first && dyn_obj_sec)
            {
                // mob *derived_ptr = static_cast<mob *>(dyn_obj_first);
                // mob *derived_ptr_sec = static_cast<mob *>(dyn_obj_sec);
                //  derived_ptr->current_state = derived_ptr->idle;
                //  derived_ptr_sec->current_state = derived_ptr_sec->idle;

                // decide whether i should generate a force or just move the objects
                float x = dyn_obj_first->force.x;
                float y = dyn_obj_first->force.y;
                float x_sec = dyn_obj_sec->force.x;
                float y_sec = dyn_obj_sec->force.y;
                int base_displacement = 40;
                float applied_x = (40 * abs(x - x_sec) / abs(x + x_sec));
                float applied_y = (40 * abs(y - y_sec) / abs(y + y_sec));

                dyn_obj_first->collider.y += (applied_x * x);
                dyn_obj_first->collider.x += (applied_y * y);
                dyn_obj_sec->collider.y += (applied_x * x_sec);
                dyn_obj_sec->collider.x += (applied_y * y_sec);
            }
            else
            {
                std::cout << "One of the objects is not dynamic." << std::endl;
            }
        }
    }
}

// world manager implementation
void collider_manager::prepare_self()
{
    fstream entity_file;
    entity_file.open("assets/Entity_data/attacks.json", ios::in);
    if (!entity_file.is_open())
    {
        throw runtime_error("Failed to open attacks.json");
    }
    attack_data = nlohmann::json::parse(entity_file);
    cout << "attack data loaded" << "\n"
         << attack_data;
}

void collider_manager::update(map<int, map<int, vector<world_object *>>> hurt_grid, float world_timer)
{

    for (int i = 0; i < hitboxes.size(); i++)
    {
        manage_hits(hurt_grid, hitboxes[i].get());

        hitboxes[i]->lifetime_timer += world_timer;
        if (hitboxes[i]->lifetime_timer >= hitboxes[i]->lifetime)
        {
            hitboxes.erase(hitboxes.begin() + i);
        }
    }
}
void collider_manager::render_hitboxes()
{
    for (auto &obj : hitboxes)
    {
        DrawRectangleRec(obj->collider, GREEN);
    }
}
void collider_manager::spawn_collider(string attack, world_object *attacker)
{

    Vector2 attack_distance = {attack_data[attack]["distance"]["x"], attack_data[attack]["distance"]["y"]};

    float positionX;
    float positionY;

    if ((attacker->collider.x - attacker->prev_pos.x) > 0)
    {
        positionX = attacker->collider.x + attack_distance.x;
    }
    else
    {
        positionX = attacker->collider.x - attack_distance.x;
    }
    if ((attacker->collider.y - attacker->prev_pos.y) > 0)
    {
        positionY = attacker->collider.y + attack_distance.y;
    }
    else
    {
        positionY = attacker->collider.y - attack_distance.y;
    }
    if (!positionX && !positionY)
    {
        cout << "reference error";
        return;
    }

    Rectangle col = {
        positionX, positionY,
        attack_data[attack]["AOE"]["width"], attack_data[attack]["AOE"]["height"]};

    auto temp = make_unique<hitbox>();
    temp->collider = col;
    temp->owner = attacker;
    temp->active = true;
    temp->name = attack;

    temp->lifetime = attack_data[attack]["lifetime"];
    temp->lifetime_timer = 0;
    hitboxes.push_back(move(temp));
}
void collider_manager::manage_hits(map<int, map<int, vector<world_object *>>> hurt_grid, hitbox *obj)
{

    int x = (obj->collider.x / t_size);
    int y = (obj->collider.y / t_size);
    int x_limit = ((obj->collider.x + obj->collider.width) / t_size);
    int y_limit = ((obj->collider.x + obj->collider.height) / t_size);

    for (int i = x; i < (x_limit * y_limit); i++)
    {
        for (int j = y; j < y_limit; j++)
        {
            vector<world_object *> wrldobj = hurt_grid[i][j];

            if (!wrldobj.empty())
            {

                for (auto &hurt : wrldobj)

                {

                    mob *newMob = dynamic_cast<mob *>(hurt);

                    if (newMob != dynamic_cast<mob *>(obj->owner) && CheckCollisionRecs(newMob->damage_collider.collider, obj->collider))
                    {
                        newMob->damage_collider.take_damage(attack_data[obj->name]["damage"]);
                    }
                }
            }
        }
    }
}
void world_manager::progress_time()
{
    world_timer += 0.1;
}
void world_manager::manage(map<string, map<int, Texture2D>> textures)
{
    switch (current_State)
    {
    case loading:
    {
        cout << "'loading ......" << "\n";

        set.loadmap(textures, "test_map");
        for (int i = 0; i < set.spawners.size(); i++)
        {
            set.spawners[i] = mob_manager.prepare_spawner(set.spawners[i]);
        }
        box_manager.prepare_self();
        current_State = world_State::playing;
        // mob_manager.mobs.reserve(1000);
        cout << "finished loading";
    }
    break;
    case menu:
        break;
    case playing:

        for (int i = 0; i < set.spawners.size(); i++)
        {

            set.spawners[i] = mob_manager.update(set.spawners[i], textures);
        }

        mob_manager.update_mobs(set.tile_grid, &col_detector, &box_manager);
        // add the code to pass the colliders here as well as code to ensure that the hurtbox collider positions are set and updated.

        box_manager.update(col_detector.entity_grid, world_timer);
        col_manager.resolve_collisions(col_detector.potential_collisions);
        col_detector.potential_collisions.clear();

        cam.move_cam();
        set.drawmap();
        mob_manager.draw_mobs();
        box_manager.render_hitboxes();

        progress_time();
        break;
    case paused:
        break;
    }
}
void world_manager::editor(map<string, map<int, Texture2D>> textures)
{

    
    bool loaded = false;
    int zoom = 1;
    int rotation = 0;
    int x = grid_tiles;
    int y = grid_tiles;
    Vector2 offset = {screenwidth / 2, screenheight / 2};
    Vector2 target = {screenwidth / 2, screenheight / 2};
    Vector2 grid[x][y];
    Rectangle menuList={cam.camera.target.x-(screenwidth / 4),cam.camera.target.y-(screenheight / 4),(screenwidth / 4),(screenwidth / 4)};
    string  entities[]={"slime"};
    string  spawners[]={"slime_spawner"};
    string  tiles[]={"grass"};
    Rectangle entitiesButton={menuList.x,menuList.y,70,50};
    Rectangle spawnersButton={menuList.x+entitiesButton.width,menuList.y,70,50};
    Rectangle tilesButton={menuList.x+spawnersButton.width,menuList.y,70,50};
    //create funnction to draw the textures in the menu with a rectangle to allow for collision checking
    

    for (int i = 0; i < grid_tiles; i++)
    {
        for (int j = 0; j < grid_tiles; j++)
        {
            grid[i][j].x = j * 40;
            grid[i][j].y = i * 40;
        }
    }

    string tile_name[textures.size()];
    cout << textures.size();
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

   
        current_tile_img = textures[tile_name[current_image]][variant];
        temp_tile.variant = variant;
        temp_tile.texture = current_tile_img;
        temp_tile.type = tile_name[current_image];

        if ((cam.camera.target.x - cam.camera.offset.x) < 0)
        {
            cam.camera.target.x += 1;
        }
        if ((cam.camera.target.y - cam.camera.offset.y) < 0)
        {
            cam.camera.target.y += 1;
        }
        cam.move_cam();

        // fix camera movement and improve block positioning ans assignment

        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && !mode_shift)
        {
            // grid placement

            temp_tile.position.width = current_tile_img.width;
            temp_tile.position.height = current_tile_img.height;
            temp_tile.used = true;
            temp_tile.ongrid = "true";
            int x = (int)(GetScreenToWorld2D(GetMousePosition(), cam.camera).x / 40);
            int y = (int)(GetScreenToWorld2D(GetMousePosition(), cam.camera).y / 40);

            temp_tile.position.x = grid[x][y].y;
            temp_tile.position.y = grid[x][y].x + 40 - temp_tile.position.height;
            bool layered = false;
            bool same_tile = false;
            if (current_tile > 0)
            {
                for (int i = 0; i < set.tiles.size(); i++)
                {
                    for (int j = 0; j < set.tiles.size(); j++)
                    {
                        if (set.tiles[i].used && set.tiles[i].ongrid == "true" && set.tiles[j].ongrid == "true")
                        {

                            if (i != j)
                            {
                                layered = CheckCollisionRecs(set.tiles[i].position, set.tiles[j].position);

                                if (layered)
                                {

                                    set.tiles.erase(set.tiles.begin() + j);
                                    same_tile = true;
                                }
                            }
                        }
                    }
                }
            }
            if (!same_tile)
            {
                set.tiles.push_back(temp_tile);
                current_tile++;
            }
            else
            {
                same_tile = false;
            }
        }
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && mode_shift)
        {
            // offgrid placement
            temp_tile.position.width = current_tile_img.width;
            temp_tile.position.height = current_tile_img.height;
            temp_tile.used = true;
            temp_tile.ongrid = "false";
            int x = (int)(GetScreenToWorld2D(GetMousePosition(), cam.camera).x / 40);
            int y = (int)(GetScreenToWorld2D(GetMousePosition(), cam.camera).y / 40);

            temp_tile.position.x = grid[x][y].y;
            temp_tile.position.y = grid[x][y].x + 40 - temp_tile.position.height;
            set.tiles.push_back(temp_tile);
            current_tile++;
            cout << current_tile;
        }

        if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
        {
            for (int i = 0; i < set.tiles.size(); i++)
            {
                if (set.tiles[i].used)
                {

                    if (CheckCollisionPointRec(GetScreenToWorld2D(GetMousePosition(), cam.camera), set.tiles[i].position))
                    {

                        set.tiles.erase((set.tiles.begin() + i));
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
            cout << current_image << "\n";
            cout << textures.size();

            current_image = ((current_image + 1) % (textures.size()));
        }
        if (IsKeyPressed(KEY_N) && !tile_shift)
        {
            // changing the variant of that specific tile type
            variant = ((variant + 1) % textures[tile_name[current_image]].size());
            if (variant >= textures[tile_name[current_image]].size())
            {
                variant = 0;
            }
        }
       
        if (IsKeyPressed(KEY_G))
        {
            // the grid to assist in tile placement
            grid_on = !grid_on;
        }
        if (IsKeyPressed(KEY_X))
        {
            // primitive tile sorting on placement
            set.autosort(textures, set.tiles.size());
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
        

        

}
