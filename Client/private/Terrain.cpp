#include "stdafx.h"
#include "..\public\Terrain.h"
#include "GameInstance.h"
#include "GroundMark.h"

CTerrain::CTerrain(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{

}

CTerrain::CTerrain(const CTerrain & rhs)
	: CGameObject(rhs)
{

}

HRESULT CTerrain::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;
	
	return S_OK;
}

HRESULT CTerrain::Initialize(void * pArg)
{
	if (pArg != nullptr)
	{
		TERRAIN_DESC* Desc = reinterpret_cast<TERRAIN_DESC*>(pArg);

		m_TerrainDesc.wstrDiffuseTag = Desc->wstrDiffuseTag;
		m_TerrainDesc.wstrFilterTag = Desc->wstrFilterTag;
		m_TerrainDesc.wstrNormalTag = Desc->wstrNormalTag;
		m_bLoadData = true;
		m_TerrainDesc.iBaseDiffuse = Desc->iBaseDiffuse;
		m_TerrainDesc.iFillterOne_TextureNum = Desc->iFillterOne_TextureNum;
		m_TerrainDesc.iFillterTwo_TextureNum = Desc->iFillterTwo_TextureNum;
		m_TerrainDesc.iFillterThree_TextureNum = Desc->iFillterThree_TextureNum;
		m_TerrainDesc.iHeightBmpNum = Desc->iHeightBmpNum;
	}

	else
	{
		m_TerrainDesc.iBaseDiffuse = 4;
		m_TerrainDesc.iFillterOne_TextureNum = 3;
		m_TerrainDesc.iFillterTwo_TextureNum = 2;
		m_TerrainDesc.iFillterThree_TextureNum = 1;

	}

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;	
	

	return S_OK;
}

HRESULT CTerrain::Late_Initialize(void * pArg)
{	

	wstring wstrFilePath = TEXT("../Bin/Resources/Terrain_Texture/Height/Terrain_Height_");
	wstrFilePath += to_wstring(m_TerrainDesc.iHeightBmpNum);
	wstrFilePath += TEXT(".bmp");
	Change_HeightMap(wstrFilePath.c_str());

	m_pVIBufferCom->initialize_World(m_pTransformCom);

	CGameInstance* pGameInst = CGameInstance::GetInstance();
	m_pGroundMark = (CGroundMark*)pGameInst->Clone_GameObject(L"Prototype_GameObject_GroundMark");
	if (m_pGroundMark == nullptr) return E_FAIL;

	// if (m_TerrainDesc.iHeightBmpNum == 0) return S_OK;

	_float4x4 WorldMatrix = m_pTransformCom->Get_WorldMatrixFloat4x4();
	m_pVIBufferCom->Create_PxTriangleMeshActor(Create_PxUserData(this, true, COL_GROUND));
	m_pVIBufferCom->Set_PxMatrix(WorldMatrix);
	
	return S_OK;
}

void CTerrain::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
	m_pGroundMark->Set_Position(m_vBrushPos + _float4(0.f, 1.f, 0.f, 0.f));
	m_pGroundMark->Tick(fTimeDelta);
}

void CTerrain::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	//m_pVIBufferCom->Culling(m_pTransformCom->Get_WorldMatrix());

	m_pRendererCom && 	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
	m_pGroundMark->Late_Tick(fTimeDelta);
}

HRESULT CTerrain::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(0);

	m_pVIBufferCom->Render();

	return S_OK;
}

void CTerrain::Imgui_RenderProperty()
{
	/*CGameObject::Imgui_RenderProperty();

	ImGui::Begin("Terrain Translation");
	m_pTransformCom->Imgui_RenderProperty();
	ImGui::End();*/
}

void CTerrain::Imgui_Tool_Add_Component(_uint iLevel, const _tchar* ProtoTag, const _tchar* ComTag)
{
	if (FAILED(__super::Add_Component(g_LEVEL, TEXT("Prototype_Component_Texture_Filter"), ComTag,
		(CComponent**)&m_pTextureCom[TYPE_FILTER])))
		assert(!"Imgui_Tool_Add_Component");	
}

void CTerrain::Erase_FilterCom()
{
	Delete_Component(TEXT("Com_Filter"));
	Safe_Release(m_pTextureCom[TYPE_FILTER]);
}

void CTerrain::Change_HeightMap(const _tchar * pHeightMapFilePath)		// 여기서 버퍼를 바꾸기때문에
{
	if (nullptr == pHeightMapFilePath)
		return;

	m_pVIBufferCom->Change_HeightMap(pHeightMapFilePath);
}

_bool CTerrain::CreateEnvrObj_PickingPos(_float4 & vPos)
{
	if (ImGui::IsMouseClicked(0))
	{
		if (m_pVIBufferCom->Picking_Terrain(g_hWnd, m_pTransformCom, &vPos))
		{
			return true;
		}
	}

	return false;
}

HRESULT CTerrain::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_VtxNorTex"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(g_LEVEL, TEXT("Prototype_Component_VIBuffer_Terrain"), TEXT("Com_VIBuffer"),
		(CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	if(false == m_bLoadData )
	{ 
		/* For.Com_Texture */
		if (FAILED(__super::Add_Component(g_LEVEL, TEXT("Prototype_Component_Texture_Terrain"), TEXT("Com_Texture"),
			(CComponent**)&m_pTextureCom[TYPE_DIFFUSE])))
			return E_FAIL;
		/* For.Com_Filter */
		if (FAILED(__super::Add_Component(g_LEVEL, TEXT("Prototype_Component_Texture_Filter"), TEXT("Com_Filter"),
			(CComponent**)&m_pTextureCom[TYPE_FILTER])))
			return E_FAIL;

		m_TerrainDesc.wstrDiffuseTag = TEXT("Prototype_Component_Texture_Terrain");
		m_TerrainDesc.wstrFilterTag = TEXT("Prototype_Component_Texture_Filter");
		m_TerrainDesc.wstrNormalTag = TEXT("");
	}
	else
	{
		/* For.Com_Texture */
		if (FAILED(__super::Add_Component(g_LEVEL, m_TerrainDesc.wstrDiffuseTag.c_str(), TEXT("Com_Texture"),
			(CComponent**)&m_pTextureCom[TYPE_DIFFUSE])))
			return E_FAIL;
		/* For.Com_Filter */
		if (FAILED(__super::Add_Component(g_LEVEL, m_TerrainDesc.wstrFilterTag.c_str(), TEXT("Com_Filter"),
			(CComponent**)&m_pTextureCom[TYPE_FILTER])))
			return E_FAIL;
	}

	/* For.Com_Brush*/								
	if (FAILED(__super::Add_Component(g_LEVEL, TEXT("Prototype_Component_Texture_GroundMark"), TEXT("Com_Brush"),
		(CComponent**)&m_pTextureCom[TYPE_BRUSH])))
		return E_FAIL;

	return S_OK;
}

HRESULT CTerrain::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	if (FAILED(m_pTextureCom[TYPE_DIFFUSE]->Bind_ShaderResource(m_pShaderCom, "g_BaseTexture",m_TerrainDesc.iBaseDiffuse)))
		return E_FAIL;

	if (FAILED(m_pTextureCom[TYPE_DIFFUSE]->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture_0", m_TerrainDesc.iFillterOne_TextureNum)))
		return E_FAIL;

	if (FAILED(m_pTextureCom[TYPE_DIFFUSE]->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture_1", m_TerrainDesc.iFillterTwo_TextureNum)))
		return E_FAIL;

	if (FAILED(m_pTextureCom[TYPE_DIFFUSE]->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture_2", m_TerrainDesc.iFillterThree_TextureNum)))
		return E_FAIL;

	if (FAILED(m_pTextureCom[TYPE_BRUSH]->Bind_ShaderResource(m_pShaderCom, "g_BrushTexture", 1)))
		return E_FAIL;

	if (FAILED(m_pTextureCom[TYPE_FILTER]->Bind_ShaderResources(m_pShaderCom, "g_FilterTexture")))
		return E_FAIL;

	CGameInstance* p_game_instance = GET_INSTANCE(CGameInstance)

	if (FAILED(m_pShaderCom->Set_RawValue("g_fFar", p_game_instance->Get_CameraFar(), sizeof(_float))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance)

	if (FAILED(m_pShaderCom->Set_RawValue("g_vBrushPos", &m_vBrushPos, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_fBrushRange", &m_vBrushRange, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

CTerrain * CTerrain::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
 	CTerrain*		pInstance = new CTerrain(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTerrain");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CTerrain::Clone(void * pArg)
{
	CTerrain*		pInstance = new CTerrain(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTerrain");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CTerrain::Free()
{
	__super::Free();

	if (m_isCloned)
	{
		int i = 0;
	}

	Safe_Release(m_pGroundMark);

	for (auto& pTextureCom : m_pTextureCom)	
		Safe_Release(pTextureCom);	

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}

void CTerrain::Connect_TriangleActor(_float4x4 Matrix)
{
	m_pVIBufferCom->Set_PxMatrix(Matrix);
}
