#pragma once
#include <windows.h>
#include <string>
#include <fstream>
#include <easyx.h>
#include <mmsystem.h>
#pragma comment(lib,"winmm.lib")

//全局常量
constexpr int H = 30;
constexpr int W = 44;
constexpr int Size = 20;
constexpr int STARS = 200;
constexpr int dir[4][2]=//方向数组
{
	{-1, 0},//上
	{0, 1},//右
	{1, 0},//下
	{0, -1}//左
}; 
constexpr const char* filename = "maxscore.txt";

enum blockType
{
	EMPTY = 0,
	FOOD1 = 1,
	FOOD4 = 4
};

struct Map
{
	blockType data[H][W];//地图数据，注意是高度优先
	bool hasfood;
};

struct Pos
{
	int x;
	int y;
};

struct Snake
{
	Pos snake[H * W];
	int snakeLength;
	short snakeDir;
};

struct button
{
	double x;
	double y;
	double w;
	double h;
	const wchar_t* text;
};
struct Star
{
	double x;
	int y;
	int color;
	double step;
};

extern int score;
extern int maxScore;
extern int musicNum;
extern bool isExitGame;
extern ExMessage msg;
extern button button1;
extern button button2;
extern button button3;
extern button button4;
extern button buttonBack;
extern button buttonNext;
extern button buttonStopMusic;
extern button buttonMusic[50];
extern Star star[STARS];

void InitStar(int i);

void initMusicButton(button buttonMusic[]);

void initSnake(Snake& snk);

void initMap(Map& map);

void MoveStar(int i);

bool mouseInButton(const button& b, int mx, int my);

bool drawbutton(const button& b);

void drawstar();

void drawLine();

void drawBody(const Pos& p);

void drawHead(const Pos& p);

void checkChangeDir(Snake& snk);

bool checkHitSelf(const Pos& p, const Snake& snk);

void checkEatFood(Map& map, Snake& snk, const Pos& tail);

void moveSnake(Map& map, Snake& snk);

bool doMove(Map& map, Snake& snk);

void setFood(Map& map, Snake& snk, const int& score);

void title(const wchar_t* title = L"Starry Trail");

void showScore();

void showGameOver();

void drawBigDipper();

void openMusicRc(UINT nID, LPCTSTR lpType = L"MUSIC");

void closeMusic();

void startGame(Snake& snake, Map& map, const int& moveTime);

void setMusic();

void aboutUs();

bool exitGame();

bool fileExists(const std::string& filename);

void saveScore();

void loadScore();

void showOpenSource(RECT& rect);

void showAllMusic1(RECT& rect);

void showAllMusic2(RECT& rect);