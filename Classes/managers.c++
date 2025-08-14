#include "root_classes.h"

//spawn manager implementation
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
    spawner spawn_manager:: prepare_spawner(spawner spawners)
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
    spawner spawn_manager:: update(spawner spawners, map<string, map<int, Texture2D>> textures)
    {
        if (spawners.mob_count >= spawners.max_mob_count)
        {

            return spawners;
        }

        spawners.spawn_timer += 1;
        if (spawners.spawn_timer > spawners.spawn_interval)
        {
            mob temp;

            Rectangle rec = {
                spawners.spawn_area.x,
                spawners.spawn_area.y,
                spawners.mob_data["basic"]["height"],
                spawners.mob_data["basic"]["width"],
            };

            float speed = spawners.mob_data["basic"]["speed"];
            temp.set_self(
                rec, textures["slime"][spawners.mob_data["basic"]["texture"]],
                rec, spawners.mob_data["basic"]["observation"],
                (base_idle - (idle_modifier * speed)), speed);

            mobs.push_back(temp);
            spawners.spawn_timer = 0;
            spawners.mob_count++;
            cout << "spawned mob here";
        }

        return spawners;
    }
    void spawn_manager::update_mobs(unordered_map<pair<int, int>, bool, pair_hash> grid)
    {
        // where all the actualization of the individual mob logic occurs
        for (int i = 0; i < mobs.size(); i++)
        {
            mobs[i].act(grid);
        }
    }
    void spawn_manager:: draw_mobs()
    {
        for (int i = 0; i < mobs.size(); i++)
        {

            DrawTexture(mobs[i].texture, mobs[i].collider.x, mobs[i].collider.y, RAYWHITE);
        }
    }
//camera manager implementation
mod_cam:: mod_cam(Vector2 target, Vector2 offset, float zoom, int rotation)
    {
        camera.target = target;
        camera.offset = offset;
        camera.rotation = rotation;
        camera.zoom = zoom;
    }
    mod_cam::mod_cam()
    {
    }

    void mod_cam:: update_position(Vector2 play, Vector2 velocity)
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

    void mod_cam:: move_cam()
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


 //world manager implementation

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
            cout << "finished loading";
            break;
        case menu:
            break;
        case playing:

            for (int i = 0; i < set.spawners.size(); i++)
            {

                set.spawners[i] = mob_manager.update(set.spawners[i], textures);
            }
            cam.move_cam();
            set.drawmap();
            mob_manager.update_mobs(set.tile_grid);
            mob_manager.draw_mobs();

            break;
        case paused:
            break;
        }
    }