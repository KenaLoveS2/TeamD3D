#pragma once

#include <Process.h>
#include <time.h>

extern HWND g_hWnd;
extern HINSTANCE g_hInst;
extern bool g_bFrameLimit;

#define FOR_MAPTOOL
//#undef FOR_MAPTOOL

#define LIFT_ROT_COUNT		6
#define FIRST_ROT			0

namespace Client
{
	enum LEVEL { LEVEL_LOADING, LEVEL_LOGO, LEVEL_GAMEPLAY, LEVEL_MAPTOOL, LEVEL_TESTPLAY, LEVEL_EFFECT, LEVEL_END };

	static void Set_FrameLimit(bool bLimitFlag)
	{
		g_bFrameLimit = bLimitFlag;
	}

	enum OBJECT_PROPERTY { 
		OP_NULL,
		OP_PLAYER, OP_ROT, 
		
		OP_MONSTER,
		OP_MON_STICKS,
		OP_ENVIROMENT,
		
		OBJECT_PROPERTY_END 
	};
}

extern bool				g_bFullScreen;
extern bool				g_bNeedResizeSwapChain;
extern bool				g_bWinActive;
extern unsigned int	g_iWinSizeX;
extern unsigned int	g_iWinSizeY;

extern unsigned int	g_LEVEL;

using namespace Client;