#include "stdafx.h"
#include "..\public\DeadZoneBossTree.h"
#include "GameInstance.h"

#include "ControlRoom.h"
#include "HunterArrow.h"
#include "E_Warrior_FireSwipe.h"
#include "Monster.h"

#include "ControlRoom.h"

#include "Delegator.h"

CDeadZoneBossTree::CDeadZoneBossTree(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CEnviromentObj(pDevice, pContext)
{
}

CDeadZoneBossTree::CDeadZoneBossTree(const CDeadZoneBossTree& rhs)
	: CEnviromentObj(rhs)
{
}

HRESULT CDeadZoneBossTree::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CDeadZoneBossTree::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_bRenderActive = true;
	return S_OK;
}

HRESULT CDeadZoneBossTree::Late_Initialize(void* pArg)
{
	_float3 vPos;
	XMStoreFloat3(&vPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));

	_float3 vSize = _float3(1.f,1.f,1.f);

	if(m_EnviromentDesc.iRoomIndex ==3 )
	{
		vSize = _float3(1.49f, 14.44f, 1.52f);
		vPos.x += 0.22f;
		vPos.y += 14.38f;
	}
	else if(m_EnviromentDesc.szModelTag  == L"Prototype_Component_Model_BossDissolveGod_Rock02")
	{
		vSize = _float3(1.26f, 1.5f, 1.51f);
	}
	else if (m_EnviromentDesc.szModelTag == L"Prototype_Component_Model_BossDissolveGod_Rock01")
	{
		vSize = _float3(1.f, 1.5f, 1.1f);
	}

	CPhysX_Manager* pPhysX = CPhysX_Manager::GetInstance();
	
	CPhysX_Manager::PX_BOX_DESC BoxDesc;
	BoxDesc.pActortag = m_szCloneObjectTag;
	BoxDesc.eType = BOX_STATIC;		
	BoxDesc.vPos = vPos;
	BoxDesc.vSize = vSize;
	BoxDesc.vRotationAxis = _float3(0.f, 0.f, 0.f);
	BoxDesc.fDegree = 0.f;
	BoxDesc.isGravity = false;
	BoxDesc.eFilterType = PX_FILTER_TYPE::FILTER_DEFULAT;
	BoxDesc.vVelocity = _float3(0.f, 0.f, 0.f);
	BoxDesc.fDensity = 0.2f;
	BoxDesc.fMass = 150.f;
	BoxDesc.fLinearDamping = 10.f;
	BoxDesc.fAngularDamping = 5.f;
	BoxDesc.bCCD = false;
	BoxDesc.fDynamicFriction = 0.5f;
	BoxDesc.fStaticFriction = 0.5f;
	BoxDesc.fRestitution = 0.1f;

	pPhysX->Create_Box(BoxDesc, Create_PxUserData(this, false, COL_ENVIROMENT));
	m_pTransformCom->Connect_PxActor_Static(m_szCloneObjectTag);


	CControlRoom* pCtrlRoom = static_cast<CControlRoom*>(CGameInstance::GetInstance()->Get_GameObjectPtr(g_LEVEL, L"Layer_ControlRoom", L"ControlRoom"));

	pCtrlRoom->m_DeadZoneChangeDelegator.bind(this, &CDeadZoneBossTree::Change_Model);

	return S_OK;
}

void CDeadZoneBossTree::Change_Model(_int iDissolveTimer)
{
	if (!m_bCollOnce)
	{
		m_bDissolve = true;
		m_fDissolveTime = 0.f;
		m_bBossAttackOn = true;
		m_bCollOnce = true;
	}
}

void CDeadZoneBossTree::Tick(_float fTimeDelta)
{

	__super::Tick(fTimeDelta);

	if (m_bBossAttackOn)
	{
		m_fDissolveTime += fTimeDelta * 0.45f;

		if (m_fDissolveTime >= 1.f)
		{
			m_bBossAttackOn = false;
			m_fDissolveTime = 0;
			m_bOriginRender = false;
			m_pTransformCom->Clear_Actor();
		}
	}


	//if(!lstrcmp(m_szCloneObjectTag,L"3_BossDissolveGodTree02_1"))
	//{
	//	m_fDissolveTime += fTimeDelta * 0.1f;

	//	/*if (m_fDissolveTime <= 1.f)
	//	{

	//		m_fDissolveTime = 0.f;
	//	}*/
	//}
;
	

	
	
}

void CDeadZoneBossTree::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);


	_matrix  WolrdMat = m_pTransformCom->Get_WorldMatrix();

	if (m_pRendererCom  && m_bOriginRender && m_bRenderActive)//&& false /*== m_pModelCom->Culling_InstancingMeshs(100.f, WolrdMat)*/)
	{
		//m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_SHADOW, this);
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
	}
}

HRESULT CDeadZoneBossTree::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();


	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture"), E_FAIL);
		FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_NORMALS, "g_NormalTexture"), E_FAIL);

		FAILED_CHECK_RETURN(m_pModelCom->Render(m_pShaderCom, i, nullptr, 25), E_FAIL); //Dissolve
	}

	return S_OK;
}

HRESULT CDeadZoneBossTree::RenderShadow()
{
	if (FAILED(__super::RenderShadow()))
		return E_FAIL;

	if (FAILED(SetUp_ShadowShaderResources()))
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

void CDeadZoneBossTree::ImGui_PhysXValueProperty()
{
	//CPhysX_Manager::GetInstance()->Imgui_Render(m_szCloneObjectTag);

	//_float4 vCurPos = m_pTransformCom->Get_Position();


	//static _float fChagePos[3] = { 0.f,0.f,0.f };
	//ImGui::DragFloat3("PX_Pos", fChagePos, 0.01f, 0.1f, 100.0f);

	//vCurPos.x += fChagePos[0];
	//vCurPos.y += fChagePos[1];
	//vCurPos.z += fChagePos[2];

	//PxRigidActor * pActor = CPhysX_Manager::GetInstance()->Find_Actor(m_szCloneObjectTag);

	//CPhysX_Manager::GetInstance()->Set_ActorPosition(pActor, CUtile::Float_4to3(vCurPos));

	//if(ImGui::Button("toggle"))
	//{
	//	m_bDissolve = !m_bDissolve;
	//	m_fDissolveTime = 0.f;
	//}


	////ImGui::Begin("Dissolve Test");

	////ImGui::InputFloat("FirstRatio", &fFirstRatio);
	////ImGui::InputFloat("SecondRatio", &fSecondRatio);

	//m_vBaseColor1 = Set_ColorValue();
	//m_vBaseColor2 = Set_ColorValue_1();

	//ImGui::End();


}

_int CDeadZoneBossTree::Execute_Collision(CGameObject* pTarget, _float3 vCollisionPos, _int iColliderIndex)
{
	

	if (m_EnviromentDesc.iRoomIndex == 4)
	{
		if (iColliderIndex == TRIGGER_DUMMY || pTarget == nullptr) return 0;
		_bool bRealAttack = false;

		if (iColliderIndex == (_int)COL_MONSTER_WEAPON && (bRealAttack = ((CMonster*)pTarget)->IsRealAttack()))
		{
			/*���� ���� ���ý�*/
			if (!m_bCollOnce)
			{
				m_bDissolve = true;
				m_fDissolveTime = 0.f;
				m_bBossAttackOn = true;
				m_bCollOnce = true;
				return 0;
			}
		}
		return 0;
	}

	if (pTarget == nullptr) return 0;
	/*To DO*/
	CHunterArrow* pArrow = dynamic_cast<CHunterArrow*>(pTarget);

	if (pArrow == nullptr)
		return 0;
	
	if(pArrow->Get_FierType() == CHunterArrow::SINGLE)
	{
		return 0;
	}

	/*���� ���� ���ý�*/
	if (!m_bCollOnce)
	{
		m_bDissolve = true;
		m_fDissolveTime = 0.f;
		m_bBossAttackOn = true;
		m_bCollOnce = true;
	}


	return 0;
}

_int CDeadZoneBossTree::Execute_TriggerTouchFound(CGameObject* pTarget, _uint iTriggerIndex, _int iColliderIndex)
{
	
	CE_Warrior_FireSwipe* pWarriorComboAttack = dynamic_cast<CE_Warrior_FireSwipe*>(pTarget);

	if (pTarget == nullptr || pWarriorComboAttack == nullptr)
		return 0;

	if (!m_bCollOnce)
	{
		m_bDissolve = true;
		m_fDissolveTime = 0.f;
		m_bBossAttackOn = true;
		m_bCollOnce = true;
	}

	return 0;
}

_float4 CDeadZoneBossTree::Set_ColorValue()
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

_float4 CDeadZoneBossTree::Set_ColorValue_1()
{
	static bool alpha_preview2 = true;
	static bool alpha_half_preview2 = false;
	static bool drag_and_drop2 = true;
	static bool options_menu2 = true;
	static bool hdr2 = false;

	ImGuiColorEditFlags misc_flags = (hdr2 ? ImGuiColorEditFlags_HDR : 0) | (drag_and_drop2 ? 0 : ImGuiColorEditFlags_NoDragDrop) | (alpha_half_preview2 ? ImGuiColorEditFlags_AlphaPreviewHalf : (alpha_preview2 ? ImGuiColorEditFlags_AlphaPreview : 0)) | (drag_and_drop2 ? 0 : ImGuiColorEditFlags_NoOptions);

	static bool   ref_color2 = false;
	static ImVec4 ref_color_v2(1.0f, 1.0f, 1.0f, 1.0f);

	static _float4 vSelectColor2 = { 1.0f, 1.0f, 1.0f, 1.0f };

	ImGui::ColorPicker4("CurColor##24", (float*)&vSelectColor2, ImGuiColorEditFlags_NoInputs | misc_flags, ref_color2 ? &ref_color_v2.x : NULL);
	ImGui::ColorEdit4("Diffuse##22f", (float*)&vSelectColor2, ImGuiColorEditFlags_DisplayRGB | misc_flags);

	return vSelectColor2;
}


HRESULT CDeadZoneBossTree::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	/*���߿�  ���� �ε��� �����ؾ߉�*/
	if (m_EnviromentDesc.iCurLevel == 0)
		m_EnviromentDesc.iCurLevel = LEVEL_MAPTOOL;

	/* For.Com_Model */ 	/*���߿�  ���� �ε��� �����ؾ߉�*/
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

HRESULT CDeadZoneBossTree::SetUp_ShaderResources()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);
	FAILED_CHECK_RETURN(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"), E_FAIL);
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_fFar", pGameInstance->Get_CameraFar(), sizeof(float)), E_FAIL);

	///*GS_Shader SetUP*/
	//FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_TimeDelta", &m_fTimeDeltaTest, sizeof(float)), E_FAIL);

	//m_vColor = _float4((255.f / 255.f), (19.f / 255.f), (19.f / 255.f), (242.f / 255.f));
	//FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("vColorData", &m_vColor, sizeof(_float4)), E_FAIL);

	if (FAILED(m_pShaderCom->Set_RawValue("g_bDissolve", &m_bDissolve, sizeof(_bool)))) return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_fDissolveTime", &m_fDissolveTime, sizeof(_float)))) return E_FAIL;
	if (FAILED(m_pDissolveTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DissolveTexture"))) return E_FAIL;

	m_vBaseColor1 = _float4( (255.f/ 255.f), (19.f/ 255.f), (19.f/255.f), (242.f/255.f));
	if (FAILED(m_pShaderCom->Set_RawValue("g_FirstColor", &m_vBaseColor1, sizeof(_float4)))) return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_SecondColor", &m_vBaseColor2, sizeof(_float4)))) return E_FAIL;

	fFirstRatio = 1.f;
	if (FAILED(m_pShaderCom->Set_RawValue("g_FirstRatio", &fFirstRatio, sizeof(_float)))) return E_FAIL;

	fSecondRatio = 0.1f;
	if (FAILED(m_pShaderCom->Set_RawValue("g_SecondRatio", &fSecondRatio, sizeof(_float)))) return E_FAIL;


	_float4		vCenterPos = m_pTransformCom->Get_Position();

	if (FAILED(m_pShaderCom->Set_RawValue("g_CenterPos", &vCenterPos, sizeof(_float4)))) return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CDeadZoneBossTree::SetUp_ShadowShaderResources()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);
	FAILED_CHECK_RETURN(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"), E_FAIL);
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_DYNAMICLIGHTVEIW)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_fFar", pGameInstance->Get_CameraFar(), sizeof(float)), E_FAIL);
	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

CDeadZoneBossTree* CDeadZoneBossTree::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDeadZoneBossTree* pInstance = new CDeadZoneBossTree(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CDeadZoneBossTree");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CDeadZoneBossTree::Clone(void* pArg)
{
	CDeadZoneBossTree* pInstance = new CDeadZoneBossTree(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CDeadZoneBossTree");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CDeadZoneBossTree::Free()
{
	__super::Free();
	Safe_Release(m_pDissolveTextureCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);

}
