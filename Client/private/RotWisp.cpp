#include "stdafx.h"
#include "..\public\RotWisp.h"
#include "GameInstance.h"
#include "E_InteractStaff.h"
#include "E_RectTrail.h"

CRotWisp::CRotWisp(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CGameObject(pDevice, pContext)
{
}

CRotWisp::CRotWisp(const CRotWisp& rhs)
	:CGameObject(rhs)
{
}

HRESULT CRotWisp::Initialize_Prototype()
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);
	return S_OK;
}

HRESULT CRotWisp::Initialize(void* pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(CGameObject::GAMEOBJECTDESC));
	GameObjectDesc.TransformDesc.fSpeedPerSec = 7.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.f);
	FAILED_CHECK_RETURN(__super::Initialize(&GameObjectDesc), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_Components(), E_FAIL);
	FAILED_CHECK_RETURN(Set_Effects(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_State(), E_FAIL);
	return S_OK;
}

void CRotWisp::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if(CGameInstance::GetInstance()->Key_Down(DIK_Q))
		m_bPulse = true;

	if (m_pFSM) m_pFSM->Tick(fTimeDelta);
	m_iAnimationIndex = m_pModelCom->Get_AnimIndex();
	m_pModelCom->Play_Animation(fTimeDelta);

	Update_MovementTrail("wisp_geo");

	if (m_pEffect)m_pEffect->Tick(fTimeDelta);
	if (m_pMovementTrail)m_pMovementTrail->Tick(fTimeDelta);
}

void CRotWisp::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);

	if (m_pEffect)m_pEffect->Late_Tick(fTimeDelta);
	if (m_pMovementTrail)m_pMovementTrail->Late_Tick(fTimeDelta);
}

HRESULT CRotWisp::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_ShaderResources(), E_FAIL);
	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();
	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		m_pWPOTextureCom->Bind_ShaderResource(m_pShaderCom, "g_WPOTexture");
		m_pDissolveAlphaTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DissolveAlphaTexture");
		m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", 0);
	}
	return S_OK;
}

void CRotWisp::Calc_RootBoneDisplacement(_fvector vDisplacement)
{
	_vector	vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	vPos = vPos + vDisplacement;
	m_pTransformCom->Set_Translation(vPos, vDisplacement);
}

void CRotWisp::ImGui_AnimationProperty()
{
	m_pModelCom->Imgui_RenderProperty();
}

HRESULT CRotWisp::SetUp_Components()
{
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Shader_VtxAnimRotWispModel", L"Com_Shader", (CComponent**)&m_pShaderCom), E_FAIL);
	FAILED_CHECK_RETURN(__super::Add_Component(g_LEVEL, L"Prototype_Component_Model_RotWisp", L"Com_Model", (CComponent**)&m_pModelCom, nullptr, this), E_FAIL);
	FAILED_CHECK_RETURN(__super::Add_Component(g_LEVEL, L"Prototype_Component_Texture_Effect", L"Com_Texture", (CComponent**)&m_pTextureCom, nullptr, this), E_FAIL);
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom, nullptr, this), E_FAIL);
	FAILED_CHECK_RETURN(__super::Add_Component(g_LEVEL, L"Prototype_Component_Texture_T_GR_Noise_Smooth_A", L"Com_WPOTexture", (CComponent**)&m_pWPOTextureCom, nullptr, this), E_FAIL);
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Texture_RotWispDissolve", L"Com_DissolveTexture", (CComponent**)&m_pDissolveAlphaTextureCom, nullptr, this), E_FAIL);
	return S_OK;
}

HRESULT CRotWisp::SetUp_ShaderResources()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);
	FAILED_CHECK_RETURN(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ViewMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ProjMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_vCamPosition", &CGameInstance::GetInstance()->Get_CamPosition(), sizeof(_float4)), E_FAIL);
	return S_OK;
}

HRESULT CRotWisp::SetUp_State()
{
	m_pFSM = CFSMComponentBuilder()
		.InitState("APPEAR")
		.AddState("APPEAR")
		.Tick([this](float fTimeDelta)
	{
		m_pModelCom->Set_AnimIndex(ROTWISP_APPEAR4);
	})
		.AddTransition("APPEAR to SPLAT" , "SPLAT")
		.Predicator([this]()
	{
		m_bAppear = true;
		return AnimFinishChecker(ROTWISP_APPEAR4);
	})

		.AddState("SPLAT")
		.OnStart([this]()
	{
		m_pModelCom->ResetAnimIdx_PlayTime(ROTWISP_SPLAT);
		m_pModelCom->Set_AnimIndex(ROTWISP_SPLAT);
	})
		.AddTransition("SPLAT to POPUP", "POPUP")
		.Predicator([this]()
	{
		return AnimFinishChecker(ROTWISP_SPLAT);
	})

		.AddState("POPUP")
		.OnStart([this]()
	{
		m_pModelCom->ResetAnimIdx_PlayTime(ROTWISP_POPUP);
		m_pModelCom->Set_AnimIndex(ROTWISP_POPUP);
	})
		.AddTransition("POPUP to FLOATINGLOOP", "FLOATINGLOOP")
		.Predicator([this]()
	{
		return AnimFinishChecker(ROTWISP_POPUP);
	})

		.AddState("FLOATINGLOOP")
		.OnStart([this]()
	{
		/* Effect */
		if(!wcscmp(m_szCloneObjectTag,TEXT("Saiya_RotWisp")))
			m_pEffect->Set_Active(false);
		else
			m_pEffect->Set_Active(true);
		/* Effect */
	})
		.Tick([this](_float fTimeDelta)
	{	
		_float4 vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
		vPos.y = vPos.y + 0.2f;
		m_pEffect->Set_Position(vPos);

		m_pModelCom->Set_AnimIndex(ROTWISP_FLOATINGLOOP1);
	})
		.AddTransition("FLOATINGLOOP to INTERACT", "INTERACT")
		.Predicator([this]()
	{
		return m_bPulse; // Q
	})

		.AddState("INTERACT")
		.OnStart([this]()
	{
		m_pModelCom->ResetAnimIdx_PlayTime(ROTWISP_INTERACT);
		m_pModelCom->Set_AnimIndex(ROTWISP_INTERACT);
	})
		.AddTransition("INTERACT to COLLECT", "COLLECT")
		.Predicator([this]()
	{
		return AnimFinishChecker(ROTWISP_INTERACT);
	})

		.AddState("COLLECT")
		.OnStart([this]()
	{
		m_pModelCom->ResetAnimIdx_PlayTime(ROTWISP_COLLECT);
		m_pModelCom->Set_AnimIndex(ROTWISP_COLLECT);

		_double		dDuration = m_pModelCom->Get_AnimationDuration();
		m_pModelCom->Set_PlayTime(0.645 * dDuration);
	})
		.OnExit([this]()
	{
		m_pEffect->Set_Active(false);
		m_bCollect = true;
	})
		.AddTransition("COLLECT to APPEAR", "APPEAR")
		.Predicator([this]()
	{
		return AnimFinishChecker(ROTWISP_COLLECT); // 사라져야함
	})

		.Build();

	return S_OK;
}

_bool CRotWisp::AnimFinishChecker(_uint eAnim, _double FinishRate)
{
	return m_pModelCom->Find_Animation(eAnim)->Get_PlayRate() >= FinishRate;

}

_bool CRotWisp::AnimIntervalChecker(_uint eAnim, _double StartRate, _double FinishRate)
{
	if (m_pModelCom->Find_Animation(eAnim)->Get_PlayRate() > StartRate &&
		m_pModelCom->Find_Animation(eAnim)->Get_PlayRate() <= FinishRate)
	{
		return true;
	}

	return false;
}

HRESULT CRotWisp::Set_Effects()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	m_pEffect = dynamic_cast<CE_InteractStaff*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_InteractStaff", L"FindEffect"));
	NULL_CHECK_RETURN(m_pEffect, E_FAIL);
	CEffect_Base::EFFECTDESC effect = m_pEffect->Get_EffectDesc();
	effect.IsBillboard = true;
	m_pEffect->Set_EffectDesc(effect);
	m_pEffect->Set_Type(CE_InteractStaff::TYPE::TYPE_ROT);
//	m_pEffect->Set_Parent(this);

/* Movement particle */
	_tchar* pDummyString = CUtile::Create_DummyString();
	m_pMovementTrail = dynamic_cast<CE_RectTrail*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_RectTrail", pDummyString));
	NULL_CHECK_RETURN(m_pMovementTrail, E_FAIL);
	m_pMovementTrail->Set_Parent(this);
	m_pMovementTrail->SetUp_Option(CE_RectTrail::OBJ_ROTWISP);

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

void CRotWisp::Update_MovementTrail(const char * pBoneTag)
{
	CBone*	pBonePtr = m_pModelCom->Get_BonePtr(pBoneTag);
	_matrix SocketMatrix = pBonePtr->Get_CombindMatrix() * m_pModelCom->Get_PivotMatrix();
	_matrix matWorldSocket = SocketMatrix * m_pTransformCom->Get_WorldMatrix();

	m_pMovementTrail->Get_TransformCom()->Set_WorldMatrix(matWorldSocket);

	if(m_bAppear == true && m_bCollect == false)
	m_pMovementTrail->Trail_InputRandomPos(matWorldSocket.r[3]);
}

CRotWisp* CRotWisp::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRotWisp * pInstance = new CRotWisp(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CRotWisp Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CRotWisp::Clone(void* pArg)
{
	CRotWisp * pInstance = new CRotWisp(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CRotWisp Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CRotWisp::Free()
{
	__super::Free();
	Safe_Release(m_pFSM);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pWPOTextureCom);
	Safe_Release(m_pDissolveAlphaTextureCom);

		/* Effect */
	Safe_Release(m_pEffect);
	Safe_Release(m_pMovementTrail);
}
