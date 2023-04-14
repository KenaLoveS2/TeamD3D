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

#define MONSTER_LINEAR_DAMING		2.0f
#define MONSTER_ANGULAR_DAMING		0.5f
#define MONSTER_MASS				25000.f

namespace Client
{
	enum LEVEL { LEVEL_LOADING, LEVEL_LOGO, LEVEL_GAMEPLAY, LEVEL_MAPTOOL, LEVEL_TESTPLAY, LEVEL_EFFECT, LEVEL_GIMMICK, LEVEL_FINAL, LEVEL_END };

	enum SOUND { SOUND_BGM, SOUND_UI, SOUND_PLAYER_VOICE, SOUND_PLAYER_SFX, SOUND_NPC, SOUND_END = 12 };

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

		COL_MONSTER, COL_MONSTER_WEAPON, COL_MONSTER_ARROW, COL_WARRIOR_GRAB_HAND, COL_BOSS_SWIPECHARGE,
		COL_ELETE_MONSTER, COL_ELETE_MONSTER_WEAPON,
		COL_BOSS_MONSTER, COL_BOSS_MONSTER_WEAPON,
		
		COL_GROUND, COL_ENVIROMENT, COL_PULSE_PLATE, COL_WATER,
		COL_PULSE, COL_GIMMICK_TREE, COL_TELEPORT_FLOWER, COL_CHEST, COL_HEALTHFLOWER, COL_PORTAL, COL_BOWTARGET, COL_SCENECHANGE_PORTAL,
		COL_DYNAMIC_ENVIOBJ, COL_TRIGGER, COL_UI_TRIGGER,
		COLLIDER_INDEX_END,
	};

	enum TRIGGER_INDEX {
		TRIGGER_ROT,
		TRIGGER_PULSE_PLATE,
		TRIGGER_PULSE,
		TRIGGER_TELEPORT_FLOWER, TRIGGER_CHEST,	TRIGGER_HEALTHFLOWER, TRIGGER_MEDITATIONSPOT,
		TRIGGER_BOWTARGET_LAUNCH, TRIGGER_RESPAWN_POINT,
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
extern unsigned int	g_LEVEL_FOR_COMPONENT;
static const XMFLOAT4X4 g_IdentityFloat4x4 = {
	1,0,0,0,
	0,1,0,0,
	0,0,1,0,
	0,0,0,1,
};
static const _float4 g_vInvisiblePosition = { -1000.f, 0.f, -1000.f, 1.f };

static const bool g_bFalse = false;
static const bool g_bTrue = true;


using namespace Client;