#ifndef charachters
#define charachters
#include "raylib.h"
#include "maploader.hpp"
class items
{
   public:
   int number; 
};
class inventory
{  
    public:
    map<string,items> stored_items;
    int max_slots;
    void open_chest(){};

    

};
class chest:inventory
{
 public:
 chest()
 {
    max_slots=10;
 }

 
};
class player
{
public:
    Rectangle size;
    Texture2D face;
    Color look;
    string inventory[15];
    Vector2 direct = {0, 0};
    int speed = 2;
    bool isjumping = false;
    int inventory_number=0;

    player(float x, float y, float width, float height, Color look)
    {
        size = {x, y, width, height};
        this->look = look;
    }
    player(float x, float y, float width, float height, Texture2D look)
    {
        size = {x, y, width, height};
        face = look;
    }
    player()
    {

    }

    void moveplayer(mapset set)
    {

        if (IsKeyDown(KEY_LEFT))
        {
            direct.x = -speed;
            size.x += direct.x;
        }
        else if (IsKeyDown(KEY_RIGHT))
        {
            direct.x = speed;
            size.x += direct.x;
        }
        else
        {
            direct.x = 0;
        }
        for (int i = 0; i < max_tiles; i++)
        {
            if (set.tiles[i].used && (set.tiles[i].type=="grass"||set.tiles[i].type=="stone"))
            {
            
                bool collide = CheckCollisionRecs(set.tiles[i].placement, size);
                if (collide)
                {
                    if (direct.x > 0)
                    {
                        size.x = set.tiles[i].placement.x - size.width;
                        look = GREEN;
                    }
                    else
                    {
                        size.x = set.tiles[i].placement.x + set.tiles[i].placement.width;
                    }
                }
            }
        }

        if (IsKeyDown(KEY_SPACE) && !isjumping)
        {
            direct.y = -15;
            size.y += direct.y;
            isjumping = true;
        }
        direct.y++;
        if (direct.y > 7)
        {
            direct.y = 7;
        }
        size.y += direct.y;
        for (int i = 0; i < max_tiles; i++)
        {

            if (set.tiles[i].used && (set.tiles[i].type=="grass"||set.tiles[i].type=="stone"))
            {
                bool collide = CheckCollisionRecs(set.tiles[i].placement, size);
             

                if (collide)
                {
                    if (direct.y > 0)
                    {
                        size.y = set.tiles[i].placement.y - size.height;
                        isjumping = false;
                    }
                    else
                    {
                        size.y = set.tiles[i].placement.y + set.tiles[i].placement.height;
                    }
                }
            }
        }
        if (size.y > 800)
        {
            size.y *= -1;
        }
    }
    void drawplayer()
    {
        DrawRectangleRec(size, look);
    }
    
    void useitem()
    {
     if(inventory[inventory_number]=="medkit")
     {
        look=PURPLE;
        inventory[inventory_number]="";
     }
    }
    

};
class mod_cam
{

public:
    Camera2D camera;
    Vector2 direct;
    int direct_x=0;
    int direct_y=0;
    mod_cam(Vector2 target, Vector2 offset, int zoom, int rotation)
    {
        camera.target = target;
        camera.offset = offset;
        camera.rotation = rotation;
        camera.zoom = zoom;
    }
    mod_cam()
    {}

    void update_position(player player)
    {
        int distance_x = player.size.x - camera.target.x;
        int distance_y = player.size.y - camera.target.y;

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
        
            if(distance_y>0 )
           {  
             camera.target.y +=  player.direct.y;
                  
            }
            else
            {

                 camera.target.y-=2;
            }

            
        }
    }

 void move_cam()
 {  

    
      if (IsKeyDown(KEY_A))
        {
              direct_x=-1;
           camera.target.x-=1;
             

        }
        else if (IsKeyDown(KEY_D))
        {
            direct_x=1;
           camera.target.x+=1;
        
        }
         if (IsKeyDown(KEY_S))
        {
            direct_y=1;
          camera.target.y+=1;
        
        }
        else if (IsKeyDown(KEY_W))
        {
            direct_y=-1;
           camera.target.y-=1;
          
        }
        
 }

};
class enemy
{
public:
Rectangle position;
Texture2D design;
Color look;
std::string type;
int off_position;
int state;
enemy(Rectangle position,Texture2D design)
{
    this->position=position;
    this->design=design;
    off_position=position.x;
}
enemy()
{
    
}
void pathfind(player p,mod_cam cam,mapset set)
{
    Vector2 velocity;
    int distance_x=position.x-p.size.x;
    if(distance_x>=200)
 {   if(distance_x>0)
    {
      velocity.x=1;
      position.x+=velocity.x;
    }
    else
    {
         velocity.x=-1;
      position.x+=velocity.x;
        
    }
 }
 else
 {
    if(position.x<off_position+100)
    {
        position.x++;
    }
      if(position.x=off_position+100)
    {
       off_position=position.x;
    }
    if(position.x<off_position+100)
    {
        position.x--;
    }

    

 }
    for(int i=0;i<set.tiles.size();i++)
    {
        bool standing=CheckCollisionRecs(set.tiles[i].placement,position);
        
    }

}; 

};


#endif