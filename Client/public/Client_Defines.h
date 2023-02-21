#pragma once

#include <Process.h>
#include <time.h>

extern HWND g_hWnd;
extern HINSTANCE g_hInst;
extern bool g_bFrmaeLimit;

//#define FOR_MAPTOOL

namespace Client
{
	enum LEVEL { LEVEL_LOADING, LEVEL_LOGO, LEVEL_GAMEPLAY, LEVEL_MAPTOOL,  LEVEL_END };

	static void Set_FrameLimit(bool bLimitFlag)
	{
		g_bFrmaeLimit = bLimitFlag;
	}
}

extern bool			g_bFullScreen;
extern bool			g_bNeedResizeSwapChain;
extern unsigned int	g_iWinSizeX;
extern unsigned int	g_iWinSizeY;

using namespace Client;