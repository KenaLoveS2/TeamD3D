#include "stdafx.h"
#include "..\public\EnemyWisp.h"
#include "GameInstance.h"
#include "Bone.h"
#include "Effect_Trail.h"
#include "E_RectTrail.h"

CEnemyWisp::CEnemyWisp(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEffect_Mesh(pDevice, pContext)
{
}

CEnemyWisp::CEnemyWisp(const CEnemyWisp & rhs)
	: CEffect_Mesh(rhs)
{
}

HRESULT CEnemyWisp::Initialize_Prototype()
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

	return S_OK;
}

HRESULT CEnemyWisp::Initialize(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(CGameObject::GAMEOBJECTDESC));

	if (pArg == nullptr)
	{
		GameObjectDesc.TransformDesc.fSpeedPerSec = 2.f;
		GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.f);
	}
	else
		memcpy(&GameObjectDesc, pArg, sizeof(CGameObject::GAMEOBJECTDESC));

	/* 모델을 별도로 설정해 주기 때문에 Desc 일부 변경 해줌 */
	m_eEFfectDesc.eMeshType = CEffect_Base::tagEffectDesc::MESH_END; // Mesh 생성 안함
	m_iTotalDTextureComCnt = 0;	m_iTotalMTextureComCnt = 0;

	FAILED_CHECK_RETURN(__super::Initialize(&GameObjectDesc), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_Components(), E_FAIL);
	m_pModelCom->Set_RootBone("EnemyWisp");
	m_pModelCom->Set_AnimIndex(2);

	FAILED_CHECK_RETURN(Set_WispTrail(), E_FAIL);
	FAILED_CHECK_RETURN(Set_WispEffects(), E_FAIL);

	_matrix matiden = XMMatrixIdentity();
	m_pTransformCom->Set_WorldMatrix(matiden);

	Push_EventFunctions();
	m_eEFfectDesc.bActive = true;

	SetUp_MovementTrail();
	return S_OK;
}

void CEnemyWisp::Tick(_float fTimeDelta)
{
	if (m_eEFfectDesc.bActive == false)
		return;

	if (m_bDissolve == true)
		m_fDissolveTimeDelta += fTimeDelta;
	else
		m_fDissolveTimeDelta = 0.0f;

	__super::Tick(fTimeDelta);
	m_pModelCom->Play_Animation(fTimeDelta);
	if (m_pMovementTrail)m_pMovementTrail->Tick(fTimeDelta);
}

void CEnemyWisp::Late_Tick(_float fTimeDelta)
{
	if (m_eEFfectDesc.bActive == false)
		return;

	__super::Late_Tick(fTimeDelta);

	/* Trail */ 
	if(m_vecChild[CHILD_TRAIL]->Get_Active() == true)
	{
		CBone*	pWispBonePtr = m_pModelCom->Get_BonePtr("EnemyWisp_Jnt2");
		_matrix SocketMatrix = pWispBonePtr->Get_CombindMatrix() * m_pModelCom->Get_PivotMatrix();
		_matrix matWorldSocket = SocketMatrix * m_pTransformCom->Get_WorldMatrix();

		m_vecChild[CHILD_TRAIL]->Get_TransformCom()->Set_WorldMatrix(matWorldSocket);
	}

	Update_MovementTrail("EnemyWisp_Jnt2");
	if (m_pMovementTrail)m_pMovementTrail->Late_Tick(fTimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CEnemyWisp::Render()
{
	if (m_eEFfectDesc.bActive == false)
		return E_FAIL;

	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_ShaderResources(), E_FAIL);

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();
	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture");
		m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_SPECULAR, "g_ReamTexture");
		m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_AMBIENT, "g_LineTexture");
		m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_MASK, "g_ShapeTexture");
		m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_ALPHA, "g_SmoothTexture");

		m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", 0);
	}

	return S_OK;
}

_bool CEnemyWisp::IsActiveState()
{
	if (m_eEFfectDesc.bActive == false)
		return false;

	m_pModelCom->Set_AnimIndex(3);
	_float4 vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	m_pTransformCom->Set_Position(vPos);

	if(m_pModelCom->Get_AnimationProgress() > 0.8f)
		m_bDissolve = true;

	if (m_pModelCom->Get_AnimIndex() == 3 && 
		m_pModelCom->Get_AnimationFinish() == true)
	{
		m_eEFfectDesc.bActive = false;

		for (auto& pChild : m_vecChild)
			pChild->Set_Active(false);

		return true;
	}

	return false;
}

void CEnemyWisp::Push_EventFunctions()
{
	TurnOnTrail(true, 0.f);
	TurnOnBack(true, 0.f);
	TurnOnGround(true, 0.f);
	TurnOnParticle(true, 0.f);

	TurnOffTrail(true, 0.f);
	TurnOffBack(true, 0.f);
	TurnOffGround(true, 0.f);
	TurnOffParticle(true, 0.f);
}

void CEnemyWisp::Calc_RootBoneDisplacement(_fvector vDisplacement)
{
	_vector	vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	vPos = vPos + vDisplacement;
	m_pTransformCom->Set_Translation(vPos, vDisplacement);
}

void CEnemyWisp::ImGui_AnimationProperty()
{
	m_pModelCom->Imgui_RenderProperty();
}

void CEnemyWisp::Imgui_RenderProperty()
{
	if (ImGui::Button("Dissolve"))
		m_bDissolve = !m_bDissolve;
}

HRESULT CEnemyWisp::SetUp_Components()
{
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Shader_VtxEffectAnimModel", L"Com_Shader", (CComponent**)&m_pShaderCom), E_FAIL);
	FAILED_CHECK_RETURN(__super::Add_Component(g_LEVEL_FOR_COMPONENT, L"Prototype_Component_Model_EnemyWisp", L"Com_Model", (CComponent**)&m_pModelCom, nullptr, this), E_FAIL);
	FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(0, WJTextureType_DIFFUSE, TEXT("../Bin/Resources/Anim/Enemy/EnemyWisp/Texture/Noise_cloudsmed.png")), E_FAIL);
	FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(0, WJTextureType_SPECULAR, TEXT("../Bin/Resources/Anim/Enemy/EnemyWisp/Texture/T_Deadzone_REAM.png")), E_FAIL);
	FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(0, WJTextureType_AMBIENT, TEXT("../Bin/Resources/Anim/Enemy/EnemyWisp/Texture/T_Black_Linear.png")), E_FAIL);
	FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(0, WJTextureType_MASK, TEXT("../Bin/Resources/Anim/Enemy/EnemyWisp/Texture/E_Effect_0.png")), E_FAIL);
	FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(0, WJTextureType_ALPHA, TEXT("../Bin/Resources/Anim/Enemy/EnemyWisp/Texture/T_GR_Noise_Smooth_A.png")), E_FAIL);

	return S_OK;
}

HRESULT CEnemyWisp::SetUp_ShaderResources()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);

	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_bDissolve", &m_bDissolve, sizeof(_bool)), E_FAIL);

	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_fDissolveTime", &m_fDissolveTimeDelta, sizeof(_float)), E_FAIL);

	return S_OK;
}

HRESULT CEnemyWisp::Set_WispTrail()
{
	/* Set Trail */
	CEffect_Base* pEffectBase = nullptr;
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	pEffectBase = dynamic_cast<CEffect_Base*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_EnemyWispTrail", L"EnemyWispTrail"));
	NULL_CHECK_RETURN(pEffectBase, E_FAIL);
	pEffectBase->Set_Parent(this);
	m_vecChild.push_back(pEffectBase);

	RELEASE_INSTANCE(CGameInstance);
	/* Set Trail */

	return S_OK;
}

HRESULT CEnemyWisp::Set_WispEffects()
{	
	CEffect_Base* pEffectBase = nullptr;
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	pEffectBase = dynamic_cast<CEffect_Base*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_EnemyWispBackground", L"EnemyWispBack"));
	NULL_CHECK_RETURN(pEffectBase, E_FAIL);
	m_vecChild.push_back(pEffectBase);

	pEffectBase = dynamic_cast<CEffect_Base*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_EnemyWispGround", L"EnemyWispGround"));
	NULL_CHECK_RETURN(pEffectBase, E_FAIL);
	m_vecChild.push_back(pEffectBase);

	pEffectBase = dynamic_cast<CEffect_Base*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_EnemyWispParticle", L"EnemyWispParticle"));
	NULL_CHECK_RETURN(pEffectBase, E_FAIL);
	m_vecChild.push_back(pEffectBase);

	for (auto& pChild : m_vecChild)
		pChild->Set_Parent(this);

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

void CEnemyWisp::TurnOnTrail(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CEnemyWisp::TurnOnTrail);
		return;
	}
	m_vecChild[CHILD_TRAIL]->Set_Active(true);
}

void CEnemyWisp::TurnOnBack(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CEnemyWisp::TurnOnBack);
		return;
	}
	m_vecChild[CHILD_BACK]->Set_Active(true);
	if(m_pParent != nullptr)
	{
		_float4 vPos = m_pParent->Get_TransformCom()->Get_State(CTransform::STATE_TRANSLATION);
		m_vecChild[CHILD_BACK]->Set_Position(vPos);
	}
}

void CEnemyWisp::TurnOnGround(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CEnemyWisp::TurnOnGround);
		return;
	}
	m_vecChild[CHILD_GROUND]->Set_Active(true);
	if (m_pParent != nullptr)
	{
		_float4 vPos = m_pParent->Get_TransformCom()->Get_State(CTransform::STATE_TRANSLATION);
		m_vecChild[CHILD_GROUND]->Set_Position(vPos);
	}
}

void CEnemyWisp::TurnOnParticle(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CEnemyWisp::TurnOnParticle);
		return;
	}
	m_vecChild[CHILD_PARTICLE]->Set_Active(true);
	if (m_pParent != nullptr)
	{
		_float4 vPos = m_pParent->Get_TransformCom()->Get_State(CTransform::STATE_TRANSLATION);
		m_vecChild[CHILD_PARTICLE]->Set_Position(vPos);
	}
}

void CEnemyWisp::TurnOffTrail(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CEnemyWisp::TurnOffTrail);
		return;
	}
	m_vecChild[CHILD_TRAIL]->Set_Active(false);
}

void CEnemyWisp::TurnOffBack(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CEnemyWisp::TurnOffBack);
		return;
	}
	m_vecChild[CHILD_BACK]->Set_Active(false);
}

void CEnemyWisp::TurnOffGround(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CEnemyWisp::TurnOffGround);
		return;
	}
	m_vecChild[CHILD_GROUND]->Set_Active(false);
}

void CEnemyWisp::TurnOffParticle(_bool bIsInit, _float fTimeDelta)
{
	if (bIsInit == true)
	{
		const _tchar* pFuncName = __FUNCTIONW__;
		CGameInstance::GetInstance()->Add_Function(this, pFuncName, &CEnemyWisp::TurnOffParticle);
		return;
	}
	m_vecChild[CHILD_PARTICLE]->Set_Active(false);
}

HRESULT CEnemyWisp::SetUp_MovementTrail()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	/* Movement particle */
	_tchar* pDummyString = CUtile::Create_DummyString();
	m_pMovementTrail = dynamic_cast<CE_RectTrail*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_RectTrail", pDummyString));
	NULL_CHECK_RETURN(m_pMovementTrail, E_FAIL);
	m_pMovementTrail->Set_Parent(this);
	m_pMovementTrail->SetUp_Option(CE_RectTrail::OBJ_MONSTER);

	return S_OK;
}

void CEnemyWisp::Update_MovementTrail(const char * pBoneTag)
{
	CBone*	pBonePtr = m_pModelCom->Get_BonePtr(pBoneTag);
	_matrix SocketMatrix = pBonePtr->Get_CombindMatrix() * m_pModelCom->Get_PivotMatrix();
	_matrix matWorldSocket = SocketMatrix * m_pTransformCom->Get_WorldMatrix();

	m_pMovementTrail->Get_TransformCom()->Set_WorldMatrix(matWorldSocket);
	m_pMovementTrail->Trail_InputRandomPos(matWorldSocket.r[3]);
}

CEnemyWisp * CEnemyWisp::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CEnemyWisp * pInstance = new CEnemyWisp(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CEnemyWisp Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CEnemyWisp::Clone(void * pArg)
{
	CEnemyWisp * pInstance = new CEnemyWisp(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CEnemyWisp Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEnemyWisp::Free()
{
	__super::Free();

	Safe_Release(m_pMovementTrail);
}
