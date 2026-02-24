#ifndef NmapMaker
#define NmapMaker
#include "../raylib.h"

#include "../Classes/root_classes.h"
#include "../json.hpp"
#include <iostream>
#include <map>
using namespace std;
#define RENDER_SCALE 2.0
#define worldGridSize 100
#define chunkSize 10
#define tileSize 40
#define textLength 16
using json = nlohmann::json;



class worldEditor
{
public:
string currentTexture;
string currentVariant;
world_object focusedWorldObj;
bool loaded = false;
int zoom = 1;
int rotation = 0;
int x;
int y; 
Vector2 offset = {screenwidth / 2, screenheight / 2};
Vector2 target = {screenwidth / 2, screenheight / 2};
mod_cam camera;
//need to confirm whether the worldGrid is meant to have variable size or stick to the over extending

Vector2 grid[worldGridSize][worldGridSize];
map<string,world_object> entities;
vector<tile> staticTiles;
map<string, map<int, Texture2D>> textures; 
vector<string> textureKeys;
mapset Mapset;

void prepareTextures(map<string, map<int, Texture2D>> textures,vector<string> textureKeys,vector<string> filepaths);
void update();
void spawnEntity();
void addTile();
void assetMenu(map<string, map<int, Texture2D>> textures);

};





#endif