#ifndef attacks
#define attacks
#include "root_classes.c++"

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
class projectile : public world_object
{
    public:
    Vector2 velocity;
    Rectangle hit_box;
    Texture2D skin;
    Vector2 deceleraion = {0, 0};
    int distance_travelled = 0;
    int distance_modif=0;
    int active = false;
    projectile(Rectangle position)
    {
        hit_box=position;
    }
    void update_projectile(int damage, int fall_off, int speed, map<int, enemy> *enemies)
    {

         if (active)
        {
        velocity.x = speed * deceleraion.x;
        distance_travelled += speed;
        velocity.y = 1 + deceleraion.y;
        if ((distance_travelled >= fall_off && speed > 0) || (distance_travelled <= fall_off && speed < 0))
        {
            deceleraion.y = 0.5*(pow(0.5,distance_modif));
            deceleraion.x = 0.3+(0.2*distance_modif);
            distance_modif=1;
        }
        if(velocity.x<=0.4)
        {
            active=false;
        }

        hit_box.x += velocity.x;
        if (deceleraion.y != 0)
        {
        hit_box.y += velocity.y;
        }
       
            for (auto &cur_enemy : *enemies)
            {
                enemy &enemy = cur_enemy.second;
                double distance = find_distance({hit_box.x, hit_box.y}, enemy.position);
                if (speed > 0)
                {
                    if (distance < hit_box.width + 40)
                    {
                        bool collide = CheckCollisionRecs({enemy.position.x, enemy.position.y, enemy.dimensions.x, enemy.dimensions.y}, hit_box);
                        if (collide)
                        {
                            active = false;
                            enemy.durability -= damage;
                            if (enemy.durability <= 0)
                            {
                                enemy.alive = false;
                            }
                        }
                    }
                }
            }
        }
    }
};

class Spell : public Item
{
public:
    Spell()
    {
        type.SPELL;
    }
};
class sword : public Weapon
{
    sword()
    {
        knockback = 10;
        value_changed = 20;
        range = 80;
        skin = LoadTexture("assets/weapons/sword1.png");
        delay = 0.7f;
    }
    
    void attack(Rectangle position, int facing, map<int, enemy> *enemies)
    {
        Rectangle attack_rec;
        if (facing > 0)
        {
            attack_rec = (Rectangle){position.x + position.width, position.y, range, range};
        }
        else
        {
            attack_rec = (Rectangle){position.x + position.width, position.y, range, range};
        }

        for (auto &cur_enemy : *enemies)
        {
            enemy &enemy = cur_enemy.second;
            double distance = find_distance({position.x, position.y}, enemy.position);

            if (distance < 100)
            {

                bool collide = CheckCollisionRecs({enemy.position.x, enemy.position.y, enemy.dimensions.x, enemy.dimensions.y}, attack_rec);
                if (collide)
                {
                    enemy.durability -= value_changed;
                    if (enemy.durability <= 0)
                    {
                        enemy.alive = false;
                    }
                }
            }
        }
    }
};
class bow : public Weapon
{
    bow()
    {
        knockback = 8;
        value_changed = 12;
        range = 90;
        delay = 0.7f;
        
        // skin=LoadTexture("assets/weapons/sword1.png");
    }
    void attack(Rectangle position, int facing, map<int, enemy> *enemies)
    {
        projectile arrow(position);
        if (facing > 0)
        {
            
            arrow.update_projectile(value_changed,range,2.5,enemies);
        }
        else
        {
          
            arrow.update_projectile(value_changed,range,-2.5,enemies);
        }
    }
};
class fire_wall : public Spell
{
    fire_wall()
    {
        knockback = 15;
        value_changed = 50;
        range = 50;
        delay = 0.7f;
        // skin=LoadTexture("assets/weapons/sword1.png");
    }
    void attack(Rectangle position, int facing, map<int, enemy> *enemies)
    {

        Rectangle attack_rec = (Rectangle){position.x - (range / 2), position.y - (range / 2), position.width + (range / 2), position.height + (range / 2)};

        for (auto &cur_enemy : *enemies)
        {
            enemy &enemy = cur_enemy.second;
            double distance = find_distance({position.x, position.y}, enemy.position);

            if (distance < 100)
            {

                bool collide = CheckCollisionRecs({enemy.position.x, enemy.position.y, enemy.dimensions.x, enemy.dimensions.y}, attack_rec);
                if (collide)
                {
                    enemy.durability -= value_changed;
                    if (enemy.durability <= 0)
                    {
                        enemy.alive = false;
                    }
                }
            }
        }
    }
};
class ice_shard : public Spell
{
    ice_shard()
    {
        knockback = 8;
        value_changed = 30;
        range = 100;
        delay = 0.7f;
        // skin=LoadTexture("assets/weapons/sword1.png");
    }
    void attack(Rectangle position, int facing, map<int, enemy> *enemies)
    {
       
        projectile shard(position);
        if (facing > 0)
        {
          
            shard.update_projectile(value_changed,range,2.5,enemies);
        }
        else
        {
        
            shard.update_projectile(value_changed,range,-2.5,enemies);
        }
    
    }
};

#endif