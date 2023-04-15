#include "stdafx.h"
#include "..\public\DeadZoneObj.h"
#include "GameInstance.h"
#include "ControlMove.h"
#include "ControlRoom.h"
#include "Interaction_Com.h"
#include "Delegator.h"

CDeadZoneObj::CDeadZoneObj(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CEnviromentObj(pDevice, pContext)
{
}

CDeadZoneObj::CDeadZoneObj(const CDeadZoneObj & rhs)
	: CEnviromentObj(rhs)
{
}

HRESULT CDeadZoneObj::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CDeadZoneObj::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	if (m_EnviromentDesc.szModelTag == L"Prototype_Component_Model_DeadZone_Plant_01")
		m_iDeadZoneModelID = 0;
	else if (m_EnviromentDesc.szModelTag == L"Prototype_Component_Model_SmallDeadTree_05")
		m_iDeadZoneModelID = 1;
	else if (m_EnviromentDesc.szModelTag == L"Prototype_Component_Model_SmallDeadTree_02")
		m_iDeadZoneModelID = 2;
	else if (m_EnviromentDesc.szModelTag == L"Prototype_Component_Model_SmallDeadTree_04")
		m_iDeadZoneModelID = 3;
	else if (m_EnviromentDesc.szModelTag == L"Prototype_Component_Model_SmallDeadTree_03")
		m_iDeadZoneModelID = 4;
	else if (m_EnviromentDesc.szModelTag == L"Prototype_Component_Model_FallenLog_a")
		m_iDeadZoneModelID = 5;
	else if (m_EnviromentDesc.szModelTag == L"Prototype_Component_Model_Bush_Dead02")
		m_iDeadZoneModelID = 6;
	else if (m_EnviromentDesc.szModelTag == L"Prototype_Component_Model_DZ_Clump_Tall")
		m_iDeadZoneModelID = 7;
	else if (m_EnviromentDesc.szModelTag == L"Prototype_Component_Model_DZ_Clump_01")
		m_iDeadZoneModelID = 8;
	else if (m_EnviromentDesc.szModelTag == L"Prototype_Component_Model_DZ_ClumpNoSpike_01")
		m_iDeadZoneModelID = 9;
	else if (m_EnviromentDesc.szModelTag == L"Prototype_Component_Model_DeadBranch4")
		m_iDeadZoneModelID = 10;
	else if (m_EnviromentDesc.szModelTag == L"Prototype_Component_Model_DeadzonePlant")
		m_iDeadZoneModelID = 11;
	else if (m_EnviromentDesc.szModelTag == L"Prototype_Component_Model_DeadBranch3")
		m_iDeadZoneModelID = 12;
	m_bRenderActive = true;
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_STATIC_SHADOW, this);
	return S_OK;
}

HRESULT CDeadZoneObj::Late_Initialize(void* pArg)
{
	
	CControlRoom* pCtrlRoom = static_cast<CControlRoom*>(CGameInstance::GetInstance()->Get_GameObjectPtr(g_LEVEL, L"Layer_ControlRoom", L"ControlRoom"));

	pCtrlRoom->m_DeadZoneChangeDelegator.bind(this, &CDeadZoneObj::Change_Model);

	if (FAILED(SetUp_ChangeModel()))
		return E_FAIL;

	return S_OK;
}

void CDeadZoneObj::Tick(_float fTimeDelta)
{
#ifdef FOR_MAP_GIMMICK
#else
	if (!m_bOnlyTest) /*Test*/
	{
		Late_Initialize();
		m_bOnlyTest = true;
	}
#endif

	m_fTimeDeltaTest += fTimeDelta;




	 if(m_bChangeModelRender == true)
	{

		 if( m_fTimeDeltaTest >= 2.0f)
		 {
			 m_bChangeModelRender = false;
			 m_iSign = -1;

			 m_bDeadZoneRender = false;
			 //	m_bDissolve = true;
		 }
	
	}

	//if(m_bDeadZoneRender== false && m_bDissolve == true)
	 if (m_bDissolve == true)
	{
		m_fDissolveTime -= fTimeDelta * 0.49f;

		if (m_fDissolveTime <= 0.f)
		{
			m_bDissolve = false;
		}
	}

	__super::Tick(fTimeDelta);
}

void CDeadZoneObj::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	_matrix  WolrdMat = m_pTransformCom->Get_WorldMatrix();

	if (m_pRendererCom && m_bRenderActive  )//&& false /*== m_pModelCom->Culling_InstancingMeshs(100.f, WolrdMat)*/)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CDeadZoneObj::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	if (m_bDeadZoneRender == true && m_iSign == 1)
	{
		for (_uint i = 0; i < iNumMeshes; ++i)
		{
			FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture"), E_FAIL);
			FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_NORMALS, "g_NormalTexture"), E_FAIL);
			if ((*m_pModelCom->Get_Material())[i].pTexture[WJTextureType_COMP_H_R_AO] != nullptr)
			{
				FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_COMP_H_R_AO, "g_HRAOTexture"), E_FAIL);

				if (m_bChangeModelRender)
				{
					FAILED_CHECK_RETURN(m_pModelCom->Render(m_pShaderCom, i, nullptr, 23), E_FAIL);
				}
				else
				{
					FAILED_CHECK_RETURN(m_pModelCom->Render(m_pShaderCom, i, nullptr, 2), E_FAIL);
				}
			}
			else	if ((*m_pModelCom->Get_Material())[i].pTexture[WJTextureType_COMP_E_R_AO] != nullptr)
			{
				FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_COMP_E_R_AO, "g_ERAOTexture"), E_FAIL);

				if (m_bChangeModelRender)
				{
					FAILED_CHECK_RETURN(m_pModelCom->Render(m_pShaderCom, i, nullptr, 23), E_FAIL);

				}
				else
				{
					FAILED_CHECK_RETURN(m_pModelCom->Render(m_pShaderCom, i, nullptr, 5), E_FAIL);
				}

			}
			else if ((*m_pModelCom->Get_Material())[i].pTexture[WJTextureType_AMBIENT_OCCLUSION] != nullptr)
			{
				FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_AMBIENT_OCCLUSION, "g_MRAOTexture"), E_FAIL);

				if (m_bChangeModelRender)
				{
					FAILED_CHECK_RETURN(m_pModelCom->Render(m_pShaderCom, i, nullptr, 23), E_FAIL);

				}
				else
				{
					FAILED_CHECK_RETURN(m_pModelCom->Render(m_pShaderCom, i, nullptr, 6), E_FAIL);
				}
			}
			else
			{
				if (m_bChangeModelRender)
				{
					FAILED_CHECK_RETURN(m_pModelCom->Render(m_pShaderCom, i, nullptr, 23), E_FAIL);
				}
				else
				{
					FAILED_CHECK_RETURN(m_pModelCom->Render(m_pShaderCom, i, nullptr, 1), E_FAIL);
				}
			}
		}
	}

	if (m_pModelChangeCom != nullptr && m_bChaning_ModelRender)
	{
		_uint iNumChangeMeshes = m_pModelChangeCom->Get_NumMeshes();
		for (_uint i = 0; i < iNumChangeMeshes; ++i)
		{
			FAILED_CHECK_RETURN(m_pModelChangeCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture"), E_FAIL);
			FAILED_CHECK_RETURN(m_pModelChangeCom->Bind_Material(m_pShaderCom, i, WJTextureType_NORMALS, "g_NormalTexture"), E_FAIL);
			if ((*m_pModelChangeCom->Get_Material())[i].pTexture[WJTextureType_COMP_H_R_AO] != nullptr)
			{

				FAILED_CHECK_RETURN(m_pModelChangeCom->Bind_Material(m_pShaderCom, i, WJTextureType_COMP_H_R_AO, "g_HRAOTexture"), E_FAIL);
			/*	if (m_bDeadZoneRender && !m_bDissolve)
				{
					FAILED_CHECK_RETURN(m_pModelChangeCom->Render(m_pShaderCom, i, nullptr, 23), E_FAIL);
				}
				else*/ if(m_bDissolve )
				{
					FAILED_CHECK_RETURN(m_pModelChangeCom->Render(m_pShaderCom, i, nullptr, 26), E_FAIL);
				}
				else
				{
					FAILED_CHECK_RETURN(m_pModelChangeCom->Render(m_pShaderCom, i, nullptr, 2), E_FAIL);
				}
			}
			else if ((*m_pModelChangeCom->Get_Material())[i].pTexture[WJTextureType_COMP_E_R_AO] != nullptr)
			{
				FAILED_CHECK_RETURN(m_pModelChangeCom->Bind_Material(m_pShaderCom, i, WJTextureType_COMP_E_R_AO, "g_ERAOTexture"), E_FAIL);

				/*if (m_bDeadZoneRender && !m_bDissolve)
				{
					FAILED_CHECK_RETURN(m_pModelChangeCom->Render(m_pShaderCom, i, nullptr, 23), E_FAIL);
				}
				else*/ if (m_bDissolve)
				{
					FAILED_CHECK_RETURN(m_pModelChangeCom->Render(m_pShaderCom, i, nullptr, 26), E_FAIL);
				}
				else
				{
					FAILED_CHECK_RETURN(m_pModelChangeCom->Render(m_pShaderCom, i, nullptr, 5), E_FAIL);
				}
			}
			else if ((*m_pModelChangeCom->Get_Material())[i].pTexture[WJTextureType_AMBIENT_OCCLUSION] != nullptr)
			{
				FAILED_CHECK_RETURN(m_pModelChangeCom->Bind_Material(m_pShaderCom, i, WJTextureType_AMBIENT_OCCLUSION, "g_MRAOTexture"), E_FAIL);
/*
				if (m_bDeadZoneRender && !m_bDissolve)
				{
					FAILED_CHECK_RETURN(m_pModelChangeCom->Render(m_pShaderCom, i, nullptr, 23), E_FAIL);
				}
				else */if (m_bDissolve)
				{
					FAILED_CHECK_RETURN(m_pModelChangeCom->Render(m_pShaderCom, i, nullptr, 26), E_FAIL);
				}
				else
				{
					FAILED_CHECK_RETURN(m_pModelChangeCom->Render(m_pShaderCom, i, nullptr, 6), E_FAIL);
				}

			}
			else
			{
				/*if (m_bDeadZoneRender && !m_bDissolve)
				{
					FAILED_CHECK_RETURN(m_pModelChangeCom->Render(m_pShaderCom, i, nullptr, 23), E_FAIL);
				}
				else*/ if (m_bDissolve)
				{
					FAILED_CHECK_RETURN(m_pModelChangeCom->Render(m_pShaderCom, i, nullptr, 26), E_FAIL);
				}
				else
				{
					FAILED_CHECK_RETURN(m_pModelChangeCom->Render(m_pShaderCom, i, nullptr, 1), E_FAIL);
				}
			}
		}
	}


	

	return S_OK;
}

HRESULT CDeadZoneObj::RenderShadow()
{
	if (FAILED(__super::RenderShadow()))
		return E_FAIL;

	if (FAILED(__super::SetUp_ShadowShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();
	
	if (m_pModelCom->Get_IStancingModel())
	{
		for (_uint i = 0; i < iNumMeshes; ++i)
		{
			FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture"), E_FAIL);
			FAILED_CHECK_RETURN(m_pModelCom->Render(m_pShaderCom, i, nullptr, 0), E_FAIL);
		}
	}
	else
	{
		for (_uint i = 0; i < iNumMeshes; ++i)
		{
			FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture"), E_FAIL);
			FAILED_CHECK_RETURN(m_pModelCom->Render(m_pShaderCom, i, nullptr, 21), E_FAIL);
		}
	}

	return S_OK;
}
#ifdef _DEBUG
_float4 CDeadZoneObj::Set_ColorValue()
{
	
	static bool alpha_preview = true;
	static bool alpha_half_preview = false;
	static bool drag_and_drop = true;
	static bool options_menu = true;
	static bool hdr = false;

	ImGuiColorEditFlags misc_flags = (hdr ? ImGuiColorEditFlags_HDR : 0) | (drag_and_drop ? 0 : ImGuiColorEditFlags_NoDragDrop) | (alpha_half_preview ? ImGuiColorEditFlags_AlphaPreviewHalf : (alpha_preview ? ImGuiColorEditFlags_AlphaPreview : 0)) | (options_menu ? 0 : ImGuiColorEditFlags_NoOptions);

	static bool   ref_color = false;
	static ImVec4 ref_color_v(1.0f, 1.0f, 1.0f, 1.0f);

	static _float4 vSelectColor = { 1.0f, 1.0f, 1.0f, 1.0f };

	ImGui::ColorPicker4("CurColor##4", (float*)&vSelectColor, ImGuiColorEditFlags_NoInputs | misc_flags, ref_color ? &ref_color_v.x : NULL);
	ImGui::ColorEdit4("Diffuse##2f", (float*)&vSelectColor, ImGuiColorEditFlags_DisplayRGB | misc_flags);

	return vSelectColor;
	
}
#endif
void CDeadZoneObj::Change_Model(_int iDissolveTimer)
{
	if (m_iDeadZoneModelID == -1)
		return;

	if(iDissolveTimer == -1)
	{
		m_bChangeModelRender = false;
		m_bChaning_ModelRender = false;
		m_bDeadZoneRender = true;
		m_bDissolve = false;
		m_iSign = 1;
		m_fTimeDeltaTest = 0.f;
		m_fDissolveTime = 1.f;
		return;
	}
	

	m_bChangeModelRender = true;
	m_bChaning_ModelRender = true;
	m_bDissolve = true;

	m_fTimeDeltaTest = 0;

	


}

HRESULT CDeadZoneObj::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	/*나중에  레벨 인덱스 수정해야됌*/
	if (m_EnviromentDesc.iCurLevel == 0)
		m_EnviromentDesc.iCurLevel = LEVEL_MAPTOOL;

	/* For.Com_Model */ 	/*나중에  레벨 인덱스 수정해야됌*/
	if (FAILED(__super::Add_Component(g_LEVEL_FOR_COMPONENT, m_EnviromentDesc.szModelTag.c_str(), TEXT("Com_Model"),
		(CComponent**)&m_pModelCom)))
		return E_FAIL;
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_VtxModelInstance"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;

	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Texture_Dissolve", L"Com_Dissolve_Texture",
		(CComponent**)&m_pDissolveTextureCom), E_FAIL);

	return S_OK;
}

HRESULT CDeadZoneObj::SetUp_ChangeModel()
{
	if (m_iDeadZoneModelID == -1)
		return S_OK;

	_tchar pModelName[13][64] =
	{
		{ L"Prototype_Component_Model_Flower4"},//0
		{ L"Prototype_Component_Model_Giant_GodTreeSmall_03"},//1
		{ L"Prototype_Component_Model_Giant_GodTreeSmall_02"},//2
		{ L"Prototype_Component_Model_Giant_GodTreeSmall_01"},//3
		{ L"Prototype_Component_Model_CadarTree_03"},//4
		{ L"Prototype_Component_Model_BigTreeLog"},//5
		{ L"Prototype_Component_Model_BigBushes_06"},//6
		{ L"Prototype_Component_Model_BigBushes_03"},//7
		{ L"Prototype_Component_Model_BigBushes_04"},//8
		{ L"Prototype_Component_Model_BigBushes_03"},//9
		{ L"Prototype_Component_Model_BigBushes_06"},//10
		{ L"Prototype_Component_Model_Flower2"},//11
		{ L"Prototype_Component_Model_Flower3"}, //12
	};


	vector<_float4x4> New_Matrix;

	for (auto pMatrix : *m_pModelCom->Get_InstancePos())
	{
		New_Matrix.push_back(*pMatrix);
	}

	if (FAILED(__super::Add_Component(g_LEVEL_FOR_COMPONENT, pModelName[m_iDeadZoneModelID], TEXT("Com_ChangeModel"),
		(CComponent**)&m_pModelChangeCom)))
		assert(!"CDeadZoneObj::SetUp_ChangeMode()");

	m_pModelChangeCom->Set_InstancePos(New_Matrix);
	return S_OK;
}

HRESULT CDeadZoneObj::SetUp_ShaderResources()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);
	FAILED_CHECK_RETURN(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"), E_FAIL);
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_fFar", pGameInstance->Get_CameraFar(), sizeof(float)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_TimeDelta", &m_fTimeDeltaTest, sizeof(float)), E_FAIL);

	_float4 vMeshCenterPos = m_pModelCom->GetMeshCenter();

	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_CenterPos", &vMeshCenterPos, sizeof(_float4)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("vColorData", &m_vColor, sizeof(_float4)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_iSign", &m_iSign, sizeof(_int)), E_FAIL);

	if (FAILED(m_pShaderCom->Set_RawValue("g_bDissolve", &m_bDissolve, sizeof(_bool)))) return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_fDissolveTime", &m_fDissolveTime, sizeof(_float)))) return E_FAIL;
	if (FAILED(m_pDissolveTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DissolveTexture"))) return E_FAIL;
	_float4 vTestColor1 = _float4((30.f / 255.f), (53.f / 255.f), (13.f / 255.f), (255.f / 255.f));
	if (FAILED(m_pShaderCom->Set_RawValue("g_FirstColor", &vTestColor1, sizeof(_float4)))) return E_FAIL;

	_float4 vTestColor2 = _float4((255.f / 255.f), (19.f / 255.f), (19.f / 255.f), (242.f / 255.f));
	if (FAILED(m_pShaderCom->Set_RawValue("g_SecondColor", &vTestColor2, sizeof(_float4)))) return E_FAIL;

	_float fFirstRatio = 1.f;
	if (FAILED(m_pShaderCom->Set_RawValue("g_FirstRatio", &fFirstRatio, sizeof(_float)))) return E_FAIL;

	_float fSecondRatio = 0.1f;
	if (FAILED(m_pShaderCom->Set_RawValue("g_SecondRatio", &fSecondRatio, sizeof(_float)))) return E_FAIL;


	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

CDeadZoneObj * CDeadZoneObj::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CDeadZoneObj*		pInstance = new CDeadZoneObj(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CDeadZoneObj");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CDeadZoneObj::Clone(void * pArg)
{
	CDeadZoneObj*		pInstance = new CDeadZoneObj(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CDeadZoneObj");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CDeadZoneObj::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pModelChangeCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pDissolveTextureCom);

}
