#include "root_classes.h"
//spawner implementation
 void spawner:: prepare_spawner(string spawn_type, Rectangle spawn_area, float spawn_interval)
    {
        this->spawn_type = spawn_type;
        this->spawn_area = spawn_area;
        this->spawn_interval = spawn_interval;
        spawn_timer = 0;
        mob_count = 0;
    }
    void spawner:: set_spawnable_mob(nlohmann::json mob_data)
    {
        this->mob_data = mob_data;
    }

    //mobs implementation
     void mob:: set_self(
        Rectangle render_rec,
        Texture2D texture,
        Rectangle collider,
        int max_wander_box,
        int max_idle_time,
        float speed)
    {
        this->render_rec = render_rec;
        this->texture = texture;
        this->collider = collider;
        this->idle_timer = 0;
        this->max_wander_box = max_wander_box;
        this->max_idle_time = max_idle_time;
        this->speed = speed;
        current_state = states::wander;
        current = wander_states::target;
    }
    void mob:: act(unordered_map<pair<int, int>, bool, pair_hash> grid)
    {
        move_state_machine(grid);
        damage_collider.collider.x=collider.x+collider_offset;
        damage_collider.collider.y=collider.y+collider_offset;
    }
    void mob:: drawself()
    {
    }
    Vector2 mob:: move(Vector2 destination)
    {
        Vector2 directions[] =
            {{0, 1},
             {0.5, 0.5},
             {1, 0},
             {0.5, -0.5},
             {0, -1},
             {-0.5, -0.5},
             {-1, 0}};
        map<int, float> score;
        for (int j = 0; j < 8; j++)
        {
            float test_dis_x = collider.x + directions[j].x;
            float test_dis_y = collider.y + directions[j].y;

            float difference_x = test_dis_x - destination.x;
            float difference_y = test_dis_y - destination.y;

            score[j] = sqrt((pow(difference_x, 2) + pow(difference_y, 2)));
        }
        int best_direction_index = 0;
        for (int i = 1; i < 7; i++)
        {
            if (score[i] < score[best_direction_index])
            {

                best_direction_index = i;
            }
        }

        return directions[best_direction_index];
    }
    void mob::move_state_machine(unordered_map<pair<int, int>, bool, pair_hash> grid)
    {
        switch (current_state)
        {
        case wander:
        {

            switch (current)
            {
            case target:
            {

                int x = t_size * GetRandomValue(-max_wander_box, max_wander_box);
                int y = t_size * GetRandomValue(-max_wander_box, max_wander_box);
                destination = {(collider.x + x), (collider.y + y)};
                int verify_x = destination.x / t_size;
                int verify_y = destination.y / t_size;
                pair<int, int> test_pair = {verify_x, verify_y};
                if (grid.find(test_pair) != grid.end())
                {
                    current = wander_states::relocate;
                }
            }
            break;
            case relocate:
            {
                force = move(destination);
                prev_pos={collider.x,collider.y};
                collider.x += force.x;
                collider.y += force.y;
                double distance_x=pow((collider.x-destination.x),2);
                double distance_y=pow((collider.y-destination.y),2);
                if (distance_x < 5 && distance_y < 5)
                {
                    current = wander_states::change;
                }
            }
                break;
            case change:
                current = wander_states::target;
                current_state = states::attack;

                break;
            }
        }
        break;
        case idle:
            idle_timer++;
            if (idle_timer >= max_idle_time)
            {
                current_state = states::wander;
                idle_timer = 0;
            }

            break;
        case track:
            break;
        case attack:
             
            //add logic to trigger the attack that spawns the collider

            break;
        }
    }

void hitbox::time_out(float count)
{
    lifetime_timer+=count;
    if(lifetime_timer>=lifetime)
    {
       active=false;
       
    }
}
void hurtbox::take_damage(float damage_value)
{
      this->durability-=damage_value;

}