#include<stdlib.h>
#include<stdio.h>
#include<conio.h>      //kbhit()、getch()
#include<windows.h>   //控制光标位置

#define MAP_W 8
#define MAP_H 8

enum{                        //地图元素枚举
    T_EMPTY = 0,
    T_WALL,
    T_TARGET,
    T_BOX,
    T_BOX_ON,
};

typedef struct{
    int map[MAP_H][MAP_W];
    int px,py;              //玩家坐标
    int totalTargets,steps; //目标总数，步数
}GameState;

void gotoxy(int x,int y){    //设置光标位置
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE),coord);
}
    
void hide_cursor(){          //隐藏光标
    CONSOLE_CURSOR_INFO cursor_info = {1, 0};
    GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursor_info);
}

void game_init(GameState *g){    //初始化游戏
    int init[MAP_H][MAP_W] = {
        {1,1,1,1,1,1,1,1},
        {1,0,0,0,0,0,0,1},
        {1,0,3,2,0,3,0,1},
        {1,0,0,2,0,0,0,1},
        {1,0,3,0,0,0,0,1},
        {1,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,1},
        {1,1,1,1,1,1,1,1}
    };
    for(int y = 0;y < MAP_H; y++){
        for(int x = 0;x < MAP_W; x++){
            g -> map[y][x] = init[y][x];
        }
    }
    g -> px = 4;
    g -> py = 4;
    g -> steps = 0;
    g -> totalTargets = 0;

    for(int y = 0;y < MAP_H; y++){       //统计目标点数
        for(int x = 0;x < MAP_W; x++){
            if(g -> map[y][x] == T_TARGET || g -> map[y][x] == T_BOX_ON){
                g -> totalTargets++;
            }
        }
    }
}

void game_draw(const GameState *g){
    gotoxy(0,0);     //每次重绘回到左上角，清除旧画面

    for(int y = 0; y < MAP_H; y++){     //画箱子和地形
        for(int x = 0;x < MAP_W ; x++){
            if(x == g->px && y == g->py){
                printf("P");  //p为玩家
                continue;
            }
            switch(g->map[y][x]){
                case T_EMPTY: 
                    printf(" "); 
                    break;
                case T_WALL: 
                    printf("#"); 
                    break;
                case T_TARGET: 
                    printf("."); 
                    break;
                case T_BOX: 
                    printf("B"); 
                    break;
                case T_BOX_ON: 
                    printf("*"); 
                    break;
            }
        }
        printf("\n");
    }
}

int game_move(GameState *g,int dx,int dy) {
    int nx = g->px + dx,ny = g->py + dy;
    if(nx < 0||nx >= MAP_W || ny < 0 || ny >= MAP_H)return 0;//边界检查
    if(g->map[ny][nx] == T_WALL)return 0;//撞墙检查
    //撞到箱子 
    if(g->map[ny][nx] == T_BOX || g->map[ny][nx] == T_BOX_ON){ 
        int bx = nx + dx,by = ny + dy;
        if(bx < 0 || bx >= MAP_W || by < 0 || by >= MAP_H)return 0;
        int c = g->map[by][bx];
        if(c == T_WALL || c == T_BOX || c == T_BOX_ON)return 0;//前方是墙箱子推不动
        if(g->map[ny][nx] == T_BOX_ON) {
            g->map[ny][nx] = T_TARGET; 
        }else{//成立为T_TARGET不成立为T_EMPTY                                                                 //移动箱子处理原位置
            g->map[ny][nx] = T_EMPTY;
        }
        if(c == T_TARGET) {
            g->map[by][bx] = T_BOX_ON;
        } else {
            g->map[by][bx] = T_BOX;
        }
    }
    g->px = nx;//移动玩家
    g->py = ny;
    g->steps++;
    return 1;
}
int game_win(const GameState *g){
    int cnt = 0;
    for(int y = 0;y < MAP_H;y++){
        for(int x = 0;x < MAP_W;x++){
            if(g->map[y][x] == T_BOX_ON) cnt++;
        }
    }
    return cnt == g->totalTargets;
}

int main(void){
    GameState game;       //初始化游戏状态
    hide_cursor();          //隐藏光标
    game_init(&game);
    game_draw(&game);     //绘制游戏画面
    while(1){
        if(kbhit()){       //键盘检测
            char c = getch();
            int dx = 0,dy = 0;
            switch(c){
                case 'w':
                case'W':
                    dy = -1;
                    break;
                case's':
                case'S':
                    dy = 1;
                    break;
                case 'a':
                case'A':
                    dx = -1;
                    break;
                case'd':
                case'D':
                    dx = 1;
                    break;
                case'q':
                case'Q':
                    return 0;
                default: continue;
            }
            if(game_move(&game,dx,dy)){   //移动有效则重新绘制
                game_draw(&game);
                if(game_win(&game)){     
                    getch();//按退出键退出
                    break;
                }
            }
        }
        Sleep(10);
    }
    return 0;
}