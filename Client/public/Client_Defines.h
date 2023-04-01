#pragma once

#include <Process.h>
#include <time.h>

extern HWND g_hWnd;
extern HINSTANCE g_hInst;
extern bool g_bFrameLimit;

#define KENA_PLUS_PIP_GUAGE_VALUE		0.2f

#define TERRAIN_COUNT		5

#define FOR_MAP_GIMMICK

//#define FOR_MAPTOOL
//#undef FOR_MAPTOOL

#define LIFT_ROT_COUNT		6
#define FIRST_ROT			0

#define MONSTER_LINEAR_DAMING		1.f
#define MONSTER_ANGULAR_DAMING		0.5f
#define MONSTER_MASS				25000.f

namespace Client
{
	enum LEVEL { LEVEL_LOADING, LEVEL_LOGO, LEVEL_GAMEPLAY, LEVEL_MAPTOOL, LEVEL_TESTPLAY, LEVEL_EFFECT, LEVEL_END };

	enum SOUND { SOUND_BGM, SOUND_UI, SOUND_END };

	static void Set_FrameLimit(bool bLimitFlag)
	{
		g_bFrameLimit = bLimitFlag;
	}

	enum OBJECT_PROPERTY { OP_NULL,
		OP_PLAYER, OP_PLAYER_WEAPON, OP_PLAYER_ARROW,
		OP_ROT,		
		OP_MONSTER_WEAPON, OP_MONSTER, OP_MON_STICKS, OP_MON_MOON_KNGIHT, OP_MON_SAMPLING,

		OP_GROUND, OP_ENVIROMENT,
		
		OBJECT_PROPERTY_END 
	};

	enum COLLIDER_INDEX {
		COL_NULL, COL_DEFAULT,
		COL_PLAYER, COL_PLAYER_BUMP, COL_PLAYER_WEAPON, COL_PLAYER_ARROW, COL_PLAYER_BOMB, COL_PLAYER_BOMB_EXPLOSION,

		COL_ROT,

		COL_MONSTER, COL_MONSTER_WEAPON, COL_MONSTER_ARROW,
		COL_ELETE_MONSTER, COL_ELETE_MONSTER_WEAPON,
		COL_BOSS_MONSTER, COL_BOSS_MONSTER_WEAPON,
		
		COL_GROUND, COL_ENVIROMENT,COL_PULSE_PLATE,
		COL_PULSE, COL_GIMMICK_TREE, COL_TELEPORT_FLOWER, COL_CHEST,
		COLLIDER_INDEX_END,
	};

	enum TRIGGER_INDEX {
		TRIGGER_ROT,
		TRIGGER_PULSE_PLATE,
		TRIGGER_PULSE,
		TRIGGER_TELEPORT_FLOWER,
		TRIGGER_CHEST,
		TRIGGER_END
	};
}

extern bool				g_bFullScreen;
extern bool				g_bNeedResizeSwapChain;
extern bool				g_bWinActive;
extern unsigned int	g_iWinSizeX;
extern unsigned int	g_iWinSizeY;
extern float				g_fSkyColorIntensity;
extern bool				g_bDayOrNight;

extern unsigned int	g_LEVEL;

static const _float4x4 g_IdentityFloat4x4 = {
	1,0,0,0,
	0,1,0,0,
	0,0,1,0,
	0,0,0,1,
};

static const _bool g_bFalse = false;
static const _bool g_bTrue = true;


using namespace Client;