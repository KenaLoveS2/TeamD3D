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

/* NPCs */
#include "Rot.h"

/* Enemies*/
#include "Moth.h"
#include "RockGolem.h"
#include "RotEater.h"
#include "Sticks01.h"
#include "VillageGuard.h"
#include "WoodKnight.h"
#include "BranchTosser.h"
#include "BranchTosser_Weapon.h"
#include "ShieldStick.h"
#include "ShieldStick_Weapon.h"
#include "Sapling.h"
#include "Mage.h"

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


#include "Pulse_Plate_Anim.h"
#include "Door_Anim.h"
#include "GroundMark.h"
#include "Rope_RotRock.h"
#include "LiftRot.h"
#include "LiftRot_Master.h"

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
//Charge
#include "E_KenaCharge.h"
#include "E_KenaChargeImpact.h"
//Monster
#include "EnemyWisp.h"

//Arrow
#include "SpiritArrow.h"

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
/* ~Effects */

/* Components*/
#include "ControlMove.h"
#include "Interaction_Com.h"
#include "Camera_Player.h"

/*Test Objects*/
#include "LodObject.h"
#include "ModelViewerObject.h"
#include "Moth.h"
#include "RotForMonster.h"

unsigned int	g_LEVEL = 0;

#include "Json/json.hpp"
#include <fstream>
#include "E_KenaDust.h"


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
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	lstrcpy(m_szLoadingText, TEXT("Loading Texture..."));
	/* For.Prototype_Component_Texture_Terrain */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Terrain"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Tile%d.dds"), 2))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Normal*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Texture_Normal"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Terrain_Texture/Flter_Texture_%d.png"), 2))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_GroundMark*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TESTPLAY, TEXT("Prototype_Component_Texture_GroundMark"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/GroundMark/GroundMark%d.png"), 3))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Filter */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Filter"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Filter.dds"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Sky */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Sky"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/SkyBox/Sky_%d.dds"), 5))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Explosion */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Explosion"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Explosion/Explosion%d.png"), 90))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("Loading VIBuffer..."));
	/* For.Prototype_Component_VIBuffer_Terrain */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Terrain"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Terrain_Texture/Test_0.bmp")))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Cube */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Cube"),
		CVIBuffer_Cube::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Rect_Instancing */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Rect_Instancing"),
		CVIBuffer_Rect_Instancing::Create(m_pDevice, m_pContext, 30))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("Loading Model..."));

	_matrix			PivotMatrix = XMMatrixIdentity();

	/* For.Prototype_Component_Model_Kena */
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, L"Prototype_Component_Model_Kena",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/Kena/Kena_Body.model"), PivotMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_RockGolem */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, L"Prototype_Component_Model_RockGolem",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/Enemy/RockGolem/RockGolem.model"), PivotMatrix))))
		return E_FAIL;

	/* Test Model */
	/*if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, L"Prototype_Component_Model_Kena",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/Kena/Kena_Body_forTest.mdat"), PivotMatrix))))
		return E_FAIL;*/

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, L"Prototype_Component_Model_Kena_Staff",
		CModel::Create(m_pDevice, m_pContext, L"../Bin/Resources/Anim/Kena/Staff/Kena_Staff.model", PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, L"Prototype_Component_Model_Kena_MainOutfit",
		CModel::Create(m_pDevice, m_pContext, L"../Bin/Resources/Anim/Kena/Outfit/MainOutfit/Kena_MainOutfit.model", PivotMatrix))))
		return E_FAIL;

	if (FAILED(LoadNonAnimFolderModel(LEVEL_GAMEPLAY, "Spirit_Arrow", false, false, false)))
		return E_FAIL;

	if (FAILED(LoadNonAnimModel(LEVEL_GAMEPLAY)))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("Loading Prototype GameObject..."));

	/* For.Prototype_GameObject_Player_Camera */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera_Player"),
		CCamera_Player::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Terrain */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Terrain"),
		CTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Player */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Kena"),
		CKena::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Kena_Staff"),
		CKena_Staff::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Kena_MainOutfit"),
		CKena_MainOutfit::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_SpiritArrow"),
		CSpiritArrow::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Sky */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Sky"),
		CSky::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	///* For.Prototype_GameObject_Effect_Rect_Instancing */
	//if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_Rect_Instancing"),
	//	CEffect_Rect_Instancing::Create(m_pDevice, m_pContext))))
	//	return E_FAIL;

	///* For.Prototype_GameObject_Effect_Point_Instancing */
	//if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_Point_Instancing"),
	//	CEffect_Point_Instancing::Create(m_pDevice, m_pContext))))
	//	return E_FAIL;

	///* For.Prototype_GameObject_Effect */
	//if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect"),
	//	CEffect::Create(m_pDevice, m_pContext))))
	//	return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_RockGolem"),
		CRockGolem::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#pragma region EFFECT
	lstrcpy(m_szLoadingText, TEXT("Loading Texture..."));
	/* For.Prototype_Component_Texture_Effect */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/DiffuseTexture/E_Effect_%d.png"), 108))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_NormalEffect */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_NormalEffect"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/NormalTexture/N_Effect_%d.png"), 11))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_PulseShield_Dissolve */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_PulseShield_Dissolve"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/PulseShield_Dissolve/E_Effect_%d.png"), 4))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_TrailFlow */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_TrailFlow"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Trail/flow/E_Flow_%d.png"), 10))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_TrailType */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_TrailType"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Trail/shapetype/E_Type_%d.png"), 11))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("Loading Obejct..."));

	/* For.Prototype_Component_VIBuffer_Point_Instancing */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Trail"),
		CVIBuffer_Trail::Create(m_pDevice, m_pContext, 300))))
		return E_FAIL;

#pragma region Model Component
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	/* For.Prototype_Component_Model_Cube */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, L"Prototype_Component_Model_Cube",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Effect/Cube.mdat"), PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Cone */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, L"Prototype_Component_Model_Cone",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Effect/Cone.mdat"), PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Cylinder */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, L"Prototype_Component_Model_Cylinder",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Effect/Cylinder.mdat"), PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Plane */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, L"Prototype_Component_Model_Plane",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Effect/Plane.mdat"), PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Sphere */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, L"Prototype_Component_Model_Sphere",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Effect/Sphere.mdat"), PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_shockball */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, L"Prototype_Component_Model_shockball",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Effect/shockball.mdat"), PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_shockball3 */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, L"Prototype_Component_Model_shockball3",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Effect/shockball3.mdat"), PivotMatrix))))
		return E_FAIL;
#pragma endregion Model Component

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
		CE_KenaHit::Create(m_pDevice, m_pContext, L"../Bin/Data/Effect/E_HitSet.json"))))
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

#pragma endregion EFFECT
	/* For.Prototype_Component_Model_EnemyWisp */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, L"Prototype_Component_Model_EnemyWisp",
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

	lstrcpy(m_szLoadingText, TEXT("Loading Complete!!"));

	m_isFinished = true;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLoader::Loading_ForMapTool()
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	lstrcpy(m_szLoadingText, TEXT("Loading Texture..."));
	///* For.Prototype_Component_Texture_Terrain */
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

	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "RuinPlatform", true, true, true)))
		assert(!"Issue");
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "PowerCrystal", true, false, true)))
		assert(!"Issue");

	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "RuinKit/RuinsKit_BridgeLarge", true, true, true)))
		assert(!"Issue");
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "RuinKit/RuinsKit_BridgeShort", true, true, true)))
		assert(!"Issue");
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "RuinKit/RuinKit_LedgeStone", true, true, true)))
		return E_FAIL;
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Cliff/Cliff_Ledge", true, true, true)))
		assert(!"Issue");

#pragma  endregion ANIM_OBJ

#ifdef FOR_MAP_GIMMICK

#pragma region KENA
	/* For.Prototype_Component_Model_Kena */
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAPTOOL, L"Prototype_Component_Model_Kena",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/Kena/Kena_Body.model"), PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAPTOOL, L"Prototype_Component_Model_Kena_Staff",
		CModel::Create(m_pDevice, m_pContext, L"../Bin/Resources/Anim/Kena/Staff/Kena_Staff.model", PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAPTOOL, L"Prototype_Component_Model_Kena_MainOutfit",
		CModel::Create(m_pDevice, m_pContext, L"../Bin/Resources/Anim/Kena/Outfit/MainOutfit/Kena_MainOutfit.model", PivotMatrix))))
		return E_FAIL;


	/* For.Prototype_GameObject_Player */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Kena"),
		CKena::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	m_fCur += 1.f;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Kena_Staff"),
		CKena_Staff::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	m_fCur += 1.f;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Kena_MainOutfit"),
		CKena_MainOutfit::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	m_fCur += 1.f;

	/* For.Prototype_GameObject_Player_Camera */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera_Player"),
		CCamera_Player::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	m_fCur += 1.f;
#pragma endregion  KENA

#pragma region EFFECT
	/* For.Prototype_Component_Texture_Effect */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Texture_Effect"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/DiffuseTexture/E_Effect_%d.png"), 108))))
		return E_FAIL;
	m_fCur += 1.f;

	/* For.Prototype_Component_Texture_NormalEffect */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Texture_NormalEffect"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/NormalTexture/N_Effect_%d.png"), 11))))
		return E_FAIL;
	m_fCur += 1.f;

	/* For.Prototype_Component_Texture_PulseShield_Dissolve */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Texture_PulseShield_Dissolve"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/PulseShield_Dissolve/E_Effect_%d.png"), 4))))
		return E_FAIL;
	m_fCur += 1.f;

	/* For.Prototype_Component_Texture_TrailFlow */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Texture_TrailFlow"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Trail/flow/E_Flow_%d.png"), 10))))
		return E_FAIL;
	m_fCur += 1.f;

	/* For.Prototype_Component_Texture_TrailType */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Texture_TrailType"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Trail/shapetype/E_Type_%d.png"), 11))))
		return E_FAIL;
	m_fCur += 1.f;

	/* For.Prototype_Component_VIBuffer_Point_Instancing */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_VIBuffer_Trail"),
		CVIBuffer_Trail::Create(m_pDevice, m_pContext, 300))))
		return E_FAIL;
	m_fCur += 1.f;

#pragma region Model Component
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	/* For.Prototype_Component_Model_Cube */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAPTOOL, L"Prototype_Component_Model_Cube",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Effect/Cube.mdat"), PivotMatrix))))
		return E_FAIL;
	m_fCur += 1.f;

	/* For.Prototype_Component_Model_Cone */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAPTOOL, L"Prototype_Component_Model_Cone",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Effect/Cone.mdat"), PivotMatrix))))
		return E_FAIL;
	m_fCur += 1.f;

	/* For.Prototype_Component_Model_Cylinder */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAPTOOL, L"Prototype_Component_Model_Cylinder",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Effect/Cylinder.mdat"), PivotMatrix))))
		return E_FAIL;
	m_fCur += 1.f;

	/* For.Prototype_Component_Model_Plane */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAPTOOL, L"Prototype_Component_Model_Plane",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Effect/Plane.mdat"), PivotMatrix))))
		return E_FAIL;
	m_fCur += 1.f;

	/* For.Prototype_Component_Model_Sphere */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAPTOOL, L"Prototype_Component_Model_Sphere",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Effect/Sphere.mdat"), PivotMatrix))))
		return E_FAIL;
	m_fCur += 1.f;

	/* For.Prototype_Component_Model_shockball */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAPTOOL, L"Prototype_Component_Model_shockball",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Effect/shockball.mdat"), PivotMatrix))))
		return E_FAIL;
	m_fCur += 1.f;

	/* For.Prototype_Component_Model_shockball3 */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAPTOOL, L"Prototype_Component_Model_shockball3",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Effect/shockball3.mdat"), PivotMatrix))))
		return E_FAIL;
	m_fCur += 1.f;
#pragma endregion Model Component

	/* For.Prototype_GameObject_KenaPulse */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_KenaPulse"),
		CE_KenaPulse::Create(m_pDevice, m_pContext, L"../Bin/Data/Effect/E_KenaPulse.json"))))
		return E_FAIL;
	m_fCur += 1.f;

	/* For.Prototype_GameObject_KenaPulseCloud */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_KenaPulseCloud"),
		CE_KenaPulseCloud::Create(m_pDevice, m_pContext, L"../Bin/Data/Effect/E_KenaPulseCloude.json"))))
		return E_FAIL;
	m_fCur += 1.f;

	/* For.Prototype_GameObject_KenaPulseDot */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_KenaPulseDot"),
		CE_KenaPulseDot::Create(m_pDevice, m_pContext, L"../Bin/Data/Effect/E_KenaPulseDot.json"))))
		return E_FAIL;
	m_fCur += 1.f;

	/* For.Prototype_GameObject_KenaCharge */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_KenaCharge"),
		CE_KenaCharge::Create(m_pDevice, m_pContext, L"../Bin/Data/Effect/E_Charge_Set.json"))))
		return E_FAIL;
	m_fCur += 1.f;

	/* For.Prototype_GameObject_KenaChargeImpact */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_KenaChargeImpact"),
		CE_KenaChargeImpact::Create(m_pDevice, m_pContext, L"../Bin/Data/Effect/E_Charge_Impact.json"))))
		return E_FAIL;
	m_fCur += 1.f;

	/* For.Prototype_GameObject_KenaDamage */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_KenaDamage"),
		CE_KenaDamage::Create(m_pDevice, m_pContext, L"../Bin/Data/Effect/E_Damage_Set.json"))))
		return E_FAIL;
	m_fCur += 1.f;

	/* For.Prototype_GameObject_KenaHit */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_KenaHit"),
		CE_KenaHit::Create(m_pDevice, m_pContext, L"../Bin/Data/Effect/E_HitSet.json"))))
		return E_FAIL;
	m_fCur += 1.f;

	/* For.Prototype_GameObject_KenaJump */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_KenaJump"),
		CE_KenaJump::Create(m_pDevice, m_pContext, L"../Bin/Data/Effect/E_JumpSet.json"))))
		return E_FAIL;
	m_fCur += 1.f;

	/* For.Prototype_GameObject_PulseObject */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_PulseObject"),
		CE_PulseObject::Create(m_pDevice, m_pContext, L"../Bin/Data/Effect/E_PulseObject.json"))))
		return E_FAIL;
	m_fCur += 1.f;

	/* For.Prototype_GameObject_KenaStaffTrail */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_KenaStaffTrail"),
		CE_KenaTrail::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	m_fCur += 1.f;
#pragma endregion EFFECT
#endif 


#ifdef FOR_MAPTOOL
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
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Cliff/Cliff_Ledge", true, true, true)))
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
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Rock/God_Rock", true, true, true)))
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
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Rot/Rot_Rock", true, true)))// json NonUse
		assert(!"Issue");
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Rot/RotCarry_Piece", true, true)))// json NonUse
		assert(!"Issue");
#pragma endregion ~Rot_Rock


#pragma region RuinPlatform
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "RuinPlatform", true, true, true)))
		assert(!"Issue");
#pragma endregion ~RuinPlatform

#pragma region Save_StoneTree
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Save_StoneTree", true, true)))// json NonUse
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
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "RuinKit/RuinsKit_FloorTile", true, true, true)))
		assert(!"Issue");
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "RuinKit/RuinsKit_Wall_Broken", true, true, true)))
		assert(!"Issue");
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "RuinKit/RuinsWall", true, true, true)))
		assert(!"Issue");
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "RuinKit/Ruinskit_Collum", true, true, true)))
		assert(!"Issue");
#pragma endregion RuinKit

#pragma region Interactive	얘네는 무조건 인스턴싱 안함
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

#pragma  region Start_Forest_Room
	//_matrix			PivotMatrix = XMMatrixIdentity();
	///* For.Prototype_Component_Model_DeadZoneTree */
	//PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAPTOOL, L"Prototype_Component_Model_DeadZoneTree",
	//	CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/DeadZoneTree_Anim/DeadzoneTree.mdat"), PivotMatrix))))
	//		return E_FAIL;

	//if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "PowerCrystal", true, false, true)))
	//	assert(!"Issue");
	//if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "PulseObjects/PluseStone_Big", true, false, true)))
	//	return E_FAIL;
	//if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "PulseObjects/PulsePlate_StoneBorder", true, false, true)))
	//	return E_FAIL;
	//if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "PulseObjects/PulsePlateEnclosure_02", true, false, true)))
	//	return E_FAIL;
	//if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "PulseObjects/PulseStoneRock", true, false, true)))
	//	return E_FAIL;
	//if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "RuinKit/RuinKit_LedgeStone", true, true, true)))
	//	return E_FAIL;
	//if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "RuinKit/RuinKit_Pillar", true, true, true)))
	//	return E_FAIL;
	//if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "RuinKit/RuinsKit_Brick", true, true, true)))
	//	return E_FAIL;
	//if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "RuinKit/RuinsKit_BridgeLarge", true, true, true)))
	//	return E_FAIL;
	//if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "RuinKit/RuinsKit_ToriGate", true, true, true)))
	//	return E_FAIL;
	//if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Rock/Rock_Arch", true, true, true)))
	//	assert(!"Issue");
	//if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "RuinPlatform", true, true,true)))
	//	assert(!"Issue");
	//if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Rock/Rock_Rubble", true, true, true)))
	//	return E_FAIL;
	//if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Rock/Rock_Big", true, true, true)))
	//	assert(!"Issue");
	//if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Cliff/Cliff_Rock", true, true, true)))
	//	return E_FAIL;
	//if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Rock/God_Rock", true, true, true)))
	//	return E_FAIL;
	//if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Cliff/Cliff_Cap", true, true, true)))
	//	assert(!"Issue");
	//if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Cliff/Cliff_W2", true, true, true)))
	//	return E_FAIL;
	//if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Cliff/Cliff_W3", true, true, true)))
	//	assert(!"Issue");
	//if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Cliff/Cliff_Wall_Large", true, true, true)))
	//	return E_FAIL;
	//if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Cliff/Cliff_Wall_Med", true, true, true)))
	//	return E_FAIL;
	//if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Cliff/Cliff_Wall_Short", true, true, true)))
	//	return E_FAIL;
	//if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Cliff/Cliff_Glitter", true, true, true)))
	//	assert(!"Issue");
	//if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Cliff/Cliff_Sheer", true, true, true)))
	//	assert(!"Issue");
	//if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "RuinKit/RuinKit_Rubble", true, true, true)))
	//	return E_FAIL;
	//if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "RuinKit/RuinStaris", true, true, true)))
	//	return E_FAIL;
	//if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Statue/Owl_WrapShrine", false, true, true)))
	//	return E_FAIL;
	//if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Statue", true, true, true)))
	//	return E_FAIL;
	//if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "RuinKit/RuinKit_Arch_Gate", true, true, true)))
	//	return E_FAIL;
	//if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "RuinKit/RuinsKit_Tree", true, true, true)))
	//	return E_FAIL;
	//if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "RuinKit/RuinsKit_FloorTile", true, true, true)))
	//	return E_FAIL;
	//if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "RuinKit/RuinKit_Piece", true, true, true)))
	//	return E_FAIL;
	//if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "RuinKit/RuinsKit_Wall_Broken", true, true, true)))
	//	return E_FAIL;
	//if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "RuinKit/RuinsWall", true, true, true)))
	//	assert(!"Issue");
	//if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "RuinKit/Ruinskit_Collum", true, true, true)))
	//	assert(!"Issue");
	//if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Cliff/Cliff_Ledge", true, true, true)))
	//	return E_FAIL;
	//if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Rock/Rock_Small", true, true, true)))
	//	return E_FAIL;
	//if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Rock/Rock_Medium", true, true, true)))
	//	return E_FAIL;
	//if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Forest_1/RotGod_Statue_crumbled", true, false, true)))
	//	return E_FAIL;
	//if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Forest_1/Start_Gate", true, true, true)))
	//	return E_FAIL;
	//if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Forest_1/FirstTear_FallenTree", true, false, true)))
	//	return E_FAIL;
	//if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Forest_1/First_Room_Deco", true, true, true)))
	//	return E_FAIL;
	//if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Forest_1/ShrineOfDeath_MainRock", true, true, true)))
	//	return E_FAIL;
	//if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Forest_1/ShrineOfLife", true, true, true)))
	//	return E_FAIL;
	//if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Stone/StoneFloor_1", true, true, true)))
	//	return E_FAIL;
	//if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Stone/StoneFloor_2", true, true, true)))
	//	return E_FAIL;
	//if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Stone/Stone_Bridge", true, true, true)))
	//	assert(!"Issue");
	//if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Stone/Stone_Stairs", true, true, true)))
	//	assert(!"Issue");
	//if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Trees/Giant", true, true, true)))
	//	return E_FAIL;
	//if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Trees/Fallen", true, true, true)))
	//	return E_FAIL;
	//if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Trees/Dead", true, true, true)))
	//	return E_FAIL;
	//if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Trees/CeDarTree", true, true, true)))
	//	return E_FAIL;
	//if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Trees/Bare", true, true, true)))
	//	return E_FAIL;
	//if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Trees/Billboard", true, true, true)))
	//	return E_FAIL;
	//if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Trees/Branches/Canopy", true, true, true)))
	//	return E_FAIL;
	//if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Trees/Branches/GodTree", true, true, true)))
	//	return E_FAIL;
	//if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Trees/Branches/Tree_Branch", true, true, true)))
	//	return E_FAIL;
	//if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Trees/Canopy", true, true, true)))
	//	return E_FAIL;
	//if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Save_StoneTree", true, true)))// json NonUse
	//	assert(!"Issue");
	//if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "DeadZone/Deadzondes_GoopWeb", true, true, true)))
	//	assert(!"Issue");
	//if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "DeadZone/DeadZoneAndPurifiedTree", true, true, true)))
	//	assert(!"Issue");
	//if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "DeadZone/DeadzoneClumps", true, true, true)))
	//	assert(!"Issue");
	//if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "DeadZone/DeadzoneGroudCover", true, true, true)))
	//	assert(!"Issue");
	//if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "DeadZone/DeadzoneHeart", true, true, true)))
	//	assert(!"Issue");
	//if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "DeadZone/DeadzoneWall", true, true, true)))
	//	assert(!"Issue");
	//if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "DeadZone/Gate", true, true, true)))
	//	assert(!"Issue");
	//if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "DeadZone/Trees", true, true, true)))
	//	assert(!"Issue");
	//if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "DeadZone/BushDead_02", true, true)))
	//	assert(!"Issue");
	//if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Rot/Rot_Rock", true, true)))// json NonUse
	//	assert(!"Issue");
	//if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Rot/RotCarry_Piece", true, true)))// json NonUse
	//	assert(!"Issue");


#pragma endregion ~Start_Forest_Room

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
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_VIBuffer_Terrain"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Terrain_Texture/Height/Terrain_Height_3.bmp")))))
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
	/* ~Test*/
	/* For.Prototype_GameObject_Cave_Rock */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_CLodObject"),
		CLodObject::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_ModelViewerObject */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ModelViewerObject"),
		CModelViewerObject::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* ~Test*/

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



	/*Anim*/
	/* For.Prototype_GameObject_Door_Anim */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_DoorAnim"),
		CDoor_Anim::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Door_Anim */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_PulsePlateAnim"),
		CPulse_Plate_Anim::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_GroundMark */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_GroundMark"),
		CGroundMark::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	m_fCur += 1.f;





	lstrcpy(m_szLoadingText, TEXT("Loading End."));

	m_isFinished = true;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLoader::Loading_ForTestPlay()
{
	m_fMax = 183.f;
	m_fCur = m_fCur / m_fMax * 100.f;

	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	/* For.Prototype_Component_Texture_Sky */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TESTPLAY, TEXT("Prototype_Component_Texture_Sky"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/SkyBox/Sky_%d.dds"), 5))))
		return E_FAIL;
	m_fCur += 1.f;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TESTPLAY, TEXT("Prototype_Component_Texture_Terrain"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Terrain_Texture/ForestGround/Test_%d.png"), 7))))
		return E_FAIL;
	m_fCur += 1.f;
	/* For.Prototype_Component_Texture_Brush*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TESTPLAY, TEXT("Prototype_Component_Texture_Normal"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Terrain_Texture/Flter_Texture_%d.png"), 2))))
		return E_FAIL;
	m_fCur += 1.f;
	/* For.Prototype_Component_Texture_Filter */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TESTPLAY, TEXT("Prototype_Component_Texture_Filter"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Terrain_Texture/Filter/Filter_%d.dds"), 3))))
		return E_FAIL;
	m_fCur += 1.f;
	/* For.Prototype_Component_Texture_Filter */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TESTPLAY, TEXT("Prototype_Component_Terrain_Two_Filter"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Terrain_Texture/Filter/Terrain2_Filter_%d.dds"), 3))))
		return E_FAIL;
	m_fCur += 1.f;
	/* For.Prototype_Component_Texture_Filter */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TESTPLAY, TEXT("Prototype_Component_Terrain_HeightMaps"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Terrain_Texture/Height/Terrain_Height_%d.bmp"), 15))))
		return E_FAIL;
	m_fCur += 1.f;
	/* For.Prototype_Component_Texture_GroundMark*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TESTPLAY, TEXT("Prototype_Component_Texture_GroundMark"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/GroundMark/GroundMark%d.png"), 3))))
		return E_FAIL;
	m_fCur += 1.f;

	/* For.Prototype_Component_VIBuffer_Cube */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TESTPLAY, TEXT("Prototype_Component_VIBuffer_Cube"),
		CVIBuffer_Cube::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	m_fCur += 1.f;

	/* For.Prototype_Component_VIBuffer_Terrain */ // TEXT("../Bin/Resources/Textures/Terrain/Height.bmp")
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TESTPLAY, TEXT("Prototype_Component_VIBuffer_Terrain"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Terrain_Texture/Height/Terrain_Height_3.bmp")))))
		return E_FAIL;
	m_fCur += 1.f;

#pragma  region 0~4 MapObj
	/* 파쿠르 용*/
	if (FAILED(LoadNonAnimFolderModel(LEVEL_TESTPLAY, "RuinKit/RuinsKit_BridgeLarge", true, true, true)))
		assert(!"Issue");
	if (FAILED(LoadNonAnimFolderModel(LEVEL_TESTPLAY, "RuinKit/RuinsKit_BridgeShort", true, true, true)))
		assert(!"Issue");
	if (FAILED(LoadNonAnimFolderModel(LEVEL_TESTPLAY, "RuinKit/RuinKit_LedgeStone", true, true, true)))
		return E_FAIL;
	if (FAILED(LoadNonAnimFolderModel(LEVEL_TESTPLAY, "Cliff/Cliff_Ledge", true, true, true)))
		assert(!"Issue");


	_matrix			PivotMatrix = XMMatrixIdentity();
	/* For.Prototype_Component_Model_DeadZoneTree */
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TESTPLAY, L"Prototype_Component_Model_DeadZoneTree",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/DeadZoneTree_Anim/DeadzoneTree.mdat"), PivotMatrix))))
		return E_FAIL;
	m_fCur += 1.f;

	/* For.Prototype_Component_Model_PulsePlateAnim*/
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TESTPLAY, L"Prototype_Component_Model_PulsePlateAnim",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/PulsePlate_Anim/PulsePlate_Anim.model"), PivotMatrix))))
		return E_FAIL;
	m_fCur += 1.f;

	if (FAILED(LoadNonAnimFolderModel(LEVEL_TESTPLAY, "PowerCrystal", true, false, true)))
		assert(!"Issue");
	m_fCur += 1.f;
	if (FAILED(LoadNonAnimFolderModel(LEVEL_TESTPLAY, "PulseObjects/PluseStone_Big", true, false, true)))
		return E_FAIL;
	m_fCur += 1.f;
	if (FAILED(LoadNonAnimFolderModel(LEVEL_TESTPLAY, "PulseObjects/PulsePlate_StoneBorder", true, false, true)))
		return E_FAIL;
	m_fCur += 1.f;
	if (FAILED(LoadNonAnimFolderModel(LEVEL_TESTPLAY, "PulseObjects/PulsePlateEnclosure_02", true, false, true)))
		return E_FAIL;
	m_fCur += 1.f;
	if (FAILED(LoadNonAnimFolderModel(LEVEL_TESTPLAY, "PulseObjects/PulseStoneRock", true, false, true)))
		return E_FAIL;
	m_fCur += 1.f;
	//if (FAILED(LoadNonAnimFolderModel(LEVEL_TESTPLAY, "RuinKit/RuinKit_LedgeStone", true, true, true)))
	//	return E_FAIL;
	//m_fCur += 1.f;
	if (FAILED(LoadNonAnimFolderModel(LEVEL_TESTPLAY, "RuinKit/RuinKit_Pillar", true, true, true)))
		return E_FAIL;
	m_fCur += 1.f;
	if (FAILED(LoadNonAnimFolderModel(LEVEL_TESTPLAY, "RuinKit/RuinsKit_Brick", true, true, true)))
		return E_FAIL;
	m_fCur += 1.f;
	//if (FAILED(LoadNonAnimFolderModel(LEVEL_TESTPLAY, "RuinKit/RuinsKit_BridgeLarge", true, true, true)))
	//	return E_FAIL;
	//m_fCur += 1.f;
	if (FAILED(LoadNonAnimFolderModel(LEVEL_TESTPLAY, "RuinKit/RuinsKit_ToriGate", true, true, true)))
		return E_FAIL;
	m_fCur += 1.f;
	if (FAILED(LoadNonAnimFolderModel(LEVEL_TESTPLAY, "Rock/Rock_Arch", true, true, true)))
		assert(!"Issue");
	m_fCur += 1.f;
	if (FAILED(LoadNonAnimFolderModel(LEVEL_TESTPLAY, "RuinPlatform", true, true, true)))
		assert(!"Issue");
	m_fCur += 1.f;
	if (FAILED(LoadNonAnimFolderModel(LEVEL_TESTPLAY, "Rock/Rock_Rubble", true, true, true)))
		return E_FAIL;
	m_fCur += 1.f;
	if (FAILED(LoadNonAnimFolderModel(LEVEL_TESTPLAY, "Rock/Rock_Big", true, true, true)))
		assert(!"Issue");
	m_fCur += 1.f;
	if (FAILED(LoadNonAnimFolderModel(LEVEL_TESTPLAY, "Cliff/Cliff_Rock", true, true, true)))
		return E_FAIL;
	m_fCur += 1.f;
	if (FAILED(LoadNonAnimFolderModel(LEVEL_TESTPLAY, "Rock/God_Rock", true, true, true)))
		return E_FAIL;
	m_fCur += 1.f;
	if (FAILED(LoadNonAnimFolderModel(LEVEL_TESTPLAY, "Cliff/Cliff_Cap", true, true, true)))
		assert(!"Issue");
	m_fCur += 1.f;
	if (FAILED(LoadNonAnimFolderModel(LEVEL_TESTPLAY, "Cliff/Cliff_W2", true, true, true)))
		return E_FAIL;
	m_fCur += 1.f;
	if (FAILED(LoadNonAnimFolderModel(LEVEL_TESTPLAY, "Cliff/Cliff_W3", true, true, true)))
		assert(!"Issue");
	m_fCur += 1.f;
	if (FAILED(LoadNonAnimFolderModel(LEVEL_TESTPLAY, "Cliff/Cliff_Wall_Large", true, true, true)))
		return E_FAIL;
	m_fCur += 1.f;
	if (FAILED(LoadNonAnimFolderModel(LEVEL_TESTPLAY, "Cliff/Cliff_Wall_Med", true, true, true)))
		return E_FAIL;
	m_fCur += 1.f;
	if (FAILED(LoadNonAnimFolderModel(LEVEL_TESTPLAY, "Cliff/Cliff_Wall_Short", true, true, true)))
		return E_FAIL;
	if (FAILED(LoadNonAnimFolderModel(LEVEL_TESTPLAY, "Cliff/Cliff_Glitter", true, true, true)))
		assert(!"Issue");
	m_fCur += 1.f;
	if (FAILED(LoadNonAnimFolderModel(LEVEL_TESTPLAY, "Cliff/Cliff_Sheer", true, true, true)))
		assert(!"Issue");
	m_fCur += 1.f;
	if (FAILED(LoadNonAnimFolderModel(LEVEL_TESTPLAY, "RuinKit/RuinKit_Rubble", true, true, true)))
		return E_FAIL;
	m_fCur += 1.f;
	if (FAILED(LoadNonAnimFolderModel(LEVEL_TESTPLAY, "RuinKit/RuinStaris", true, true, true)))
		return E_FAIL;
	m_fCur += 1.f;
	if (FAILED(LoadNonAnimFolderModel(LEVEL_TESTPLAY, "Statue/Owl_WrapShrine", false, true, true)))
		return E_FAIL;
	m_fCur += 1.f;
	if (FAILED(LoadNonAnimFolderModel(LEVEL_TESTPLAY, "Statue", true, true, true)))
		return E_FAIL;
	m_fCur += 1.f;
	if (FAILED(LoadNonAnimFolderModel(LEVEL_TESTPLAY, "RuinKit/RuinKit_Arch_Gate", true, true, true)))
		return E_FAIL;
	m_fCur += 1.f;
	if (FAILED(LoadNonAnimFolderModel(LEVEL_TESTPLAY, "RuinKit/RuinsKit_Tree", true, true, true)))
		return E_FAIL;
	m_fCur += 1.f;
	if (FAILED(LoadNonAnimFolderModel(LEVEL_TESTPLAY, "RuinKit/RuinsKit_FloorTile", true, true, true)))
		return E_FAIL;
	m_fCur += 1.f;
	if (FAILED(LoadNonAnimFolderModel(LEVEL_TESTPLAY, "RuinKit/RuinKit_Piece", true, true, true)))
		return E_FAIL;
	m_fCur += 1.f;
	if (FAILED(LoadNonAnimFolderModel(LEVEL_TESTPLAY, "RuinKit/RuinsKit_Wall_Broken", true, true, true)))
		return E_FAIL;
	m_fCur += 1.f;
	if (FAILED(LoadNonAnimFolderModel(LEVEL_TESTPLAY, "RuinKit/RuinsWall", true, true, true)))
		assert(!"Issue");
	m_fCur += 1.f;
	if (FAILED(LoadNonAnimFolderModel(LEVEL_TESTPLAY, "RuinKit/Ruinskit_Collum", true, true, true)))
		assert(!"Issue");
	m_fCur += 1.f;
	//if (FAILED(LoadNonAnimFolderModel(LEVEL_TESTPLAY, "Cliff/Cliff_Ledge", true, true, true)))
	//	return E_FAIL;
	//m_fCur += 1.f;
	if (FAILED(LoadNonAnimFolderModel(LEVEL_TESTPLAY, "Rock/Rock_Small", true, true, true)))
		return E_FAIL;
	m_fCur += 1.f;
	if (FAILED(LoadNonAnimFolderModel(LEVEL_TESTPLAY, "Rock/Rock_Medium", true, true, true)))
		return E_FAIL;
	m_fCur += 1.f;
	if (FAILED(LoadNonAnimFolderModel(LEVEL_TESTPLAY, "Forest_1/RotGod_Statue_crumbled", true, false, true)))
		return E_FAIL;
	m_fCur += 1.f;
	if (FAILED(LoadNonAnimFolderModel(LEVEL_TESTPLAY, "Forest_1/Start_Gate", true, true, true)))
		return E_FAIL;
	m_fCur += 1.f;
	if (FAILED(LoadNonAnimFolderModel(LEVEL_TESTPLAY, "Forest_1/FirstTear_FallenTree", true, false, true)))
		return E_FAIL;
	m_fCur += 1.f;
	if (FAILED(LoadNonAnimFolderModel(LEVEL_TESTPLAY, "Forest_1/First_Room_Deco", true, true, true)))
		return E_FAIL;
	m_fCur += 1.f;
	if (FAILED(LoadNonAnimFolderModel(LEVEL_TESTPLAY, "Forest_1/ShrineOfDeath_MainRock", true, true, true)))
		return E_FAIL;
	m_fCur += 1.f;
	if (FAILED(LoadNonAnimFolderModel(LEVEL_TESTPLAY, "Forest_1/ShrineOfLife", true, true, true)))
		return E_FAIL;
	m_fCur += 1.f;
	if (FAILED(LoadNonAnimFolderModel(LEVEL_TESTPLAY, "Stone/StoneFloor_1", true, true, true)))
		return E_FAIL;
	m_fCur += 1.f;
	if (FAILED(LoadNonAnimFolderModel(LEVEL_TESTPLAY, "Stone/StoneFloor_2", true, true, true)))
		return E_FAIL;
	m_fCur += 1.f;
	if (FAILED(LoadNonAnimFolderModel(LEVEL_TESTPLAY, "Stone/Stone_Bridge", true, true, true)))
		assert(!"Issue");
	m_fCur += 1.f;
	if (FAILED(LoadNonAnimFolderModel(LEVEL_TESTPLAY, "Stone/Stone_Stairs", true, true, true)))
		assert(!"Issue");
	m_fCur += 1.f;
	if (FAILED(LoadNonAnimFolderModel(LEVEL_TESTPLAY, "Trees/Giant", true, true, true)))
		return E_FAIL;
	m_fCur += 1.f;
	if (FAILED(LoadNonAnimFolderModel(LEVEL_TESTPLAY, "Trees/Fallen", true, true, true)))
		return E_FAIL;
	m_fCur += 1.f;
	if (FAILED(LoadNonAnimFolderModel(LEVEL_TESTPLAY, "Trees/Dead", true, true, true)))
		return E_FAIL;
	m_fCur += 1.f;
	if (FAILED(LoadNonAnimFolderModel(LEVEL_TESTPLAY, "Trees/CeDarTree", true, true, true)))
		return E_FAIL;
	m_fCur += 1.f;
	if (FAILED(LoadNonAnimFolderModel(LEVEL_TESTPLAY, "Trees/Bare", true, true, true)))
		return E_FAIL;
	m_fCur += 1.f;
	if (FAILED(LoadNonAnimFolderModel(LEVEL_TESTPLAY, "Trees/Billboard", true, true, true)))
		return E_FAIL;
	m_fCur += 1.f;
	if (FAILED(LoadNonAnimFolderModel(LEVEL_TESTPLAY, "Trees/Branches/Canopy", true, true, true)))
		return E_FAIL;
	m_fCur += 1.f;
	if (FAILED(LoadNonAnimFolderModel(LEVEL_TESTPLAY, "Trees/Branches/GodTree", true, true, true)))
		return E_FAIL;
	m_fCur += 1.f;
	if (FAILED(LoadNonAnimFolderModel(LEVEL_TESTPLAY, "Trees/Branches/Tree_Branch", true, true, true)))
		return E_FAIL;
	m_fCur += 1.f;
	if (FAILED(LoadNonAnimFolderModel(LEVEL_TESTPLAY, "Trees/Canopy", true, true, true)))
		return E_FAIL;
	m_fCur += 1.f;
	if (FAILED(LoadNonAnimFolderModel(LEVEL_TESTPLAY, "Save_StoneTree", true, true)))// json NonUse
		assert(!"Issue");
	m_fCur += 1.f;
	if (FAILED(LoadNonAnimFolderModel(LEVEL_TESTPLAY, "DeadZone/Deadzondes_GoopWeb", true, true, true)))
		assert(!"Issue");
	m_fCur += 1.f;
	if (FAILED(LoadNonAnimFolderModel(LEVEL_TESTPLAY, "DeadZone/DeadZoneAndPurifiedTree", true, true, true)))
		assert(!"Issue");
	m_fCur += 1.f;
	if (FAILED(LoadNonAnimFolderModel(LEVEL_TESTPLAY, "DeadZone/DeadzoneClumps", true, true, true)))
		assert(!"Issue");
	m_fCur += 1.f;
	if (FAILED(LoadNonAnimFolderModel(LEVEL_TESTPLAY, "DeadZone/DeadzoneGroudCover", true, true, true)))
		assert(!"Issue");
	m_fCur += 1.f;
	if (FAILED(LoadNonAnimFolderModel(LEVEL_TESTPLAY, "DeadZone/DeadzoneHeart", true, true, true)))
		assert(!"Issue");
	m_fCur += 1.f;
	if (FAILED(LoadNonAnimFolderModel(LEVEL_TESTPLAY, "DeadZone/DeadzoneWall", true, true, true)))
		assert(!"Issue");
	m_fCur += 1.f;
	if (FAILED(LoadNonAnimFolderModel(LEVEL_TESTPLAY, "DeadZone/Gate", true, true, true)))
		assert(!"Issue");
	m_fCur += 1.f;
	if (FAILED(LoadNonAnimFolderModel(LEVEL_TESTPLAY, "DeadZone/Trees", true, true, true)))
		assert(!"Issue");
	m_fCur += 1.f;
	if (FAILED(LoadNonAnimFolderModel(LEVEL_TESTPLAY, "DeadZone/BushDead_02", true, true)))
		assert(!"Issue");
	m_fCur += 1.f;
	if (FAILED(LoadNonAnimFolderModel(LEVEL_TESTPLAY, "Rot/Rot_Rock", true, true)))// json NonUse
		assert(!"Issue");
	m_fCur += 1.f;
	if (FAILED(LoadNonAnimFolderModel(LEVEL_TESTPLAY, "Rot/RotCarry_Piece", true, true)))// json NonUse
		assert(!"Issue");
	m_fCur += 1.f;
#pragma endregion ~ 0~4 MapObj


	if (FAILED(LoadNonAnimFolderModel(LEVEL_TESTPLAY, "Map_Base")))
		return E_FAIL;
	m_fCur += 1.f;

	PivotMatrix = XMMatrixIdentity();

	/* For.Prototype_Component_Model_Kena */
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TESTPLAY, L"Prototype_Component_Model_Kena",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/Kena/Kena_Body.model"), PivotMatrix))))
		return E_FAIL;
	m_fCur += 1.f;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TESTPLAY, L"Prototype_Component_Model_Kena_Staff",
		CModel::Create(m_pDevice, m_pContext, L"../Bin/Resources/Anim/Kena/Staff/Kena_Staff.model", PivotMatrix))))
		return E_FAIL;
	m_fCur += 1.f;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TESTPLAY, L"Prototype_Component_Model_Kena_MainOutfit",
		CModel::Create(m_pDevice, m_pContext, L"../Bin/Resources/Anim/Kena/Outfit/MainOutfit/Kena_MainOutfit.model", PivotMatrix))))
		return E_FAIL;
	m_fCur += 1.f;

	if (FAILED(LoadNonAnimFolderModel(LEVEL_TESTPLAY, "Spirit_Arrow", false, false, false)))
		return E_FAIL;
	m_fCur += 1.f;

	/* NPC */
	///* Prototype_Component_Model_Beni */
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_TESTPLAY, L"Prototype_Component_Model_Beni",
	//	CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/NPC/Beni/Beni.mdat"), PivotMatrix))))
	//	return E_FAIL;
	//m_fCur += 1.f;

	///* Prototype_Component_Model_Saiya */
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_TESTPLAY, L"Prototype_Component_Model_Saiya",
	//	CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/NPC/Saiya/Saiya.mdat"), PivotMatrix))))
	//	return E_FAIL;
	//m_fCur += 1.f;

	// 작업 시작 안한것들은 주석처리.

	/* Prototype_Component_Model_Mage */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TESTPLAY, L"Prototype_Component_Model_Mage",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/Enemy/Mage/Mage.model"), PivotMatrix))))
		return E_FAIL;
	m_fCur += 1.f;

	/* Prototype_Component_Model_ShieldStick */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TESTPLAY, L"Prototype_Component_Model_ShieldStick",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/Enemy/ShieldSticks/ShieldSticks.model"), PivotMatrix))))
		return E_FAIL;
	m_fCur += 1.f;

	/* Prototype_Component_Model_ShieldStick_Weapon */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TESTPLAY, L"Prototype_Component_Model_ShieldStick_Weapon",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/Enemy/ShieldSticks/Shield.mdat"), PivotMatrix))))
		return E_FAIL;
	m_fCur += 1.f;

	///* Prototype_Component_Model_CorruptVillager */
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_TESTPLAY, L"Prototype_Component_Model_CorruptVillager",
	//	CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/Enemy/CorruptVillager/CorruptVillager.mdat"), PivotMatrix))))
	//	return E_FAIL;
	//m_fCur += 1.f;

	///* Prototype_Component_Model_Moth */
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_TESTPLAY, L"Prototype_Component_Model_Moth",
	//	CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/Enemy/Moth/Moth.mdat"), PivotMatrix))))
	//	return E_FAIL;
	//m_fCur += 1.f;

	/* Prototype_Component_Model_RockGolem */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TESTPLAY, L"Prototype_Component_Model_RockGolem",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/Enemy/RockGolem/RockGolem.model"), PivotMatrix))))
		return E_FAIL;
	m_fCur += 1.f;

	/* Prototype_Component_Model_RotEater */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TESTPLAY, L"Prototype_Component_Model_RotEater",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/Enemy/RotEater/RotEater.model"), PivotMatrix))))
		return E_FAIL;
	m_fCur += 1.f;

	/* Prototype_Component_Model_Sticks01 */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TESTPLAY, L"Prototype_Component_Model_Sticks01",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/Enemy/Sticks01/Sticks01.model"), PivotMatrix))))
		return E_FAIL;
	m_fCur += 1.f;

	///* Prototype_Component_Model_VillageGuard */
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_TESTPLAY, L"Prototype_Component_Model_VillageGuard",
	//	CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/Enemy/VillageGuard/VillageGuard.mdat"), PivotMatrix))))
	//	return E_FAIL;
	//m_fCur += 1.f;

	/* Prototype_Component_Model_WoodKnight */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TESTPLAY, L"Prototype_Component_Model_WoodKnight",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/Enemy/WoodKnight/WoodKnight.model"), PivotMatrix))))
		return E_FAIL;
	m_fCur += 1.f;

	/* Prototype_Component_Model_Sapling */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TESTPLAY, L"Prototype_Component_Model_Sapling",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/Enemy/Sapling/Sapling.model"), PivotMatrix))))
		return E_FAIL;
	m_fCur += 1.f;

	/* Prototype_Component_Model_BranchTosser */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TESTPLAY, L"Prototype_Component_Model_BranchTosser",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/Enemy/BranchTosser/BranchTosser.model"), PivotMatrix))))
		return E_FAIL;
	m_fCur += 1.f;

	/* Prototype_Component_Model_Rope_Rock */
	if (FAILED(LoadNonAnimFolderModel(LEVEL_TESTPLAY, "Rope_RotRock", true, false, true)))
		return E_FAIL;
	m_fCur += 1.f;

	/* Prototype_Component_Model_BranchTosser_Projectile */
	if (FAILED(LoadNonAnimFolderModel(LEVEL_TESTPLAY, "Enemy/BranchTosser", true, false, false)))
		return E_FAIL;
	m_fCur += 1.f;

	/* Prototype_Component_Model_HeroRot */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TESTPLAY, L"Prototype_Component_Model_HeroRot",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/HeroRot/HeroRot.mdat"), PivotMatrix))))
		return E_FAIL;
	m_fCur += 1.f;

	/* Prototype_Component_Model_Rot */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TESTPLAY, L"Prototype_Component_Model_Rot",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/Rot/Rot.mdat"), PivotMatrix))))
		return E_FAIL;
	m_fCur += 1.f;

	/*map_Object Compoents */
	/* For.Prototype_Component_ControlMove*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TESTPLAY, TEXT("Prototype_Component_ControlMove"),
		CControlMove::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	m_fCur += 1.f;

	/* For.Prototype_Component_Interaction_Com*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TESTPLAY, TEXT("Prototype_Component_Interaction_Com"),
		CInteraction_Com::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	m_fCur += 1.f;

	/* For.Prototype_GameObject_Sky */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Sky"),
		CSky::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	m_fCur += 1.f;

	/* For.Prototype_GameObject_Player_Camera */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera_Player"),
		CCamera_Player::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	m_fCur += 1.f;

	/* ~Test*/
	/* For.Prototype_GameObject_Cave_Rock */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_CLodObject"),
		CLodObject::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	m_fCur += 1.f;
	/* For.Prototype_GameObject_ModelViewerObject */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ModelViewerObject"),
		CModelViewerObject::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* ~Test*/
	m_fCur += 1.f;

	/* For.Prototype_GameObject_Cave_Rock */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Cliff_Rock"),
		CCliff_Rock::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	m_fCur += 1.f;

	/* For.Prototype_GameObject_Terrain */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Terrain"),
		CTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	m_fCur += 1.f;

	/* For.Prototype_GameObject_Tree */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Tree"),
		CTree::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	m_fCur += 1.f;

	/* For.Prototype_GameObject_Crystal */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Crystal"),
		CCrystal::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	m_fCur += 1.f;

	/* For.Prototype_GameObject_BowTarget */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BowTarget"),
		CBowTarget::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	m_fCur += 1.f;

	/* For.Prototype_GameObject_PulseStone */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_PulseStone"),
		CPulseStone::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	m_fCur += 1.f;

	/* For.Prototype_GameObject_BaseGround */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BaseGround"),
		CBase_Ground::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	m_fCur += 1.f;

	/* For.Prototype_GameObject_Wall */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Wall"),
		CWall::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	m_fCur += 1.f;

	/* For.Prototype_GameObject_Wall */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Root"),
		CRoots::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	m_fCur += 1.f;

	/* For.Prototype_GameObject_Stair */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Stair"),
		CStair::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	m_fCur += 1.f;

	/* For.Prototype_GameObject_Wall */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Pillar"),
		CPillar::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	m_fCur += 1.f;

	/* For.Prototype_GameObject_Statue */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Statue"),
		CStatue::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	m_fCur += 1.f;

	/* For.Prototype_GameObject_Border */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Border"),
		CBorder::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	m_fCur += 1.f;

	/* For.Prototype_GameObject_Slope */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Slope"),
		CSlope::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	m_fCur += 1.f;

	/* For.Prototype_GameObject_Bridge */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Bridge"),
		CBridge::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	m_fCur += 1.f;

	/* For.Prototype_GameObject_Path_Mesh */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Path_Mesh"),
		CPath_Mesh::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	m_fCur += 1.f;

	/* For.Prototype_GameObject_FloorTile */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_FloorTile"),
		CFloorTile::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	m_fCur += 1.f;

	/* For.Prototype_GameObject_Pulse_PlateForm */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Pulse_PlateForm"),
		CPulse_PlateForm::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	m_fCur += 1.f;

	/* For.Prototype_GameObject_Gate */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Gate"),
		CGate::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	m_fCur += 1.f;

	/* For.Prototype_GameObject_RuinKit */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_RuinKit"),
		CRuinKit::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	m_fCur += 1.f;
	/* For.Prototype_GameObject_Stone */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Stone"),
		CStone::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	m_fCur += 1.f;

	/* For.Prototype_GameObject_Flower */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Flower"),
		CFlower::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	m_fCur += 1.f;

	/* For.Prototype_GameObject_Grass */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Grass"),
		CGrass::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	m_fCur += 1.f;

	/* For.Prototype_GameObject_DeadZoneObj */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_DeadZoneObj"),
		CDeadZoneObj::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	m_fCur += 1.f;

	/* For.Prototype_GameObject_DeadZoneObj */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_GimmickObj"),
		CGimmick_EnviObj::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	m_fCur += 1.f;
	/*Anim*/
	/* For.Prototype_GameObject_Door_Anim */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_DoorAnim"),
		CDoor_Anim::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	m_fCur += 1.f;

	/* For.Prototype_GameObject_Door_Anim */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_PulsePlateAnim"),
		CPulse_Plate_Anim::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	m_fCur += 1.f;

	/* For.Prototype_GameObject_GroundMark */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_GroundMark"),
		CGroundMark::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	m_fCur += 1.f;

	/* For.Prototype_GameObject_ControlRoom */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ControlRoom"),
		CControlRoom::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	m_fCur += 1.f;

	// Effect
#pragma region EFFECT_COMPONENT
	lstrcpy(m_szLoadingText, TEXT("Loading Texture..."));
	/* For.Prototype_Component_Texture_Effect */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TESTPLAY, TEXT("Prototype_Component_Texture_Effect"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/DiffuseTexture/E_Effect_%d.png"), 108))))
		return E_FAIL;
	m_fCur += 1.f;

	/* For.Prototype_Component_Texture_NormalEffect */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TESTPLAY, TEXT("Prototype_Component_Texture_NormalEffect"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/NormalTexture/N_Effect_%d.png"), 11))))
		return E_FAIL;
	m_fCur += 1.f;

	/* For.Prototype_Component_Texture_PulseShield_Dissolve */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TESTPLAY, TEXT("Prototype_Component_Texture_PulseShield_Dissolve"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/PulseShield_Dissolve/E_Effect_%d.png"), 4))))
		return E_FAIL;
	m_fCur += 1.f;

	/* For.Prototype_Component_Texture_TrailFlow */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TESTPLAY, TEXT("Prototype_Component_Texture_TrailFlow"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Trail/flow/E_Flow_%d.png"), 10))))
		return E_FAIL;
	m_fCur += 1.f;

	/* For.Prototype_Component_Texture_TrailType */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TESTPLAY, TEXT("Prototype_Component_Texture_TrailType"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Trail/shapetype/E_Type_%d.png"), 11))))
		return E_FAIL;
	m_fCur += 1.f;

	lstrcpy(m_szLoadingText, TEXT("Loading Obejct..."));
	/* For.Prototype_Component_VIBuffer_Point_Instancing */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TESTPLAY, TEXT("Prototype_Component_VIBuffer_Trail"),
		CVIBuffer_Trail::Create(m_pDevice, m_pContext, 300))))
		return E_FAIL;
	m_fCur += 1.f;

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	/* For.Prototype_Component_Model_Cube */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TESTPLAY, L"Prototype_Component_Model_Cube",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Effect/Cube.mdat"), PivotMatrix))))
		return E_FAIL;
	m_fCur += 1.f;

	/* For.Prototype_Component_Model_Cone */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TESTPLAY, L"Prototype_Component_Model_Cone",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Effect/Cone.mdat"), PivotMatrix))))
		return E_FAIL;
	m_fCur += 1.f;

	/* For.Prototype_Component_Model_Cylinder */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TESTPLAY, L"Prototype_Component_Model_Cylinder",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Effect/Cylinder.mdat"), PivotMatrix))))
		return E_FAIL;
	m_fCur += 1.f;

	/* For.Prototype_Component_Model_Plane */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TESTPLAY, L"Prototype_Component_Model_Plane",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Effect/Plane.mdat"), PivotMatrix))))
		return E_FAIL;
	m_fCur += 1.f;

	/* For.Prototype_Component_Model_Sphere */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TESTPLAY, L"Prototype_Component_Model_Sphere",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Effect/Sphere.mdat"), PivotMatrix))))
		return E_FAIL;
	m_fCur += 1.f;

	/* For.Prototype_Component_Model_shockball */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TESTPLAY, L"Prototype_Component_Model_shockball",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Effect/shockball.mdat"), PivotMatrix))))
		return E_FAIL;
	m_fCur += 1.f;

	/* For.Prototype_Component_Model_shockball3 */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TESTPLAY, L"Prototype_Component_Model_shockball3",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Effect/shockball3.mdat"), PivotMatrix))))
		return E_FAIL;
	m_fCur += 1.f;

	/* For.Prototype_Component_Model_Wind */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TESTPLAY, L"Prototype_Component_Model_Wind",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Effect/Wind.mdat"), PivotMatrix))))
		return E_FAIL;
	m_fCur += 1.f;

	/* For.Prototype_Component_Model_WindLoop */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TESTPLAY, L"Prototype_Component_Model_WindLoop",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Effect/WindLoop.mdat"), PivotMatrix))))
		return E_FAIL;
	m_fCur += 1.f;

#pragma endregion EFFECT_COMPONENT

#pragma region Effect_Object
	/* For.Prototype_GameObject_KenaPulse */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_KenaPulse"),
		CE_KenaPulse::Create(m_pDevice, m_pContext, L"../Bin/Data/Effect/E_KenaPulse.json"))))
		return E_FAIL;
	m_fCur += 1.f;

	/* For.Prototype_GameObject_KenaPulseCloud */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_KenaPulseCloud"),
		CE_KenaPulseCloud::Create(m_pDevice, m_pContext, L"../Bin/Data/Effect/E_KenaPulseCloude.json"))))
		return E_FAIL;
	m_fCur += 1.f;

	/* For.Prototype_GameObject_KenaPulseDot */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_KenaPulseDot"),
		CE_KenaPulseDot::Create(m_pDevice, m_pContext, L"../Bin/Data/Effect/E_KenaPulseDot.json"))))
		return E_FAIL;
	m_fCur += 1.f;

	/* For.Prototype_GameObject_KenaCharge */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_KenaCharge"),
		CE_KenaCharge::Create(m_pDevice, m_pContext, L"../Bin/Data/Effect/E_Charge_Set.json"))))
		return E_FAIL;
	m_fCur += 1.f;

	/* For.Prototype_GameObject_KenaChargeImpact */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_KenaChargeImpact"),
		CE_KenaChargeImpact::Create(m_pDevice, m_pContext, L"../Bin/Data/Effect/E_Charge_Impact.json"))))
		return E_FAIL;
	m_fCur += 1.f;

	/* For.Prototype_GameObject_KenaDamage */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_KenaDamage"),
		CE_KenaDamage::Create(m_pDevice, m_pContext, L"../Bin/Data/Effect/E_Damage_Set.json"))))
		return E_FAIL;
	m_fCur += 1.f;

	/* For.Prototype_GameObject_KenaHit */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_KenaHit"),
		CE_KenaHit::Create(m_pDevice, m_pContext, L"../Bin/Data/Effect/E_HitSet.json"))))
		return E_FAIL;
	m_fCur += 1.f;

	/* For.Prototype_GameObject_KenaJump */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_KenaJump"),
		CE_KenaJump::Create(m_pDevice, m_pContext, L"../Bin/Data/Effect/E_JumpSet.json"))))
		return E_FAIL;
	m_fCur += 1.f;

	/* For.Prototype_GameObject_PulseObject */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_PulseObject"),
		CE_PulseObject::Create(m_pDevice, m_pContext, L"../Bin/Data/Effect/E_PulseObject.json"))))
		return E_FAIL;
	m_fCur += 1.f;

	/* For.Prototype_GameObject_KenaStaffTrail */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_KenaStaffTrail"),
		CE_KenaTrail::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	m_fCur += 1.f;

	/* For.Prototype_GameObject_Wind */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Wind"),
		CE_Wind::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	m_fCur += 1.f;

	/* For.Prototype_GameObject_EffectFlower */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_EffectFlower"),
		CE_P_Flower::Create(m_pDevice, m_pContext, L"../Bin/Data/Effect/E_P_Flower.json"))))
		return E_FAIL;
	m_fCur += 1.f;

	/* For.Prototype_GameObject_KenaDust */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_KenaDust"),
		CE_KenaDust::Create(m_pDevice, m_pContext, L"../Bin/Data/Effect/E_KenaDust.json"))))
		return E_FAIL;
	m_fCur += 1.f;

#pragma endregion Effect_Object

#pragma  region	MONSTER
	/* For.Prototype_Component_Model_EnemyWisp */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TESTPLAY, L"Prototype_Component_Model_EnemyWisp",
		CModel::Create(m_pDevice, m_pContext, L"../Bin/Resources/Anim/Enemy/EnemyWisp/EnemyWisp.model", PivotMatrix))))
		return E_FAIL;
	m_fCur += 1.f;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_EnemyWisp"),
		CEnemyWisp::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	m_fCur += 1.f;

	/* EnemyWisp Effects */
	/* For.Prototype_GameObject_EnemyWispTrail */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_EnemyWispTrail"),
		CE_EnemyWispTrail::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	m_fCur += 1.f;

	/* For.Prototype_GameObject_EnemyWispBackground */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_EnemyWispBackground"),
		CE_EnemyWispBackground::Create(m_pDevice, m_pContext, L"../Bin/Data/Effect/E_EnemyWispBackGround.json"))))
		return E_FAIL;
	m_fCur += 1.f;

	/* For.Prototype_GameObject_EnemyWispGround */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_EnemyWispGround"),
		CE_EnemyWispGround::Create(m_pDevice, m_pContext, L"../Bin/Data/Effect/E_EnemyWispGround.json"))))
		return E_FAIL;
	m_fCur += 1.f;

	/* For.Prototype_GameObject_EnemyWispParticle */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_EnemyWispParticle"),
		CE_P_EnemyWisp::Create(m_pDevice, m_pContext, L"../Bin/Data/Effect/E_P_EnemyWispSpawn.json"))))
		return E_FAIL;
	m_fCur += 1.f;
	/* ~EnemyWisp Effects */
#pragma  endregion	MONSTER

	/* For.Prototype_GameObject_Player */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Kena"),
		CKena::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	m_fCur += 1.f;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Kena_Staff"),
		CKena_Staff::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	m_fCur += 1.f;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Kena_MainOutfit"),
		CKena_MainOutfit::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	m_fCur += 1.f;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_SpiritArrow"),
		CSpiritArrow::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Moth"),
		CMoth::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	m_fCur += 1.f;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_RockGolem"),
		CRockGolem::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	m_fCur += 1.f;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_RotEater"),
		CRotEater::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	m_fCur += 1.f;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Sticks01"),
		CSticks01::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	m_fCur += 1.f;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_VillageGuard"),
		CVillageGuard::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	m_fCur += 1.f;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_WoodKnight"),
		CWoodKnight::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	m_fCur += 1.f;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Mage"),
		CMage::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	m_fCur += 1.f;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BranchTosser"),
		CBranchTosser::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	m_fCur += 1.f;

	// if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BranchTosserWeapon"), CBranchTosser_Weapon::Create(m_pDevice, m_pContext)))) return E_FAIL;
	m_fCur += 1.f;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ShieldStick"),
		CShieldStick::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	m_fCur += 1.f;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ShieldStickWeapon"),
		CShieldStick_Weapon::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	m_fCur += 1.f;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Sapling"),
		CSapling::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	m_fCur += 1.f;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Rot"),
		CRot::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	m_fCur += 1.f;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_CRope_RotRock"),
		CRope_RotRock::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	m_fCur += 1.f;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_LiftRot"),
		CLiftRot::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	m_fCur += 1.f;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_LiftRot_Master"),
		CLiftRot_Master::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	m_fCur += 1.f;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_RotForMonster"),
		CRotForMonster::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	m_fCur += 1.f;

#pragma  region	MONSTER

	m_isFinished = true;

	SetWindowText(g_hWnd, TEXT("Loading Complete!! Wait a moment"));

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLoader::Loading_ForTestEffect()
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	_matrix			PivotMatrix = XMMatrixIdentity();

#pragma  region	PLAYER
	/* For.Prototype_Component_Model_Kena */
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_EFFECT, L"Prototype_Component_Model_Kena",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/Kena/Kena_Body.model"), PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_EFFECT, L"Prototype_Component_Model_Kena_Staff", CModel::Create(m_pDevice, m_pContext, L"../Bin/Resources/Anim/Kena/Staff/Kena_Staff.model", PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_EFFECT, L"Prototype_Component_Model_Kena_MainOutfit", CModel::Create(m_pDevice, m_pContext, L"../Bin/Resources/Anim/Kena/Outfit/MainOutfit/Kena_MainOutfit.model", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_GameObject_Player_Camera */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera_Player"),
		CCamera_Player::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Player */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Kena"),
		CKena::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Kena_Staff"),
		CKena_Staff::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Kena_MainOutfit"),
		CKena_MainOutfit::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#pragma  endregion	PLAYER

#pragma region EFFECT_COMPONENT
	lstrcpy(m_szLoadingText, TEXT("Loading Texture..."));
	/* For.Prototype_Component_Texture_Effect */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_EFFECT, TEXT("Prototype_Component_Texture_Effect"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/DiffuseTexture/E_Effect_%d.png"), 108))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_NormalEffect */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_EFFECT, TEXT("Prototype_Component_Texture_NormalEffect"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/NormalTexture/N_Effect_%d.png"), 11))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_PulseShield_Dissolve */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_EFFECT, TEXT("Prototype_Component_Texture_PulseShield_Dissolve"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/PulseShield_Dissolve/E_Effect_%d.png"), 4))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_TrailFlow */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_EFFECT, TEXT("Prototype_Component_Texture_TrailFlow"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Trail/flow/E_Flow_%d.png"), 10))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_TrailType */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_EFFECT, TEXT("Prototype_Component_Texture_TrailType"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Trail/shapetype/E_Type_%d.png"), 11))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("Loading Obejct..."));
	/* For.Prototype_Component_VIBuffer_Point_Instancing */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_EFFECT, TEXT("Prototype_Component_VIBuffer_Trail"),
		CVIBuffer_Trail::Create(m_pDevice, m_pContext, 300))))
		return E_FAIL;

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	/* For.Prototype_Component_Model_Cube */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_EFFECT, L"Prototype_Component_Model_Cube",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Effect/Cube.mdat"), PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Cone */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_EFFECT, L"Prototype_Component_Model_Cone",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Effect/Cone.mdat"), PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Cylinder */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_EFFECT, L"Prototype_Component_Model_Cylinder",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Effect/Cylinder.mdat"), PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Plane */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_EFFECT, L"Prototype_Component_Model_Plane",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Effect/Plane.mdat"), PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Sphere */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_EFFECT, L"Prototype_Component_Model_Sphere",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Effect/Sphere.mdat"), PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_shockball */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_EFFECT, L"Prototype_Component_Model_shockball",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Effect/shockball.mdat"), PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_shockball3 */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_EFFECT, L"Prototype_Component_Model_shockball3",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Effect/shockball3.mdat"), PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Wind */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_EFFECT, L"Prototype_Component_Model_Wind",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Effect/Wind.mdat"), PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_WindLoop */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_EFFECT, L"Prototype_Component_Model_WindLoop",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Effect/WindLoop.mdat"), PivotMatrix))))
		return E_FAIL;

#pragma endregion EFFECT_COMPONENT

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
		CE_KenaHit::Create(m_pDevice, m_pContext, L"../Bin/Data/Effect/E_HitSet.json"))))
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

#pragma endregion Effect_Object

#pragma  region	MONSTER
	/* For.Prototype_Component_Model_EnemyWisp */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_EFFECT, L"Prototype_Component_Model_EnemyWisp", 
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

	lstrcpy(m_szLoadingText, TEXT("Loading End."));

	m_isFinished = true;

	Safe_Release(pGameInstance);

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

HRESULT CLoader::LoadNonAnimFolderModel(_uint iLevelIndex, string strFolderName, _bool bIsLod, _bool bIsInstancing, _bool bIsJsonMatarial)
{
	return S_OK; // 임시 조치

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
				CModel::Create(m_pDevice, m_pContext, WideFilePath, PivotMatrix, nullptr, bIsLod, bIsInstancing, szJSonFullPath))))
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

