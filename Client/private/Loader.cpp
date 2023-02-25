#include "stdafx.h"
#include "..\public\Loader.h"
#include "GameInstance.h"

/* SkyBox */
#include "Sky.h"

/* Terrain */
#include "Terrain.h"

/* Player */
#include "Kena.h"
#include "Kena_Staff.h"
#include "Kena_MainOutfit.h"

/* NPCs */

/* Enemies*/

/* Objects */
#include "Cave_Rock.h"

/* UI */
#include "BackGround.h"

/* Effects */
#include "Effect.h"
#include "Effect_Rect_Instancing.h"
#include "Effect_Point_Instancing.h"

/* Components*/
#include "ControlMove.h"
#include "Interaction_Com.h"
#include "Camera_Player.h"

/*Test Objects*/
#include "LodObject.h"
#include "ModelViewerObject.h"

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
#ifdef _DEBUG
	case LEVEL_MAPTOOL:
		pLoader->Loading_ForMapTool();
		break;
	case LEVEL_TESTPLAY:
		pLoader->Loading_ForTestPlay();
		break;
	case LEVEL_EFFECT:
		pLoader->Loading_ForTestEffect();
		break;
#endif
	}
	
	LeaveCriticalSection(&pLoader->Get_CriticalSection());

	return 0;
}

HRESULT CLoader::Initialize(LEVEL eNextLevelID)
{
	m_eNextLevelID = eNextLevelID;

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

	/* For.Prototype_Component_Texture_Brush*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Brush"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Brush.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Filter */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Filter"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Filter.dds"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Sky */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Sky"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/SkyBox/Sky_%d.dds"), 4))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Explosion */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Explosion"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Explosion/Explosion%d.png"), 90))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("Loading VIBuffer..."));
	/* For.Prototype_Component_VIBuffer_Terrain */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Terrain"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Height.bmp")))))
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
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/Kena/Kena_Body.mdat"), PivotMatrix))))
		return E_FAIL;

	/* Test Model */
	/*if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, L"Prototype_Component_Model_Kena",
		CModel::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Anim/Kena/Kena_Body_forTest.mdat"), PivotMatrix))))
		return E_FAIL;*/

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, L"Prototype_Component_Model_Kena_Staff", CModel::Create(m_pDevice, m_pContext, L"../Bin/Resources/Anim/Kena/Staff/Kena_Staff.mdat", PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, L"Prototype_Component_Model_Kena_MainOutfit", CModel::Create(m_pDevice, m_pContext, L"../Bin/Resources/Anim/Kena/Outfit/MainOutfit/Kena_MainOutfit.mdat", PivotMatrix))))
		return E_FAIL;

	if (FAILED(LoadNonAnimModel(LEVEL_GAMEPLAY)))
		return E_FAIL;
	

	lstrcpy(m_szLoadingText, TEXT("Loading Collider..."));
	/* For.Prototype_Component_Collider_AABB*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_AABB))))
		return E_FAIL;
	/* For.Prototype_Component_Collider_OBB*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_OBB"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_OBB))))
		return E_FAIL;
	/* For.Prototype_Component_Collider_SPHERE*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_SPHERE"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_SPHERE))))
		return E_FAIL;


	lstrcpy(m_szLoadingText, TEXT("Loading Shader..."));
	/* For.Prototype_Component_Shader_VtxNorTex */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxNorTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX_DECLARATION::Elements, VTXNORTEX_DECLARATION::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxModel*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxModel"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxModel.hlsl"), VTXMODEL_DECLARATION::Elements, VTXMODEL_DECLARATION::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxAnimModel*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimModel"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxAnimModel.hlsl"), VTXANIMMODEL_DECLARATION::Elements, VTXANIMMODEL_DECLARATION::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxCubeTex */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxCubeTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxCubeTex.hlsl"), VTXCUBETEX_DECLARATION::Elements, VTXCUBETEX_DECLARATION::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxRectInstance */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxRectInstance"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxRectInstance.hlsl"), VTXRECTINSTANCE_DECLARATION::Elements, VTXRECTINSTANCE_DECLARATION::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxPointInstance */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxPointInstance"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPointInstance.hlsl"), VTXPOINT_DECLARATION::Elements, VTXPOINT_DECLARATION::iNumElements))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("Loading Navigation Info..."));
	/* For.Prototype_Component_Navigation */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../Bin/Data/Navigation.dat")))))
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
	/* For.Prototype_Component_Texture_Terrain */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Texture_Terrain"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Tile%d.dds"), 2))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Brush*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Texture_Brush"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Brush.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Filter */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Texture_Filter"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Filter.dds"), 1))))

	lstrcpy(m_szLoadingText, TEXT("Loading Model..."));
		
	//if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "AncientWells")))
	//	return E_FAIL;
	//if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Bow_Target")))
	//	return E_FAIL;
	//if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Cave_Big_Pillar")))
	//	return E_FAIL;
	//if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Cave_Ciff_Rock")))
	//	return E_FAIL;
	//if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Cave_Broken_Crystal")))
	//	return E_FAIL;
	//if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Cave_RockRubble")))
	//	return E_FAIL;
	//if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Hanging_Objects/Hanging_Brazier")))
	//	return E_FAIL;
	//if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Hanging_Objects/HangingChimes")))
	//	return E_FAIL;
	//if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "PowerCrystal")))
	//	return E_FAIL;
	//if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "PulseObjects/PluseStone_Big")))
	//	return E_FAIL;
	//if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "PulseObjects/PulsePlate_NonAnim")))
	//	return E_FAIL;
	//if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "PulseObjects/PulsePlate_StoneBorder")))
	//	return E_FAIL;
	//if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "PulseObjects/PulsePlateEnclosure_02")))
	//	return E_FAIL;
	//if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "PulseObjects/PulseStoneRock")))
	//	return E_FAIL;
	//if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Roots/Roots_Nomral")))
	//	return E_FAIL;
	//if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Rot/Rot_Rock")))
	//	return E_FAIL;
	//if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Rot/RotCarry_Piece")))
	//	return E_FAIL;
	//if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "RuinDebris")))
	//	return E_FAIL;
	//if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "RuinPlatform")))
	//	return E_FAIL;
	//if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Save_StoneTree")))
	//	return E_FAIL;
	//if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Slopes/DirtBank")))
	//	return E_FAIL;
	//if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Slopes/DirtSlope_Spline02")))
	//	return E_FAIL;
	//if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Slopes/RootSlope_convex")))
	//	return E_FAIL;
	//if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Slopes/RootSlope_Rock")))
	//	return E_FAIL;
	//if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Slopes/SlideCap_Spline")))
	//	return E_FAIL;
	//if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Statue/Owl_WrapShrine")))
	//	return E_FAIL;
	//if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Statue/RotGod_Statue")))
	//	return E_FAIL;
	//if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Statue")))
	//	return E_FAIL;
	//if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "StoneStep_Seperate")))
	//	return E_FAIL;
	//if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "VantagePlatform")))
	//	return E_FAIL;

	//if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "RuinKit/RuinKit_LedgeStone")))
	//	return E_FAIL;
	//if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "RuinKit/RuinKit_Pillar")))
	//	return E_FAIL;
	//if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "RuinKit/RuinKit_Railing")))
	//	return E_FAIL;
	//if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "RuinKit/RuinKit_Rubble")))
	//	return E_FAIL;
	//if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "RuinKit/RuinsKit_Brick")))
	//	return E_FAIL;
	//if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "RuinKit/RuinsKit_BridgeLarge")))
	//	return E_FAIL;
	//if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "RuinKit/RuinsKit_BridgeShort")))
	//	return E_FAIL;
	//if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "RuinKit/RuinsKit_FloorTile")))
	//	return E_FAIL;
	//if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "RuinKit/RuinsKit_ToriGate")))
	//	return E_FAIL;
	//if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "RuinKit/RuinStaris")))
	//	return E_FAIL;
	//if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "RuinKit/RuinsKit_ToriGate")))
	//	return E_FAIL;
	//if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "RuinKit/RuinStaris")))
	//	return E_FAIL;
	//if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Stone/Stone_Bridge")))
	//	return E_FAIL;
	//if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Stone/Stone_Stairs")))
	//	return E_FAIL;
	//if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Stone/StoneBridge")))
	//	return E_FAIL;
	//if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Stone/StoneFloor_1")))
	//	return E_FAIL;
	//if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Stone/StoneFloor_2")))
	//	return E_FAIL;
	if (FAILED(LoadNonAnimFolderModel(LEVEL_MAPTOOL, "Stone/StonePath")))
		return E_FAIL;


	lstrcpy(m_szLoadingText, TEXT("Loading Collider..."));
	/* For.Prototype_Component_Collider_AABB*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Collider_AABB"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_AABB))))
		return E_FAIL;
	/* For.Prototype_Component_Collider_OBB*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Collider_OBB"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_OBB))))
		return E_FAIL;
	/* For.Prototype_Component_Collider_SPHERE*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Collider_SPHERE"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_SPHERE))))
		return E_FAIL;
	
	/* For.Prototype_Component_VIBuffer_Terrain */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_VIBuffer_Terrain"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Height.bmp")))))
		return E_FAIL;


	/*map_Object Compoents */
	/* For.Prototype_Component_ControlMove*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_ControlMove"),
		CControlMove::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_Component_Interaction_Com*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Interaction_Com"),
		CInteraction_Com::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("Loading Shader..."));
	/* For.Prototype_Component_Shader_VtxNorTex */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Shader_VtxNorTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX_DECLARATION::Elements, VTXNORTEX_DECLARATION::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxModel*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Shader_VtxModel"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxModel.hlsl"), VTXMODEL_DECLARATION::Elements, VTXMODEL_DECLARATION::iNumElements))))
		return E_FAIL;
	
	/* For.Prototype_Component_Shader_VtxModel*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Shader_VtxModelTess"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxModel_Tess.hlsl"), VTXMODEL_DECLARATION::Elements, VTXMODEL_DECLARATION::iNumElements))))
		return E_FAIL;


	/* For.Prototype_Component_Shader_VtxAnimModel*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Shader_VtxAnimModel"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxAnimModel.hlsl"), VTXANIMMODEL_DECLARATION::Elements, VTXANIMMODEL_DECLARATION::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxCubeTex */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Shader_VtxCubeTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxCubeTex.hlsl"), VTXCUBETEX_DECLARATION::Elements, VTXCUBETEX_DECLARATION::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxRectInstance */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Shader_VtxRectInstance"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxRectInstance.hlsl"), VTXRECTINSTANCE_DECLARATION::Elements, VTXRECTINSTANCE_DECLARATION::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxPointInstance */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Shader_VtxPointInstance"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPointInstance.hlsl"), VTXPOINT_DECLARATION::Elements, VTXPOINT_DECLARATION::iNumElements))))
		return E_FAIL;


	lstrcpy(m_szLoadingText, TEXT("Loading GameObjects..."));

	/* For.Prototype_GameObject_Cave_Rock */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Cave_Rock"),
		CCave_Rock::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_Cave_Rock */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_CLodObject"),
		CLodObject::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_ModelViewerObject */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ModelViewerObject"),
		CModelViewerObject::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	

	lstrcpy(m_szLoadingText, TEXT("Loading End."));

	m_isFinished = true;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLoader::Loading_ForTestPlay()
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	lstrcpy(m_szLoadingText, TEXT("Loading Texture..."));

	lstrcpy(m_szLoadingText, TEXT("Loading Model..."));

	if (FAILED(LoadNonAnimFolderModel(LEVEL_TESTPLAY, "Cave_Ciff_Rock")))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("Loading Collider..."));
	/* For.Prototype_Component_Collider_AABB*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TESTPLAY, TEXT("Prototype_Component_Collider_AABB"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_AABB))))
		return E_FAIL;
	/* For.Prototype_Component_Collider_OBB*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TESTPLAY, TEXT("Prototype_Component_Collider_OBB"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_OBB))))
		return E_FAIL;
	/* For.Prototype_Component_Collider_SPHERE*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TESTPLAY, TEXT("Prototype_Component_Collider_SPHERE"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_SPHERE))))
		return E_FAIL;

	/*map_Object Compoents */
	/* For.Prototype_Component_ControlMove*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TESTPLAY, TEXT("Prototype_Component_ControlMove"),
		CControlMove::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_Component_Interaction_Com*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TESTPLAY, TEXT("Prototype_Component_Interaction_Com"),
		CInteraction_Com::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("Loading Shader..."));
	/* For.Prototype_Component_Shader_VtxNorTex */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TESTPLAY, TEXT("Prototype_Component_Shader_VtxNorTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX_DECLARATION::Elements, VTXNORTEX_DECLARATION::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxModel*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TESTPLAY, TEXT("Prototype_Component_Shader_VtxModel"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxModel.hlsl"), VTXMODEL_DECLARATION::Elements, VTXMODEL_DECLARATION::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxAnimModel*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TESTPLAY, TEXT("Prototype_Component_Shader_VtxAnimModel"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxAnimModel.hlsl"), VTXANIMMODEL_DECLARATION::Elements, VTXANIMMODEL_DECLARATION::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxCubeTex */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TESTPLAY, TEXT("Prototype_Component_Shader_VtxCubeTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxCubeTex.hlsl"), VTXCUBETEX_DECLARATION::Elements, VTXCUBETEX_DECLARATION::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxRectInstance */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TESTPLAY, TEXT("Prototype_Component_Shader_VtxRectInstance"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxRectInstance.hlsl"), VTXRECTINSTANCE_DECLARATION::Elements, VTXRECTINSTANCE_DECLARATION::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxPointInstance */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TESTPLAY, TEXT("Prototype_Component_Shader_VtxPointInstance"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPointInstance.hlsl"), VTXPOINT_DECLARATION::Elements, VTXPOINT_DECLARATION::iNumElements))))
		return E_FAIL;


	lstrcpy(m_szLoadingText, TEXT("Loading GameObjects..."));

	/* For.Prototype_GameObject_Cave_Rock */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Cave_Rock"),
		CCave_Rock::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("Loading End."));

	m_isFinished = true;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLoader::Loading_ForTestEffect()
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	lstrcpy(m_szLoadingText, TEXT("Loading Texture..."));
	/* For.Prototype_Component_Texture_Effect */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_EFFECT, TEXT("Prototype_Component_Texture_Effect"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Texture/E_Effect_%d.png"), 94))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("Loading Shader..."));
	/* For.Prototype_Component_Shader_VtxEffectTex */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_EFFECT, TEXT("Prototype_Component_Shader_VtxEffectTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxEffectTex.hlsl"), VTXTEX_DECLARATION::Elements, VTXTEX_DECLARATION::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxPointInstance */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_EFFECT, TEXT("Prototype_Component_Shader_VtxEffectPointInstance"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxEffectPointInstance.hlsl"), VTXPOINT_DECLARATION::Elements, VTXPOINT_DECLARATION::iNumElements))))
		return E_FAIL;

	//lstrcpy(m_szLoadingText, TEXT("Loading VIBuffer..."));
	///* For.Prototype_Component_VIBuffer_Point_Instancing */
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_EFFECT, TEXT("Prototype_Component_VIBuffer_Point_Instancing"),
	//	CVIBuffer_Point_Instancing::Create(m_pDevice, m_pContext, 30))))
	//	return E_FAIL;

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

HRESULT CLoader::LoadNonAnimFolderModel(_uint iLevelIndex, string strFolderName)
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
