#pragma once

#include <Process.h>
#include <time.h>

extern HWND g_hWnd;
extern HINSTANCE g_hInst;
extern bool g_bFrmaeLimit;

namespace Client
{
	static const unsigned int g_iWinSizeX = 1280;
	static const unsigned int g_iWinSizeY = 720;

	enum LEVEL { LEVEL_LOADING, LEVEL_LOGO, LEVEL_GAMEPLAY, LEVEL_END };

	static void Set_FrameLimit(bool bLimitFlag)
	{
		g_bFrmaeLimit = bLimitFlag;
	}
}

using namespace Client;



