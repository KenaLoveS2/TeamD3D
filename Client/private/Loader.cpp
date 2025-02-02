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
#include "Taro_Mask.h"
#include "SpiritArrow.h"
#include "RotBomb.h"

/* NPCs */
#include "Rot.h"
#include "Beni.h"
#include "Saiya.h"
#include "CameraForNpc.h"
#include "RotWisp.h"
#include "RotHat.h"

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
#include "ShamanTrapHex.h"
#include "ShamanTrapGeo.h"
#include "ShamanTrapPlane.h"
#include "HunterArrow.h"

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
#include "FloorMesh.h"
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
#include "DZ_FallenTree_Anim.h"
#include "Chest_Anim.h"
#include "HatCart.h"
#include "E_P_Sakura.h"
#include "Frog.h"
#include "Pet.h"
#include "HealthFlower_Anim.h"
#include "Dynamic_Stone.h"
#include "Dynamic_StoneCube.h"
#include "BombPlatform.h"
#include "SceneChangePortal.h"
#include "MannequinRot.h"
#include "Meditation_Spot.h"
#include "WorldTrigger.h"
#include "WorldTrigger_S2.h"

#include "BossRock.h"
#include "BossRock_Pool.h"
#include "DeadZoneBossTree.h"
#include "Fire_Brazier.h"
#include "BossShaman_Mask.h"
#include "Camera_Shaman.h"
#include "Camera_Photo.h"

/* UI */
#include "BackGround.h"
#include "Effect_Particle_Base.h"
#include "Effect_Mesh_Base.h"
#include "Effect_Texture_Base.h"

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
#include "E_RectTrail.h"
#include "E_Swipes_Charged.h"
#include "E_WarriorTrail.h"
#include "E_Warrior_FireSwipe.h"
#include "E_Distortion_Plane.h"
#include "E_EnrageInto.h"
#include "E_Warrior_PlaneRoot.h"
#include "E_Warrior_Root.h"
#include "E_Warriorcloud.h"
#include "E_P_Warrior.h"
#include "E_Warrior_ShockFronExtended.h"
#include "E_Hieroglyph.h"
#include "E_P_ShockFrontEntended.h"
#include "E_Warrior_ShockFronExtended_Plane.h"
#include "E_ShamanBossPlate.h"
#include "E_ShamanBossHandPlane.h"
#include "E_ShamanThunderCylinder.h"
#include "E_ShamanSummons.h"
#include "E_ShamanTrail.h"
#include "E_ShamanSmoke.h"
#include "E_P_ExplosionGravity.h"
#include "E_KenaDash.h"
#include "E_KenaDashRing.h"
#include "E_KenaDashCone.h"
#include "E_HunterTrail.h"
#include "E_ShamanIceDagger.h"
#include "E_ShamanLazer.h"
#include "E_P_ShamanTeleport.h"
#include "E_Common_CIrcleSp.h"
#include "E_LazerTrail.h"
#include "E_P_Level.h"
#include "E_P_Rot.h"
#include "E_Chest.h"
#include "E_P_Chest.h"
#include "E_P_Bombplatform.h"
#include "E_P_CommonBox.h"
#include "E_P_Maple.h"
#include "E_P_Beech.h"
#include "E_P_Meditation_Spot.h"
#include "E_FireBrazier.h"
#include "E_P_EnvironmentDust.h"
#include "E_P_Rain.h"
#include "E_P_DeadZoneDust.h"
#include "E_ShamanHeadTrail.h"
#include "E_ShamanWeaponBall.h"
#include "E_ShamanElectric.h"
#include "E_WarriorWeaponAcc.h"
#include "E_P_WarriorBody.h"
#include "E_P_DeadZone_SmallPlace.h"
/* ~Effects */

/* Components*/
#include "ControlMove.h"
#include "Interaction_Com.h"
#include "Camera_Player.h"
#include "CameraForRot.h"
#include "CinematicCamera.h"
#include "CPortalPlane.h"

/*Test Objects*/
#include "LodObject.h"
#include "Moth.h"
#include "RotForMonster.h"

unsigned int	g_LEVEL = 0;


#include "Json/json.hpp"
#include <fstream>
#include "BowTarget_Trigger.h"
#include "Respawn_Trigger.h"
#include "E_WarriorEyeTrail.h"
#include "Village_Trigger.h"
#include "E_P_ShamanDust.h"
#include "E_P_Level_Floor.h"
#include "E_P_Level_RiseY.h"

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
	case LEVEL_GIMMICK:
		pLoader->Loading_ForMiniGame();
		break;
	case LEVEL_FINAL:
		pLoader->Loading_ForFinal();
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

	FAILED_CHECK_RETURN(Loading_ForJH((_uint)LEVEL_GAMEPLAY), E_FAIL);

	// hyunwook
	 FAILED_CHECK_RETURN(Loading_ForHW((_uint)LEVEL_GAMEPLAY), E_FAIL);

	// hyaewon
	FAILED_CHECK_RETURN(Loading_ForHO((_uint)LEVEL_GAMEPLAY), E_FAIL);

	/* Line Changed : Effect Texture component needed */
	FAILED_CHECK_RETURN(Loading_ForSY((_uint)LEVEL_GAMEPLAY), E_FAIL);

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

	/* For.Prototype_Component_Texture_Filter3 */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Terrain_Three_Filter"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Terrain_Texture/Filter/Terrain3_Filter_%d.dds"), 3))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Filter3 */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Terrain_Four_Filter"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Terrain_Texture/Filter/Terrain4_Filter_%d.dds"), 3))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Filter3 */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Terrain_Five_Filter"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Terrain_Texture/Filter/Terrain5_Filter_%d.dds"), 3))))
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

	/* For.Prototype_Component_Texture_Filter*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Texture_TerrainDiffuse"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Terrain_Texture/Filter/T_GDC_Grass01_D_NoisyAlpha.png")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Filter*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Texture_TerrainNormal"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Terrain_Texture/Filter/T_Ground_Grass_N.png")))))
		return E_FAIL;


	lstrcpy(m_szLoadingText, TEXT("Loading Model..."));
#pragma region ANIM_OBJ
	_matrix			PivotMatrix = XMMatrixIdentity();
	/* For.Prototype_Component_Model_DeadZoneTree */
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAPTOOL, L"Prototype_Component_Model_DeadZoneTree",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/DeadZoneTree_Anim/DeadzoneTree.model"), PivotMatrix))))
		return E_FAIL;
	/* For.Prototype_Component_Model_PulsePlateAnim*/
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAPTOOL, L"Prototype_Component_Model_PulsePlateAnim",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/PulsePlate_Anim/PulsePlate_Anim.model"), PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_FieldBeaconAnim*/
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAPTOOL, L"Prototype_Component_Model_FieldBeaconAnim",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/FieldBeacon_Anim/FieldBeacon.mdat"), PivotMatrix))))
		return E_FAIL;

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAPTOOL, L"Prototype_Component_Model_DoorCrystal_Anim",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/Door/DoorCrystal/DoorCrystal_Anim_0.mdat"), PivotMatrix))))
		return E_FAIL;

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAPTOOL, L"Prototype_Component_Model_StoneDoor_Anim",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/Door/StoneDoor/StoneDoor_Anim_0.mdat"), PivotMatrix))))
		return E_FAIL;

	PivotMatrix = XMMatrixScaling(0.03f, 0.03f, 0.03f);
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAPTOOL, L"Prototype_Component_Model_Pet",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/Pet/Pet.model"), PivotMatrix))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Pet"), CPet::Create(m_pDevice, m_pContext)))) return E_FAIL;


	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAPTOOL, L"Prototype_Component_Model_Frog",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/Frog/Frog.model"), PivotMatrix))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Frog"), CFrog::Create(m_pDevice, m_pContext)))) return E_FAIL;

	PivotMatrix = XMMatrixScaling(0.004f, 0.004f, 0.004f);
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAPTOOL, L"Prototype_Component_Model_Dy_RockSmall03",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/NonAnim/Rock/Rock_Small/Rock_Small_03.mdat"),
			PivotMatrix, nullptr, false, false, "../Bin/Resources/NonAnim/Rock/Rock_Small/Rock_Small_03.json"))))
		return E_FAIL;

	PivotMatrix = XMMatrixScaling(0.004f, 0.004f, 0.004f);
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAPTOOL, L"Prototype_Component_Model_Dy_RockSmall04",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/NonAnim/Rock/Rock_Small/Rock_Small_04.mdat"), 
			PivotMatrix, nullptr, false, false, "../Bin/Resources/NonAnim/Rock/Rock_Small/Rock_Small_04.json"))))
		return E_FAIL;

	PivotMatrix = XMMatrixScaling(0.004f, 0.004f, 0.004f);
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAPTOOL, L"Prototype_Component_Model_Dy_RockSmall05",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/NonAnim/Rock/Rock_Small/Rock_Small_05.mdat"),
			PivotMatrix, nullptr, false, false, "../Bin/Resources/NonAnim/Rock/Rock_Small/Rock_Small_05.json"))))
		return E_FAIL;

	PivotMatrix = XMMatrixScaling(0.004f, 0.004f, 0.004f);
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAPTOOL, L"Prototype_Component_Model_Dy_RockSmall06",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/NonAnim/Rock/Rock_Small/Rock_Small_06.mdat"),
			PivotMatrix, nullptr, false, false, "../Bin/Resources/NonAnim/Rock/Rock_Small/Rock_Small_06.json"))))
		return E_FAIL;


	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAPTOOL, L"Prototype_Component_Model_Dy_MaskShrine_Rock_09",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/NonAnim/Map4/HouseKit/Rock/M4_MaskShrine_Rock_09.mdat"),
			PivotMatrix, nullptr, false, false, "../Bin/Resources/NonAnim/Map4/HouseKit/Rock/M4_MaskShrine_Rock_09.json"))))
		return E_FAIL;


	PivotMatrix = XMMatrixScaling(0.002f, 0.002f, 0.002f);
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAPTOOL, L"Prototype_Component_Model_Dy_RockMedium04",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/NonAnim/Rock/Rock_Medium/Rock_Medium_04.mdat"),
			PivotMatrix, nullptr, false, false, "../Bin/Resources/NonAnim/Rock/Rock_Medium/Rock_Medium_04.json"))))
		return E_FAIL;


	PivotMatrix = XMMatrixScaling(0.002f, 0.002f, 0.002f);
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAPTOOL, L"Prototype_Component_Model_Dy_RockMedium06",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/NonAnim/Rock/Rock_Medium/Rock_Medium_06.mdat"),
			PivotMatrix, nullptr, false, false, "../Bin/Resources/NonAnim/Rock/Rock_Medium/Rock_Medium_06.json"))))
		return E_FAIL;

	PivotMatrix = XMMatrixScaling(0.002f, 0.002f, 0.002f);
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAPTOOL, L"Prototype_Component_Model_Dy_RockMedium07",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/NonAnim/Rock/Rock_Medium/Rock_Medium_07.mdat"),
			PivotMatrix, nullptr, false, false, "../Bin/Resources/NonAnim/Rock/Rock_Medium/Rock_Medium_07.json"))))
		return E_FAIL;



#pragma region Boss_Attack DissolveTreeAndStone

	PivotMatrix = XMMatrixScaling(0.002f, 0.003f, 0.002f);
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAPTOOL, L"Prototype_Component_Model_BossDissolveGodTree",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/NonAnim/Trees/Giant/Giant_GodTree.mdat"),
			PivotMatrix, nullptr, false, false, "../Bin/Resources/NonAnim/Trees/Giant/Giant_GodTree.json", false, false))))
		return E_FAIL;

	PivotMatrix = XMMatrixScaling(0.002f, 0.003f, 0.002f);
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAPTOOL, L"Prototype_Component_Model_BossDissolveGodTree02",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/NonAnim/Trees/Giant/Giant_GodTree02.mdat"),
			PivotMatrix, nullptr, false, false, "../Bin/Resources/NonAnim/Trees/Giant/Giant_GodTree02.json", false, false))))
		return E_FAIL;

	PivotMatrix = XMMatrixScaling(0.002f, 0.003f, 0.002f);
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAPTOOL, L"Prototype_Component_Model_BossDissolveGodTree03",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/NonAnim/Trees/Giant/Giant_GodTree.mdat"),
			PivotMatrix, nullptr, false, false, "../Bin/Resources/NonAnim/Trees/Giant/Giant_GodTree03.json", false, false))))
		return E_FAIL;

	// GodRock
	PivotMatrix = XMMatrixScaling(0.002f, 0.002f, 0.002f);
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAPTOOL, L"Prototype_Component_Model_BossDissolveGod_Rock01",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/NonAnim/Rock/God_Rock/GodRock_01.mdat"),
			PivotMatrix, nullptr, false, false, "../Bin/Resources/NonAnim/Rock/God_Rock/GodRock_01.json", false, false))))
		return E_FAIL;

	PivotMatrix = XMMatrixScaling(0.002f, 0.002f, 0.002f);
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAPTOOL, L"Prototype_Component_Model_BossDissolveGod_Rock02",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/NonAnim/Rock/God_Rock/GodRock_02.mdat"),
			PivotMatrix, nullptr, false, false, "../Bin/Resources/NonAnim/Rock/God_Rock/GodRock_02.json", false, false))))
		return E_FAIL;


#pragma  endregion


#pragma  endregion ANIM_OBJ
	
	if (FAILED(Loading_ForWJ((_uint)LEVEL_MAPTOOL)))
		return E_FAIL;

	_bool bRealObject = true;
	_bool bFlowerCheck = false;

#ifdef FOR_MAPTOOL   

#else
	if(bFlowerCheck == true)
	{
		PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
		// Prototype_Component_Model_Rope_Rock
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Rope_RotRock", true, false, true))) return E_FAIL;

	}

#pragma region Test_Gimmick_OBJ

#pragma endregion

#endif FOR_MAPTOOL

#ifdef FOR_MAP_GIMMICK
	
	//if (FAILED(Loading_ForJH((_uint)LEVEL_MAPTOOL)))
	//	return E_FAIL;

	//if (FAILED(Loading_ForHO((_uint)LEVEL_MAPTOOL)))
	//	return E_FAIL;

	//// Prototype_GameObject_RotForMonster
	//if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_RotForMonster"), CRotForMonster::Create(m_pDevice, m_pContext)))) return E_FAIL;

	//// Prototype_Component_Model_Rot
	//PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAPTOOL, L"Prototype_Component_Model_Rot",
	//	CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/Rot/Rot.model"), PivotMatrix)))) return E_FAIL;

	/*if (FAILED(Loading_ForBJ((_uint)LEVEL_MAPTOOL)))
		return E_FAIL;*/

	/*if (FAILED(Loading_ForSY((_uint)LEVEL_MAPTOOL)))
		return E_FAIL;*/

#else

	/* Prototype_Component_Model_BowTarget */
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	FAILED_CHECK_RETURN(CGameInstance::GetInstance()->Add_Prototype(LEVEL_MAPTOOL, L"Prototype_Component_Model_BowTarget", CModel::Create(m_pDevice, m_pContext, L"../Bin/Resources/Anim/BowTarget/BowTarget.model", PivotMatrix)), E_FAIL);
	/* Prototype_GameObject_BowTarget */
	FAILED_CHECK_RETURN(CGameInstance::GetInstance()->Add_Prototype(L"Prototype_GameObject_BowTarget", CBowTarget::Create(m_pDevice, m_pContext)), E_FAIL);

	/* Prototype_Component_Model_TeleportFlower */
	
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationX(XMConvertToRadians(90.f));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAPTOOL, L"Prototype_Component_Model_TeleportFlowerAnim",
		CModel::Create(m_pDevice, m_pContext, L"../Bin/Resources/Anim/TeleportFlower/TeleportFlower.model", PivotMatrix))))
		return E_FAIL;
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(L"Prototype_GameObject_TeleportFlower", CTelePort_Flower::Create(m_pDevice, m_pContext)), E_FAIL);

	/* For.Prototype_Component_Model_ChestAnim*/
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAPTOOL, L"Prototype_Component_Model_ChestAnim",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/Chest_Anim/Chest.model"), PivotMatrix))))
		return E_FAIL;
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(L"Prototype_GameObject_Chest", CChest_Anim::Create(m_pDevice, m_pContext)), E_FAIL);

	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Rope_RotRock", true, false, true))) return E_FAIL;
	// Prototype_GameObject_CRope_RotRock
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_CRope_RotRock"), CRope_RotRock::Create(m_pDevice, m_pContext)))) return E_FAIL;
	

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	FAILED_CHECK_RETURN(CGameInstance::GetInstance()->Add_Prototype(LEVEL_MAPTOOL, L"Prototype_Component_Model_HealthFlower",
		CModel::Create(m_pDevice, m_pContext, L"../Bin/Resources/Anim/HealthFlower/HealthFlower.model", PivotMatrix)), E_FAIL);
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_HealthFlower_Anim"), CHealthFlower_Anim::Create(m_pDevice, m_pContext)))) return E_FAIL;

	/* Prototype Component_Texture_Dissolve_BowTarget */
	FAILED_CHECK_RETURN(CGameInstance::GetInstance()->Add_Prototype(LEVEL_MAPTOOL, L"Prototype_Component_Texture_Dissolve_BowTarget", CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effect/DiffuseTexture/E_Effect_37.png")), E_FAIL);


#endif 

#pragma region Map_Four
	if(bRealObject==true)
	{
		PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationX(XMConvertToRadians(-90.f));
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAPTOOL, L"Prototype_Component_Model_Sakura_Flower",
			CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/NonAnim/Sakura/Sakura_Flower.mdat"), PivotMatrix, nullptr, false, true, "../Bin/Resources/NonAnim/Sakura/Sakura_Flower.json", false))))
			return E_FAIL;

		PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAPTOOL, L"Prototype_Component_Model_CherryBlossomTreeAlive",
			CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/NonAnim/Sakura/CherryBlossomTreeAlive.mdat"), PivotMatrix, nullptr, false, true, "../Bin/Resources/NonAnim/Sakura/CherryBlossomTreeAlive.json", false))))
			return E_FAIL;

		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "WoodlandHouse_1", true, true, true)))
			assert(!"WoodlandHouse_1");
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "WoodlandHouse_2", true, true, true)))
			assert(!"WoodlandHouse_2");

		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Map4/Bell", true, true, true,false,true)))
			assert(!"Map4/Bell");
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Map4/FarmEntranceStructure/Beam", true, true, true, false, true)))
			assert(!"Map4/FarmEntranceStructure/Beam");
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Map4/FarmEntranceStructure/Pillar", true, true, true,false, true)))
			assert(!"Map4/FarmEntranceStructure/Pillar");
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Map4/FarmEntranceStructure/Root", true, true, true, false, true)))
			assert(!"Map4/FarmEntranceStructure/Root");
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Map4/FarmEntranceStructure/Structure", true, true, true, false, true)))
			assert(!"Map4/FarmEntranceStructure/Structure");
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Map4/FarmEntranceStructure/Tree_Rock", true, true, true, false, true)))
			assert(!"Map4/FarmEntranceStructure/Tree_Rock");
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Map4/Forge", true, true, true, false, true)))
			assert(!"Map4/Forge");
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Map4/HouseKit/BirdHouse", true, true, true, false, true)))
			assert(!"Map4/HouseKit/BirdHouse");
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Map4/HouseKit/MainGate", true, true, true, false, true)))
			assert(!"Map4/HouseKit/MainGate");
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Map4/HouseKit/MainHouse", true, true, true, false, true)))
			assert(!"Map4/HouseKit/MainHouse");
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Map4/HouseKit/Rock", true, true, true, false, true)))
			assert(!"Map4/HouseKit/Rock");
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Map4/LightHouse/Arena", true, true, true, false, true)))
			assert(!"Map4/LightHouse/Arena");
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Map4/LightHouse/BigRoots", true, true, true, false, true)))
			assert(!"Map4/HouseKit/BigRoots");
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Map4/LightHouse/LightHouse", true, true, true, false, true)))
			assert(!"Map4/LightHouse/LightHouse");
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Map4/LightHouse/Roots", true, true, true, false, true)))
			assert(!"Map4/LightHouse/Roots");
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Map4/LightHouse/Structure", true, true, true, false, true)))
			assert(!"Map4/LightHouse/Structure");
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Map4/RusuHut", true, true, true, false, true)))
			assert(!"Map4/RusuHut");
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Map4/SummoningRoom", true, true, true, false, true)))
			assert(!"Map4/SummoningRoom");
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "VillageCart", true, false, true, false, true)))
			assert(!"VillageCart");
	}
#pragma endregion Map_Four


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
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Cliff/Cliff_Sheer", true, true, true, false, true)))
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
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Stone/Stone_Bridge", true, true, true,false,true)))
		assert(!"Issue");
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Stone/Stone_Stairs", true, true, true,false,true)))
		assert(!"Issue");
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Stone/StoneFloor_1", true, true, true,false,true)))
		assert(!"Issue");
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Stone/StoneFloor_2", true, true, true,false,true)))
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
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "VantagePlatform", true, true, true)))						
		assert(!"Issue");
#pragma endregion ~VantagePlatform

#pragma region RuinKit
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "RuinKit/RuinKit_LedgeStone", true, true, true)))
		assert(!"Issue");
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "RuinKit/RuinKit_Pillar", true, true, true)))
		assert(!"Issue");
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "RuinKit/RuinKit_Railing", true, true)))				
		assert(!"Issue");
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "RuinKit/RuinKit_Rubble", true, true, true)))
		assert(!"Issue");
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "RuinKit/RuinsKit_Brick", true, true, true)))
		assert(!"Issue");
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "RuinKit/RuinsKit_BridgeLarge", true, true, true)))
		assert(!"Issue");
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "RuinKit/RuinsKit_BridgeShort", true, true, true)))
		assert(!"Issue");
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "RuinKit/RuinStaris", true, true, true,false,true)))
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
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "DeadZone/BushDead_02", true, true)))
		assert(!"Issue");

#pragma endregion DeadZone

#pragma region RuinDebris
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "RuinDebris", true, true)))
		assert(!"Issue");
#pragma endregion RuinDebris

#endif
	if (true == bRealObject)
	{
	
#pragma region GroundCover /* NoPXTriangle*/
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

#pragma region Foliage  /* NoPXTriangle*/
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

		/* Prototype_Component_Model_RuinsKit_BombPlatForm */
		PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(90.f));
		FAILED_CHECK_RETURN(CGameInstance::GetInstance()->Add_Prototype(LEVEL_MAPTOOL, L"Prototype_Component_Model_RuinsKit_BombPlatForm",
			CModel::Create(m_pDevice, m_pContext, L"../Bin/Resources/NonAnim/RuinPlatform/RuinPlatform05.mdat", PivotMatrix, nullptr, false, false, "../Bin/Resources/NonAnim/RuinPlatform/RuinPlatform05.json", false)), E_FAIL);
		/* Prototype_GameObject_BombPlatform */
		FAILED_CHECK_RETURN(CGameInstance::GetInstance()->Add_Prototype(L"Prototype_GameObject_RuinsKit_BombPlatForm", CBombPlatform::Create(m_pDevice, m_pContext)), E_FAIL);

	


#pragma  endregion Born_GroundCover

#pragma  region Start_Forest_Room
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "PowerCrystal", true, false, true)))
			assert(!"Issue");
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "PulseObjects/PluseStone_Big", true, false, true,false,true)))
			return E_FAIL;
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "PulseObjects/PulsePlate_StoneBorder", true, false, true, false, true)))
			return E_FAIL;
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "PulseObjects/PulsePlateEnclosure_02", true, false, true, false, true)))
			return E_FAIL;
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "PulseObjects/PulseStoneRock", true, false, true, false, true)))
			return E_FAIL;
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "RuinKit/RuinKit_LedgeStone", true, true, true, false, true)))
			return E_FAIL;
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "RuinKit/RuinKit_Pillar", true, true, true, false, true)))
			return E_FAIL;
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "RuinKit/RuinsKit_Brick", true, true, true, false, true)))
			return E_FAIL;
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "RuinKit/RuinsKit_BridgeLarge", true, true, true, false, true)))
			return E_FAIL;
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "RuinKit/RuinsKit_ToriGate", true, true, true, false, true)))
			return E_FAIL;
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Rock/Rock_Arch", true, true, true, false, true)))
			assert(!"Issue");
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "RuinPlatform", true, true, true,false,true)))
			assert(!"Issue");
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Rock/Rock_Rubble", true, true, true, false, true)))
			return E_FAIL;
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Rock/Rock_Big", true, true, true, false, true)))
			assert(!"Issue");
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Cliff/Cliff_Rock", true, true, true, false, true)))
			return E_FAIL;
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Rock/God_Rock", true, true, true, false, true)))
			return E_FAIL;
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Cliff/Cliff_Cap", true, true, true, false, true)))
			assert(!"Issue");
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Cliff/Cliff_W2", true, true, true, false, true)))
			return E_FAIL;
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Cliff/Cliff_W3", true, true, true, false, true)))
			assert(!"Issue");
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Cliff/Cliff_Wall_Large", true, true, true, false, true)))
			return E_FAIL;
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Cliff/Cliff_Wall_Med", true, true, true, false, true)))
			return E_FAIL;
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Cliff/Cliff_Wall_Short", true, true, true, false, true)))
			return E_FAIL;
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Cliff/Cliff_Glitter", true, true, true, false, true)))
			assert(!"Issue");
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Cliff/Cliff_Sheer", true, true, true, false, true)))
			assert(!"Issue");
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "RuinKit/RuinKit_Rubble", true, true, true, false, true)))
			return E_FAIL;
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "RuinKit/RuinStaris", true, true, true,false,true)))
			return E_FAIL;
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Statue/Owl_WrapShrine", false, true, true, false, true)))
			return E_FAIL;
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Statue", true, true, true, false, true)))
			return E_FAIL;
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "RuinKit/RuinKit_Arch_Gate", true, true, true, false, true)))
			return E_FAIL;
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "RuinKit/RuinsKit_Tree", true, true, true, false, true)))
			return E_FAIL;
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "RuinKit/RuinsKit_FloorTile", true, true, true,false,true)))
			return E_FAIL;
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "RuinKit/RuinKit_Piece", true, true, true, false, true)))
			return E_FAIL;
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "RuinKit/RuinsKit_Wall_Broken", true, true, true, false, true)))
			return E_FAIL;
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "RuinKit/RuinsWall", true, true, true, false, true)))
			assert(!"Issue");
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "RuinKit/Ruinskit_Collum", true, true, true, false, true)))
			assert(!"Issue");
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Cliff/Cliff_Ledge", true, true, true, false, true)))
			return E_FAIL;
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Rock/Rock_Small", true, true, true, false, true)))
			return E_FAIL;
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Rock/Rock_Medium", true, true, true, false, true)))
			return E_FAIL;
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Forest_1/RotGod_Statue_crumbled", true, false, true, false, true)))
			return E_FAIL;
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Forest_1/Start_Gate", true, true, true, false, true)))
			return E_FAIL;
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Forest_1/FirstTear_FallenTree", true, false, true, false, true)))
			return E_FAIL;
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Forest_1/First_Room_Deco", true, true, true, false, true)))
			return E_FAIL;
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Forest_1/ShrineOfDeath_MainRock", true, true, true, false, true)))
			return E_FAIL;
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Forest_1/ShrineOfLife", true, true, true, false, true)))
			return E_FAIL;
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Stone/StoneFloor_1", true, true, true, false, true)))
			return E_FAIL;
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Stone/StoneFloor_2", true, true, true,false,true)))
			return E_FAIL;
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Stone/Stone_Bridge", true, true, true, false, true)))
			assert(!"Issue");
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Stone/Stone_Stairs", true, true, true,false,true)))
			assert(!"Issue");
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Trees/Giant", true, true, true, false, true)))
			return E_FAIL;
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Trees/Fallen", true, true, true, false, true)))
			return E_FAIL;
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Trees/Dead", true, true, true, false, true)))
			return E_FAIL;
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Trees/CeDarTree", true, true, true, false, true)))
			return E_FAIL;
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Trees/Bare", true, true, true, false, true)))
			return E_FAIL;
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Trees/Billboard", true, true, true, false, true)))
			return E_FAIL;
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Trees/Branches/Canopy", true, true, true, false, true)))
			return E_FAIL;
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Trees/Branches/GodTree", true, true, true, false, true)))
			return E_FAIL;
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Trees/Branches/Tree_Branch", true, true, true, false, true)))
			return E_FAIL;
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Trees/Canopy", true, true, true, false, true)))
			return E_FAIL;
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Save_StoneTree", true, true,true, false, true)))
			assert(!"Issue");
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "DeadZone/Deadzondes_GoopWeb", true, true, true)))
			assert(!"Issue");
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "DeadZone/DeadZoneAndPurifiedTree", true, true, true, false, true)))
			assert(!"Issue");
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "DeadZone/DeadzoneClumps", true, true, true, false, true)))
			assert(!"Issue");
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "DeadZone/DeadzoneGroudCover", true, true, true)))
			assert(!"Issue");
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "DeadZone/DeadzoneHeart", true, true, true, false, true)))
			assert(!"Issue");
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "DeadZone/DeadzoneWall", true, true, true, false, true)))
			assert(!"Issue");
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "DeadZone/Gate", true, true, true, false, true)))
			assert(!"Issue");
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "DeadZone/Trees", true, true, true, false, true)))
			assert(!"Issue");
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "DeadZone/BushDead_02", true, true)))
			assert(!"Issue");
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Rot/Rot_Rock", true, true,true, false, true)))
			assert(!"Issue");
		
		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Rot/RotCarry_Piece", true, true,true, false, true)))
			assert(!"Issue");

		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "MeditationStump", true, false, true, false, true)))
			assert(!"Issue");

		if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Fire_Brazier", false, false, true)))
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
	/* For.Prototype_GameObject_FloorTile */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_FloorMesh"),
		CFloorMesh::Create(m_pDevice, m_pContext))))
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
	/* For.Prototype_GameObject_SceneChangePortal */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_SceneChangePortal"),
		CSceneChangePortal::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	

	/* For.Prototype_GameObject_Meditation_Spot */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Meditation_Spot"),
		CMeditation_Spot::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_GimmickObj */
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

	/* For.Prototype_GameObject_FieldBecon_Anim */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_FieldBecon_Anim"),
		CFieldBecon_Anim::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_DZ_FallenTree_Anim"),
		CDZ_FallenTree_Anim::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_GroundMark */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_GroundMark"),
		CGroundMark::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Born_GroundCover"),
		CBorn_GroundCover::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_HatCart"),
		CHatCart::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_Sakura */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Sakura"),
		CE_P_Sakura::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	/* For.Prototype_GameObject_Dynamic_Stone */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Dynamic_Stone"),
		CDynamic_Stone::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Dynamic_StoneCube */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Dynamic_StoneCube"),
		CDynamic_StoneCube::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_DeadZoneBossTree */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_DeadZoneBossTree"),
		CDeadZoneBossTree::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Fire_Brazier */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Fire_Brazier"),
		CFire_Brazier::Create(m_pDevice, m_pContext))))
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
	FAILED_CHECK_RETURN(Loading_ForBJ((_uint)LEVEL_EFFECT), E_FAIL);

	lstrcpy(m_szLoadingText, TEXT("Loading End."));
	
	m_isFinished = true;
	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CLoader::Loading_ForMiniGame()
{
	lstrcpy(m_szLoadingText, TEXT("Loading..."));

	Sleep(4000);

	m_isFinished = true;
	SetWindowText(g_hWnd, TEXT("Loading Complete!! Wait a moment"));
	return S_OK;
}

HRESULT CLoader::Loading_ForFinal()
{
	lstrcpy(m_szLoadingText, TEXT("Loading..."));

	Sleep(4000);

	m_isFinished = true;
	SetWindowText(g_hWnd, TEXT("Loading Complete!! Wait a moment"));
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
	bIsLod = false;

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
	lstrcpy(m_szLoadingText, TEXT("Loading WJ..."));

	if (FAILED(LoadNonAnimFolderModel(iLevelIndex, "Water"))) return E_FAIL;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance)

	_matrix	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);

	// Prototype_Component_Model_CineCam
	if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, L"Prototype_Component_Model_CineCam", CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/NonAnim/CineCam.mdat"), PivotMatrix)))) return E_FAIL;

	// Prototype_GameObject_WaterNormalTexture
	if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, TEXT("Prototype_GameObject_WaterNormalTexture"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/NonAnim/Water/Texture/Normal/%d.png"), 13)))) return E_FAIL;

	// Prototype_GameObject_WaterNoiseTexture
	if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, TEXT("Prototype_GameObject_WaterNoiseTexture"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/NonAnim/Water/Texture/Noise/%d.png"), 2)))) return E_FAIL;

	// Prototype_GameObject_WaterMaskTexture
	if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, TEXT("Prototype_GameObject_WaterMaskTexture"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/NonAnim/Water/Texture/Mask/%d.png"), 2)))) return E_FAIL;

	// Prototype_GameObject_WaterPlane
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_WaterPlane"), CWaterPlane::Create(m_pDevice, m_pContext)))) return E_FAIL;

	// Prototype_GameObject_CinematicCamera
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_CinematicCamera"), CCinematicCamera::Create(m_pDevice, m_pContext)))) return E_FAIL;

	// Prototype_GameObject_PortalPlane
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_PortalPlane"), CPortalPlane::Create(m_pDevice, m_pContext)))) return E_FAIL;

	// Prototype_GameObject_WorldTrigger
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_WorldTrigger"), CWorldTrigger::Create(m_pDevice, m_pContext)))) return E_FAIL;

	RELEASE_INSTANCE(CGameInstance)

	return S_OK;
}

HRESULT CLoader::Loading_ForJH(_uint iLevelIndex)
{
	lstrcpy(m_szLoadingText, TEXT("Loading JH ..."));

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


	/* Prototype_Component_Model_ChestAnim */
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	FAILED_CHECK_RETURN(CGameInstance::GetInstance()->Add_Prototype(iLevelIndex, L"Prototype_Component_Model_ChestAnim", CModel::Create(m_pDevice, m_pContext, L"../Bin/Resources/Anim/Chest_Anim/Chest.model", PivotMatrix)), E_FAIL);

	/* Prototype_Component_Model_HealthFlower */
	FAILED_CHECK_RETURN(CGameInstance::GetInstance()->Add_Prototype(iLevelIndex, L"Prototype_Component_Model_HealthFlower", CModel::Create(m_pDevice, m_pContext, L"../Bin/Resources/Anim/HealthFlower/HealthFlower.model", PivotMatrix)), E_FAIL);

	/* Prototype_Component_Model_Spirit_Arrow */
	FAILED_CHECK_RETURN(LoadNonAnimFolderModel(iLevelIndex, "Spirit_Arrow", false, false, false), E_FAIL);

	/* Prototype_Component_Model_Taro_Mask */
	FAILED_CHECK_RETURN(LoadNonAnimFolderModel(iLevelIndex, "Taro_Mask", false, false, false), E_FAIL);

	/* Prototype_Component_Model_BowTarget */
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	FAILED_CHECK_RETURN(CGameInstance::GetInstance()->Add_Prototype(iLevelIndex, L"Prototype_Component_Model_BowTarget", CModel::Create(m_pDevice, m_pContext, L"../Bin/Resources/Anim/BowTarget/BowTarget.model", PivotMatrix)), E_FAIL);
	/* Prototype_GameObject_BowTarget */
	FAILED_CHECK_RETURN(CGameInstance::GetInstance()->Add_Prototype(L"Prototype_GameObject_BowTarget", CBowTarget::Create(m_pDevice, m_pContext)), E_FAIL);

	/* Prototype_Component_Model_BowTarget_Husk */
	FAILED_CHECK_RETURN(CGameInstance::GetInstance()->Add_Prototype(iLevelIndex, L"Prototype_Component_Model_BowTarget_Husk", CModel::Create(m_pDevice, m_pContext, L"../Bin/Resources/Anim/BowTarget/BowTarget_Husk.mdat", PivotMatrix)), E_FAIL);

	/* Prototype_Component_Model_Broken_BowTarget */
	FAILED_CHECK_RETURN(LoadNonAnimFolderModel(iLevelIndex, "Broken_BowTarget", false, false, false), E_FAIL);

	/* Prototype Component_Texture_Dissolve_BowTarget */
	FAILED_CHECK_RETURN(CGameInstance::GetInstance()->Add_Prototype(iLevelIndex, L"Prototype_Component_Texture_Dissolve_BowTarget", CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Effect/DiffuseTexture/E_Effect_37.png")), E_FAIL);

	/* GAMEOBJECTS */
	/* Prototype_GameObject_Kena */
	FAILED_CHECK_RETURN(CGameInstance::GetInstance()->Add_Prototype(L"Prototype_GameObject_Kena", CKena::Create(m_pDevice, m_pContext)), E_FAIL);

	/* Prototype_GameObject_Kena_Staff */
	FAILED_CHECK_RETURN(CGameInstance::GetInstance()->Add_Prototype(L"Prototype_GameObject_Kena_Staff", CKena_Staff::Create(m_pDevice, m_pContext)), E_FAIL);

	/* Prototype_GameObject_Kena_MainOutfit */
	FAILED_CHECK_RETURN(CGameInstance::GetInstance()->Add_Prototype(L"Prototype_GameObject_Kena_MainOutfit", CKena_MainOutfit::Create(m_pDevice, m_pContext)), E_FAIL);

	/* Prototype_GameObject_Kena_Taro_Mask */
	FAILED_CHECK_RETURN(CGameInstance::GetInstance()->Add_Prototype(L"Prototype_GameObject_Kena_Taro_Mask", CTaro_Mask::Create(m_pDevice, m_pContext)), E_FAIL);

	/* Prototype_GameObject_Rot_Bomb */
	FAILED_CHECK_RETURN(CGameInstance::GetInstance()->Add_Prototype(L"Prototype_GameObject_Rot_Bomb", CRotBomb::Create(m_pDevice, m_pContext)), E_FAIL);

	/* Prototype_GameObject_BombTrail */
	FAILED_CHECK_RETURN(CGameInstance::GetInstance()->Add_Prototype(L"Prototype_GameObject_BombTrail", CE_BombTrail::Create(m_pDevice, m_pContext)), E_FAIL);

	/* Prototype_GameObject_Spirit_Arrow */
	FAILED_CHECK_RETURN(CGameInstance::GetInstance()->Add_Prototype(L"Prototype_GameObject_SpiritArrow", CSpiritArrow::Create(m_pDevice, m_pContext)), E_FAIL);

	/* Prototype_Component_Model_TeleportFlower */
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationX(XMConvertToRadians(90.f));
	FAILED_CHECK_RETURN(CGameInstance::GetInstance()->Add_Prototype(iLevelIndex, L"Prototype_Component_Model_TeleportFlowerAnim", CModel::Create(m_pDevice, m_pContext, L"../Bin/Resources/Anim/TeleportFlower/TeleportFlower.model", PivotMatrix)), E_FAIL);

	/* Prototype_GameObject_TeleportFlower */
	FAILED_CHECK_RETURN(CGameInstance::GetInstance()->Add_Prototype(L"Prototype_GameObject_TeleportFlower", CTelePort_Flower::Create(m_pDevice, m_pContext)), E_FAIL);

	/* Prototype_GameObject_Chest */
	FAILED_CHECK_RETURN(CGameInstance::GetInstance()->Add_Prototype(L"Prototype_GameObject_Chest", CChest_Anim::Create(m_pDevice, m_pContext)), E_FAIL);

	/* Prototype_GameObject_HealthFlower */
	FAILED_CHECK_RETURN(CGameInstance::GetInstance()->Add_Prototype(L"Prototype_GameObject_HealthFlower_Anim", CHealthFlower_Anim::Create(m_pDevice, m_pContext)), E_FAIL);

	/* Prototype_GameObject_BowTarget_Trigger */
	FAILED_CHECK_RETURN(CGameInstance::GetInstance()->Add_Prototype(L"Prototype_GameObject_BowTarget_Trigger", CBowTarget_Trigger::Create(m_pDevice, m_pContext)), E_FAIL);

	/* Prototype_GameObject_Respawn_Trigger */
	FAILED_CHECK_RETURN(CGameInstance::GetInstance()->Add_Prototype(L"Prototype_GameObject_Respawn_Trigger", CRespawn_Trigger::Create(m_pDevice, m_pContext)), E_FAIL);

	/* Prototype_GameObject_Village_Trigger */
	FAILED_CHECK_RETURN(CGameInstance::GetInstance()->Add_Prototype(L"Prototype_GameObject_Village_Trigger", CVillage_Trigger::Create(m_pDevice, m_pContext)), E_FAIL);
	
	/* Prototype_GameObject_Player_Camera */
	FAILED_CHECK_RETURN(CGameInstance::GetInstance()->Add_Prototype(L"Prototype_GameObject_Camera_Player", CCamera_Player::Create(m_pDevice, m_pContext)), E_FAIL);

	return S_OK;
}

HRESULT CLoader::Loading_ForSY(_uint iLevelIndex)
{
	lstrcpy(m_szLoadingText, TEXT("Loading SY..."));

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	//Safe_AddRef(pGameInstance);

	/********************************************/
	/*							For. VIBuffer							*/
	/********************************************/

	/* Prototype_Component_VIBuffer_Point_Instancing_S2 */
	if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, TEXT("Prototype_Component_VIBuffer_PtInstancing_S2"),
		CVIBuffer_Point_Instancing_S2::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/********************************************/
	/*								For. Collider						*/
	/********************************************/

	/* For.Prototype_Component_Collider_AABB*/
	if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, TEXT("Prototype_Component_Collider_AABB_ForTrigger"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_AABB))))
		assert(!"Issue");

	/********************************************/
	/*				For. ModelCom				*/
	/********************************************/

	_matrix	 PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);

	/* For.Prototype_Component_Model_ShockRing */
	if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, L"Prototype_Component_Model_ShockRing",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Effect/ShockArrow_Ring.mdat"), PivotMatrix))))
		return E_FAIL;


	//if (iLevelIndex == (_uint)LEVEL_GAMEPLAY)
	//{
	//	FAILED_CHECK_RETURN(CGameInstance::GetInstance()->Add_Prototype(iLevelIndex, L"Prototype_Component_Model_HealthFlower", CModel::Create(m_pDevice, m_pContext, L"../Bin/Resources/Anim/HealthFlower/HealthFlower.model", PivotMatrix)), E_FAIL);

	//	FAILED_CHECK_RETURN(CGameInstance::GetInstance()->Add_Prototype(L"Prototype_GameObject_HealthFlower_Anim", CHealthFlower_Anim::Create(m_pDevice, m_pContext)), E_FAIL);
	//}


	/********************************************/
	/*				For. GameObject				*/
	/********************************************/

	/* Effect_Particle_Base  */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_Particle_Base"),
		CEffect_Particle_Base::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Effect_Mesh_Base  */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_Mesh_Base"),
		CEffect_Mesh_Base::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Effect_Texture_Base  */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_Texture_Base"),
		CEffect_Texture_Base::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* WorldTrigger */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_WorldTrigger_S2"),
		CWorldTrigger_S2::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* HatCart */
// 	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_HatCart"),
// 		CHatCart::Create(m_pDevice, m_pContext))))
// 		return E_FAIL;
	//MSG_BOX("GameObject HatCart : ForSY");


	return S_OK;
}

HRESULT CLoader::Loading_ForBJ(_uint iLevelIndex)
{
	lstrcpy(m_szLoadingText, TEXT("Loading BJ..."));

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
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/Enemy/VillageGuard/VillageGuard.model"), PivotMatrix)))) return E_FAIL;

	// Prototype_Component_Model_WoodKnight
	if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, L"Prototype_Component_Model_WoodKnight",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/Enemy/WoodKnight/WoodKnight.model"), PivotMatrix)))) return E_FAIL;

	// Prototype_Component_Model_Sapling
	if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, L"Prototype_Component_Model_Sapling",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/Enemy/Sapling/Sapling.model"), PivotMatrix)))) return E_FAIL;

	// Prototype_Component_Model_BranchTosser
	if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, L"Prototype_Component_Model_BranchTosser",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/Enemy/BranchTosser/BranchTosser.model"), PivotMatrix)))) return E_FAIL;

	// Prototype_Component_Model_RotHat_Acorn
	if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, L"Prototype_Component_Model_RotHat_Acorn",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/NonAnim/RotHat/00_Acorn/RotHat_Acorn.mdat"), PivotMatrix)))) return E_FAIL;
	// Prototype_Component_Model_RotHat_Clover
	if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, L"Prototype_Component_Model_RotHat_Clover",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/NonAnim/RotHat/01_Clover/RotHat_Clover.mdat"), PivotMatrix)))) return E_FAIL;
	// Prototype_Component_Model_RotHat_Egg
	if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, L"Prototype_Component_Model_RotHat_Egg",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/NonAnim/RotHat/02_Egg/RotHat_Egg.mdat"), PivotMatrix)))) return E_FAIL;
	// Prototype_Component_Model_RotHat_Mushroom
	if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, L"Prototype_Component_Model_RotHat_Mushroom",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/NonAnim/RotHat/03_Mushroom/RotHat_Mushroom.mdat"), PivotMatrix)))) return E_FAIL;
	// Prototype_Component_Model_RotHat_Butterfly
	if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, L"Prototype_Component_Model_RotHat_Butterfly",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/NonAnim/RotHat/04_Butterfly/RotHat_Butterfly.mdat"), PivotMatrix)))) return E_FAIL;
	// Prototype_Component_Model_RotHat_Dinosaur
	if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, L"Prototype_Component_Model_RotHat_Dinosaur",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/NonAnim/RotHat/05_Dinosaur/RotHat_Dinosaur.mdat"), PivotMatrix)))) return E_FAIL;
	// Prototype_Component_Model_RotHat_RedBow
	if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, L"Prototype_Component_Model_RotHat_RedBow",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/NonAnim/RotHat/06_RedBow/RotHat_RedBow.mdat"), PivotMatrix)))) return E_FAIL;
	// Prototype_Component_Model_RotHat_Cowboy
	if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, L"Prototype_Component_Model_RotHat_Cowboy",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/NonAnim/RotHat/07_Cowboy/RotHat_Cowboy.mdat"), PivotMatrix)))) return E_FAIL;
	// Prototype_Component_Model_RotHat_Halo
	if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, L"Prototype_Component_Model_RotHat_Halo",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/NonAnim/RotHat/08_Halo/RotHat_Halo.mdat"), PivotMatrix)))) return E_FAIL;
	// Prototype_Component_Model_RotHat_Acorn
	if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, L"Prototype_Component_Model_RotHat_Deer",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/NonAnim/RotHat/09_Deer/RotHat_Deer.mdat"), PivotMatrix)))) return E_FAIL;
	// Prototype_Component_Model_RotHat_Pancake
	if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, L"Prototype_Component_Model_RotHat_Pancake",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/NonAnim/RotHat/10_Pancake/RotHat_Pancake.mdat"), PivotMatrix)))) return E_FAIL;
	// Prototype_Component_Model_RotHat_Sunflower
	if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, L"Prototype_Component_Model_RotHat_Sunflower",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/NonAnim/RotHat/11_Sunflower/RotHat_Sunflower.mdat"), PivotMatrix)))) return E_FAIL;
	// Prototype_Component_Model_RotHat_Cat
	if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, L"Prototype_Component_Model_RotHat_Cat",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/NonAnim/RotHat/12_Cat/RotHat_Cat.mdat"), PivotMatrix)))) return E_FAIL;
	// Prototype_Component_Model_RotHat_Bunny
	if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, L"Prototype_Component_Model_RotHat_Bunny",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/NonAnim/RotHat/13_Bunny/RotHat_Bunny.mdat"), PivotMatrix)))) return E_FAIL;
	// Prototype_Component_Model_RotHat_Crown
	if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, L"Prototype_Component_Model_RotHat_Crown",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/NonAnim/RotHat/14_Crown/RotHat_Crown.mdat"), PivotMatrix)))) return E_FAIL;
	// Prototype_Component_Model_RotHat_Samurai
	if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, L"Prototype_Component_Model_RotHat_Samurai",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/NonAnim/RotHat/15_Samurai/RotHat_Samurai.mdat"), PivotMatrix)))) return E_FAIL;
	
	/**********************************/
	/************For.Warrior***********/
	/**********************************/
	// Prototype_Component_Model_Boss_Warrior
	PivotMatrix = XMMatrixScaling(0.015f, 0.015f, 0.015f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, L"Prototype_Component_Model_Boss_Warrior",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/Enemy/Boss_Warrior/Boss_Warrior.model"), PivotMatrix)))) return E_FAIL;

	// Prototype_Component_Model_Boss_Warrior_Hat
	if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, L"Prototype_Component_Model_Boss_Warrior_Hat",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/NonAnim/Enemy/Boss_Warrior_Hat/Boss_Warrior_Hat.mdat"), PivotMatrix)))) return E_FAIL;
	/*********************************************************************************************************************************************/
	
	/**********************************/
	/************ For. Hunter **********/
	/**********************************/
	// Prototype_Component_Model_Boss_Hunter
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, L"Prototype_Component_Model_Boss_Hunter",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/Enemy/Boss_Hunter/Boss_Hunter.model"), PivotMatrix)))) return E_FAIL;

	// Prototype_Component_Model_Boss_Hunter_Arrow
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationX(XMConvertToRadians(90.0f)) * XMMatrixRotationZ(XMConvertToRadians(90.0f)) * XMMatrixTranslation(0.f, 0.f, 0.75f);
	if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, L"Prototype_Component_Model_Boss_Hunter_Arrow",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/NonAnim/Boss_HunterArrow/Arrow_NonAnim.mdat"), PivotMatrix)))) return E_FAIL;
	
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	/*********************************************************************************************************************************************/

	/**********************************/
	/************For. Shaman**********/
	/**********************************/
	// Prototype_Component_Model_Boss_Shaman
	if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, L"Prototype_Component_Model_Boss_Shaman",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/Enemy/Boss_Shaman/Boss_Shaman.model"), PivotMatrix)))) return E_FAIL;

	_matrix ShamanTrapPivotMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	// Prototype_Component_Model_ShamanTrapHex
	if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, L"Prototype_Component_Model_ShamanTrapHex",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/Enemy/Boss_TrapAsset/ShamanTrapHex.model"), ShamanTrapPivotMatrix)))) return E_FAIL;

	// Prototype_Component_Model_ShamanTrapHexPlane
	if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, L"Prototype_Component_Model_ShamanTrapHexPlane",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/NonAnim/ShamanTrapDecal/ShamanTrapHexPlane.mdat"), ShamanTrapPivotMatrix)))) return E_FAIL;

	// Prototype_Component_Model_FakeShaman
	if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, L"Prototype_Component_Model_FakeShaman",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/NonAnim/Boss_FakeShaman/FakeShaman.mdat"), PivotMatrix)))) return E_FAIL;

	// Prototype_Component_Model_ShamanTrap_DecalGeo_Rescale
	if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, L"Prototype_Component_Model_ShamanTrap_DecalGeo_Rescale",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/NonAnim/ShamanTrapDecal/ShamanTrap_DecalGeo_Rescale.mdat"), PivotMatrix)))) return E_FAIL;
	
	// Prototype_Component_Model_ShamanMask
	if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, L"Prototype_Component_Model_ShamanMask",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/NonAnim/Boss_ShamanMask/ShamanMask.mdat"), PivotMatrix)))) return E_FAIL;

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
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
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/Rot/Rot.model"), PivotMatrix)))) return E_FAIL;

	// Prototype_Component_Model_CorruptVillager
	if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, L"Prototype_Component_Model_CorruptVillager",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/Enemy/CorruptVillager/CorruptVillager.mdat"), PivotMatrix)))) return E_FAIL;

	// Prototype_Component_Model_Moth
	if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, L"Prototype_Component_Model_Moth",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/Enemy/Moth/Moth.model"), PivotMatrix)))) return E_FAIL;
		
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

	// Prototype_GameObject_Rot
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_RotHat"), CRotHat::Create(m_pDevice, m_pContext)))) return E_FAIL;
	
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

	// Prototype_GameObject_ShamanTrapHex
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ShamanTrapHex"), CShamanTrapHex::Create(m_pDevice, m_pContext)))) return E_FAIL;

	// Prototype_GameObject_ShamanTrapGeo
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ShamanTrapGeo"), CShamanTrapGeo::Create(m_pDevice, m_pContext, L"../Bin/Data/Effect/E_HexGround.json")))) return E_FAIL;

	// Prototype_GameObject_ShamanTrapPlane
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ShamanTrapPlane"), CShamanTrapPlane::Create(m_pDevice, m_pContext)))) return E_FAIL;

	// Prototype_GameObject_ShamanMask
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ShamanMask"), CBossShaman_Mask::Create(m_pDevice, m_pContext)))) return E_FAIL;	

	// Prototype_GameObject_BossHunter
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BossHunter"), CBossHunter::Create(m_pDevice, m_pContext)))) return E_FAIL;

	// Prototype_GameObject_HunterArrow
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_HunterArrow"), CHunterArrow::Create(m_pDevice, m_pContext)))) return E_FAIL;

	// Prototype_GameObject_HunterArrow
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MannequinRot"), CMannequinRot::Create(m_pDevice, m_pContext)))) return E_FAIL;
	
	// Prototype_GameObject_BossRock
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BossRock"), CBossRock::Create(m_pDevice, m_pContext)))) return E_FAIL;

	// Prototype_GameObject_BossRockPool
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BossRockPool"), CBossRock_Pool::Create(m_pDevice, m_pContext)))) return E_FAIL;

	// Prototype_GameObject_CameraShaman
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_CameraShaman"), CCamera_Shaman::Create(m_pDevice, m_pContext)))) return E_FAIL;

	// Prototype_GameObject_CameraPhoto
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_CameraPhoto"), CCamera_Photo::Create(m_pDevice, m_pContext)))) return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Loading_ForHO(_uint iLevelIndex)
{
	lstrcpy(m_szLoadingText, TEXT("Loading HO..."));

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	_matrix	 PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
		
#pragma region EFFECT_COMPONENT

	/* For.Prototype_Component_Texture_Effect */
	if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, TEXT("Prototype_Component_Texture_Effect"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/DiffuseTexture/E_Effect_%d.png"), 150))))
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

	/* For.Prototype_Component_Texture_EffectShaman */
	if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, TEXT("Prototype_Component_Texture_EffectShaman"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Boss/Shaman/E_Shaman_%d.png"), 50))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Point_Instancing */
	if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, TEXT("Prototype_Component_VIBuffer_Trail"),
		CVIBuffer_Trail::Create(m_pDevice, m_pContext, 300))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_PointInstancing */
	if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, TEXT("Prototype_Component_VIBuffer_PointInstancing"),
		CVIBuffer_Point_Instancing::Create(m_pDevice, m_pContext, 300))))
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

	/* For.Prototype_Component_Model_Warrior_Root */
	if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, L"Prototype_Component_Model_Warrior_Root",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Effect/Warrior_Root.mdat"), PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Plane_Root */
	if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, L"Prototype_Component_Model_Plane_Root",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Effect/Plane_Root.mdat"), PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_FireSwipe */
	if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, L"Prototype_Component_Model_FireSwipe",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Effect/FireSwipe.mdat"), PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_ShockFront_Extended */
	if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, L"Prototype_Component_Model_ShockFront_Extended",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Effect/ShockFront_Extended.mdat"), PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_BossPlate */
	if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, L"Prototype_Component_Model_BossPlate",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Effect/BossPlate.mdat"), PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_BossHandPlate */
	if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, L"Prototype_Component_Model_BossHandPlate",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Effect/BossHandPlate.mdat"), PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_MonsterPlate */
	if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, L"Prototype_Component_Model_MonsterPlate",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Effect/MonsterPlate.mdat"), PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_ThunderCylinder */
	if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, L"Prototype_Component_Model_ThunderCylinder",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Effect/ThunderCylinder.mdat"), PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_IceDagger */
	if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, L"Prototype_Component_Model_IceDagger",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Effect/IceDagger.mdat"), PivotMatrix))))
		return E_FAIL;
	
	/* For.Prototype_Component_Model_MapleLeaf */
	if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, L"Prototype_Component_Model_MapleLeaf",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Effect/MapleLeaf.mdat"), PivotMatrix, nullptr, false, true))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Beech */
	if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, L"Prototype_Component_Model_Beech",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Effect/Beech.mdat"), PivotMatrix, nullptr, false, true))))
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
		CE_FireBulletExplosion::Create(m_pDevice, m_pContext, L"../Bin/Data/Effect/E_FireBulletExplosion2.json"))))
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

	/* For.Prototype_GameObject_RectTrail */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_RectTrail"),
		CE_RectTrail::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Swipes_Charged */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Swipes_Charged"),
		CE_Swipes_Charged::Create(m_pDevice, m_pContext, L"../Bin/Data/Effect/E_Swipes_Changed.json"))))
		return E_FAIL;

	/* For.Prototype_GameObject_WarriorTrail */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_WarriorTrail"),
		CE_WarriorTrail::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Warrior_FireSwipe */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Warrior_FireSwipe"),
		CE_Warrior_FireSwipe::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Warrior_Root */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Warrior_Root"),
		CE_Warrior_Root::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Warrior_PlaneRoot */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Warrior_PlaneRoot"),
		CE_Warrior_PlaneRoot::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	/* For.Prototype_GameObject_WarriorCloud */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_WarriorCloud"),
		CE_Warriorcloud::Create(m_pDevice, m_pContext, L"../Bin/Data/Effect/E_WarriorCloud.json"))))
		return E_FAIL;

	/* For.Prototype_GameObject_Warrior_P */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Warrior_P"),
		CE_P_Warrior::Create(m_pDevice, m_pContext, L"../Bin/Data/Effect/E_P_Warrior.json"))))
		return E_FAIL;

	/* For.Prototype_GameObject_Warrior_ShockFrontExtended */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Warrior_ShockFrontExtended"),
		CE_Warrior_ShockFrontExtended::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Warrior_Hieroglyph */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Warrior_Hieroglyph"),
		CE_Hieroglyph::Create(m_pDevice, m_pContext, L"../Bin/Data/Effect/E_Hieroglyph.json"))))
		return E_FAIL;

	/* For.Prototype_GameObject_Warrior_ShockFrontEntended_P */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Warrior_ShockFrontEntended_P"),
		CE_P_ShockFrontEntended::Create(m_pDevice, m_pContext, L"../Bin/Data/Effect/E_ShamanCloud.json"))))
		return E_FAIL;
	// E_P_ShockFrontEntended
	// E_ShamanCloud

	/* For.Prototype_GameObject_Warrior_ShockFronExtended_Plane */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Warrior_ShockFronExtended_Plane"),
		CE_Warrior_ShockFronExtended_Plane::Create(m_pDevice, m_pContext, L"../Bin/Data/Effect/E_GroundPlane.json"))))
		return E_FAIL;

	/* For.Prototype_GameObject_Distortion_Plane */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Distortion_Plane"),
		CE_Distortion_Plane::Create(m_pDevice, m_pContext, L"../Bin/Data/Effect/E_DistortionPlane.json"))))
		return E_FAIL;

	/* For.Prototype_GameObject_EnrageInto */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_EnrageInto"),
		CE_EnrageInto::Create(m_pDevice, m_pContext, L"../Bin/Data/Effect/E_EnrageInto.json"))))
		return E_FAIL;

	/* Shaman */
	/* For.Prototype_GameObject_EnrageInto */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ShamanBossPlate"),
		CE_ShamanBossPlate::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_ShamanBossHandPlane */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ShamanBossHandPlane"),
		CE_ShamanBossHandPlane::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_ShamanThunderCylinder */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ShamanThunderCylinder"),
		CE_ShamanThunderCylinder::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_ShamanSummons */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ShamanSummons"),
		CE_ShamanSummons::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_ShamanTrail */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ShamanTrail"),
		CE_ShamanTrail::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_ShamanSmoke */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ShamanSmoke"),
		CE_ShamanSmoke::Create(m_pDevice, m_pContext, L"../Bin/Data/Effect/E_ShamanIdle.json"))))
		return E_FAIL;
	// E_ShamanBodyCloud
	/* Shaman */

	/* For.Prototype_GameObject_ExplosionGravity */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ExplosionGravity"),
		CE_P_ExplosionGravity::Create(m_pDevice, m_pContext))))
	return E_FAIL;

	/* Kena Dash */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_DashCloud"),
		CE_KenaDash::Create(m_pDevice, m_pContext, L"../Bin/Data/Effect/E_DashCloud.json"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_DashCone"),
		CE_KenaDashCone::Create(m_pDevice, m_pContext, L"../Bin/Data/Effect/E_M_DashCone.json"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_DashRing"),
		CE_KenaDashRing::Create(m_pDevice, m_pContext, L"../Bin/Data/Effect/E_DashCircle.json"))))
		return E_FAIL;
	/* Kena Dash */

	/* For.Prototype_GameObject_HunterTrail */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_HunterTrail"),
		CE_HunterTrail::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_ShammanIceDagger */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ShammanIceDagger"),
		CE_ShamanIceDagger::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_ShammanLazer */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ShammanLazer"),
		CE_ShamanLazer::Create(m_pDevice, m_pContext, L"../Bin/Data/Effect/E_ShamanLazer.json"))))
		return E_FAIL;

	/* For.Prototype_GameObject_ShamanTeleport */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ShamanTeleport"),
		CE_P_ShamanTeleport::Create(m_pDevice, m_pContext, L"../Bin/Data/Effect/E_P_BossPlate.json"))))
		return E_FAIL;

	/* For.Prototype_GameObject_CommonCircleSp */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_CommonCircleSp"),
		CE_Common_CircleSp::Create(m_pDevice, m_pContext, L"../Bin/Data/Effect/E_Common_CircleSp.json"))))
		return E_FAIL;

	/* For.Prototype_GameObject_LazerTrail */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_LazerTrail"), CE_LazerTrail::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_LevelUp */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_LevelUp"), CE_P_Level::Create(m_pDevice, m_pContext, L"../Bin/Data/Effect/Particle/E_KenaLevelUp.json"))))
		return E_FAIL;

	/* For.Prototype_GameObject_Rot_P */ 
 	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Rot_P"), CE_P_Rot::Create(m_pDevice, m_pContext, L"../Bin/Data/Effect/Particle/E_P_Rot.json"))))
		return E_FAIL;

	/* For.Prototype_GameObject_ShamanDust */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ShamanDust"), CE_P_ShamanDust::Create(m_pDevice, m_pContext, L"../Bin/Data/Effect/Particle/E_ShamanDust.json"))))
		return E_FAIL;

#pragma endregion Effect_Object

	lstrcpy(m_szLoadingText, TEXT("Loading Effects MapObject..."));
#pragma region MapObject
	/* For.Prototype_GameObject_E_Chest */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_E_Chest"), CE_Chest::Create(m_pDevice, m_pContext, L"../Bin/Data/Effect/E_Chest.json"))))
		return E_FAIL;

	/* For.Prototype_GameObject_E_P_Chest */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_E_P_Chest"), CE_P_Chest::Create(m_pDevice, m_pContext, L"../Bin/Data/Effect/E_P_Chest.json"))))
		return E_FAIL;

	/* For.Prototype_GameObject_P_BombPlatform */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_P_BombPlatform"), CE_P_Bombplatform::Create(m_pDevice, m_pContext, L"../Bin/Data/Effect/E_P_BombPlate.json"))))
		return E_FAIL;

	/* For.Prototype_GameObject_P_CommonBox */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_P_CommonBox"), CE_P_CommonBox::Create(m_pDevice, m_pContext, L"../Bin/Data/Effect/E_P_CommonBox.json"))))
		return E_FAIL;

	/* For.Prototype_GameObject_P_Meditation */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_P_Meditation"), CE_P_Meditation_Spot::Create(m_pDevice, m_pContext, L"../Bin/Data/Effect/E_P_Meditation_Spot.json"))))
		return E_FAIL;

	/* For.Prototype_GameObject_MapleLeaf */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MapleLeaf"), CE_P_Maple::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_BeechLeaf */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BeechLeaf"), CE_P_Beech::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_FireBrazier */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_FireBrazier"), CE_FireBrazier::Create(m_pDevice, m_pContext, L"../Bin/Data/Effect/E_ObjectFire.json"))))
		return E_FAIL;

 	/* For.Prototype_GameObject_EnvironmentDust */
 	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_EnvironmentDust"), CE_P_EnvironmentDust::Create(m_pDevice, m_pContext, L"../Bin/Data/Effect/Particle/E_P_Environment2.json"))))
 		return E_FAIL;

	/* For.Prototype_GameObject_P_Rain */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_P_Rain"), CE_P_Rain::Create(m_pDevice, m_pContext, L"../Bin/Data/Effect/Particle/E_P_Rain.json"))))
		return E_FAIL;

	/* For.Prototype_GameObject_P_DeadZoneDust */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_P_DeadZoneDust"), CE_P_DeadZoneDust::Create(m_pDevice, m_pContext, L"../Bin/Data/Effect/Particle/E_P_BossStage.json"))))
		return E_FAIL;

	/* For.Prototype_GameObject_ShamanHeadTrail */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ShamanHeadTrail"), CE_ShamanHeadTrail::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_ShamanWeaponBall */ 
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ShamanWeaponBall"), CE_ShamanWeaponBall::Create(m_pDevice, m_pContext, L"../Bin/Data/Effect/E_ShamanBall.json"))))
		return E_FAIL;

	/* For.Prototype_GameObject_ShamanElectric */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ShamanElectric"), CE_ShamanElectric::Create(m_pDevice, m_pContext, L"../Bin/Data/Effect/E_Electric.json"))))
		return E_FAIL;

	/* For.Prototype_GameObject_WarriorWeaponAcc */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_WarriorWeaponAcc"), CE_WarriorWeaponAcc::Create(m_pDevice, m_pContext, L"../Bin/Data/Effect/Particle/E_WarriorBody.json"))))
		return E_FAIL;

	/* For.Prototype_GameObject_WarriorBodyAcc */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_WarriorBodyAcc"), CE_P_WarriorBody::Create(m_pDevice, m_pContext, L"../Bin/Data/Effect/Particle/E_WarriorBody2.json"))))
		return E_FAIL;

	/* For.Prototype_GameObject_DeadZoneSmallPlace */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_DeadZoneSmallPlace"), CE_P_DeadZone_SmallPlace::Create(m_pDevice, m_pContext, L"../Bin/Data/Effect/E_DeadZone_SmallRange.json"))))
		return E_FAIL;

	/* For.Prototype_GameObject_WarriorEyeTrail */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_WarriorEyeTrail"), CE_WarriorEyeTrail::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_KenaLevel_RiseY */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_KenaLevel_RiseY"), CE_P_Level_RiseY::Create(m_pDevice, m_pContext, L"../Bin/Data/Effect/Particle/E_KenaLevelUp2.json"))))
		return E_FAIL;

	/* For.Prototype_GameObject_KenaLevel_Floor */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_KenaLevel_Floor"), CE_P_Level_Floor::Create(m_pDevice, m_pContext, L"../Bin/Data/Effect/Particle/E_KenaLevelUp3.json"))))
		return E_FAIL;

#pragma endregion MapObject

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
	lstrcpy(m_szLoadingText, TEXT("Loading HW..."));

	CGameInstance *pGameInstance = CGameInstance::GetInstance();

	if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, TEXT("Prototype_Component_Texture_Terrain"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Terrain_Texture/ForestGround/Test_%d.png"), 7))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Brush*/
	if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, TEXT("Prototype_Component_Texture_Normal"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Terrain_Texture/Flter_Texture_%d.png"), 2))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Filter*/
	if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, TEXT("Prototype_Component_Texture_TerrainDiffuse"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Terrain_Texture/Filter/T_GDC_Grass01_D_NoisyAlpha.png")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Filter*/
	if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, TEXT("Prototype_Component_Texture_TerrainNormal"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Terrain_Texture/Filter/T_Ground_Grass_N.png")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Filter*/
	if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, TEXT("Prototype_Component_Texture_Filter"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Terrain_Texture/Filter/Filter_%d.dds"), 3))))
		return E_FAIL;

	/* For.Prototype_Component_Terrain_Two_Filter*/
	if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, TEXT("Prototype_Component_Terrain_Two_Filter"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Terrain_Texture/Filter/Terrain2_Filter_%d.dds"), 3))))
		return E_FAIL;

	/* For.Prototype_Component_Terrain_Three_Filter */
	if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, TEXT("Prototype_Component_Terrain_Three_Filter"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Terrain_Texture/Filter/Terrain3_Filter_%d.dds"), 3))))
		return E_FAIL;

	/* For.Prototype_Component_Terrain_Four_Filter */
	if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, TEXT("Prototype_Component_Terrain_Four_Filter"),		
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Terrain_Texture/Filter/Terrain4_Filter_%d.dds"), 3))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, TEXT("Prototype_Component_Terrain_Five_Filter"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Terrain_Texture/Filter/Terrain5_Filter_%d.dds"), 3))))
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

	//// 임시	
	//if (FAILED(LoadNonAnimFolderModel(iLevelIndex, "VillageCart", true, true, true)))
	//	assert(!"VillageCart");
	//if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Terrain"),
	//	CTerrain::Create(m_pDevice, m_pContext))))
	//	return E_FAIL;
	//if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_GroundMark"),
	//	CGroundMark::Create(m_pDevice, m_pContext))))
	//	return E_FAIL;
	//if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_HatCart"),
	//	CHatCart::Create(m_pDevice, m_pContext))))
	//	return E_FAIL;
	//return S_OK; 
	
	_matrix			PivotMatrix = XMMatrixIdentity();
	/* For.Prototype_Component_Model_DeadZoneTree */
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, L"Prototype_Component_Model_DeadZoneTree",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/DeadZoneTree_Anim/DeadzoneTree.model"), PivotMatrix))))
		return E_FAIL;
	
	/* For.Prototype_Component_Model_PulsePlateAnim*/
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, L"Prototype_Component_Model_PulsePlateAnim",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/PulsePlate_Anim/PulsePlate_Anim.model"), PivotMatrix))))
		return E_FAIL;
	

	/* For.Prototype_Component_Model_FieldBeaconAnim*/
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, L"Prototype_Component_Model_FieldBeaconAnim",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/FieldBeacon_Anim/FieldBeacon.mdat"), PivotMatrix))))
		return E_FAIL;

	PivotMatrix = XMMatrixScaling(0.03f, 0.03f, 0.03f);
	if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, L"Prototype_Component_Model_Pet",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/Pet/Pet.model"), PivotMatrix))))
		return E_FAIL;

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, L"Prototype_Component_Model_Frog",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/Frog/Frog.model"), PivotMatrix))))
		return E_FAIL;

	PivotMatrix = XMMatrixScaling(0.004f, 0.004f, 0.004f);
	if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, L"Prototype_Component_Model_Dy_RockSmall03",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/NonAnim/Rock/Rock_Small/Rock_Small_03.mdat"),
			PivotMatrix, nullptr, false, false, "../Bin/Resources/NonAnim/Rock/Rock_Small/Rock_Small_03.json"))))
		return E_FAIL;

	PivotMatrix = XMMatrixScaling(0.004f, 0.004f, 0.004f);
	if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, L"Prototype_Component_Model_Dy_RockSmall04",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/NonAnim/Rock/Rock_Small/Rock_Small_04.mdat"),
			PivotMatrix, nullptr, false, false, "../Bin/Resources/NonAnim/Rock/Rock_Small/Rock_Small_04.json"))))
		return E_FAIL;

	PivotMatrix = XMMatrixScaling(0.004f, 0.004f, 0.004f);
	if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, L"Prototype_Component_Model_Dy_RockSmall05",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/NonAnim/Rock/Rock_Small/Rock_Small_05.mdat"),
			PivotMatrix, nullptr, false, false, "../Bin/Resources/NonAnim/Rock/Rock_Small/Rock_Small_05.json"))))
		return E_FAIL;

	PivotMatrix = XMMatrixScaling(0.004f, 0.004f, 0.004f);
	if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, L"Prototype_Component_Model_Dy_RockSmall06",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/NonAnim/Rock/Rock_Small/Rock_Small_06.mdat"),
			PivotMatrix, nullptr, false, false, "../Bin/Resources/NonAnim/Rock/Rock_Small/Rock_Small_06.json"))))
		return E_FAIL;


	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, L"Prototype_Component_Model_Dy_MaskShrine_Rock_09",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/NonAnim/Map4/HouseKit/Rock/M4_MaskShrine_Rock_09.mdat"),
			PivotMatrix, nullptr, false, false, "../Bin/Resources/NonAnim/Map4/HouseKit/Rock/M4_MaskShrine_Rock_09.json"))))
		return E_FAIL;


	PivotMatrix = XMMatrixScaling(0.002f, 0.002f, 0.002f);
	if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, L"Prototype_Component_Model_Dy_RockMedium04",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/NonAnim/Rock/Rock_Medium/Rock_Medium_04.mdat"),
			PivotMatrix, nullptr, false, false, "../Bin/Resources/NonAnim/Rock/Rock_Medium/Rock_Medium_04.json"))))
		return E_FAIL;


	PivotMatrix = XMMatrixScaling(0.002f, 0.002f, 0.002f);
	if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, L"Prototype_Component_Model_Dy_RockMedium06",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/NonAnim/Rock/Rock_Medium/Rock_Medium_06.mdat"),
			PivotMatrix, nullptr, false, false, "../Bin/Resources/NonAnim/Rock/Rock_Medium/Rock_Medium_06.json"))))
		return E_FAIL;

	PivotMatrix = XMMatrixScaling(0.002f, 0.002f, 0.002f);
	if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, L"Prototype_Component_Model_Dy_RockMedium07",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/NonAnim/Rock/Rock_Medium/Rock_Medium_07.mdat"),
			PivotMatrix, nullptr, false, false, "../Bin/Resources/NonAnim/Rock/Rock_Medium/Rock_Medium_07.json"))))
		return E_FAIL;


#pragma region Map_Four
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationX(XMConvertToRadians(-90.f));
	if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, L"Prototype_Component_Model_Sakura_Flower",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/NonAnim/Sakura/Sakura_Flower.mdat"), PivotMatrix, nullptr, false, true, "../Bin/Resources/NonAnim/Sakura/Sakura_Flower.json", false))))
		return E_FAIL;

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, L"Prototype_Component_Model_CherryBlossomTreeAlive",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/NonAnim/Sakura/CherryBlossomTreeAlive.mdat"), PivotMatrix, nullptr, false, true, "../Bin/Resources/NonAnim/Sakura/CherryBlossomTreeAlive.json", false))))
		return E_FAIL;

	if (FAILED(LoadNonAnimFolderModel(iLevelIndex, "WoodlandHouse_1", true, true, true)))
		assert(!"WoodlandHouse_1");
	if (FAILED(LoadNonAnimFolderModel(iLevelIndex, "WoodlandHouse_2", true, true, true)))
		assert(!"WoodlandHouse_2");
	if (FAILED(LoadNonAnimFolderModel(iLevelIndex, "Map4/Bell", true, true, true)))
		assert(!"Map4/Bell");
	if (FAILED(LoadNonAnimFolderModel(iLevelIndex, "Map4/FarmEntranceStructure/Beam", true, true, true)))
		assert(!"Map4/FarmEntranceStructure/Beam");
	if (FAILED(LoadNonAnimFolderModel(iLevelIndex, "Map4/FarmEntranceStructure/Pillar", true, true, true)))
		assert(!"Map4/FarmEntranceStructure/Pillar");
	if (FAILED(LoadNonAnimFolderModel(iLevelIndex, "Map4/FarmEntranceStructure/Root", true, true, true)))
		assert(!"Map4/FarmEntranceStructure/Root");
	if (FAILED(LoadNonAnimFolderModel(iLevelIndex, "Map4/FarmEntranceStructure/Structure", true, true, true)))
		assert(!"Map4/FarmEntranceStructure/Structure");
	if (FAILED(LoadNonAnimFolderModel(iLevelIndex, "Map4/FarmEntranceStructure/Tree_Rock", true, true, true)))
		assert(!"Map4/FarmEntranceStructure/Tree_Rock");
	if (FAILED(LoadNonAnimFolderModel(iLevelIndex, "Map4/Forge", true, true, true)))
		assert(!"Map4/Forge");
	if (FAILED(LoadNonAnimFolderModel(iLevelIndex, "Map4/HouseKit/BirdHouse", true, true, true)))
		assert(!"Map4/HouseKit/BirdHouse");
	if (FAILED(LoadNonAnimFolderModel(iLevelIndex, "Map4/HouseKit/MainGate", true, true, true)))
		assert(!"Map4/HouseKit/MainGate");
	if (FAILED(LoadNonAnimFolderModel(iLevelIndex, "Map4/HouseKit/MainHouse", true, true, true)))
		assert(!"Map4/HouseKit/MainHouse");
	if (FAILED(LoadNonAnimFolderModel(iLevelIndex, "Map4/HouseKit/Rock", true, true, true)))
		assert(!"Map4/HouseKit/Rock");
	if (FAILED(LoadNonAnimFolderModel(iLevelIndex, "Map4/LightHouse/Arena", true, true, true)))
		assert(!"Map4/LightHouse/Arena");
	if (FAILED(LoadNonAnimFolderModel(iLevelIndex, "Map4/LightHouse/BigRoots", true, true, true)))
		assert(!"Map4/HouseKit/BigRoots");
	if (FAILED(LoadNonAnimFolderModel(iLevelIndex, "Map4/LightHouse/LightHouse", true, true, true, false, true)))
		assert(!"Map4/LightHouse/LightHouse");
	if (FAILED(LoadNonAnimFolderModel(iLevelIndex, "Map4/LightHouse/Roots", true, true, true)))
		assert(!"Map4/LightHouse/Roots");
	if (FAILED(LoadNonAnimFolderModel(iLevelIndex, "Map4/LightHouse/Structure", true, true, true)))
		assert(!"Map4/LightHouse/Structure");
	if (FAILED(LoadNonAnimFolderModel(iLevelIndex, "Map4/RusuHut", true, true, true)))
		assert(!"Map4/RusuHut");
	if (FAILED(LoadNonAnimFolderModel(iLevelIndex, "Map4/SummoningRoom", true, true, true, false, true)))
		assert(!"Map4/SummoningRoom");
	if (FAILED(LoadNonAnimFolderModel(iLevelIndex, "VillageCart", true, false, true)))
		assert(!"VillageCart");

#pragma endregion Map_Four

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
	
	if (FAILED(LoadNonAnimFolderModel(iLevelIndex, "Rock/Rock_Rubble", true, true, true,false,true)))
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
	
	if (FAILED(LoadNonAnimFolderModel(iLevelIndex, "Cliff/Cliff_Sheer", true, true, true,false,true)))
		assert(!"Issue");
	
	if (FAILED(LoadNonAnimFolderModel(iLevelIndex, "RuinKit/RuinKit_Rubble", true, true, true,false,true)))
		return E_FAIL;
	
	if (FAILED(LoadNonAnimFolderModel(iLevelIndex, "RuinKit/RuinStaris", true, true, true,false,true)))
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
	
	if (FAILED(LoadNonAnimFolderModel(iLevelIndex, "RuinKit/RuinsKit_Wall_Broken", true, true, true,false,true)))
		return E_FAIL;
	
	if (FAILED(LoadNonAnimFolderModel(iLevelIndex, "RuinKit/RuinsWall", true, true, true, false, true)))
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
	
	if (FAILED(LoadNonAnimFolderModel(iLevelIndex, "Stone/StoneFloor_1", true, true, true,false,true)))
		return E_FAIL;
	
	if (FAILED(LoadNonAnimFolderModel(iLevelIndex, "Stone/StoneFloor_2", true, true, true,false,true)))
		return E_FAIL;
	
	if (FAILED(LoadNonAnimFolderModel(iLevelIndex, "Stone/Stone_Bridge", true, true, true, false, true)))
		assert(!"Issue");
	
	if (FAILED(LoadNonAnimFolderModel(iLevelIndex, "Stone/Stone_Stairs", true, true, true,false,true)))
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

	if (FAILED(LoadNonAnimFolderModel(iLevelIndex, "MeditationStump", true, false, true, false, true)))
		assert(!"Issue");


	/*화로 */
	if (FAILED(LoadNonAnimFolderModel(iLevelIndex, "Fire_Brazier", false, false, true)))
		assert(!"Issue");
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Fire_Brazier"),
		CFire_Brazier::Create(m_pDevice, m_pContext))))
		return E_FAIL;



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
		/* Prototype_Component_Model_RuinsKit_BombPlatForm */
		PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(90.f));
		FAILED_CHECK_RETURN(CGameInstance::GetInstance()->Add_Prototype(iLevelIndex, L"Prototype_Component_Model_RuinsKit_BombPlatForm",
			CModel::Create(m_pDevice, m_pContext, L"../Bin/Resources/NonAnim/RuinPlatform/RuinPlatform05.mdat", PivotMatrix, nullptr, false, false, "../Bin/Resources/NonAnim/RuinPlatform/RuinPlatform05.json", false)), E_FAIL);
		/* Prototype_GameObject_BombPlatform */
		FAILED_CHECK_RETURN(CGameInstance::GetInstance()->Add_Prototype(L"Prototype_GameObject_RuinsKit_BombPlatForm", CBombPlatform::Create(m_pDevice, m_pContext)), E_FAIL);

#pragma  endregion Born_GroundCover

#pragma region Boss_Attack DissolveTree

		PivotMatrix = XMMatrixScaling(0.002f, 0.003f, 0.002f);
		if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, L"Prototype_Component_Model_BossDissolveGodTree",
			CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/NonAnim/Trees/Giant/Giant_GodTree.mdat"),
				PivotMatrix, nullptr, false, false, "../Bin/Resources/NonAnim/Trees/Giant/Giant_GodTree.json", false, false))))
			return E_FAIL;

		PivotMatrix = XMMatrixScaling(0.002f, 0.003f, 0.002f);
		if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, L"Prototype_Component_Model_BossDissolveGodTree02",
			CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/NonAnim/Trees/Giant/Giant_GodTree02.mdat"),
				PivotMatrix, nullptr, false, false, "../Bin/Resources/NonAnim/Trees/Giant/Giant_GodTree02.json", false, false))))
			return E_FAIL;

		PivotMatrix = XMMatrixScaling(0.002f, 0.003f, 0.002f);
		if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, L"Prototype_Component_Model_BossDissolveGodTree03",
			CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/NonAnim/Trees/Giant/Giant_GodTree.mdat"),
				PivotMatrix, nullptr, false, false, "../Bin/Resources/NonAnim/Trees/Giant/Giant_GodTree03.json", false, false))))
			return E_FAIL;

		// GodRock
		PivotMatrix = XMMatrixScaling(0.002f, 0.002f, 0.002f);
		if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, L"Prototype_Component_Model_BossDissolveGod_Rock01",
			CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/NonAnim/Rock/God_Rock/GodRock_01.mdat"),
				PivotMatrix, nullptr, false, false, "../Bin/Resources/NonAnim/Rock/God_Rock/GodRock_01.json", false, false))))
			return E_FAIL;

		PivotMatrix = XMMatrixScaling(0.002f, 0.002f, 0.002f);
		if (FAILED(pGameInstance->Add_Prototype(iLevelIndex, L"Prototype_Component_Model_BossDissolveGod_Rock02",
			CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/NonAnim/Rock/God_Rock/GodRock_02.mdat"),
				PivotMatrix, nullptr, false, false, "../Bin/Resources/NonAnim/Rock/God_Rock/GodRock_02.json", false, false))))
			return E_FAIL;

#pragma  endregion

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
	
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_FloorMesh"),
		CFloorMesh::Create(m_pDevice, m_pContext))))
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

	/* For.Prototype_GameObject_Meditation_Spot */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Meditation_Spot"),
		CMeditation_Spot::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_SceneChangePortal */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_SceneChangePortal"),
		CSceneChangePortal::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_GimmickObj */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_GimmickObj"),
		CGimmick_EnviObj::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	/*Anim*/
	/* For.Prototype_GameObject_Door_Anim */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_DoorAnim"),
		CDoor_Anim::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	/* For.Prototype_GameObject_FieldBecon_Anim */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_FieldBecon_Anim"),
		CFieldBecon_Anim::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_DZ_FallenTree_Anim"),
		CDZ_FallenTree_Anim::Create(m_pDevice, m_pContext))))
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

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_HatCart"),
		CHatCart::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Sakura */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Sakura"),
		CE_P_Sakura::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Dynamic_Stone */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Dynamic_Stone"),
		CDynamic_Stone::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Dynamic_Stone */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Dynamic_StoneCube"),
		CDynamic_StoneCube::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_DeadZoneBossTree */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_DeadZoneBossTree"),
		CDeadZoneBossTree::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Pet"), CPet::Create(m_pDevice, m_pContext)))) return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Frog"), CFrog::Create(m_pDevice, m_pContext)))) return E_FAIL;

#pragma endregion HHW_OBJ

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