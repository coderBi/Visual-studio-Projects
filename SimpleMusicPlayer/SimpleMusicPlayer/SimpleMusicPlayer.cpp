// SimpleMusicPlayer.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <graphics.h>
#include <mmsystem.h>
#include <windows.h>
#pragma comment(lib, "winmm.lib")

int _tmain(int argc, _TCHAR* argv[])
{
	mciSendString("open \"The Dawn.mp3\" alias back", 0, 0, 0);
	mciSendString("play back", 0, 0, 0);

	initgraph(300, 300);
	IMAGE bg;
	//loadimage(&bg, "./image/bkg.png", 300, 300); //loadimage不支持png
	loadimage(&bg, "./image/bkg.jpg", 300, 300);
	putimage(0, 0, &bg);
	system("pause");
	closegraph();
	return 0;
}

