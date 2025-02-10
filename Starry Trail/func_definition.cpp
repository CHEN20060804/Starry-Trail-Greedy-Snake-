#include "functions.h"
#include<cstdlib>
#include<ctime>
#include<easyx.h>

int score = 0;
int maxScore = 0;//最高分
int musicNum = 0;//背景音乐数量，初始化为0
bool isExitGame = false;//控制星空线程退出
Star star[STARS];

ExMessage msg;
button button1 = { 100,100,200,50,L"开始游戏" };
button button2 = { 100,200,200,50,L"设置音乐" };
button button3 = { 100,300,200,50,L"关于我们" };
button button4 = { 100,400,200,50,L"退出游戏" };
button buttonBack = { 50, 500, 70, 50, L"返回" };
button buttonNext = { 700, 500, 100, 50, L"下一页" };
button buttonStopMusic = { 600, 500, 140, 50, L"停止音乐" };
button buttonMusic[50];
// 初始化星星
void InitStar(int i)
{
	star[i].x = 0;
	star[i].y = rand() % (H * Size);
	star[i].step = (rand() % 5000) / 2500.0 + 1;
	star[i].color = (int)(star[i].step * 255 / 3.0 + 0.5);	// 速度越快，颜色越亮
	star[i].color = RGB(star[i].color, star[i].color, star[i].color);
}

// 移动星星
void MoveStar(int i)
{
	// 擦掉原来的星星
	putpixel((int)star[i].x, star[i].y, 0);
	// 计算新位置
	star[i].x += star[i].step;
	if (star[i].x > W * Size)	InitStar(i);
	// 画新星星
	putpixel((int)star[i].x, star[i].y, star[i].color);
}

bool mouseInButton(const button& b, int mx, int my)
{
	if (b.x <= mx && mx <= b.x + b.w && b.y <= my && my <= b.y + b.h)
		return true;
	return false;
}

bool drawbutton(const button& b)//绘制按钮并判断是否被点击
{
	settextcolor(WHITE);
	setlinecolor(WHITE);
	setbkmode(TRANSPARENT);
	settextstyle(b.h / 2, 0, L"黑体", 0, 0, 0, 0, 0, 0);
	int W = b.x + (b.w - textwidth(b.text)) / 2;
	int H = b.y + (b.h - textheight(b.text)) / 2;
	if (mouseInButton(b, msg.x, msg.y))
	{
		setfillcolor(RGB(256, 158, 176));
		fillroundrect(b.x, b.y, b.x + b.w, b.y + b.h, 10, 10);
		outtextxy(W, H, b.text);
		if (msg.message == WM_LBUTTONDOWN)
		{
			return true;
		}
		return false;
	}
	else
	{
		setfillcolor(BLACK);
		fillroundrect(b.x, b.y, b.x + b.w, b.y + b.h, 10, 10);
		outtextxy(W, H, b.text);
		return false;
	}
}

// 绘制星星
void drawstar()
{
	for (int i = 0; i < STARS; i++)
	{
		InitStar(i);
		star[i].x = rand() % (W * Size);
	}
	while (1)
	{
		if (isExitGame)break;
		peekmessage(&msg, EX_MOUSE);
		BeginBatchDraw();
		for (int i = 0; i < STARS; i++)
		{
			MoveStar(i);
		}
		EndBatchDraw();
		Sleep(16);
	}
}

void drawLine()//画网格线
{
	setlinecolor(RGB(30, 30, 30));
	for (int i = 1; i < W - 4; i++)
	{
		line(i * Size, 0, i * Size, H * Size);
	}
	for (int i = 1; i <= H; i++)
	{
		line(0, i * Size, (W - 4) * Size, i * Size);
	}
	setlinecolor(RGB(150, 150, 150));
	line((W - 4) * Size, 0, (W - 4) * Size, H * Size);
}

void drawBody(const Pos& p)//画渐变的蛇身方块
{
	int cx = p.x * Size + Size / 2;
	int cy = p.y * Size + Size / 2;
	int radius = 10;       // 圆半径
	COLORREF outerColor = RGB(255, 255, 255); // 外部颜色（白色）
	COLORREF innerColor = RGB(64, 64, 64);    // 内部颜色（浅黑色）
	// 逐像素绘制渐变圆
	for (int x = cx - radius; x <= cx + radius; ++x) {
		for (int y = cy - radius; y <= cy + radius; ++y) {
			int dx = x - cx;
			int dy = y - cy;
			double distance = sqrt(dx * dx + dy * dy);
			if (distance <= radius) {
				double ratio = distance / radius;
				int r = GetRValue(innerColor) + (GetRValue(outerColor) - GetRValue(innerColor)) * ratio;
				int g = GetGValue(innerColor) + (GetGValue(outerColor) - GetGValue(innerColor)) * ratio;
				int b = GetBValue(innerColor) + (GetBValue(outerColor) - GetBValue(innerColor)) * ratio;
				putpixel(x, y, RGB(r, g, b));
			}
		}
	}

}

void drawHead(const Pos& p)//画白色的蛇头
{
	setfillcolor(RGB(220, 220, 220));
	fillroundrect(p.x * Size, p.y * Size, p.x * Size + Size, p.y * Size + Size, 10, 10);
}

void initSnake(Snake& snk)
{
	snk.snake[0] = { W / 2 ,H / 2 };
	snk.snake[1] = { W / 2 - 1 ,H / 2 };
	snk.snake[2] = { W / 2 - 2,H / 2 };
	snk.snakeLength = 3;
	snk.snakeDir = 1;
	drawHead(snk.snake[0]);
	drawBody(snk.snake[1]);
	drawBody(snk.snake[2]);
}

void initMap(Map& map)
{
	for (int y = 0; y < H; y++)
	{
		for (int x = 0; x < W - 4; x++)
		{
			map.data[y][x] = EMPTY;
		}
	}
	map.hasfood = false;
}

void checkChangeDir(Snake& snk)
{
	static short lastDir = snk.snakeDir;
	if (GetAsyncKeyState(VK_UP) & 0x8000 || GetAsyncKeyState('W') & 0x8000)
	{
		if (lastDir != 2)
			snk.snakeDir = 0;
	}
	else if (GetAsyncKeyState(VK_RIGHT) & 0x8000 || GetAsyncKeyState('D') & 0x8000)
	{
		if (lastDir != 3)
			snk.snakeDir = 1;
	}
	else if (GetAsyncKeyState(VK_DOWN) & 0x8000 || GetAsyncKeyState('S') & 0x8000)
	{
		if (lastDir != 0)
			snk.snakeDir = 2;
	}
	else if (GetAsyncKeyState(VK_LEFT) & 0x8000 || GetAsyncKeyState('A') & 0x8000)
	{
		if (lastDir != 1)
			snk.snakeDir = 3;
	}
	lastDir = snk.snakeDir;
}


bool checkHitSelf(const Pos& p, const Snake& snk)
{
	for (int i = 3; i < snk.snakeLength; i++)
	{
		if (snk.snake[i].x == p.x && snk.snake[i].y == p.y)
		{
			return true;
		}
	}
	return false;
}

void checkEatFood(Map& map, Snake& snk, const Pos& tail)
{
	Pos head = snk.snake[0];
	if (map.data[head.y][head.x] == FOOD1 || map.data[head.y][head.x] == FOOD4)
	{
		snk.snake[snk.snakeLength++] = tail;
		map.hasfood = false;
		if (map.data[head.y][head.x] == FOOD1)
		{
			score += FOOD1;
		}
		else
		{
			score += FOOD4;
		}
		map.data[head.y][head.x] = EMPTY;
		drawBody(tail);
	}
}

void moveSnake(Map& map, Snake& snk)
{
	// 移动蛇身
	for (int i = snk.snakeLength - 1; i > 0; i--)
	{
		snk.snake[i] = snk.snake[i - 1];
	}
	snk.snake[0].x += dir[snk.snakeDir][1];
	snk.snake[0].y += dir[snk.snakeDir][0];
	//处理蛇头碰撞到边界的情况
	if (snk.snake[0].x < 0)
	{
		snk.snake[0].x = W - 5;
		return;
	}
	else if (snk.snake[0].x >= W - 4)
	{
		snk.snake[0].x = 0;
		return;
	}
	else if (snk.snake[0].y < 0)
	{
		snk.snake[0].y = H - 1;
		return;
	}
	else if (snk.snake[0].y >= H)
	{
		snk.snake[0].y = 0;
		return;
	}

}

bool doMove(Map& map, Snake& snk)
{
	Pos tail = snk.snake[snk.snakeLength - 1];
	setfillcolor(BLACK);
	setlinecolor(RGB(30, 30, 30));
	fillrectangle(tail.x * Size, tail.y * Size, tail.x * Size + Size, tail.y * Size + Size);
	moveSnake(map, snk);
	if (checkHitSelf(snk.snake[0], snk))
	{
		return false;
	}
	checkEatFood(map, snk, tail);
	setfillcolor(WHITE);
	drawHead(snk.snake[0]);
	drawBody(snk.snake[1]);
	return true;
}

void setFood(Map& map, Snake& snk, const int& score)
{
	static int m, n;//记录食物位置
	static int foodCount = 0;//控制生成食物种类
	static COLORREF lastColor;
	if (map.hasfood == false)
	{
		while (1)
		{
			m = rand() % (W - 4);
			n = rand() % H;
			int i = 0;
			//判断食物是否与蛇身重合
			while (i < snk.snakeLength)
			{
				if (snk.snake[i].x == m && snk.snake[i].y == n)
				{
					break;
				}
				i++;
			}
			if (i == snk.snakeLength)//食物与蛇身不重合
			{
				map.hasfood = true;
				if (foodCount % 4 == 0 && foodCount)
				{
					map.data[n][m] = FOOD4;
					setfillcolor(RGB(142, 132, 211));
					fillcircle(m * Size + Size / 2, n * Size + Size / 2, Size / 2);
					foodCount++;
					lastColor = RGB(142, 132, 211);
				}
				else
				{
					map.data[n][m] = FOOD1;
					setfillcolor(RGB(100, 200, 150));
					fillcircle(m * Size + Size / 2, n * Size + Size / 2, Size / 2);
					foodCount++;
					lastColor = RGB(100, 200, 150);
				}
				return;
			}
		}
	}
	else//食物未被吃掉，维持原状
	{
		setfillcolor(lastColor);
		fillcircle(m * Size + Size / 2, n * Size + Size / 2, Size / 2);
	}
}

void title(const wchar_t* title)
{
	HWND hwnd = GetHWnd();
	SetWindowText(hwnd, title);
}

void showScore()
{
	settextstyle(Size * 1.2, 0, L"黑体", 0, 0, 600, 1, 0, 0);
	setbkmode(TRANSPARENT);
	outtextxy((W - 4) * Size + 5, Size, L"Score");
	clearrectangle((W - 4) * Size + 5, 3 * Size, W * Size, 5 * Size);
	outtextxy((W - 3) * Size + 5, 3 * Size, std::to_wstring(score).c_str());
	outtextxy((W - 4) * Size + 10, 6 * Size, L"Best");
	outtextxy((W - 3) * Size + 5, 8 * Size, std::to_wstring(maxScore).c_str());
	setlinecolor(RGB(150, 150, 150));
	line((W - 4) * Size, 5 * Size, W * Size, 5 * Size);
}

void showGameOver()
{
	settextcolor(RGB(220, 240, 220));
	setbkmode(TRANSPARENT);
	settextstyle(Size * 3, 0, L"微软雅黑", 0, 0, 0, 0, 0, 0);
	BeginBatchDraw();
	outtextxy((W / 2 - 8) * Size, (H / 2 - 3) * Size, L"Game Over!");
}

void drawBigDipper()
{
	// 设置绘图颜色为白色
	setlinecolor(WHITE);
	int offset = (W - 15) * Size;
	// 定义北斗七星的坐标
	POINT stars[7] = {
		{544, 231}, // 天枢
		{515, 155}, // 天璇
		{629, 129}, // 天玑
		{700, 200}, // 天权
		{771, 271}, // 玉衡
		{806, 306}, // 开阳
		{757, 373}  // 摇光
	};
	// 绘制连接星星的折线
	polyline(stars, 7);
	// 绘制星星（用小圆点表示）
	setfillcolor(WHITE);
	for (int i = 0; i < 7; ++i)
	{
		fillcircle(stars[i].x, stars[i].y, 3);
	}
}

void openMusicRc(UINT ID, LPCTSTR lpType) {
	static bool isMusicOpen = false;
	if (isMusicOpen) {
		// Stop and close any currently playing music
		mciSendString(L"stop bgm", NULL, 0, NULL);
		mciSendString(L"close bgm", NULL, 0, NULL);
	}
	//寻找资源
	HRSRC hResInfo = FindResource(NULL, MAKEINTRESOURCE(ID), lpType);
	if (!hResInfo) return;
	//载入资源
	HGLOBAL hResource = LoadResource(NULL, hResInfo);
	if (!hResource) return;
	// 锁定资源
	LPVOID lpData = LockResource(hResource);
	if (!lpData) return;
	// 获取资源大小
	DWORD dwSize = SizeofResource(NULL, hResInfo);
	if (!dwSize) return;
	// 创建临时文件
	wchar_t tempPath[MAX_PATH];
	GetTempPath(MAX_PATH, tempPath);
	wchar_t tempFile[MAX_PATH];
	GetTempFileName(tempPath, L"MP3", 0, tempFile);
	// 写入临时文件
	HANDLE hFile = CreateFile(tempFile, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_TEMPORARY, NULL);
	if (hFile == INVALID_HANDLE_VALUE) return;
	DWORD written;
	WriteFile(hFile, lpData, dwSize, &written, NULL);
	CloseHandle(hFile);
	// 播放音乐
	wchar_t command[256];
	swprintf_s(command, L"open \"%s\" type mpegvideo alias bgm", tempFile);
	mciSendString(command, NULL, 0, NULL);
	mciSendString(L"play bgm repeat", NULL, 0, NULL);
	mciSendString(L"setaudio bgm volume to 150", NULL, 0, NULL);
	isMusicOpen = true;
}

void startGame(Snake& snake, Map& map, const int& moveTime)
{
	if (drawbutton(button1))
	{
		cleardevice();
		score = 0;
		initSnake(snake);
		initMap(map);
		loadScore();
		while (1)
		{
			peekmessage(&msg, EX_MOUSE);
			checkChangeDir(snake);
			BeginBatchDraw();
			drawLine();
			if (doMove(map, snake) == false)
			{
				break;
			}
			setFood(map, snake, score);
			showScore();
			EndBatchDraw();
			if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)break;
			Sleep(moveTime);
		}
		saveScore();
		while (1)
		{
			peekmessage(&msg, EX_MOUSE);
			showGameOver();
			if (drawbutton(buttonBack))
			{
				cleardevice();
				break;
			}
			EndBatchDraw();
			Sleep(16);
		}
	}
}

void setMusic()
{
	if (drawbutton(button2))
	{
		msg.message = 0;//清除消息队列,防止保留上次的鼠标消息
		cleardevice();
		initMusicButton(buttonMusic);
		while (1)
		{
			peekmessage(&msg, EX_MOUSE);
			BeginBatchDraw();
			for (int i = 0; i < musicNum; i++)
			{
				if (drawbutton(buttonMusic[i]))
				{
					openMusicRc(101 + i);//ID参见<resource.h>，记事本编辑
				}
			}
			if (drawbutton(buttonBack))
			{
				cleardevice();
				break;
			}
			if (drawbutton(buttonStopMusic))
			{
				closeMusic();
			}
			EndBatchDraw();
			Sleep(16);
		}
	}
}

void aboutUs()
{
	RECT rect = { 50, 50, 600, 120 };
	RECT rectmusic1 = { 150, 150, 600, 500 };
	RECT rectmusic2 = { 150, 50, 600, 500 };
	if (drawbutton(button3))
	{
		cleardevice();
		while (1)
		{
			setbkmode(TRANSPARENT);
			peekmessage(&msg, EX_MOUSE);
			setfillcolor(BLACK);
			settextstyle(15, 0, L"黑体", 0, 0, 0, 0, 0, 0);
			BeginBatchDraw();
			showOpenSource(rect);
			showAllMusic1(rectmusic1);
			if (drawbutton(buttonNext))
			{
				cleardevice();
				while (1)
				{
					peekmessage(&msg, EX_MOUSE);
					setfillcolor(BLACK);
					settextstyle(15, 0, L"黑体", 0, 0, 0, 0, 0, 0);
					BeginBatchDraw();
					showAllMusic2(rectmusic2);
					if (drawbutton(buttonBack))
					{
						cleardevice();
						msg.message = 0;//清除消息队列,防止保留上次的鼠标消息
						break;
					}
					EndBatchDraw();
					Sleep(16);
				}
			}
			if (drawbutton(buttonBack))
			{
				cleardevice();
				break;
			}
			EndBatchDraw();
			Sleep(16);
		}
	}
}

bool exitGame()
{
	if (drawbutton(button4))
	{
		return true;
	}
	return false;
}

void closeMusic()
{
	mciSendString(L"stop bgm", 0, 0, 0);
	mciSendString(L"close bgm", 0, 0, 0);
}

void initMusicButton(button buttonMusic[])
{
	//第一列
	buttonMusic[0] = { 2 * Size, Size, 9 * Size, 2 * Size, L"Fireflies" };
	buttonMusic[1] = { 2 * Size, 3.5 * Size, 9 * Size, 2 * Size, L"N" };
	buttonMusic[2] = { 2 * Size, 6 * Size, 9 * Size, 2 * Size, L" やったー！" };//太好了！
	buttonMusic[3] = { 2 * Size, 8.5 * Size, 9 * Size, 2 * Size, L"5:20AM" };
	buttonMusic[4] = { 2 * Size, 11 * Size, 9 * Size, 2 * Size, L"Dream" };
	buttonMusic[5] = { 2 * Size, 13.5 * Size, 9 * Size, 2 * Size, L"Mood" };
	buttonMusic[6] = { 2 * Size, 16 * Size, 9 * Size, 2 * Size, L"Visons" };
	buttonMusic[7] = { 2 * Size, 18.5 * Size, 9 * Size, 2 * Size, L"I'm Not Alone" };
	buttonMusic[8] = { 2 * Size, 21 * Size, 9 * Size, 2 * Size, L"Thinking About You" };
	//第二列
	buttonMusic[9] = { 15 * Size, Size, 9 * Size, 2 * Size, L"One Kiss" };
	buttonMusic[10] = { 15 * Size, 3.5 * Size, 9 * Size, 2 * Size, L"Timber" };
	buttonMusic[11] = { 15 * Size, 6 * Size, 9 * Size, 2 * Size, L"寻" };
	buttonMusic[12] = { 15 * Size, 8.5 * Size, 9 * Size, 2 * Size, L"雨夜" };
	buttonMusic[13] = { 15 * Size, 11 * Size, 9 * Size, 2 * Size, L"NYA!!!" };
	buttonMusic[14] = { 15 * Size, 13.5 * Size, 9 * Size, 2 * Size, L"のんびりした場所" }; // 轻松之地
	buttonMusic[15] = { 15 * Size, 16 * Size, 9 * Size, 2 * Size, L"唯一" };
	buttonMusic[16] = { 15 * Size, 18.5 * Size, 9 * Size, 2 * Size, L"Winter Without You" };
	buttonMusic[17] = { 15 * Size, 21 * Size, 9 * Size, 2 * Size, L"Rumors" };
	//第三列
	buttonMusic[18] = { 28 * Size, Size, 9 * Size, 2 * Size, L"Lumen" };
	buttonMusic[19] = { 28 * Size, 3.5 * Size, 9 * Size, 2 * Size, L"Pluto" };
	buttonMusic[20] = { 28 * Size, 6 * Size, 9 * Size, 2 * Size, L"海底列车" };
	buttonMusic[21] = { 28 * Size, 8.5 * Size, 9 * Size, 2 * Size, L"Skyscape" };
	buttonMusic[22] = { 28 * Size, 11 * Size, 9 * Size, 2 * Size, L"His Theme" };
	buttonMusic[23] = { 28 * Size, 13.5 * Size, 9 * Size, 2 * Size, L"for ya" };
	buttonMusic[24] = { 28 * Size, 16 * Size, 9 * Size, 2 * Size, L"罗生门" };
	buttonMusic[25] = { 28 * Size, 18.5 * Size, 9 * Size, 2 * Size, L"Always Online" };
	buttonMusic[26] = { 28 * Size, 21 * Size, 9 * Size, 2 * Size, L"아름다운 그대여" };
	musicNum = 27;
}

bool fileExists(const std::string& filename)
{
	std::ifstream file(filename);
	return file.good();
}

void saveScore()
{
	if (score >= maxScore)
	{
		std::ofstream file(filename);
		file << score;
		file.close();
	}
}

void loadScore()
{
	if (fileExists(filename))
	{
		std::ifstream file(filename);
		file >> maxScore;
		file.close();
	}
}

void showOpenSource(RECT& rect)
{
	fillroundrect(rect.left - 10, rect.top - 10, rect.right, rect.bottom, 10, 10);
	drawtext(L"Snake Trail已开源！\n开源地址:https://gitee.com/ye-yichen2100/starry-trail---greedy-snake.git\n作者邮箱：3420909084@qq.com", &rect, DT_LEFT | DT_WORDBREAK);
}

void showAllMusic1(RECT& rect)
{
	fillroundrect(rect.left, rect.top, rect.right, rect.bottom, 10, 10);

	outtextxy(rect.left + 10, rect.top + 10, L"歌曲列表：");
	outtextxy(rect.left + 10, rect.top + 30, L"千坂,SaMZIng - Fireflies");
	outtextxy(rect.left + 10, rect.top + 50, L"刀酱 - 5:20AM");
	outtextxy(rect.left + 10, rect.top + 70, L"西郷憲一郎 - のんびりした場所");
	outtextxy(rect.left + 10, rect.top + 90, L"水谷広実 - やったー！");
	outtextxy(rect.left + 10, rect.top + 110, L"四季音色 - 雨夜");
	outtextxy(rect.left + 10, rect.top + 130, L"三亩地 - 寻");
	outtextxy(rect.left + 10, rect.top + 150, L"路灰气球 - 아름다운 그대여【钢琴】");
	outtextxy(rect.left + 10, rect.top + 170, L"林俊杰 - Always Online");
	outtextxy(rect.left + 10, rect.top + 190, L"梨冻紧,Wiz_H张子豪 - 罗生门（Follow）");
	outtextxy(rect.left + 10, rect.top + 210, L"蒋小呢 - 给你呀（又名：for ya）");
	outtextxy(rect.left + 10, rect.top + 230, L"Toby Fox - His Theme");
	outtextxy(rect.left + 10, rect.top + 250, L"Plum - Melodic Artist - Skyscape");
	outtextxy(rect.left + 10, rect.top + 270, L"Plum - Melodic Artist - N");
	outtextxy(rect.left + 10, rect.top + 290, L"PIKASONIC,なこたんまる - 海底列車");
	outtextxy(rect.left + 10, rect.top + 310, L"Nyte - Pluto");
	outtextxy(rect.left + 10, rect.top + 330, L"Mr Chip - Lumen");
}

void showAllMusic2(RECT& rect)
{
	fillroundrect(rect.left, rect.top, rect.right, rect.bottom, 10, 10);

	outtextxy(rect.left + 10, rect.top + 10, L"Jake Miller - Rumors");
	outtextxy(rect.left + 10, rect.top + 30, L"Gloria Kim - Winter Without You");
	outtextxy(rect.left + 10, rect.top + 50, L"G.E.M.邓紫棋 - 唯一");
	outtextxy(rect.left + 10, rect.top + 70, L"FLuoRiTe - NYA!!!");
	outtextxy(rect.left + 10, rect.top + 90, L"Dio Deer,Evanti3s - Dream");
	outtextxy(rect.left + 10, rect.top + 110, L"CDM Project - Timber");
	outtextxy(rect.left + 10, rect.top + 130, L"Acreix - Visions");
	outtextxy(rect.left + 10, rect.top + 150, L"24kGoldn,iann dior - Mood");
	outtextxy(rect.left + 10, rect.top + 170, L"Calvin Harris - I'm Not Alone (Radio Edit)");
	outtextxy(rect.left + 10, rect.top + 190, L"Calvin Harris,Ayah Marar - Thinking About You");
	outtextxy(rect.left + 10, rect.top + 210, L"Calvin Harris,Dua Lipa - One Kiss");
}