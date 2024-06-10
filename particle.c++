#ifndef particle
#define particle
#include <iostream>
#include "raylib.h"
using namespace std;
#define tree_num 300
#define max_anim 4

class particles
{
public:
    Rectangle pos;
    string type;
    Texture2D animation;
    bool kill=false;
    int frame;
    bool active;
    particles(Rectangle arr, std::string type, Texture2D pic,int frame)
    {
       
            pos=arr;

        this->type=type;
        this->frame=frame;
     
    }
    particles()
    {

    }

    bool update()
    {
       if(frame>200)
       {
        kill=true;
       }
       frame++;
    return kill;

    }

void render()
{
   DrawTexture(animation,pos.x,pos.y,WHITE);

}
void move(Vector2 velocity)
{
   pos.x+=velocity.x;
   pos.y+=velocity.y;
}

};

#endif