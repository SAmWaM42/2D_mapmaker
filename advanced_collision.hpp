#ifndef advanced_collide
#define advanced_collide
#include "raylib.h"
#include <math.h>
class rectangle:public Rectangle
{

    public:
    Vector2 vertices[4];
    Vector2 edges[4];
    Vector2 axes[4];
    int rotation_angle;
  

    rectangle()
    {
        vertices[0]={x,y};
        vertices[1]={x+width,y};
        vertices[2]={x+width,y+height};
        vertices[3]={x,y+height};

    
    }
    void update_position()
    {
        vertices[0]={x,y};
        vertices[1]={x+width,y};
        vertices[2]={x+width,y+height};
        vertices[3]={x,y+height};
        rotate_box(180);
    }
    void rotate_box(int angle) 
    {
        rotation_angle=angle;
         float cosTheta=cos(angle);
        float sinTheta=sin(angle);
         float transformation[]={cosTheta,-sinTheta,sinTheta,cosTheta};
        
      //mess with the rotation algorithim
            vertices[1].x=vertices[1].x*transformation[0]+vertices[1].y*transformation[1];
            vertices[1].y=vertices[1].x*transformation[4]+vertices[1].y*transformation[4];
            vertices[3].x=vertices[3].x*transformation[0]+vertices[3].y*transformation[1];
            vertices[3].y=vertices[3].x*transformation[4]+vertices[3].y*transformation[4];
            vertices[2]={vertices[1].x,vertices[3].y};


    }
     /*
    void prepare_vectors()
    {
    edges[0]={vertices[1].x-vertices[0].x,vertices[1].y-vertices[0].y};
    edges[1]={vertices[2].x-vertices[1].x,vertices[2].y-vertices[1].y};
    edges[2]={vertices[3].x-vertices[2].x,vertices[3].y-vertices[2].y};
    edges[3]={vertices[0].x-vertices[3].x,vertices[0].y-vertices[3].y};
    
    axes[0]={-edges[0].y,edges[0].x};
    axes[1]={-edges[1].y,edges[1].x};
    axes[2]={-edges[2].y,edges[2].x};
    axes[3]={-edges[3].y,edges[3].x};

    }
   float dot_product(Vector2  a,Vector2 b)
    {
        return (a.x*b.x+a.y*b.y);
    }
     float projectRectangle(rectangle rec,Vector2 axis)
     {
        float min=dot_product(rec.vertices[0],axis);
        float max=min;
        for(int i=0;i<4;i++)
        {
            float proj=dot_product(rec.vertices[i],axis);
           min=fmin(min,proj);
           max=fmax(max,proj);
        }
        
    return(max+min)/2;
     }  
     bool overlaps(float minA,float maxA,float minB,float maxB)
     {
    return !(maxA<minB || maxB<minA);
     }
    bool checkcollision(rectangle rec)
    { 
        bool colliding=false;
      prepare_vectors();
   //blank spaceerror i will look at this later
      for(int i=0;i<4;i++)
      {
        float projectA=projectRectangle(*this,axes[i]);
        float projectB=projectRectangle(rec,axes[i]);
        float lengthA=projectRectangle(*this,axes[i])-projectA;
        float lengthB=projectRectangle(rec,axes[i])-projectB;

        if(!overlaps(projectA-lengthA/2,projectA+lengthA/2,projectB-lengthB/2,projectB+lengthB/2))
        {
            colliding= false;
        }
        else {colliding=true;}
      }

 return colliding;
    }
    */ 
   bool colliding(rectangle rec)
   {
    if(vertices[1].x>rec.vertices[3].x||vertices[1].x<rec.vertices[3].x)
    {
        return false;
    }
    if(vertices[1].y>rec.vertices[3].y||vertices[1].y<rec.vertices[3].y)
    {
        return false;
    }

return true;
    
   }
    void drawself(Color Color)
    {
            DrawLine(vertices[0].x,vertices[0].y,vertices[1].x,vertices[1].y,Color);
            
            DrawLine(vertices[1].x,vertices[1].y,vertices[2].x,vertices[2].y,Color);
            
            DrawLine(vertices[2].x,vertices[2].y,vertices[3].x,vertices[3].y,Color);
            DrawLine(vertices[3].x,vertices[3].y,vertices[0].x,vertices[0].y,Color);
       
    }


};


#endif