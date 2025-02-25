#include<cstdlib>
#include<ctime>
#include<thread>
#include"functions.h"
// 主函数
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	initgraph(W * Size, H * Size);
	title();//设置标题
	srand(time(0));
	Snake snake;
	Map map;
	int moveTime = 110;//移动时间间隔
	std::thread background(drawstar);//开启绘制背景星空的线程
	while (1)
	{
		peekmessage(&msg, EX_MOUSE);
		BeginBatchDraw();
		drawBigDipper();//绘制北斗七星
		//设置选项
		startGame(snake, map, moveTime);
		setMusic();
		aboutUs();
		if (exitGame())
		{
			isExitGame = true;
			break;
		}
		EndBatchDraw();
		msg.message = 0;
		Sleep(16);
	}
	background.join();//等待线程结束
	//释放资源
	closeMusic();
	closegraph();
	return 0;
}


