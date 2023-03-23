#include "stdafx.h"
#include "..\public\Loader.h"
#include "GameInstance.h"

#include "ControlRoom.h"

/* SkyBox */
#include "Sky.h"

/* Terrain */
#include "Terrain.h"

/* Player */
#include "Kena.h"
#include "Kena_Staff.h"
#include "Kena_MainOutfit.h"
#include "SpiritArrow.h"
#include "RotBomb.h"

/* NPCs */
#include "Rot.h"
#include "Beni.h"
#include "Saiya.h"
#include "CameraForNpc.h"
#include "RotWisp.h"

/* Enemies*/
#include "Moth.h"
#include "RockGolem.h"
#include "RotEater.h"
#include "Sticks01.h"
#include "VillageGuard.h"
#include "WoodKnight.h"
#include "BranchTosser.h"
#include "BranchTosser_Weapon.h"
#include "BranchTosser_Tree.h"
#include "ShieldStick.h"
#include "ShieldStick_Weapon.h"
#include "Sapling.h"
#include "Mage.h"
#include "BossWarrior.h"
#include "BossWarrior_Hat.h"
#include "BossShaman.h"
#include "FakeShaman.h"
#include "BossHunter.h"
#include "ShamanTrap.h"
#include "ShamanTrapHex.h"

/* Objects */
#include "Cliff_Rock.h"
#include "Tree.h"
#include "Crystal.h"
#include "BowTarget.h"
#include "Pillar.h"
#include "Stair.h"
#include "Border.h"
#include "Statue.h"
#include "Roots.h"
#include "PulseStone.h"
#include "Base_Ground.h"
#include "Wall.h"
#include "Slope.h"
#include "Bridge.h"
#include "Path_Mesh.h"
#include "FloorTile.h"
#include "Pulse_PlateForm.h"
#include "Gate.h"
#include "Rot.h"
#include "RuinKit.h"
#include "Stone.h"
#include "Grass.h"
#include "Flower.h"
#include "DeadZoneObj.h"
#include "Gimmick_EnviObj.h"
#include "Born_GroundCover.h"
#include "Pulse_Plate_Anim.h"
#include "Door_Anim.h"
#include "GroundMark.h"
#include "Rope_RotRock.h"
#include "LiftRot.h"
#include "LiftRot_Master.h"
#include "WaterPlane.h"
#include "TelePort_Flower.h"
#include "FieldBecon_Anim.h"

/* UI */
#include "BackGround.h"

/* Effects */
#include "Effect_Point_Instancing.h"
//Pulse
#include "E_KenaPulse.h"
#include "E_KenaPulseCloud.h"
#include "E_KenaPulseDot.h"
//Trail
#include "E_KenaTrail.h"
#include "E_RotTrail.h"

//Charge
#include "E_KenaCharge.h"
#include "E_KenaChargeImpact.h"
//Attack 
#include "E_KenaDust.h"
#include "E_KenaHeavyAttack_Into.h"

//Monster
#include "EnemyWisp.h"
#include "FireBullet.h"

#include "E_EnemyWispTrail.h"
#include "E_KenaDamage.h"
#include "E_KenaHit.h"
#include "E_KenaJump.h"
#include "E_PulseObject.h"
#include "E_EnemyWispBackground.h"
#include "E_EnemyWispGround.h"
#include "E_P_EnemyWisp.h"
#include "E_EnemyWispTrail.h"
#include "E_KenaDamage.h"
#include "E_KenaHit.h"
#include "E_KenaJump.h"
#include "E_PulseObject.h"
#include "E_Wind.h"
#include "E_P_Flower.h"
#include "E_P_KenaHeavyAttack_Into.h"
#include "E_SpiritArrowHit.h"
#include "E_SpiritArrowPosition.h"
#include "E_P_KenaHit.h"
#include "E_P_SpiritArrow.h"
#include "E_SpiritArrowTrail.h"
#include "E_D_Sphere.h"
#include "E_InteractStaff.h"
#include "E_P_InteractStaff.h"
#include "E_TeleportRot.h"
#include "E_Sapling.h"
#include "E_P_Sapling.h"
#include "E_FireBulletCover.h"
#include "E_FireBulletCloud.h"
#include "E_FireBulletExplosion.h"
#include "E_KenaParry.h"
#include "E_KenaParryHand.h"
#include "E_RotBombExplosion.h"
#include "E_P_Explosion.h"
#include "E_BombTrail.h"


/* ~Effects */

/* Components*/
#include "ControlMove.h"
#include "Interaction_Com.h"
#include "Camera_Player.h"
#include "CameraForRot.h"

/*Test Objects*/
#include "LodObject.h"
#include "Moth.h"
#include "RotForMonster.h"

unsigned int	g_LEVEL = 0;

#include "Json/json.hpp"
#include <fstream>


CLoader::CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

_uint APIENTRY LoadingThread(void* pArg)
{
	CLoader*		pLoader = (CLoader*)pArg;

	EnterCriticalSection(&pLoader->Get_CriticalSection());

	switch (pLoader->Get_NextLevelID())
	{
	case LEVEL_LOGO:
		pLoader->Loading_ForLogo();
		break;
	case LEVEL_GAMEPLAY:
		pLoader->Loading_ForGamePlay();
		break;
	case LEVEL_TESTPLAY:
		pLoader->Loading_ForTestPlay();
		break;
	case LEVEL_MAPTOOL:
		pLoader->Loading_ForMapTool();
		break;
	case LEVEL_EFFECT:
		pLoader->Loading_ForTestEffect();
		break;

	}

	LeaveCriticalSection(&pLoader->Get_CriticalSection());

	return 0;
}

HRESULT CLoader::Initialize(LEVEL eNextLevelID)
{
	m_eNextLevelID = eNextLevelID;

	g_LEVEL = eNextLevelID;

	InitializeCriticalSection(&m_Critical_Section);

	/* Make additional flow for Loading(Thread). */
	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, LoadingThread, this, 0, nullptr);
	if (0 == m_hThread)
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Loading_ForLogo()
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	lstrcpy(m_szLoadingText, TEXT("Loading Texture..."));

	lstrcpy(m_szLoadingText, TEXT("Loading VIBuffer..."));

	lstrcpy(m_szLoadingText, TEXT("Loading Model..."));

	lstrcpy(m_szLoadingText, TEXT("Loading Shader..."));

	lstrcpy(m_szLoadingText, TEXT("Loading Prototype GameObject..."));

	lstrcpy(m_szLoadingText, TEXT("Loading Complete!!"));

	m_isFinished = true;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLoader::Loading_ForGamePlay()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	
	FAILED_CHECK_RETURN(Loading_ForWJ((_uint)LEVEL_GAMEPLAY), E_FAIL);

	FAILED_CHECK_RETURN(Loading_ForSY((_uint)LEVEL_GAMEPLAY), E_FAIL);
	
	FAILED_CHECK_RETURN(Loading_ForJH((_uint)LEVEL_GAMEPLAY), E_FAIL);

	// FAILED_CHECK_RETURN(Loading_ForHW((_uint)LEVEL_GAMEPLAY), E_FAIL);

	FAILED_CHECK_RETURN(Loading_ForHO((_uint)LEVEL_GAMEPLAY), E_FAIL);

	FAILED_CHECK_RETURN(Loading_ForBJ((_uint)LEVEL_GAMEPLAY), E_FAIL);
		
	lstrcpy(m_szLoadingText, TEXT("Loading ETC..."));
	
	///* For.Prototype_Component_VIBuffer_Rect_Instancing */
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Rect_Instancing"),
	//	CVIBuffer_Rect_Instancing::Create(m_pDevice, m_pContext, 30))))
	//	return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("Loading Complete!!"));

	m_isFinished = true;
	Safe_Release(pGameInstance);
	return S_OK;
}

HRESULT CLoader::Loading_ForMapTool()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	lstrcpy(m_szLoadingText, TEXT("Loading Texture..."));
	/* For.Prototype_Component_Texture_Terrain */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Texture_Terrain"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Terrain_Texture/ForestGround/Test_%d.png"), 7))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Brush*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Texture_Normal"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Terrain_Texture/Flter_Texture_%d.png"), 2))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Filter */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Texture_Filter"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Terrain_Texture/Filter/Filter_%d.dds"), 3))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Filter */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Terrain_Two_Filter"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Terrain_Texture/Filter/Terrain2_Filter_%d.dds"), 3))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Filter */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Terrain_HeightMaps"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Terrain_Texture/Height/Terrain_Height_%d.bmp"), 15))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_GroundMark*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Texture_GroundMark"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/GroundMark/GroundMark%d.png"), 3))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Texture_Cave_Rock_MasterDiffuse"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/NonAnim/Textures/T_GDC_Grass01_D_NoisyAlpha.png")))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("Loading Model..."));
#pragma region ANIM_OBJ
	_matrix			PivotMatrix = XMMatrixIdentity();
	/* For.Prototype_Component_Model_DeadZoneTree */
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAPTOOL, L"Prototype_Component_Model_DeadZoneTree",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/DeadZoneTree_Anim/DeadzoneTree.mdat"), PivotMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_PulsePlateAnim*/
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAPTOOL, L"Prototype_Component_Model_PulsePlateAnim",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/PulsePlate_Anim/PulsePlate_Anim.model"), PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_TeleportFlowerAnim*/
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAPTOOL, L"Prototype_Component_Model_TeleportFlowerAnim",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/TeleportFlower/TeleportFlower.mdat"), PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_FieldBeaconAnim*/
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAPTOOL, L"Prototype_Component_Model_FieldBeaconAnim",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/FieldBeacon_Anim/FieldBeacon.mdat"), PivotMatrix))))
		return E_FAIL;

#pragma  endregion ANIM_OBJ
	
	if (FAILED(Loading_ForWJ((_uint)LEVEL_MAPTOOL)))
		return E_FAIL;

	_bool bRealObject = true;
#ifdef FOR_MAPTOOL   

#else
#pragma region Test_Gimmick_OBJ
	if (bRealObject == false)
	{
#pragma region Born_GroundCover
		PivotMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f);
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAPTOOL, L"Prototype_Component_Model_SK_BasilLarge",
			CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/SK_Basil/SK_BasilLarge.mdat"), PivotMatrix, nullptr, false, true))))
			return E_FAIL;
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAPTOOL, L"Prototype_Component_Model_SK_BasilMedium",
			CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/SK_Basil/SK_BasilMedium.mdat"), PivotMatrix, nullptr, false, true))))
			return E_FAIL;
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAPTOOL, L"Prototype_Component_Model_SK_BasilSmall",
			CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/SK_Basil/SK_BasilSmall.mdat"), PivotMatrix, nullptr, false, true))))
			return E_FAIL;
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAPTOOL, L"Prototype_Component_Model_SK_Bush_02",
			CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/SK_Bush/SK_Bush_02.mdat"), PivotMatrix, nullptr, false, true))))
			return E_FAIL;
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAPTOOL, L"Prototype_Component_Model_SK_Fern_01",
			CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/SK_Fern/SK_Fern_01.mdat"), PivotMatrix, nullptr, false, true))))
			return E_FAIL;
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAPTOOL, L"Prototype_Component_Model_SK_Fern_02",
			CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/SK_Fern/SK_Fern_02.mdat"), PivotMatrix, nullptr, false, true))))
			return E_FAIL;
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAPTOOL, L"Prototype_Component_Model_SK_Fern_03",
			CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/SK_Fern/SK_Fern_03.mdat"), PivotMatrix, nullptr, false, true))))
			return E_FAIL;
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAPTOOL, L"Prototype_Component_Model_SK_Fern_04",
			CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/SK_Fern/SK_Fern_04.mdat"), PivotMatrix, nullptr, false, true))))
			return E_FAIL;
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAPTOOL, L"Prototype_Component_Model_SK_Flower_02",
			CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/SK_Flower/SK_Flower_02.mdat"), PivotMatrix, nullptr, false, true))))
			return E_FAIL;
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAPTOOL, L"Prototype_Component_Model_SK_Flower_03",
			CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/SK_Flower/SK_Flower_03.mdat"), PivotMatrix, nullptr, false, true))))
			return E_FAIL;
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAPTOOL, L"Prototype_Component_Model_SK_Flower_05",
			CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/SK_Flower/SK_Flower_05.mdat"), PivotMatrix, nullptr, false, true))))
			return E_FAIL;
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAPTOOL, L"Prototype_Component_Model_SK_Mushroom_01",
			CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/SK_Mushroom/SK_Mushroom_01.mdat"), PivotMatrix, nullptr, false, true))))
			return E_FAIL;
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAPTOOL, L"Prototype_Component_Model_SK_Mushroom_02",
			CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/SK_Mushroom/SK_Mushroom_02.mdat"), PivotMatrix, nullptr, false, true))))
			return E_FAIL;
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAPTOOL, L"Prototype_Component_Model_SK_Pampas",
			CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/SK_Pampas/SK_Pampas.mdat"), PivotMatrix, nullptr, false, true))))
			return E_FAIL;
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAPTOOL, L"Prototype_Component_Model_SK_Plant_05",
			CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/SK_Plant/SK_Plant_05.mdat"), PivotMatrix, nullptr, false, true))))
			return E_FAIL;
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAPTOOL, L"Prototype_Component_Model_SK_Plants3",
			CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/SK_Plant/SK_Plants3.mdat"), PivotMatrix, nullptr, false, true, false))))
			return E_FAIL;

#pragma  endregion Born_GroundCover
	}
#pragma endregion

#endif FOR_MAPTOOL

#ifdef FOR_MAP_GIMMICK
	
	if (FAILED(Loading_ForJH((_uint)LEVEL_MAPTOOL)))
		return E_FAIL;

	if (FAILED(Loading_ForHO((_uint)LEVEL_MAPTOOL)))
		return E_FAIL;

	if (FAILED(Loading_ForBJ((_uint)LEVEL_MAPTOOL)))
		return E_FAIL;



#endif 

#ifdef FOR_MAPTOOL

#pragma region Born_GroundCover
	PivotMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f);
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAPTOOL, L"Prototype_Component_Model_SK_BasilLarge",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/SK_Basil/SK_BasilLarge.mdat"), PivotMatrix, nullptr, false, true))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAPTOOL, L"Prototype_Component_Model_SK_BasilMedium",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/SK_Basil/SK_BasilMedium.mdat"), PivotMatrix, nullptr, false, true))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAPTOOL, L"Prototype_Component_Model_SK_BasilSmall",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/SK_Basil/SK_BasilSmall.mdat"), PivotMatrix, nullptr, false, true))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAPTOOL, L"Prototype_Component_Model_SK_Bush_02",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/SK_Bush/SK_Bush_02.mdat"), PivotMatrix, nullptr, false, true))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAPTOOL, L"Prototype_Component_Model_SK_Fern_01",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/SK_Fern/SK_Fern_01.mdat"), PivotMatrix, nullptr, false, true))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAPTOOL, L"Prototype_Component_Model_SK_Fern_02",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/SK_Fern/SK_Fern_02.mdat"), PivotMatrix, nullptr, false, true))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAPTOOL, L"Prototype_Component_Model_SK_Fern_03",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/SK_Fern/SK_Fern_03.mdat"), PivotMatrix, nullptr, false, true))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAPTOOL, L"Prototype_Component_Model_SK_Fern_04",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/SK_Fern/SK_Fern_04.mdat"), PivotMatrix, nullptr, false, true))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAPTOOL, L"Prototype_Component_Model_SK_Flower_02",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/SK_Flower/SK_Flower_02.mdat"), PivotMatrix, nullptr, false, true))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAPTOOL, L"Prototype_Component_Model_SK_Flower_03",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/SK_Flower/SK_Flower_03.mdat"), PivotMatrix, nullptr, false, true))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAPTOOL, L"Prototype_Component_Model_SK_Flower_05",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/SK_Flower/SK_Flower_05.mdat"), PivotMatrix, nullptr, false, true))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAPTOOL, L"Prototype_Component_Model_SK_Mushroom_01",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/SK_Mushroom/SK_Mushroom_01.mdat"), PivotMatrix, nullptr, false, true))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAPTOOL, L"Prototype_Component_Model_SK_Mushroom_02",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/SK_Mushroom/SK_Mushroom_02.mdat"), PivotMatrix, nullptr, false, true))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAPTOOL, L"Prototype_Component_Model_SK_Pampas",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/SK_Pampas/SK_Pampas.mdat"), PivotMatrix, nullptr, false, true))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAPTOOL, L"Prototype_Component_Model_SK_Plant_05",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/SK_Plant/SK_Plant_05.mdat"), PivotMatrix, nullptr, false, true))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAPTOOL, L"Prototype_Component_Model_SK_Plants3",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/SK_Plant/SK_Plants3.mdat"), PivotMatrix, nullptr, false, true, false))))
		return E_FAIL;

#pragma  endregion Born_GroundCover

#pragma region GroundCover
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "GroundCover/Branches", true, true, true)))
		assert(!"Issue");
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "GroundCover/Clovers", true, true, true)))
		assert(!"Issue");
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "GroundCover/Deadzone", true, true, true)))
		assert(!"Issue");
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "GroundCover/Grass", true, true, true)))
		assert(!"Issue");
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "GroundCover/Needles", true, true, true)))
		assert(!"Issue");
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "GroundCover/Roots", true, true, true)))
		assert(!"Issue");
#pragma endregion GroundCover


#pragma region Foliage
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Foliage/BushDead", true, true, true)))
		assert(!"Issue");
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Foliage/Bushes", true, true, true)))
		assert(!"Issue");
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Foliage/Ferns", true, true, true)))
		assert(!"Issue");
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Foliage/Flowers", true, true, true)))
		assert(!"Issue");
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Foliage/Grass", true, true, true)))
		assert(!"Issue");
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Foliage/LilyPad", true, true, true)))
		assert(!"Issue");
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Foliage/Moss", true, true, true)))
		assert(!"Issue");
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Foliage/Overhang", true, true, true)))
		assert(!"Issue");
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Foliage/Plants", true, true, true)))
		assert(!"Issue");
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Foliage/Roots", true, true, true)))
		assert(!"Issue");
#pragma endregion Foliage

#pragma region AncientWells
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "AncientWells", true, true, true)))
		assert(!"Issue");
#pragma endregion ~AncientWells

#pragma region BowTarget
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Bow_Target", true, true, true)))
		assert(!"Issue");
#pragma endregion ~BowTarget

#pragma region Crystal
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Cave_Broken_Crystal", true, true, true)))
		assert(!"Issue");
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "PowerCrystal", true, false, true)))
		assert(!"Issue");
#pragma endregion ~Crystal


#pragma region Cliff
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Cliff/Cliff_Cap", true, true, true)))
		assert(!"Issue");
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Cliff/Cliff_Glitter", true, true, true)))
		assert(!"Issue");
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Cliff/Cliff_Ledge", true, true, true, false, true)))
		assert(!"Issue");
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Cliff/Cliff_Rock", true, true, true)))
		assert(!"Issue");
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Cliff/Cliff_Sheer", true, true, true)))
		assert(!"Issue");
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Cliff/Cliff_W2", true, true, true)))
		assert(!"Issue");
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Cliff/Cliff_W3", true, true, true)))
		assert(!"Issue");
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Cliff/Cliff_Wall_Large", true, true, true)))
		assert(!"Issue");
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Cliff/Cliff_Wall_Med", true, true, true)))
		assert(!"Issue");
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Cliff/Cliff_Wall_Short", true, true, true)))
		assert(!"Issue");

#pragma endregion ~Cliff


#pragma region Forest
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Forest_1/First_Room_Deco", true, true, true)))
		assert(!"Issue");
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Forest_1/ShrineOfDeath_MainRock", true, true, true)))
		assert(!"Issue");
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Forest_1/ShrineOfLife", true, true, true)))
		assert(!"Issue");
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Forest_1/RotGod_Statue_crumbled", true, false, true)))
		assert(!"Issue");
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Forest_1/Start_Gate", true, true, true)))
		assert(!"Issue");
#pragma region ~Forest

#pragma region PulseStone
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "PulseObjects/PluseStone_Big", true, false, true)))
		assert(!"Issue");
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "PulseObjects/PulsePlate_StoneBorder", true, false, true)))
		assert(!"Issue");
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "PulseObjects/PulsePlateEnclosure_02", true, false, true)))
		assert(!"Issue");
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "PulseObjects/PulseStoneRock", true, false, true)))
		assert(!"Issue");
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "PulseObjects/PulsePlate_Wood", true, false, true)))
		assert(!"Issue");
#pragma endregion PulseStone

#pragma  region Rock
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Rock/God_Rock", true, true, true, false, true)))
		assert(!"Issue");
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Rock/Rock_Arch", true, true, true)))
		assert(!"Issue");
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Rock/Rock_Big", true, true, true)))
		assert(!"Issue");
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Rock/Rock_Medium", true, true, true)))
		assert(!"Issue");
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Rock/Rock_Rubble", true, true, true)))
		assert(!"Issue");
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Rock/Rock_Small", true, true, true)))
		assert(!"Issue");
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Rock/Rock_Tiny", true, true, true)))
		assert(!"Issue");
#pragma endregion Rock

#pragma region Hanging_Objects
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Hanging_Objects/Hanging_Brazier", true, true)))// json NonUse
		assert(!"Issue");
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Hanging_Objects/HangingChimes", true, true)))// json NonUse
		assert(!"Issue");
#pragma endregion ~Hanging_Objects

#pragma region Roots
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Roots/Roots_Nomral", true, true)))// json NonUse
		assert(!"Issue");
#pragma endregion ~Roots

#pragma region Rot_Rock
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Rot/Rot_Rock", true, true,true)))
		assert(!"Issue");
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Rot/RotCarry_Piece", true, true,true)))
		assert(!"Issue");
#pragma endregion ~Rot_Rock

#pragma region RuinPlatform
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "RuinPlatform", true, true, true,false,true)))
		assert(!"Issue");
#pragma endregion ~RuinPlatform

#pragma region Save_StoneTree
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Save_StoneTree", true, true,true)))
		assert(!"Issue");
#pragma endregion ~Save_StoneTree

#pragma region Slopes
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Slopes/DirtBank", true, true)))// json NonUse
		assert(!"Issue");
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Slopes/DirtSlope_Spline02", true, true)))// json NonUse
		assert(!"Issue");
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Slopes/RootSlope_convex", true, true)))// json NonUse
		assert(!"Issue");
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Slopes/RootSlope_Rock", true, true)))// json NonUse
		assert(!"Issue");
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Slopes/SlideCap_Spline", true, true)))// json NonUse
		assert(!"Issue");
#pragma endregion ~Slopes


#pragma region Statue
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Statue", true, true, true)))
		assert(!"Issue");
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Statue/Owl_WrapShrine", true, true, true)))
		assert(!"Issue");
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Statue/RotGod_Statue", true, true)))		// json NonUse					
		assert(!"Issue");
#pragma endregion ~Statue

#pragma region Stone
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Stone/Stone_Bridge", true, true, true)))
		assert(!"Issue");
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Stone/Stone_Stairs", true, true, true)))
		assert(!"Issue");
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Stone/StoneFloor_1", true, true, true)))
		assert(!"Issue");
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Stone/StoneFloor_2", true, true, true)))
		assert(!"Issue");
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Stone/StonePath", true, true, true)))
		assert(!"Issue");
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "StoneStep_Seperate", true, true, true)))
		assert(!"Issue");
#pragma endregion Stone

#pragma  region Tree
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Trees/Giant", true, true, true)))
		assert(!"Issue");
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Trees/Bare", true, true, true)))
		assert(!"Issue");
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Trees/Billboard", true, true, true)))
		assert(!"Issue");
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Trees/Branches/Canopy", true, true, true)))
		assert(!"Issue");
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Trees/Branches/GodTree", true, true, true)))
		assert(!"Issue");
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Trees/Branches/Tree_Branch", true, true, true)))
		assert(!"Issue");
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Trees/Canopy", true, true, true)))
		assert(!"Issue");
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Trees/CeDarTree", true, true, true)))
		assert(!"Issue");
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Trees/Dead", true, true, true)))
		assert(!"Issue");
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Trees/Fallen", true, true, true)))
		assert(!"Issue");
#pragma endregion ~Tree

#pragma region VantagePlatform
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "VantagePlatform", true, true, true)))											// json NonUse
		assert(!"Issue");
#pragma endregion ~VantagePlatform

#pragma region RuinKit
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "RuinKit/RuinKit_LedgeStone", true, true, true)))
		assert(!"Issue");
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "RuinKit/RuinKit_Pillar", true, true, true)))
		assert(!"Issue");
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "RuinKit/RuinKit_Railing", true, true)))					// json NonUse
		assert(!"Issue");
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "RuinKit/RuinKit_Rubble", true, true, true)))
		assert(!"Issue");
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "RuinKit/RuinsKit_Brick", true, true, true)))
		assert(!"Issue");
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "RuinKit/RuinsKit_BridgeLarge", true, true, true)))
		assert(!"Issue");
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "RuinKit/RuinsKit_BridgeShort", true, true, true)))
		assert(!"Issue");
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "RuinKit/RuinStaris", true, true, true)))
		assert(!"Issue");
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "RuinKit/RuinsKit_ToriGate", true, true, true)))
		assert(!"Issue");
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "RuinKit/RuinsKit_Tree", true, true, true)))
		assert(!"Issue");
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "RuinKit/RuinKit_Piece", true, true, true)))
		assert(!"Issue");
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "RuinKit/RuinKit_Arch_Gate", true, true, true)))
		assert(!"Issue");
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "RuinKit/RuinsKit_FloorTile", true, true, true, false, true)))
		assert(!"Issue");
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "RuinKit/RuinsKit_Wall_Broken", true, true, true)))
		assert(!"Issue");
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "RuinKit/RuinsWall", true, true, true)))
		assert(!"Issue");
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "RuinKit/Ruinskit_Collum", true, true, true)))
		assert(!"Issue");
#pragma endregion RuinKit

#pragma region Interactive	
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Rope_RotRock", true, false, true)))
		assert(!"Issue");
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Forest_1/FirstTear_FallenTree", true, false, true)))
		assert(!"Issue");
#pragma endregion Interactive

#pragma region  DeadZone
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "DeadZone/Deadzondes_GoopWeb", true, true, true)))
		assert(!"Issue");
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "DeadZone/DeadZoneAndPurifiedTree", true, true, true)))
		assert(!"Issue");
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "DeadZone/DeadzoneClumps", true, true, true)))
		assert(!"Issue");
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "DeadZone/DeadzoneGroudCover", true, true, true)))
		assert(!"Issue");
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "DeadZone/DeadzoneHeart", true, true, true)))
		assert(!"Issue");
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "DeadZone/DeadzoneWall", true, true, true)))
		assert(!"Issue");
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "DeadZone/Gate", true, true, true)))
		assert(!"Issue");
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "DeadZone/Trees", true, true, true)))
		assert(!"Issue");
#pragma endregion DeadZone

#pragma region RuinDebris
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "RuinDebris", true, true)))
		assert(!"Issue");
#pragma endregion RuinDebris

#endif
	if (true == bRealObject)
	{
	
#pragma region GroundCover
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "GroundCover/Branches", true, true, true,true)))
			assert(!"Issue");
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "GroundCover/Clovers", true, true, true, true)))
			assert(!"Issue");
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "GroundCover/Deadzone", true, true, true, true)))
			assert(!"Issue");
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "GroundCover/Grass", true, true, true, true)))
			assert(!"Issue");
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "GroundCover/Needles", true, true, true, true)))
			assert(!"Issue");
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "GroundCover/Roots", true, true, true, true)))
			assert(!"Issue");
#pragma endregion GroundCover

#pragma region Foliage
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Foliage/BushDead", true, true, true, true)))
			assert(!"Issue");
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Foliage/Bushes", true, true, true, true)))
			assert(!"Issue");
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Foliage/Ferns", true, true, true, true)))
			assert(!"Issue");
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Foliage/Flowers", true, true, true, true)))
			assert(!"Issue");
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Foliage/Grass", true, true, true, true)))
			assert(!"Issue");
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Foliage/LilyPad", true, true, true, true)))
			assert(!"Issue");
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Foliage/Moss", true, true, true, true)))
			assert(!"Issue");
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Foliage/Overhang", true, true, true, true)))
			assert(!"Issue");
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Foliage/Plants", true, true, true, true)))
			assert(!"Issue");
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Foliage/Roots", true, true, true, true)))
			assert(!"Issue");
#pragma endregion Foliage

#pragma region Born_GroundCover
		PivotMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f);
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAPTOOL, L"Prototype_Component_Model_SK_BasilLarge",
			CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/SK_Basil/SK_BasilLarge.mdat"), PivotMatrix, nullptr, false, true))))
			return E_FAIL;
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAPTOOL, L"Prototype_Component_Model_SK_BasilMedium",
			CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/SK_Basil/SK_BasilMedium.mdat"), PivotMatrix, nullptr, false, true))))
			return E_FAIL;
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAPTOOL, L"Prototype_Component_Model_SK_BasilSmall",
			CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/SK_Basil/SK_BasilSmall.mdat"), PivotMatrix, nullptr, false, true))))
			return E_FAIL;
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAPTOOL, L"Prototype_Component_Model_SK_Bush_02",
			CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/SK_Bush/SK_Bush_02.mdat"), PivotMatrix, nullptr, false, true))))
			return E_FAIL;
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAPTOOL, L"Prototype_Component_Model_SK_Fern_01",
			CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/SK_Fern/SK_Fern_01.mdat"), PivotMatrix, nullptr, false, true))))
			return E_FAIL;
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAPTOOL, L"Prototype_Component_Model_SK_Fern_02",
			CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/SK_Fern/SK_Fern_02.mdat"), PivotMatrix, nullptr, false, true))))
			return E_FAIL;
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAPTOOL, L"Prototype_Component_Model_SK_Fern_03",
			CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/SK_Fern/SK_Fern_03.mdat"), PivotMatrix, nullptr, false, true))))
			return E_FAIL;
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAPTOOL, L"Prototype_Component_Model_SK_Fern_04",
			CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/SK_Fern/SK_Fern_04.mdat"), PivotMatrix, nullptr, false, true))))
			return E_FAIL;
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAPTOOL, L"Prototype_Component_Model_SK_Flower_02",
			CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/SK_Flower/SK_Flower_02.mdat"), PivotMatrix, nullptr, false, true))))
			return E_FAIL;
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAPTOOL, L"Prototype_Component_Model_SK_Flower_03",
			CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/SK_Flower/SK_Flower_03.mdat"), PivotMatrix, nullptr, false, true))))
			return E_FAIL;
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAPTOOL, L"Prototype_Component_Model_SK_Flower_05",
			CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/SK_Flower/SK_Flower_05.mdat"), PivotMatrix, nullptr, false, true))))
			return E_FAIL;
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAPTOOL, L"Prototype_Component_Model_SK_Mushroom_01",
			CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/SK_Mushroom/SK_Mushroom_01.mdat"), PivotMatrix, nullptr, false, true))))
			return E_FAIL;
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAPTOOL, L"Prototype_Component_Model_SK_Mushroom_02",
			CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/SK_Mushroom/SK_Mushroom_02.mdat"), PivotMatrix, nullptr, false, true))))
			return E_FAIL;
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAPTOOL, L"Prototype_Component_Model_SK_Pampas",
			CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/SK_Pampas/SK_Pampas.mdat"), PivotMatrix, nullptr, false, true))))
			return E_FAIL;
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAPTOOL, L"Prototype_Component_Model_SK_Plant_05",
			CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/SK_Plant/SK_Plant_05.mdat"), PivotMatrix, nullptr, false, true))))
			return E_FAIL;
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAPTOOL, L"Prototype_Component_Model_SK_Plants3",
			CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/SK_Plant/SK_Plants3.mdat"), PivotMatrix, nullptr, false, true, false))))
			return E_FAIL;

#pragma  endregion Born_GroundCover

#pragma  region Start_Forest_Room
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "PowerCrystal", true, false, true)))
			assert(!"Issue");
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "PulseObjects/PluseStone_Big", true, false, true)))
			return E_FAIL;
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "PulseObjects/PulsePlate_StoneBorder", true, false, true)))
			return E_FAIL;
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "PulseObjects/PulsePlateEnclosure_02", true, false, true)))
			return E_FAIL;
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "PulseObjects/PulseStoneRock", true, false, true)))
			return E_FAIL;
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "RuinKit/RuinKit_LedgeStone", true, true, true)))
			return E_FAIL;
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "RuinKit/RuinKit_Pillar", true, true, true)))
			return E_FAIL;
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "RuinKit/RuinsKit_Brick", true, true, true)))
			return E_FAIL;
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "RuinKit/RuinsKit_BridgeLarge", true, true, true)))
			return E_FAIL;
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "RuinKit/RuinsKit_ToriGate", true, true, true)))
			return E_FAIL;
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Rock/Rock_Arch", true, true, true)))
			assert(!"Issue");
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "RuinPlatform", true, true, true,false,true)))
			assert(!"Issue");
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Rock/Rock_Rubble", true, true, true)))
			return E_FAIL;
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Rock/Rock_Big", true, true, true)))
			assert(!"Issue");
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Cliff/Cliff_Rock", true, true, true)))
			return E_FAIL;
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Rock/God_Rock", true, true, true, false, true)))
			return E_FAIL;
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Cliff/Cliff_Cap", true, true, true)))
			assert(!"Issue");
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Cliff/Cliff_W2", true, true, true)))
			return E_FAIL;
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Cliff/Cliff_W3", true, true, true)))
			assert(!"Issue");
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Cliff/Cliff_Wall_Large", true, true, true)))
			return E_FAIL;
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Cliff/Cliff_Wall_Med", true, true, true)))
			return E_FAIL;
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Cliff/Cliff_Wall_Short", true, true, true)))
			return E_FAIL;
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Cliff/Cliff_Glitter", true, true, true)))
			assert(!"Issue");
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Cliff/Cliff_Sheer", true, true, true)))
			assert(!"Issue");
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "RuinKit/RuinKit_Rubble", true, true, true)))
			return E_FAIL;
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "RuinKit/RuinStaris", true, true, true)))
			return E_FAIL;
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Statue/Owl_WrapShrine", false, true, true)))
			return E_FAIL;
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Statue", true, true, true)))
			return E_FAIL;
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "RuinKit/RuinKit_Arch_Gate", true, true, true)))
			return E_FAIL;
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "RuinKit/RuinsKit_Tree", true, true, true)))
			return E_FAIL;
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "RuinKit/RuinsKit_FloorTile", true, true, true,false,true)))
			return E_FAIL;
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "RuinKit/RuinKit_Piece", true, true, true)))
			return E_FAIL;
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "RuinKit/RuinsKit_Wall_Broken", true, true, true)))
			return E_FAIL;
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "RuinKit/RuinsWall", true, true, true)))
			assert(!"Issue");
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "RuinKit/Ruinskit_Collum", true, true, true)))
			assert(!"Issue");
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Cliff/Cliff_Ledge", true, true, true, false, true)))
			return E_FAIL;
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Rock/Rock_Small", true, true, true)))
			return E_FAIL;
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Rock/Rock_Medium", true, true, true)))
			return E_FAIL;
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Forest_1/RotGod_Statue_crumbled", true, false, true)))
			return E_FAIL;
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Forest_1/Start_Gate", true, true, true)))
			return E_FAIL;
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Forest_1/FirstTear_FallenTree", true, false, true)))
			return E_FAIL;
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Forest_1/First_Room_Deco", true, true, true)))
			return E_FAIL;
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Forest_1/ShrineOfDeath_MainRock", true, true, true)))
			return E_FAIL;
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Forest_1/ShrineOfLife", true, true, true)))
			return E_FAIL;
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Stone/StoneFloor_1", true, true, true)))
			return E_FAIL;
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Stone/StoneFloor_2", true, true, true)))
			return E_FAIL;
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Stone/Stone_Bridge", true, true, true)))
			assert(!"Issue");
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Stone/Stone_Stairs", true, true, true)))
			assert(!"Issue");
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Trees/Giant", true, true, true)))
			return E_FAIL;
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Trees/Fallen", true, true, true)))
			return E_FAIL;
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Trees/Dead", true, true, true)))
			return E_FAIL;
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Trees/CeDarTree", true, true, true)))
			return E_FAIL;
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Trees/Bare", true, true, true)))
			return E_FAIL;
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Trees/Billboard", true, true, true)))
			return E_FAIL;
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Trees/Branches/Canopy", true, true, true)))
			return E_FAIL;
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Trees/Branches/GodTree", true, true, true)))
			return E_FAIL;
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Trees/Branches/Tree_Branch", true, true, true)))
			return E_FAIL;
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Trees/Canopy", true, true, true)))
			return E_FAIL;
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Save_StoneTree", true, true,true)))
			assert(!"Issue");
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "DeadZone/Deadzondes_GoopWeb", true, true, true)))
			assert(!"Issue");
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "DeadZone/DeadZoneAndPurifiedTree", true, true, true)))
			assert(!"Issue");
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "DeadZone/DeadzoneClumps", true, true, true)))
			assert(!"Issue");
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "DeadZone/DeadzoneGroudCover", true, true, true)))
			assert(!"Issue");
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "DeadZone/DeadzoneHeart", true, true, true)))
			assert(!"Issue");
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "DeadZone/DeadzoneWall", true, true, true)))
			assert(!"Issue");
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "DeadZone/Gate", true, true, true)))
			assert(!"Issue");
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "DeadZone/Trees", true, true, true)))
			assert(!"Issue");
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "DeadZone/BushDead_02", true, true)))
			assert(!"Issue");
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Rot/Rot_Rock", true, true,true)))
			assert(!"Issue");
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Rot/RotCarry_Piece", true, true,true)))
			assert(!"Issue");
		
#pragma endregion ~Start_Forest_Room
	}
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Map_Base")))
		assert(!"Issue");
	
	lstrcpy(m_szLoadingText, TEXT("Loading Collider..."));
	/* For.Prototype_Component_Collider_AABB*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Collider_AABB"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_AABB))))
		assert(!"Issue");
	/* For.Prototype_Component_Collider_OBB*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Collider_OBB"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_OBB))))
		assert(!"Issue");
	/* For.Prototype_Component_Collider_SPHERE*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Collider_SPHERE"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_SPHERE))))
		assert(!"Issue");

	lstrcpy(m_szLoadingText, TEXT("Loading VIBuffer..."));
	/* For.Prototype_Component_VIBuffer_Terrain */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_VIBuffer_Terrain_0"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Terrain_Texture/Real_Height/Terrain_Height_0.bmp")))))
		assert(!"Issue");
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_VIBuffer_Terrain_1"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Terrain_Texture/Real_Height/Terrain_Height_1.bmp")))))
		assert(!"Issue");
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_VIBuffer_Terrain_2"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Terrain_Texture/Real_Height/Terrain_Height_2.bmp")))))
		assert(!"Issue");
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_VIBuffer_Terrain_3"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Terrain_Texture/Real_Height/Terrain_Height_3.bmp")))))
		assert(!"Issue");

	/*map_Object Compoents */
	/* For.Prototype_Component_ControlMove*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_ControlMove"),
		CControlMove::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_Component_Interaction_Com*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Interaction_Com"),
		CInteraction_Com::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("Loading GameObjects..."));
	
	/* For.Prototype_GameObject_Cave_Rock */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Cliff_Rock"),
		CCliff_Rock::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_Terrain */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Terrain"),
		CTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_Tree */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Tree"),
		CTree::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_Crystal */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Crystal"),
		CCrystal::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_BowTarget */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BowTarget"),
		CBowTarget::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_PulseStone */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_PulseStone"),
		CPulseStone::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_BaseGround */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BaseGround"),
		CBase_Ground::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_Wall */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Wall"),
		CWall::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_Wall */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Root"),
		CRoots::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_Stair */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Stair"),
		CStair::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_Wall */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Pillar"),
		CPillar::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_Statue */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Statue"),
		CStatue::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_Border */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Border"),
		CBorder::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_Slope */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Slope"),
		CSlope::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_Bridge */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Bridge"),
		CBridge::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_Path_Mesh */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Path_Mesh"),
		CPath_Mesh::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_FloorTile */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_FloorTile"),
		CFloorTile::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_Pulse_PlateForm */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Pulse_PlateForm"),
		CPulse_PlateForm::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_Gate */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Gate"),
		CGate::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_RuinKit */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_RuinKit"),
		CRuinKit::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_RuinKit */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Stone"),
		CStone::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_RuinKit */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Flower"),
		CFlower::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_RuinKit */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Grass"),
		CGrass::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_DeadZoneObj */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_DeadZoneObj"),
		CDeadZoneObj::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_DeadZoneObj */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_GimmickObj"),
		CGimmick_EnviObj::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_ControlRoom */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ControlRoom"),
		CControlRoom::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/*Anim*/
	/* For.Prototype_GameObject_Door_Anim */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_DoorAnim"),
		CDoor_Anim::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Door_Anim */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_PulsePlateAnim"),
		CPulse_Plate_Anim::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_TeleportFlower */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TeleportFlower"),
		CTelePort_Flower::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_FieldBecon_Anim */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_FieldBecon_Anim"),
		CFieldBecon_Anim::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	

	/* For.Prototype_GameObject_GroundMark */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_GroundMark"),
		CGroundMark::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Born_GroundCover"),
		CBorn_GroundCover::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("Loading End."));

	m_isFinished = true;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLoader::Loading_ForTestPlay()
{	
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	FAILED_CHECK_RETURN(Loading_ForWJ((_uint)LEVEL_TESTPLAY), E_FAIL);
	
	FAILED_CHECK_RETURN(Loading_ForSY((_uint)LEVEL_TESTPLAY), E_FAIL);

	FAILED_CHECK_RETURN(Loading_ForJH((_uint)LEVEL_TESTPLAY), E_FAIL);

	FAILED_CHECK_RETURN(Loading_ForHW((_uint)LEVEL_TESTPLAY), E_FAIL);
		
	FAILED_CHECK_RETURN(Loading_ForHO((_uint)LEVEL_TESTPLAY), E_FAIL);

	FAILED_CHECK_RETURN(Loading_ForBJ((_uint)LEVEL_TESTPLAY), E_FAIL);

	m_isFinished = true;
	SetWindowText(g_hWnd, TEXT("Loading Complete!! Wait a moment"));
	Safe_Release(pGameInstance);
	return S_OK;
}

HRESULT CLoader::Loading_ForTestEffect()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		
	FAILED_CHECK_RETURN(Loading_ForHO((_uint)LEVEL_EFFECT), E_FAIL);
	
	FAILED_CHECK_RETURN(Loading_ForJH((_uint)LEVEL_EFFECT), E_FAIL);

	_matrix	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_EFFECT, L"Prototype_Component_Model_Rot",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/Rot/Rot.mdat"), PivotMatrix)))) return E_FAIL;

	// Prototype_GameObject_RotForMonster
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_RotForMonster"), CRotForMonster::Create(m_pDevice, m_pContext)))) return E_FAIL;

	// Prototype_Component_Model_Sticks01
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_EFFECT, L"Prototype_Component_Model_Sticks01",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/Enemy/Sticks01/Sticks01.model"), PivotMatrix)))) return E_FAIL;

	// Prototype_Component_Model_Mage
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_EFFECT, L"Prototype_Component_Model_Mage",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/Enemy/Mage/Mage.model"), PivotMatrix)))) return E_FAIL;

	// Prototype_GameObject_Sticks01
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Sticks01"), CSticks01::Create(m_pDevice, m_pContext)))) return E_FAIL;

	// Prototype_GameObject_Mage
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Mage"), CMage::Create(m_pDevice, m_pContext)))) return E_FAIL;

	// Prototype_GameObject_FireBullet
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_FireBullet"), CFireBullet::Create(m_pDevice, m_pContext)))) return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("Loading End."));
	
	m_isFinished = true;
	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CLoader::LoadNonAnimModel(_uint iLevelIndex)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	char FindPath[MAX_PATH] = "../Bin/Resources/NonAnim/*.*";
	char FilePath[MAX_PATH] = "../Bin/Resources/NonAnim/";

	_tchar WideFilePath[MAX_PATH] = L"";

	_finddata_t FindData;
	intptr_t handle = _findfirst(FindPath, &FindData);
	if (handle == -1) return E_FAIL;

	char szFullPath[MAX_PATH] = "";
	char szFileName[MAX_PATH] = "";
	char szExt[MAX_PATH] = "";

	const _tchar *pPrototype = L"Prototype_Component_Model_";
	int iPrototyeLengh = lstrlen(pPrototype);

	_matrix PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);

	int iResult = 0;
	while (iResult != -1)
	{
		strcpy_s(szFullPath, FilePath);
		strcat_s(szFullPath, FindData.name);

		_splitpath_s(szFullPath, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExt, MAX_PATH);
		if (!strcmp(szExt, ".mdat"))
		{
			int iFileNameLen = (int)strlen(szFileName) + 1;
			_tchar* pFileName = new _tchar[iFileNameLen];
			ZeroMemory(pFileName, sizeof(_tchar) * iFileNameLen);

			int iTagLen = iPrototyeLengh + iFileNameLen;
			_tchar* pPrototypeTag = new _tchar[iTagLen];
			ZeroMemory(pPrototypeTag, iTagLen);
			lstrcat(pPrototypeTag, pPrototype);

			MultiByteToWideChar(CP_ACP, 0, szFullPath, _int(strlen(szFullPath)) + 1, WideFilePath, _int(strlen(szFullPath)) + 1);
			MultiByteToWideChar(CP_ACP, 0, szFileName, iFileNameLen, pFileName, iFileNameLen);
			lstrcat(pPrototypeTag, pFileName);

			if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, pPrototypeTag,
				CModel::Create(m_pDevice, m_pContext, WideFilePath, PivotMatrix))))
				return E_FAIL;

			Safe_Delete_Array(pFileName);
			pGameInstance->Add_String(iLevelIndex, pPrototypeTag);
		}

		iResult = _findnext(handle, &FindData);
	}

	_findclose(handle);
	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLoader::LoadNonAnimFolderModel(_uint iLevelIndex, string strFolderName, _bool bIsLod, _bool bIsInstancing, 
	_bool bIsJsonMatarial, _bool bPivotScale, _bool bUseTriangleMeshActor)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	//char FindPath[MAX_PATH] = "../Bin/Resources/NonAnim/*.*";
	//char FilePath[MAX_PATH] = "../Bin/Resources/NonAnim/";

	char FindPath[MAX_PATH] = "../Bin/Resources/NonAnim/";

	strcat_s(FindPath, MAX_PATH, strFolderName.c_str());
	strcat_s(FindPath, MAX_PATH, "/*.*");

	char FilePath[MAX_PATH] = "../Bin/Resources/NonAnim/";
	strcat_s(FilePath, MAX_PATH, strFolderName.c_str());
	strcat_s(FilePath, MAX_PATH, "/");

	_tchar WideFilePath[MAX_PATH] = L"";

	_finddata_t FindData;
	intptr_t handle = _findfirst(FindPath, &FindData);
	if (handle == -1) return E_FAIL;

	char szFullPath[MAX_PATH] = "";
	char szJSonFullPath[MAX_PATH] = "";
	char szFileName[MAX_PATH] = "";
	char szExt[MAX_PATH] = "";

	const _tchar *pPrototype = L"Prototype_Component_Model_";
	int iPrototyeLengh = lstrlen(pPrototype);

	_matrix PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	
	if(bPivotScale)
		PivotMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f);

	int iResult = 0;
	while (iResult != -1)
	{
		strcpy_s(szFullPath, FilePath);
		strcpy_s(szJSonFullPath, FilePath);
		strcat_s(szFullPath, FindData.name);

		_splitpath_s(szFullPath, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExt, MAX_PATH);
		if (!strcmp(szExt, ".mdat"))
		{
			if (bIsJsonMatarial == true)
			{
				strcat_s(szJSonFullPath, szFileName);
				strcat_s(szJSonFullPath, ".json");
			}
			else
				strcpy_s(szJSonFullPath, sizeof(char) * 5, "NULL");

			int iFileNameLen = (int)strlen(szFileName) + 1;
			_tchar* pFileName = new _tchar[iFileNameLen];
			ZeroMemory(pFileName, sizeof(_tchar) * iFileNameLen);

			int iTagLen = iPrototyeLengh + iFileNameLen;
			_tchar* pPrototypeTag = new _tchar[iTagLen];
			ZeroMemory(pPrototypeTag, iTagLen);
			lstrcat(pPrototypeTag, pPrototype);

			MultiByteToWideChar(CP_ACP, 0, szFullPath, _int(strlen(szFullPath)) + 1, WideFilePath, _int(strlen(szFullPath)) + 1);
			MultiByteToWideChar(CP_ACP, 0, szFileName, iFileNameLen, pFileName, iFileNameLen);
			lstrcat(pPrototypeTag, pFileName);

			if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, pPrototypeTag,
				CModel::Create(m_pDevice, m_pContext, WideFilePath, PivotMatrix, nullptr, bIsLod, bIsInstancing, szJSonFullPath, bUseTriangleMeshActor))))
				return E_FAIL;

			Safe_Delete_Array(pFileName);
			pGameInstance->Add_String(iLevelIndex, pPrototypeTag);
		}
		iResult = _findnext(handle, &FindData);
	}

	_findclose(handle);
	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLoader::Loading_ForWJ(_uint iLevelIndex)
{
	lstrcpy(m_szLoadingText, TEXT("Loading 원준..."));

	if (FAILED(LoadNonAnimFolderModel(iLevelIndex, "Water")))
		return E_FAIL;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance)

	// Prototype_GameObject_WaterPlane
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_WaterPlane"), 
		CWaterPlane::Create(m_pDevice, m_pContext)))) return E_FAIL;

	RELEASE_INSTANCE(CGameInstance)

	return S_OK;
}

HRESULT CLoader::Loading_ForJH(_uint iLevelIndex)
{
	lstrcpy(m_szLoadingText, TEXT("Loading 재호..."));

	_matrix	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	/* COMPONENTS */
	/* Prototype_Component_Model_Kena */
	FAILED_CHECK_RETURN(CGameInstance::GetInstance()->Add_Prototype(iLevelIndex, L"Prototype_Component_Model_Kena", CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/Kena/Kena_Body.model"), PivotMatrix)), E_FAIL);
	
	/* Prototype_Component_Model_Kena_Staff */
	FAILED_CHECK_RETURN(CGameInstance::GetInstance()->Add_Prototype(iLevelIndex, L"Prototype_Component_Model_Kena_Staff", CModel::Create(m_pDevice, m_pContext, L"../Bin/Resources/Anim/Kena/Staff/Kena_Staff.model", PivotMatrix)), E_FAIL);

	/* Prototype_Component_Model_Kena_MainOutfit */
	FAILED_CHECK_RETURN(CGameInstance::GetInstance()->Add_Prototype(iLevelIndex, L"Prototype_Component_Model_Kena_MainOutfit", CModel::Create(m_pDevice, m_pContext, L"../Bin/Resources/Anim/Kena/Outfit/MainOutfit/Kena_MainOutfit.model", PivotMatrix)), E_FAIL);

	/* Prototype_Component_Model_Rot_Bomb */
	PivotMatrix = XMMatrixScaling(0.0025f, 0.0025f, 0.0025f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	FAILED_CHECK_RETURN(CGameInstance::GetInstance()->Add_Prototype(iLevelIndex, L"Prototype_Component_Model_Rot_Bomb", CModel::Create(m_pDevice, m_pContext, L"../Bin/Resources/Anim/Rot Bomb/Rot_Bomb.model", PivotMatrix)), E_FAIL);

	/* Prototype_Component_Model_Spirit_Arrow */
	FAILED_CHECK_RETURN(LoadNonAnimFolderModel(iLevelIndex, "Spirit_Arrow", false, false, false), E_FAIL);

	/* GAMEOBJECTS */
	/* Prototype_GameObject_Kena */
	FAILED_CHECK_RETURN(CGameInstance::GetInstance()->Add_Prototype(L"Prototype_GameObject_Kena", CKena::Create(m_pDevice, m_pContext)), E_FAIL);

	/* Prototype_GameObject_Kena_Staff */
	FAILED_CHECK_RETURN(CGameInstance::GetInstance()->Add_Prototype(L"Prototype_GameObject_Kena_Staff", CKena_Staff::Create(m_pDevice, m_pContext)), E_FAIL);

	/* Prototype_GameObject_Kena_MainOutfit */
	FAILED_CHECK_RETURN(CGameInstance::GetInstance()->Add_Prototype(L"Prototype_GameObject_Kena_MainOutfit", CKena_MainOutfit::Create(m_pDevice, m_pContext)), E_FAIL);

	/* Prototype_GameObject_Rot_Bomb */
	FAILED_CHECK_RETURN(CGameInstance::GetInstance()->Add_Prototype(L"Prototype_GameObject_Rot_Bomb", CRotBomb::Create(m_pDevice, m_pContext)), E_FAIL);

	/* Prototype_GameObject_BombTrail */
	FAILED_CHECK_RETURN(CGameInstance::GetInstance()->Add_Prototype(L"Prototype_GameObject_BombTrail", CE_BombTrail::Create(m_pDevice, m_pContext)), E_FAIL);

	/* Prototype_GameObject_Spirit_Arrow */
	FAILED_CHECK_RETURN(CGameInstance::GetInstance()->Add_Prototype(L"Prototype_GameObject_SpiritArrow", CSpiritArrow::Create(m_pDevice, m_pContext)), E_FAIL);

	/* Prototype_GameObject_Player_Camera */
	FAILED_CHECK_RETURN(CGameInstance::GetInstance()->Add_Prototype(L"Prototype_GameObject_Camera_Player", CCamera_Player::Create(m_pDevice, m_pContext)), E_FAIL);

	return S_OK;
}

HRESULT CLoader::Loading_ForSY(_uint iLevelIndex)
{
	lstrcpy(m_szLoadingText, TEXT("Loading 소영..."));

	return S_OK;
}

HRESULT CLoader::Loading_ForBJ(_uint iLevelIndex)
{
	lstrcpy(m_szLoadingText, TEXT("Loading 병주..."));

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	_matrix PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	// Prototype_Component_Model_Beni
	if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, L"Prototype_Component_Model_Beni",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/NPC/Beni/Beni.model"), PivotMatrix)))) return E_FAIL;

	// Prototype_Component_Model_Saiya
	if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, L"Prototype_Component_Model_Saiya",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/NPC/Saiya/Saiya.model"), PivotMatrix)))) return E_FAIL;

	// Prototype_Component_Model_RotWisp
	if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, L"Prototype_Component_Model_RotWisp",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/RotWisp/RotWisp.mdat"), PivotMatrix)))) return E_FAIL;

	// Prototype_Component_Texture_T_GR_Noise_Smooth_A
	if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, L"Prototype_Component_Texture_T_GR_Noise_Smooth_A",
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/RotWisp/T_GR_Noise_Smooth_A.png"))))) return E_FAIL;
	
	// Prototype_Component_Model_Mage
	if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, L"Prototype_Component_Model_Mage",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/Enemy/Mage/Mage.model"), PivotMatrix)))) return E_FAIL;

	// Prototype_Component_Model_ShieldStick
	if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, L"Prototype_Component_Model_ShieldStick",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/Enemy/ShieldSticks/ShieldSticks.model"), PivotMatrix)))) return E_FAIL;

	// Prototype_Component_Model_ShieldStick_Weapon
	if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, L"Prototype_Component_Model_ShieldStick_Weapon",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/Enemy/ShieldSticks/Shield.mdat"), PivotMatrix)))) return E_FAIL;

	// Prototype_Component_Model_RockGolem
	if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, L"Prototype_Component_Model_RockGolem",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/Enemy/RockGolem/RockGolem.model"), PivotMatrix)))) return E_FAIL;

	// Prototype_Component_Model_RotEater
	if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, L"Prototype_Component_Model_RotEater",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/Enemy/RotEater/RotEater.model"), PivotMatrix)))) return E_FAIL;

	// Prototype_Component_Model_Sticks01
	if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, L"Prototype_Component_Model_Sticks01",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/Enemy/Sticks01/Sticks01.model"), PivotMatrix)))) return E_FAIL;

	// Prototype_Component_Model_VillageGuard
	if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, L"Prototype_Component_Model_VillageGuard",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/Enemy/VillageGuard/VillageGuard.mdat"), PivotMatrix)))) return E_FAIL;

	// Prototype_Component_Model_WoodKnight
	if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, L"Prototype_Component_Model_WoodKnight",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/Enemy/WoodKnight/WoodKnight.model"), PivotMatrix)))) return E_FAIL;

	// Prototype_Component_Model_Sapling
	if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, L"Prototype_Component_Model_Sapling",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/Enemy/Sapling/Sapling.model"), PivotMatrix)))) return E_FAIL;

	// Prototype_Component_Model_BranchTosser
	if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, L"Prototype_Component_Model_BranchTosser",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/Enemy/BranchTosser/BranchTosser.model"), PivotMatrix)))) return E_FAIL;

	/**********************************/
	/************For.Warrior***********/
	/**********************************/
	// Prototype_Component_Model_Boss_Warrior
	if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, L"Prototype_Component_Model_Boss_Warrior",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/Enemy/Boss_Warrior/Boss_Warrior.mdat"), PivotMatrix)))) return E_FAIL;

	// Prototype_Component_Model_Boss_Warrior_Hat
	if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, L"Prototype_Component_Model_Boss_Warrior_Hat",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/NonAnim/Enemy/Boss_Warrior_Hat/Boss_Warrior_Hat.mdat"), PivotMatrix)))) return E_FAIL;
	/*********************************************************************************************************************************************/

	/**********************************/
	/************ For. Hunter **********/
	/**********************************/
	// Prototype_Component_Model_Boss_Hunter
	if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, L"Prototype_Component_Model_Boss_Hunter",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/Enemy/Boss_Hunter/Boss_Hunter.mdat"), PivotMatrix)))) return E_FAIL;

	// Prototype_Component_Model_Boss_Hunter_Arrow
	if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, L"Prototype_Component_Model_Boss_Hunter_Arrow",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/NonAnim/Boss_HunterArrow/Arrow_NonAnim.mdat"), PivotMatrix)))) return E_FAIL;
	/*********************************************************************************************************************************************/

	/**********************************/
	/************For. Shaman**********/
	/**********************************/
	// Prototype_Component_Model_Boss_Shaman
	if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, L"Prototype_Component_Model_Boss_Shaman",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/Enemy/Boss_Shaman/Boss_Shaman.mdat"), PivotMatrix)))) return E_FAIL;

	// Prototype_Component_Model_ShamanTrap
	if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, L"Prototype_Component_Model_ShamanTrap",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/Enemy/Boss_TrapAsset/ShamanTrap.mdat"), PivotMatrix)))) return E_FAIL;

	// Prototype_Component_Model_ShamanTrapHex
	if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, L"Prototype_Component_Model_ShamanTrapHex",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/Enemy/Boss_TrapAsset/ShamanTrapHex.mdat"), PivotMatrix)))) return E_FAIL;

	// Prototype_Component_Model_FakeShaman
	if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, L"Prototype_Component_Model_FakeShaman",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/NonAnim/Boss_FakeShaman/FakeShaman.mdat"), PivotMatrix)))) return E_FAIL;
	/*********************************************************************************************************************************************/


	// Prototype_Component_Model_Rope_Rock
	if (FAILED(LoadNonAnimFolderModel(iLevelIndex, "Rope_RotRock", true, false, true))) return E_FAIL;

	// Prototype_Component_Model_BranchTosser_Projectile	
	if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, L"Prototype_Component_Model_BranchTosser_Projectile",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/NonAnim/Enemy/BranchTosser/BranchTosser_Projectile.mdat"), PivotMatrix)))) return E_FAIL;

	// Prototype_Component_Model_MY_BranchTosser_Tree
	_matrix TempMatrix = XMMatrixScaling(0.1f, 0.05f, 0.1f);
	if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, L"Prototype_Component_Model_MY_BranchTosser_Tree",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/NonAnim/Enemy/BranchTosser/BranchTosser_Projectile.mdat"), TempMatrix)))) return E_FAIL;
	
	// Prototype_Component_Model_HeroRot
	if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, L"Prototype_Component_Model_HeroRot",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/HeroRot/HeroRot.mdat"), PivotMatrix)))) return E_FAIL;

	// Prototype_Component_Model_Rot
	if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, L"Prototype_Component_Model_Rot",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/Rot/Rot.mdat"), PivotMatrix)))) return E_FAIL;

	// Prototype_Component_Model_CorruptVillager
	if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, L"Prototype_Component_Model_CorruptVillager",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/Enemy/CorruptVillager/CorruptVillager.mdat"), PivotMatrix)))) return E_FAIL;

	// Prototype_Component_Model_Moth
	if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, L"Prototype_Component_Model_Moth",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/Enemy/Moth/Moth.mdat"), PivotMatrix)))) return E_FAIL;
		
	// Prototype_GameObject_CameraForRot
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_CameraForRot"), CCameraForRot::Create(m_pDevice, m_pContext)))) return E_FAIL;
	
	// Prototype_GameObject_CameraForNpc
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_CameraForNpc"), CCameraForNpc::Create(m_pDevice, m_pContext)))) return E_FAIL;	
	
	// Prototype_GameObject_Moth
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Moth"), CMoth::Create(m_pDevice, m_pContext)))) return E_FAIL;	
	
	// Prototype_GameObject_RockGolem
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_RockGolem"), CRockGolem::Create(m_pDevice, m_pContext)))) return E_FAIL;	
	
	// Prototype_GameObject_RotEater
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_RotEater"), CRotEater::Create(m_pDevice, m_pContext)))) return E_FAIL;	
	
	// Prototype_GameObject_Sticks01
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Sticks01"), CSticks01::Create(m_pDevice, m_pContext)))) return E_FAIL;	
	
	// Prototype_GameObject_VillageGuard
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_VillageGuard"), CVillageGuard::Create(m_pDevice, m_pContext)))) return E_FAIL;
	
	// Prototype_GameObject_WoodKnight
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_WoodKnight"), CWoodKnight::Create(m_pDevice, m_pContext)))) return E_FAIL;
	
	// Prototype_GameObject_Mage
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Mage"), CMage::Create(m_pDevice, m_pContext)))) return E_FAIL;
	
	// Prototype_GameObject_BranchTosser
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BranchTosser"), CBranchTosser::Create(m_pDevice, m_pContext)))) return E_FAIL;
	
	// Prototype_GameObject_BranchTosserWeapon
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BranchTosserWeapon"), CBranchTosser_Weapon::Create(m_pDevice, m_pContext)))) return E_FAIL;
	
	// Prototype_GameObject_BranchTosserWeapon
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BranchTosserTree"), CBranchTosser_Tree::Create(m_pDevice, m_pContext)))) return E_FAIL;

	// Prototype_GameObject_ShieldStick
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ShieldStick"), CShieldStick::Create(m_pDevice, m_pContext)))) return E_FAIL;

	// Prototype_GameObject_ShieldStickWeapon
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ShieldStickWeapon"), CShieldStick_Weapon::Create(m_pDevice, m_pContext)))) return E_FAIL;
	
	// Prototype_GameObject_Sapling
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Sapling"), CSapling::Create(m_pDevice, m_pContext)))) return E_FAIL;
	
	// Prototype_GameObject_Rot
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Rot"), CRot::Create(m_pDevice, m_pContext)))) return E_FAIL;
	
	// Prototype_GameObject_CRope_RotRock
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_CRope_RotRock"), CRope_RotRock::Create(m_pDevice, m_pContext)))) return E_FAIL;
	
	// Prototype_GameObject_LiftRot
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_LiftRot"), CLiftRot::Create(m_pDevice, m_pContext)))) return E_FAIL;
	
	// Prototype_GameObject_LiftRot_Master
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_LiftRot_Master"), CLiftRot_Master::Create(m_pDevice, m_pContext)))) return E_FAIL;
	
	// Prototype_GameObject_RotForMonster
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_RotForMonster"), CRotForMonster::Create(m_pDevice, m_pContext)))) return E_FAIL;
	
	// Prototype_GameObject_FireBullet
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_FireBullet"), CFireBullet::Create(m_pDevice, m_pContext)))) return E_FAIL;

	// Prototype_GameObject_Beni
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Beni"), CBeni::Create(m_pDevice, m_pContext)))) return E_FAIL;
	
	// Prototype_GameObject_Saiya
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Saiya"), CSaiya::Create(m_pDevice, m_pContext)))) return E_FAIL;

	// Prototype_GameObject_BossWarrior
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BossWarrior"), CBossWarrior::Create(m_pDevice, m_pContext)))) return E_FAIL;

	// Prototype_GameObject_BossWarrior_Hat
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BossWarrior_Hat"), CBossWarrior_Hat::Create(m_pDevice, m_pContext)))) return E_FAIL;

	// Prototype_GameObject_BossShaman
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BossShaman"), CBossShaman::Create(m_pDevice, m_pContext)))) return E_FAIL;

	// Prototype_GameObject_BossFakeShaman
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BossFakeShaman"), CFakeShaman::Create(m_pDevice, m_pContext)))) return E_FAIL;

	// Prototype_GameObject_ShamanTrap
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ShamanTrap"), CShamanTrap::Create(m_pDevice, m_pContext)))) return E_FAIL;

	// Prototype_GameObject_BossHunter
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BossHunter"), CBossHunter::Create(m_pDevice, m_pContext)))) return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Loading_ForHO(_uint iLevelIndex)
{
	lstrcpy(m_szLoadingText, TEXT("Loading 혜원..."));

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	_matrix	 PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
		
#pragma region EFFECT_COMPONENT

	/* For.Prototype_Component_Texture_Effect */
	if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, TEXT("Prototype_Component_Texture_Effect"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/DiffuseTexture/E_Effect_%d.png"), 134))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_NormalEffect */
	if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, TEXT("Prototype_Component_Texture_NormalEffect"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/NormalTexture/N_Effect_%d.png"), 11))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_PulseShield_Dissolve */
	if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, TEXT("Prototype_Component_Texture_PulseShield_Dissolve"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/PulseShield_Dissolve/E_Effect_%d.png"), 4))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_TrailFlow */
	if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, TEXT("Prototype_Component_Texture_TrailFlow"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Trail/flow/E_Flow_%d.png"), 10))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_TrailType */
	if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, TEXT("Prototype_Component_Texture_TrailType"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Trail/shapetype/E_Type_%d.png"), 11))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Point_Instancing */
	if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, TEXT("Prototype_Component_VIBuffer_Trail"),
		CVIBuffer_Trail::Create(m_pDevice, m_pContext, 300))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Cube */
	if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, L"Prototype_Component_Model_Cube",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Effect/Cube.mdat"), PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Cone */
	if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, L"Prototype_Component_Model_Cone",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Effect/Cone.mdat"), PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Cylinder */
	if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, L"Prototype_Component_Model_Cylinder",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Effect/Cylinder.mdat"), PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Plane */
	if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, L"Prototype_Component_Model_Plane",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Effect/Plane.mdat"), PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Sphere */
	if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, L"Prototype_Component_Model_Sphere",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Effect/Sphere.mdat"), PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_shockball */
	if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, L"Prototype_Component_Model_shockball",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Effect/shockball.mdat"), PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_shockball3 */
	if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, L"Prototype_Component_Model_shockball3",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Effect/shockball3.mdat"), PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Wind */
	if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, L"Prototype_Component_Model_Wind",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Effect/Wind.mdat"), PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_WindLoop */
	if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, L"Prototype_Component_Model_WindLoop",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Effect/WindLoop.mdat"), PivotMatrix))))
		return E_FAIL;

#pragma endregion EFFECT_COMPONENT

	lstrcpy(m_szLoadingText, TEXT("Loading Effects MonsterObject..."));
#pragma  region	MONSTER
	/* For.Prototype_Component_Model_EnemyWisp */
	if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, L"Prototype_Component_Model_EnemyWisp",
		CModel::Create(m_pDevice, m_pContext, L"../Bin/Resources/Anim/Enemy/EnemyWisp/EnemyWisp.model", PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_EnemyWisp"),
		CEnemyWisp::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* EnemyWisp Effects */
	/* For.Prototype_GameObject_EnemyWispTrail */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_EnemyWispTrail"),
		CE_EnemyWispTrail::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_RotTrail */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_RotTrail"),
		CE_RotTrail::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_EnemyWispBackground */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_EnemyWispBackground"),
		CE_EnemyWispBackground::Create(m_pDevice, m_pContext, L"../Bin/Data/Effect/E_EnemyWispBackGround.json"))))
		return E_FAIL;

	/* For.Prototype_GameObject_EnemyWispGround */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_EnemyWispGround"),
		CE_EnemyWispGround::Create(m_pDevice, m_pContext, L"../Bin/Data/Effect/E_EnemyWispGround.json"))))
		return E_FAIL;

	/* For.Prototype_GameObject_EnemyWispParticle */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_EnemyWispParticle"),
		CE_P_EnemyWisp::Create(m_pDevice, m_pContext, L"../Bin/Data/Effect/E_P_EnemyWispSpawn.json"))))
		return E_FAIL;
	/* ~EnemyWisp Effects */
#pragma  endregion	MONSTER

	lstrcpy(m_szLoadingText, TEXT("Loading Effects Object..."));
#pragma region Effect_Object
	/* For.Prototype_GameObject_KenaPulse */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_KenaPulse"),
		CE_KenaPulse::Create(m_pDevice, m_pContext, L"../Bin/Data/Effect/E_KenaPulse.json"))))
		return E_FAIL;

	/* For.Prototype_GameObject_KenaPulseCloud */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_KenaPulseCloud"),
		CE_KenaPulseCloud::Create(m_pDevice, m_pContext, L"../Bin/Data/Effect/E_KenaPulseCloude.json"))))
		return E_FAIL;

	/* For.Prototype_GameObject_KenaPulseDot */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_KenaPulseDot"),
		CE_KenaPulseDot::Create(m_pDevice, m_pContext, L"../Bin/Data/Effect/E_KenaPulseDot.json"))))
		return E_FAIL;

	/* For.Prototype_GameObject_KenaCharge */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_KenaCharge"),
		CE_KenaCharge::Create(m_pDevice, m_pContext, L"../Bin/Data/Effect/E_Charge_Set.json"))))
		return E_FAIL;

	/* For.Prototype_GameObject_KenaChargeImpact */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_KenaChargeImpact"),
		CE_KenaChargeImpact::Create(m_pDevice, m_pContext, L"../Bin/Data/Effect/E_Charge_Impact.json"))))
		return E_FAIL;

	/* For.Prototype_GameObject_KenaDamage */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_KenaDamage"),
		CE_KenaDamage::Create(m_pDevice, m_pContext, L"../Bin/Data/Effect/E_Damage_Set.json"))))
		return E_FAIL;

	/* For.Prototype_GameObject_KenaHit */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_KenaHit"),
		CE_KenaHit::Create(m_pDevice, m_pContext, L"../Bin/Data/Effect/Hit.json"))))
		return E_FAIL;
	// E_HitSet

	/* For.Prototype_GameObject_KenaHitParticle */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_KenaHit_P"),
		CE_P_KenaHit::Create(m_pDevice, m_pContext, L"../Bin/Data/Effect/E_P_HIT.json"))))
		return E_FAIL;

	/* For.Prototype_GameObject_KenaJump */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_KenaJump"),
		CE_KenaJump::Create(m_pDevice, m_pContext, L"../Bin/Data/Effect/E_JumpSet.json"))))
		return E_FAIL;

	/* For.Prototype_GameObject_PulseObject */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_PulseObject"),
		CE_PulseObject::Create(m_pDevice, m_pContext, L"../Bin/Data/Effect/E_PulseObject.json"))))
		return E_FAIL;

	/* For.Prototype_GameObject_KenaStaffTrail */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_KenaStaffTrail"),
		CE_KenaTrail::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_SpiritArrowTrail */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_SpiritArrowTrail"),
		CE_SpiritArrowTrail::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Wind */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Wind"),
		CE_Wind::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_EffectFlower */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_EffectFlower"),
		CE_P_Flower::Create(m_pDevice, m_pContext, L"../Bin/Data/Effect/E_P_Flower.json"))))
		return E_FAIL;

	/* For.Prototype_GameObject_KenaDust */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_KenaDust"),
		CE_KenaDust::Create(m_pDevice, m_pContext, L"../Bin/Data/Effect/E_KenaDust.json"))))
		return E_FAIL;

	/* For.Prototype_GameObject_KenaHeavyAttackInto */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_KenaHeavyAttackInto"),
		CE_KenaHeavyAttack_Into::Create(m_pDevice, m_pContext, L"../Bin/Data/Effect/E_HeavyAttack_0.json"))))
		return E_FAIL;

	/* For.Prototype_GameObject_KenaHeavyAttackInto_P */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_KenaHeavyAttackInto_P"),
		CE_P_KenaHeavyAttack_Into::Create(m_pDevice, m_pContext, L"../Bin/Data/Effect/E_P_HeavyAttack_0.json"))))
		return E_FAIL;

	/* For.Prototype_GameObject_SpiritArrowPosition */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_SpiritArrowPosition"),
		CE_SpiritArrowPosition::Create(m_pDevice, m_pContext, L"../Bin/Data/Effect/E_SpiritArrowPosition.json"))))
		return E_FAIL;

	/* For.Prototype_GameObject_SpiritArrowHit */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_SpiritArrowHit"),
		CE_SpiritArrowHit::Create(m_pDevice, m_pContext, L"../Bin/Data/Effect/E_SpritArrowHit.json"))))
		return E_FAIL;

	/* For.Prototype_GameObject_SpiritArrow_P */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_SpiritArrow_P"),
		CE_P_SpiritArrow::Create(m_pDevice, m_pContext, L"../Bin/Data/Effect/E_P_SpritArrowSpot.json"))))
		return E_FAIL;

	/* For.Prototype_GameObject_RotWisp */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_RotWisp"),
		CRotWisp::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_InteractStaff */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_InteractStaff"),
		CE_InteractStaff::Create(m_pDevice, m_pContext, L"../Bin/Data/Effect/E_IneractStaff.json"))))
		return E_FAIL;

	/* For.Prototype_GameObject_InteractStaff */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_InteractStaff_P"),
		CE_P_InteractStaff::Create(m_pDevice, m_pContext, L"../Bin/Data/Effect/E_P_InteractStaff.json"))))
		return E_FAIL;

	/* For.Prototype_GameObject_TeleportRot */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TeleportRot"),
		CE_TeleportRot::Create(m_pDevice, m_pContext, L"../Bin/Data/Effect/E_TeleportRot.json"))))
		return E_FAIL;

	/* For.Prototype_GameObject_Sapling */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ExplosionSapling"),
		CE_Sapling::Create(m_pDevice, m_pContext, L"../Bin/Data/Effect/E_Sapling.json"))))
		return E_FAIL;

	/* For.Prototype_GameObject_Sapling_P */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Sapling_P"),
		CE_P_Sapling::Create(m_pDevice, m_pContext, L"../Bin/Data/Effect/E_P_Sapling.json"))))
		return E_FAIL;

	/* For.Prototype_GameObject_FireBulletCover */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_FireBulletCover"),
		CE_FireBulletCover::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_FireBulletCloud */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_FireBulletCloud"),
		CE_FireBulletCloud::Create(m_pDevice, m_pContext, L"../Bin/Data/Effect/E_SaplingFire.json"))))
		return E_FAIL;

	/* For.Prototype_GameObject_FireBulletExplosion */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_FireBulletExplosion"),
		CE_FireBulletExplosion::Create(m_pDevice, m_pContext, L"../Bin/Data/Effect/E_FireBulletExplosion.json"))))
		return E_FAIL;

	/* For.Prototype_GameObject_FireBulletExplosion */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_KenaParry"),
		CE_KenaParry::Create(m_pDevice, m_pContext, L"../Bin/Data/Effect/PulseParry/E_PulseParrySet.json"))))
		return E_FAIL;

	/* For.Prototype_GameObject_FireBulletExplosion */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_KenaParryHand"),
		CE_KenaParryHand::Create(m_pDevice, m_pContext, L"../Bin/Data/Effect/PulseParry/E_PulseParryHand.json"))))
		return E_FAIL;

	/* For.Prototype_GameObject_RotBombExplosion */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_RotBombExplosion"),
		CE_RotBombExplosion::Create(m_pDevice, m_pContext, L"../Bin/Data/Effect/E_RotBombExplosion.json"))))
		return E_FAIL;

	/* For.Prototype_GameObject_Explosion_p */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Explosion_p"),
		CE_P_Explosion::Create(m_pDevice, m_pContext, L"../Bin/Data/Effect/E_P_Explosion.json"))))
		return E_FAIL;

#pragma endregion Effect_Object

	lstrcpy(m_szLoadingText, TEXT("Loading Effects Distortion..."));

#pragma region Distortion
	/* For.Prototype_GameObject_DistortionSphere */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_DistortionSphere"),
		CE_D_Sphere::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion Distortion

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CLoader::Loading_ForHW(_uint iLevelIndex)
{
	lstrcpy(m_szLoadingText, TEXT("Loading 현욱..."));

	CGameInstance *pGameInstance = CGameInstance::GetInstance();

	if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, TEXT("Prototype_Component_Texture_Terrain"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Terrain_Texture/ForestGround/Test_%d.png"), 7))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Brush*/
	if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, TEXT("Prototype_Component_Texture_Normal"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Terrain_Texture/Flter_Texture_%d.png"), 2))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Filter */
	if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, TEXT("Prototype_Component_Texture_Filter"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Terrain_Texture/Filter/Filter_%d.dds"), 3))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Filter */
	if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, TEXT("Prototype_Component_Terrain_Two_Filter"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Terrain_Texture/Filter/Terrain2_Filter_%d.dds"), 3))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Filter */
	if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, TEXT("Prototype_Component_Terrain_HeightMaps"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Terrain_Texture/Height/Terrain_Height_%d.bmp"), 15))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_GroundMark*/
	if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, TEXT("Prototype_Component_Texture_GroundMark"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/GroundMark/GroundMark%d.png"), 3))))
		return E_FAIL;
	
	if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, TEXT("Prototype_Component_VIBuffer_Terrain_0"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Terrain_Texture/Real_Height/Terrain_Height_0.bmp")))))
		assert(!"Issue");
	
	if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, TEXT("Prototype_Component_VIBuffer_Terrain_1"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Terrain_Texture/Real_Height/Terrain_Height_1.bmp")))))
		assert(!"Issue");
	
	if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, TEXT("Prototype_Component_VIBuffer_Terrain_2"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Terrain_Texture/Real_Height/Terrain_Height_2.bmp")))))
		assert(!"Issue");
	
	if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, TEXT("Prototype_Component_VIBuffer_Terrain_3"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Terrain_Texture/Real_Height/Terrain_Height_3.bmp")))))
		assert(!"Issue");
	

	_matrix			PivotMatrix = XMMatrixIdentity();
	/* For.Prototype_Component_Model_DeadZoneTree */
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, L"Prototype_Component_Model_DeadZoneTree",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/DeadZoneTree_Anim/DeadzoneTree.mdat"), PivotMatrix))))
		return E_FAIL;
	
	/* For.Prototype_Component_Model_PulsePlateAnim*/
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, L"Prototype_Component_Model_PulsePlateAnim",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/PulsePlate_Anim/PulsePlate_Anim.model"), PivotMatrix))))
		return E_FAIL;
	

	/* For.Prototype_Component_Model_PulsePlateAnim*/
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, L"Prototype_Component_Model_TeleportFlowerAnim",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/TeleportFlower/TeleportFlower.mdat"), PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_FieldBeaconAnim*/
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, L"Prototype_Component_Model_FieldBeaconAnim",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/FieldBeacon_Anim/FieldBeacon.mdat"), PivotMatrix))))
		return E_FAIL;


	if (FAILED(LoadNonAnimFolderModel(iLevelIndex, "PowerCrystal", true, false, true)))
		assert(!"Issue");
	
	if (FAILED(LoadNonAnimFolderModel(iLevelIndex, "PulseObjects/PluseStone_Big", true, false, true)))
		return E_FAIL;
	
	if (FAILED(LoadNonAnimFolderModel(iLevelIndex, "PulseObjects/PulsePlate_StoneBorder", true, false, true)))
		return E_FAIL;
	
	if (FAILED(LoadNonAnimFolderModel(iLevelIndex, "PulseObjects/PulsePlateEnclosure_02", true, false, true)))
		return E_FAIL;
	
	if (FAILED(LoadNonAnimFolderModel(iLevelIndex, "PulseObjects/PulseStoneRock", true, false, true)))
		return E_FAIL;
	
	if (FAILED(LoadNonAnimFolderModel(iLevelIndex, "RuinKit/RuinKit_LedgeStone", true, true, true)))
		return E_FAIL;
	
	if (FAILED(LoadNonAnimFolderModel(iLevelIndex, "RuinKit/RuinKit_Pillar", true, true, true)))
		return E_FAIL;
	
	if (FAILED(LoadNonAnimFolderModel(iLevelIndex, "RuinKit/RuinsKit_Brick", true, true, true)))
		return E_FAIL;
	
	if (FAILED(LoadNonAnimFolderModel(iLevelIndex, "RuinKit/RuinsKit_BridgeLarge", true, true, true)))
		return E_FAIL;
	
	if (FAILED(LoadNonAnimFolderModel(iLevelIndex, "RuinKit/RuinsKit_ToriGate", true, true, true)))
		return E_FAIL;
	
	if (FAILED(LoadNonAnimFolderModel(iLevelIndex, "Rock/Rock_Arch", true, true, true)))
		assert(!"Issue");
	
	if (FAILED(LoadNonAnimFolderModel(iLevelIndex, "RuinPlatform", true, true, true,false,true)))
		assert(!"Issue");
	
	if (FAILED(LoadNonAnimFolderModel(iLevelIndex, "Rock/Rock_Rubble", true, true, true)))
		return E_FAIL;
	
	if (FAILED(LoadNonAnimFolderModel(iLevelIndex, "Rock/Rock_Big", true, true, true)))
		assert(!"Issue");
	
	if (FAILED(LoadNonAnimFolderModel(iLevelIndex, "Cliff/Cliff_Rock", true, true, true)))
		return E_FAIL;
	
	if (FAILED(LoadNonAnimFolderModel(iLevelIndex, "Rock/God_Rock", true, true, true, false, true)))
		return E_FAIL;
	
	if (FAILED(LoadNonAnimFolderModel(iLevelIndex, "Cliff/Cliff_Cap", true, true, true)))
		assert(!"Issue");
	
	if (FAILED(LoadNonAnimFolderModel(iLevelIndex, "Cliff/Cliff_W2", true, true, true)))
		return E_FAIL;
	
	if (FAILED(LoadNonAnimFolderModel(iLevelIndex, "Cliff/Cliff_W3", true, true, true)))
		assert(!"Issue");
	
	if (FAILED(LoadNonAnimFolderModel(iLevelIndex, "Cliff/Cliff_Wall_Large", true, true, true)))
		return E_FAIL;
	
	if (FAILED(LoadNonAnimFolderModel(iLevelIndex, "Cliff/Cliff_Wall_Med", true, true, true)))
		return E_FAIL;
	
	if (FAILED(LoadNonAnimFolderModel(iLevelIndex, "Cliff/Cliff_Wall_Short", true, true, true)))
		return E_FAIL;
	if (FAILED(LoadNonAnimFolderModel(iLevelIndex, "Cliff/Cliff_Glitter", true, true, true)))
		assert(!"Issue");
	
	if (FAILED(LoadNonAnimFolderModel(iLevelIndex, "Cliff/Cliff_Sheer", true, true, true)))
		assert(!"Issue");
	
	if (FAILED(LoadNonAnimFolderModel(iLevelIndex, "RuinKit/RuinKit_Rubble", true, true, true)))
		return E_FAIL;
	
	if (FAILED(LoadNonAnimFolderModel(iLevelIndex, "RuinKit/RuinStaris", true, true, true)))
		return E_FAIL;
	
	if (FAILED(LoadNonAnimFolderModel(iLevelIndex, "Statue/Owl_WrapShrine", false, true, true)))
		return E_FAIL;
	
	if (FAILED(LoadNonAnimFolderModel(iLevelIndex, "Statue", true, true, true)))
		return E_FAIL;
	
	if (FAILED(LoadNonAnimFolderModel(iLevelIndex, "RuinKit/RuinKit_Arch_Gate", true, true, true)))
		return E_FAIL;
	
	if (FAILED(LoadNonAnimFolderModel(iLevelIndex, "RuinKit/RuinsKit_Tree", true, true, true)))
		return E_FAIL;
	
	if (FAILED(LoadNonAnimFolderModel(iLevelIndex, "RuinKit/RuinsKit_FloorTile", true, true, true, false,true)))
		return E_FAIL;
	
	if (FAILED(LoadNonAnimFolderModel(iLevelIndex, "RuinKit/RuinKit_Piece", true, true, true)))
		return E_FAIL;
	
	if (FAILED(LoadNonAnimFolderModel(iLevelIndex, "RuinKit/RuinsKit_Wall_Broken", true, true, true)))
		return E_FAIL;
	
	if (FAILED(LoadNonAnimFolderModel(iLevelIndex, "RuinKit/RuinsWall", true, true, true)))
		assert(!"Issue");
	
	if (FAILED(LoadNonAnimFolderModel(iLevelIndex, "RuinKit/Ruinskit_Collum", true, true, true)))
		assert(!"Issue");
	
	if (FAILED(LoadNonAnimFolderModel(iLevelIndex, "Cliff/Cliff_Ledge", true, true, true, false, true)))
		return E_FAIL;
	
	if (FAILED(LoadNonAnimFolderModel(iLevelIndex, "Rock/Rock_Small", true, true, true)))
		return E_FAIL;
	
	if (FAILED(LoadNonAnimFolderModel(iLevelIndex, "Rock/Rock_Medium", true, true, true)))
		return E_FAIL;
	
	if (FAILED(LoadNonAnimFolderModel(iLevelIndex, "Forest_1/RotGod_Statue_crumbled", true, false, true)))
		return E_FAIL;
	
	if (FAILED(LoadNonAnimFolderModel(iLevelIndex, "Forest_1/Start_Gate", true, true, true)))
		return E_FAIL;
	
	if (FAILED(LoadNonAnimFolderModel(iLevelIndex, "Forest_1/FirstTear_FallenTree", true, false, true)))
		return E_FAIL;
	
	if (FAILED(LoadNonAnimFolderModel(iLevelIndex, "Forest_1/First_Room_Deco", true, true, true)))
		return E_FAIL;
	
	if (FAILED(LoadNonAnimFolderModel(iLevelIndex, "Forest_1/ShrineOfDeath_MainRock", true, true, true)))
		return E_FAIL;
	
	if (FAILED(LoadNonAnimFolderModel(iLevelIndex, "Forest_1/ShrineOfLife", true, true, true)))
		return E_FAIL;
	
	if (FAILED(LoadNonAnimFolderModel(iLevelIndex, "Stone/StoneFloor_1", true, true, true)))
		return E_FAIL;
	
	if (FAILED(LoadNonAnimFolderModel(iLevelIndex, "Stone/StoneFloor_2", true, true, true)))
		return E_FAIL;
	
	if (FAILED(LoadNonAnimFolderModel(iLevelIndex, "Stone/Stone_Bridge", true, true, true)))
		assert(!"Issue");
	
	if (FAILED(LoadNonAnimFolderModel(iLevelIndex, "Stone/Stone_Stairs", true, true, true)))
		assert(!"Issue");
	
	if (FAILED(LoadNonAnimFolderModel(iLevelIndex, "Trees/Giant", true, true, true)))
		return E_FAIL;
	
	if (FAILED(LoadNonAnimFolderModel(iLevelIndex, "Trees/Fallen", true, true, true)))
		return E_FAIL;
	
	if (FAILED(LoadNonAnimFolderModel(iLevelIndex, "Trees/Dead", true, true, true)))
		return E_FAIL;
	
	if (FAILED(LoadNonAnimFolderModel(iLevelIndex, "Trees/CeDarTree", true, true, true)))
		return E_FAIL;
	
	if (FAILED(LoadNonAnimFolderModel(iLevelIndex, "Trees/Bare", true, true, true)))
		return E_FAIL;
	
	if (FAILED(LoadNonAnimFolderModel(iLevelIndex, "Trees/Billboard", true, true, true)))
		return E_FAIL;
	
	if (FAILED(LoadNonAnimFolderModel(iLevelIndex, "Trees/Branches/Canopy", true, true, true)))
		return E_FAIL;
	
	if (FAILED(LoadNonAnimFolderModel(iLevelIndex, "Trees/Branches/GodTree", true, true, true)))
		return E_FAIL;
	
	if (FAILED(LoadNonAnimFolderModel(iLevelIndex, "Trees/Branches/Tree_Branch", true, true, true)))
		return E_FAIL;
	
	if (FAILED(LoadNonAnimFolderModel(iLevelIndex, "Trees/Canopy", true, true, true)))
		return E_FAIL;
	
	if (FAILED(LoadNonAnimFolderModel(iLevelIndex, "Save_StoneTree", true, true,true)))
		assert(!"Issue");
	
	if (FAILED(LoadNonAnimFolderModel(iLevelIndex, "DeadZone/Deadzondes_GoopWeb", true, true, true)))
		assert(!"Issue");
	
	if (FAILED(LoadNonAnimFolderModel(iLevelIndex, "DeadZone/DeadZoneAndPurifiedTree", true, true, true)))
		assert(!"Issue");
	
	if (FAILED(LoadNonAnimFolderModel(iLevelIndex, "DeadZone/DeadzoneClumps", true, true, true)))
		assert(!"Issue");
	
	if (FAILED(LoadNonAnimFolderModel(iLevelIndex, "DeadZone/DeadzoneGroudCover", true, true, true)))
		assert(!"Issue");
	
	if (FAILED(LoadNonAnimFolderModel(iLevelIndex, "DeadZone/DeadzoneHeart", true, true, true)))
		assert(!"Issue");
	
	if (FAILED(LoadNonAnimFolderModel(iLevelIndex, "DeadZone/DeadzoneWall", true, true, true)))
		assert(!"Issue");
	
	if (FAILED(LoadNonAnimFolderModel(iLevelIndex, "DeadZone/Gate", true, true, true)))
		assert(!"Issue");
	
	if (FAILED(LoadNonAnimFolderModel(iLevelIndex, "DeadZone/Trees", true, true, true)))
		assert(!"Issue");
	
	if (FAILED(LoadNonAnimFolderModel(iLevelIndex, "DeadZone/BushDead_02", true, true)))
		assert(!"Issue");
	
	if (FAILED(LoadNonAnimFolderModel(iLevelIndex, "Rot/Rot_Rock", true, true,true)))
		assert(!"Issue");
	
	if (FAILED(LoadNonAnimFolderModel(iLevelIndex, "Rot/RotCarry_Piece", true, true,true)))
		assert(!"Issue");
	

#pragma region GroundCover
		if (FAILED(LoadNonAnimFolderModel(iLevelIndex, "GroundCover/Branches", true, true, true, true)))
			assert(!"Issue");
		if (FAILED(LoadNonAnimFolderModel(iLevelIndex, "GroundCover/Clovers", true, true, true, true)))
			assert(!"Issue");
		if (FAILED(LoadNonAnimFolderModel(iLevelIndex, "GroundCover/Deadzone", true, true, true, true)))
			assert(!"Issue");
		if (FAILED(LoadNonAnimFolderModel(iLevelIndex, "GroundCover/Grass", true, true, true, true)))
			assert(!"Issue");
		if (FAILED(LoadNonAnimFolderModel(iLevelIndex, "GroundCover/Needles", true, true, true, true)))
			assert(!"Issue");
		if (FAILED(LoadNonAnimFolderModel(iLevelIndex, "GroundCover/Roots", true, true, true, true)))
			assert(!"Issue");
#pragma endregion GroundCover

#pragma region Foliage
		if (FAILED(LoadNonAnimFolderModel(iLevelIndex, "Foliage/BushDead", true, true, true, true)))
			assert(!"Issue");
		if (FAILED(LoadNonAnimFolderModel(iLevelIndex, "Foliage/Bushes", true, true, true, true)))
			assert(!"Issue");
		if (FAILED(LoadNonAnimFolderModel(iLevelIndex, "Foliage/Ferns", true, true, true, true)))
			assert(!"Issue");
		if (FAILED(LoadNonAnimFolderModel(iLevelIndex, "Foliage/Flowers", true, true, true, true)))
			assert(!"Issue");
		if (FAILED(LoadNonAnimFolderModel(iLevelIndex, "Foliage/Grass", true, true, true, true)))
			assert(!"Issue");
		if (FAILED(LoadNonAnimFolderModel(iLevelIndex, "Foliage/LilyPad", true, true, true, true)))
			assert(!"Issue");
		if (FAILED(LoadNonAnimFolderModel(iLevelIndex, "Foliage/Moss", true, true, true, true)))
			assert(!"Issue");
		if (FAILED(LoadNonAnimFolderModel(iLevelIndex, "Foliage/Overhang", true, true, true, true)))
			assert(!"Issue");
		if (FAILED(LoadNonAnimFolderModel(iLevelIndex, "Foliage/Plants", true, true, true, true)))
			assert(!"Issue");
		if (FAILED(LoadNonAnimFolderModel(iLevelIndex, "Foliage/Roots", true, true, true, true)))
			assert(!"Issue");
#pragma endregion Foliage

#pragma region Born_GroundCover
		PivotMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f);
		if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, L"Prototype_Component_Model_SK_BasilLarge",
			CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/SK_Basil/SK_BasilLarge.mdat"), PivotMatrix, nullptr, false, true))))
			return E_FAIL;
		if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, L"Prototype_Component_Model_SK_BasilMedium",
			CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/SK_Basil/SK_BasilMedium.mdat"), PivotMatrix, nullptr, false, true))))
			return E_FAIL;
		if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, L"Prototype_Component_Model_SK_BasilSmall",
			CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/SK_Basil/SK_BasilSmall.mdat"), PivotMatrix, nullptr, false, true))))
			return E_FAIL;
		if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, L"Prototype_Component_Model_SK_Bush_02",
			CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/SK_Bush/SK_Bush_02.mdat"), PivotMatrix, nullptr, false, true))))
			return E_FAIL;
		if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, L"Prototype_Component_Model_SK_Fern_01",
			CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/SK_Fern/SK_Fern_01.mdat"), PivotMatrix, nullptr, false, true))))
			return E_FAIL;
		if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, L"Prototype_Component_Model_SK_Fern_02",
			CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/SK_Fern/SK_Fern_02.mdat"), PivotMatrix, nullptr, false, true))))
			return E_FAIL;
		if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, L"Prototype_Component_Model_SK_Fern_03",
			CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/SK_Fern/SK_Fern_03.mdat"), PivotMatrix, nullptr, false, true))))
			return E_FAIL;
		if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, L"Prototype_Component_Model_SK_Fern_04",
			CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/SK_Fern/SK_Fern_04.mdat"), PivotMatrix, nullptr, false, true))))
			return E_FAIL;
		if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, L"Prototype_Component_Model_SK_Flower_02",
			CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/SK_Flower/SK_Flower_02.mdat"), PivotMatrix, nullptr, false, true))))
			return E_FAIL;
		if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, L"Prototype_Component_Model_SK_Flower_03",
			CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/SK_Flower/SK_Flower_03.mdat"), PivotMatrix, nullptr, false, true))))
			return E_FAIL;
		if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, L"Prototype_Component_Model_SK_Flower_05",
			CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/SK_Flower/SK_Flower_05.mdat"), PivotMatrix, nullptr, false, true))))
			return E_FAIL;
		if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, L"Prototype_Component_Model_SK_Mushroom_01",
			CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/SK_Mushroom/SK_Mushroom_01.mdat"), PivotMatrix, nullptr, false, true))))
			return E_FAIL;
		if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, L"Prototype_Component_Model_SK_Mushroom_02",
			CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/SK_Mushroom/SK_Mushroom_02.mdat"), PivotMatrix, nullptr, false, true))))
			return E_FAIL;
		if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, L"Prototype_Component_Model_SK_Pampas",
			CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/SK_Pampas/SK_Pampas.mdat"), PivotMatrix, nullptr, false, true))))
			return E_FAIL;
		if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, L"Prototype_Component_Model_SK_Plant_05",
			CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/SK_Plant/SK_Plant_05.mdat"), PivotMatrix, nullptr, false, true))))
			return E_FAIL;
		if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, L"Prototype_Component_Model_SK_Plants3",
			CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/SK_Plant/SK_Plants3.mdat"), PivotMatrix, nullptr, false, true, false))))
			return E_FAIL;
#pragma  endregion Born_GroundCover

#pragma  region HHW_OBJ
	/* For.Prototype_GameObject_Cave_Rock */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Cliff_Rock"),
		CCliff_Rock::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	/* For.Prototype_GameObject_Terrain */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Terrain"),
		CTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	

	/* For.Prototype_GameObject_Tree */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Tree"),
		CTree::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	

	/* For.Prototype_GameObject_Crystal */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Crystal"),
		CCrystal::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	

	/* For.Prototype_GameObject_BowTarget */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BowTarget"),
		CBowTarget::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	

	/* For.Prototype_GameObject_PulseStone */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_PulseStone"),
		CPulseStone::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	

	/* For.Prototype_GameObject_BaseGround */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BaseGround"),
		CBase_Ground::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	

	/* For.Prototype_GameObject_Wall */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Wall"),
		CWall::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	

	/* For.Prototype_GameObject_Wall */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Root"),
		CRoots::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	

	/* For.Prototype_GameObject_Stair */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Stair"),
		CStair::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	

	/* For.Prototype_GameObject_Wall */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Pillar"),
		CPillar::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	

	/* For.Prototype_GameObject_Statue */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Statue"),
		CStatue::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	

	/* For.Prototype_GameObject_Border */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Border"),
		CBorder::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	

	/* For.Prototype_GameObject_Slope */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Slope"),
		CSlope::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	

	/* For.Prototype_GameObject_Bridge */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Bridge"),
		CBridge::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	

	/* For.Prototype_GameObject_Path_Mesh */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Path_Mesh"),
		CPath_Mesh::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	

	/* For.Prototype_GameObject_FloorTile */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_FloorTile"),
		CFloorTile::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	

	/* For.Prototype_GameObject_Pulse_PlateForm */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Pulse_PlateForm"),
		CPulse_PlateForm::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	

	/* For.Prototype_GameObject_Gate */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Gate"),
		CGate::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	

	/* For.Prototype_GameObject_RuinKit */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_RuinKit"),
		CRuinKit::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	/* For.Prototype_GameObject_Stone */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Stone"),
		CStone::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	

	/* For.Prototype_GameObject_Flower */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Flower"),
		CFlower::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	

	/* For.Prototype_GameObject_Grass */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Grass"),
		CGrass::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	

	/* For.Prototype_GameObject_DeadZoneObj */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_DeadZoneObj"),
		CDeadZoneObj::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	

	/* For.Prototype_GameObject_DeadZoneObj */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_GimmickObj"),
		CGimmick_EnviObj::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	/*Anim*/
	/* For.Prototype_GameObject_Door_Anim */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_DoorAnim"),
		CDoor_Anim::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	/* For.Prototype_GameObject_TeleportFlower */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TeleportFlower"),
		CTelePort_Flower::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_FieldBecon_Anim */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_FieldBecon_Anim"),
		CFieldBecon_Anim::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	/* For.Prototype_GameObject_Door_Anim */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_PulsePlateAnim"),
		CPulse_Plate_Anim::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	

	/* For.Prototype_GameObject_GroundMark */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_GroundMark"),
		CGroundMark::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	

	/* For.Prototype_GameObject_ControlRoom */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ControlRoom"),
		CControlRoom::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Born_GroundCover"),
		CBorn_GroundCover::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
#pragma endregion HHW_OBJ

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

CLoader * CLoader::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, LEVEL eNextLevelID)
{
	CLoader*		pInstance = new CLoader(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevelID)))
	{
		MSG_BOX("Failed to Created : CLoader");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLoader::Free()
{
	WaitForSingleObject(m_hThread, INFINITE);
	CloseHandle(m_hThread);
	DeleteObject(m_hThread);
	DeleteCriticalSection(&m_Critical_Section);

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}