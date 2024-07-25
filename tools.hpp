#ifndef tools
#define tools
#include "root_classes.c++"

#endif
/*class weapon:public items
{
    public:

    int damage;
    string name;
    bool eqiupped=false;
    int rotation=0;
    bool active=true;
    void pickup(player *p)
    {
        name="sword";
    if(IsKeyPressed(KEY_J))
   { if(CheckCollisionRecs(p->position,position))
    {
        eqiupped=true;
    }
     if(active)

    {
        p->backpack.stored_items[ p->backpack.stored_items.size()+1]=name;
     
      active=false;
    }
    }
}
};
class  item_spawner
{
    map<int, weapon> tool;

public:
    void set(map<int, enemy_store> store)
    {
        for (int i = 0; i < store.size(); i++)
        {
            tool[i].position = store[i].position;
            tool[i].appearance = store[i].design;
            tool[i].damage=10;
            tool[i].active=false;
            
        }
    }
    void spawn(player * p)
    {

      
        for (int i = 0; i < tool.size(); i++)
        {
            if(IsKeyPressed(KEY_L))
            {
                tool[i].rotation+=5;
            }
            tool[i].pickup(p);
            //not viable as the hit box doent change
            
            if(tool[i].active=true)
            {
            DrawTextureEx(tool[i].appearance,{tool[i].position.x, tool[i].position.y},tool[i].rotation,1,RAYWHITE); 
          
            }
            
        }
    }
};
*/



