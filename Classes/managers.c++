#include "root_classes.h"

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
spawner spawn_manager::update(spawner spawners, map<string, map<int, Texture2D>> textures)
{
    if (spawners.mob_count >= spawners.max_mob_count)
    {

        return spawners;
    }

    spawners.spawn_timer += 1;
    if (spawners.spawn_timer > spawners.spawn_interval)
    {
        auto temp=make_unique<mob>();

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

            temp->my_type=temp->dynamic_object;
            
           mobs.push_back(std::move(temp));

        spawners.spawn_timer = 0;
        spawners.mob_count++;
    }

    return spawners;
}
void spawn_manager::update_mobs(unordered_map<pair<int, int>, bool, pair_hash> grid, collision_detector *detect)
{
    // where all the actualization of the individual mob logic occurs
    for (int i = 0; i < mobs.size(); i++)
    {
         mobs[i]->act(grid);
         detect->update(mobs[i].get());
    }
    cout<<"breaking after update"<<"\n";
    for (int i = 0; i < mobs.size(); i++)
    {
      
       detect->check_adjacent(mobs[i].get());
    }
    cout<<"breaking after ptential collidion detection"<<"\n";
    

}
void spawn_manager::draw_mobs()
{
    for (int i = 0; i < mobs.size(); i++)
    {

        DrawTexture(mobs[i]->texture, mobs[i]->collider.x, mobs[i]->collider.y, RAYWHITE);
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
    if(prev_x<0)
    {
        prev_x=0;
    }
    if(prev_y<0)
    {
        prev_y=0;
    }if(x<0)
    {
        x=0;
    }
    if(y<0)
    {
        y=0;
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

        dynamic_obj* dyn_obj_first =dynamic_cast<dynamic_obj*>(entity);
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

        // add checking for non rigid_body tiles
        if (CheckCollisionRecs(entity_pair.first->collider, entity_pair.second->collider))
        {
               if(entity_pair.first->my_type==entity_pair.first->dynamic_object&&entity_pair.second->my_type==entity_pair.second->dynamic_object)
             {
                 dynamic_obj* dyn_obj_first =dynamic_cast<dynamic_obj*>(entity_pair.first);
                dynamic_obj* dyn_obj_sec = dynamic_cast<dynamic_obj*>(entity_pair.second);
              


                if (dyn_obj_first && dyn_obj_sec)
                {

                    // decide whether i should generate a force or just move the objects
                    float x = dyn_obj_first->force.x;
                    float y = dyn_obj_first->force.y;
                    int displacement = 20;
                    dyn_obj_first->collider.x += (-displacement * x);
                    dyn_obj_first->collider.y += (-displacement * y);
                  
                    cout<<endl;
                    mob* derived_ptr = static_cast<mob*>(dyn_obj_first);
                    derived_ptr->current_state=derived_ptr->idle;
                    
                    float x_sec = dyn_obj_sec->force.x;
                    float y_sec = dyn_obj_sec->force.y;

                    dyn_obj_sec->collider.x += (-displacement * x_sec);
                    dyn_obj_sec->collider.y += (-displacement * y_sec);
                   
                    mob* derived_ptr_sec = static_cast<mob*>(dyn_obj_sec);
                    derived_ptr_sec->current_state=derived_ptr_sec->idle;
                }
                else
                {
                    std::cout << "One of the objects is not dynamic." << std::endl;
                }
            
        }
    }
}
    
}

// world manager implementation

void world_manager::manage(map<string, map<int, Texture2D>> textures)
{
    switch (current_State)
    {

    case loading:
        cout << "'loading ......" << "\n";
        set.loadmap(textures, "test_map");
        for (int i = 0; i < set.spawners.size(); i++)
        {
            set.spawners[i] = mob_manager.prepare_spawner(set.spawners[i]);
        }

        current_State = world_State::playing;
        mob_manager.mobs.reserve(1000);
        cout << "finished loading";
        break;
    case menu:
        break;
    case playing:

        for (int i = 0; i < set.spawners.size(); i++)
        {

            set.spawners[i] = mob_manager.update(set.spawners[i], textures);
        }

        mob_manager.update_mobs(set.tile_grid, &col_detector);

       col_manager.resolve_collisions(col_detector.potential_collisions);
        col_detector.potential_collisions.clear();

        cam.move_cam();
        set.drawmap();
        mob_manager.draw_mobs();

        break;
    case paused:
        break;
    }
}