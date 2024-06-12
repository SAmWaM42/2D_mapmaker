#ifndef screens
#define screens
#include "maploader.hpp"
#include "charachters.c++"
#include "particle.c++"
#include <map>
#define screenheight 720
#define max_particles 5
class button
{
public:
    Rectangle size;
    Texture2D texture;
    Color Color;
};

class game_screen
{
public:
    player play;
    mapset set;
    enemy_spawner spawner;
    map<string, map<int, Texture2D>> textures;
    int counter = 0;
    map<int, particles> leaves;

    void set_game(mod_cam cam, player player, string on_map, int *state)
    {

        textures["grass"][0] = LoadTexture("assets/grass/grass4.png");
        textures["grass"][1] = LoadTexture("assets/grass/grass1.png");
        textures["grass"][2] = LoadTexture("assets/grass/grass3.png");
        textures["grass"][3] = LoadTexture("assets/grass/grass9.png");
        textures["grass"][4] = LoadTexture("assets/grass/grass2.png");
        textures["grass"][5] = LoadTexture("assets/grass/grass8.png");
        textures["grass"][6] = LoadTexture("assets/grass/grass6.png");
        textures["grass"][7] = LoadTexture("assets/grass/grass7.png");
        textures["grass"][8] = LoadTexture("assets/grass/grass5.png");
        textures["stone"][0] = LoadTexture("assets/stone/stone3.png");
        textures["stone"][1] = LoadTexture("assets/stone/stone1.png");
        textures["stone"][2] = LoadTexture("assets/stone/stone4.png");
        textures["stone"][3] = LoadTexture("assets/stone/stone8.png");
        textures["stone"][4] = LoadTexture("assets/stone/stone2.png");
        textures["stone"][5] = LoadTexture("assets/stone/stone9.png");
        textures["stone"][6] = LoadTexture("assets/stone/stone7.png");
        textures["stone"][7] = LoadTexture("assets/stone/stone5.png");
        textures["stone"][8] = LoadTexture("assets/stone/stone6.png");
        textures["tree"][0] = LoadTexture("assets/trees/mytree.png");
        textures["particles"][0] = LoadTexture("assets/particles/leaf.png");
        textures["mob"][0]=LoadTexture("assets/mob/slime.png");
        set.load_map(textures, on_map);
        play = player;
        spawner.set(set.enemies);

        set_leaves();
        *state = 1;
    }
    void set_leaves()
    {
        set.extract("tree", 0, true);

        int leaf = 0;

        for (int i = 0; i < set.copy_tiles.size(); i++)
        {
            if (set.copy_tiles[i].used == true)
            {

                float x = 0;
                float y = 0;
                for (int j = 0; j < max_particles; j++)
                {
                    // improve random number algorithim for better leaf positioning
                    leaves[leaf].animation = textures["particles"][0];
                    leaves[leaf].frame = GetRandomValue(0, 50);
                    int tree = GetRandomValue(0, set.copy_tiles.size());
                    float x = (set.copy_tiles[tree].placement.x + GetRandomValue(0, set.copy_tiles[tree].placement.width));
                    float y = (set.copy_tiles[tree].placement.y + GetRandomValue(0, set.copy_tiles[tree].placement.height));
                    leaves[leaf].pos = {x, y, (float)leaves[leaf].animation.width, (float)leaves[leaf].animation.width};
                    leaf++;
                }
            }
        }
    }
    void update_leaves()
    {

        int i;
        int add = 0;
        for (i = 0; i < leaves.size(); i++)
        {
            leaves[i].move({-0.1, 0.3});
            leaves[i].render();

            if (leaves[i].update())
            {
                leaves[i].frame = GetRandomValue(0, 50);
                int tree = GetRandomValue(0, (set.copy_tiles.size()-1));
                float x = (set.copy_tiles[tree].placement.x + GetRandomValue(0, set.copy_tiles[tree].placement.width));
                float y = (set.copy_tiles[tree].placement.y + GetRandomValue(0, set.copy_tiles[tree].placement.height));
                leaves[i].pos = {x, y, (float)leaves[i].animation.width, (float)leaves[i].animation.width};
                leaves[i].kill = false;
                
            }
        }
    }

    void run_game()
    {

        play.moveplayer(set);
        set.drawmap();
        update_leaves();
        spawner.spawn(play,set);
        play.drawplayer();
    }
    void pause_screen(int *state)
    {
        if (IsKeyPressed(KEY_P))
        {
            *state = 2;
        }
    }
};

class home_screen
{
    button buttons[3];
    const char text1[10] = "NEW GAME";
    const char text2[10] = "LOAD GAME";
    const char text3[10] = "QUIT";

public:
    home_screen()
    {
        for (int i = 0; i < 3; i++)
        {
            buttons[i].size.width = 300;
            buttons[i].size.height = 70;
            buttons[i].size.x = screenheight / 2;
            buttons[i].size.y = i * (buttons[i].size.height + 20);
            buttons[i].Color = BLACK;
        }
    }

    void draw_screen()
    {
        for (int i = 0; i < 3; i++)
        {

            DrawRectangleRec(buttons[i].size, buttons[i].Color);
        }
        DrawText(text1, (buttons[0].size.x), (buttons[0].size.y), 50, WHITE);
        DrawText(text2, (buttons[1].size.x), (buttons[1].size.y), 50, WHITE);
        DrawText(text3, (buttons[2].size.x), (buttons[2].size.y), 50, WHITE);
    }
    void run_screen(int *state, Camera2D cam)
    {
        for (int i = 0; i < 3; i++)
        {
            bool click = CheckCollisionPointRec(GetScreenToWorld2D(GetMousePosition(), cam), buttons[i].size);
            if (click)
            {
                buttons[i].Color = RED;
            }
            else
            {
                buttons[i].Color = BLACK;
            }
            if (click && IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) && i == 0)
            {
                *state = 1;
                cout << *state;
            }
            if (click && IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) && i == 1)
            {
                CloseWindow();
            }
            if (click && IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) && i == 2)
            {
                CloseWindow();
            }
        }
    }
};
class pause
{

public:
    Rectangle board = {0, 0, 600, 400};
    Color but_col[2] = {RED, RED};
    Rectangle buttons = {0, 0, 60, 20};
    Rectangle quit = {0, 0, 60, 20};
    void pause_on(mod_cam cam)
    {
        board.x = cam.camera.target.x - (board.width / 2);
        board.y = cam.camera.target.y - (board.width / 2);
        buttons.x =
            buttons.y = board.y + 250;
        quit.x = board.x + 400;
        quit.y = board.y + 250;
        DrawRectangleRec(board, BLACK);
        DrawRectangleRec(buttons, but_col[0]);
        DrawRectangleRec(quit, but_col[1]);
        DrawText("PAUSED", board.x + 60, board.y + 60, 60, RED);
        DrawText("Continue", buttons.x, buttons.y, 15, WHITE);
        DrawText("Quit", quit.x, quit.y, 15, WHITE);
    }
    void pause_off(int *state, mod_cam cam)
    {
        if (CheckCollisionPointRec(GetScreenToWorld2D(GetMousePosition(), cam.camera), buttons))
        {
            but_col[0] = GREEN;
            if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
            {
                *state = 1;
            }
        }
        else
        {
            but_col[0] = RED;
        }

        if (CheckCollisionPointRec(GetScreenToWorld2D(GetMousePosition(), cam.camera), quit))
        {
            but_col[1] = GREEN;
            if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
            {
                CloseWindow();
            }
        }
        else
        {
            but_col[1] = RED;
        }
    }
};

#endif