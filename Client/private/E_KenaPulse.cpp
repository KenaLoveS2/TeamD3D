#include "stdafx.h"
#include "..\public\E_KenaPulse.h"
#include "GameInstance.h"
#include "Effect_Trail.h"
#include "E_KenaPulseCloud.h"
#include "E_KenaPulseDot.h"
#include "Kena.h"
#include "Kena_Status.h"
#include "Monster.h"

CE_KenaPulse::CE_KenaPulse(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEffect_Mesh(pDevice, pContext)
{
}

CE_KenaPulse::CE_KenaPulse(const CE_KenaPulse & rhs)
	:CEffect_Mesh(rhs)
{
	
}

void CE_KenaPulse::Set_InitMatrixScaled(_float3 vScale)
{
	_float4 vRight, vUp, vLook;

	XMStoreFloat4(&vRight, XMVector3Normalize(XMLoadFloat4x4(&m_InitWorldMatrix).r[0]) * vScale.x);
	XMStoreFloat4(&vUp, XMVector3Normalize(XMLoadFloat4x4(&m_InitWorldMatrix).r[1])  * vScale.y);
	XMStoreFloat4(&vLook, XMVector3Normalize(XMLoadFloat4x4(&m_InitWorldMatrix).r[2])  *vScale.z);

	memcpy(&m_InitWorldMatrix.m[0][0], &vRight, sizeof vRight);
	memcpy(&m_InitWorldMatrix.m[1][0], &vUp, sizeof vUp);
	memcpy(&m_InitWorldMatrix.m[2][0], &vLook, sizeof vLook);
}

_float3 CE_KenaPulse::Get_InitMatrixScaled()
{
	return _float3(XMVectorGetX(XMVector3Length(XMLoadFloat4x4(&m_InitWorldMatrix).r[0])),
		XMVectorGetX(XMVector3Length(XMLoadFloat4x4(&m_InitWorldMatrix).r[1])),
		XMVectorGetX(XMVector3Length(XMLoadFloat4x4(&m_InitWorldMatrix).r[2])));
}

void CE_KenaPulse::Set_Child()
{
	CEffect_Base* pEffectBase = nullptr;
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	pEffectBase = dynamic_cast<CEffect_Base*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_KenaPulseCloud", L"KenaPulseCloud"));
	NULL_CHECK_RETURN(pEffectBase, );
	m_vecChild.push_back(pEffectBase);

	pEffectBase = dynamic_cast<CEffect_Base*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_KenaPulseDot", L"KenaPulseDot"));
	NULL_CHECK_RETURN(pEffectBase, );
	m_vecChild.push_back(pEffectBase);

	for (auto* pChild : m_vecChild)
		pChild->Set_Parent(this);

	RELEASE_INSTANCE(CGameInstance);
}

HRESULT CE_KenaPulse::Initialize_Prototype(const _tchar* pFilePath)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	if (FAILED(Load_E_Desc(pFilePath)))
		return E_FAIL;

	return S_OK;
}

HRESULT CE_KenaPulse::Initialize(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 2.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	/* Component */
	if (FAILED(__super::Add_Component(g_LEVEL, TEXT("Prototype_Component_Texture_PulseShield_Dissolve"), L"Com_DissolveTexture", (CComponent**)&m_pDissolveTexture, this)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(),
		TEXT("Prototype_Component_Shader_VtxEffectModel"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;

	Set_ModelCom(m_eEFfectDesc.eMeshType);
	/* ~Component */

	m_eEFfectDesc.bActive = false;
	memcpy(&m_SaveInitWorldMatrix, &m_InitWorldMatrix, sizeof(_float4x4));

	Set_Child();
	m_eEFfectDesc.vColor = XMVectorSet(0.0f, 116.f, 255.f, 255.f) / 255.f;

	return S_OK;
}

HRESULT CE_KenaPulse::Late_Initialize(void * pArg)
{	
	/* kena Status */
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	m_pKena = (CKena*)pGameInstance->Get_GameObjectPtr(g_LEVEL, TEXT("Layer_Player"), TEXT("Kena"));

	m_pStatus = m_pKena->Get_Status();
	m_eStatus.fCurHp = (_float)m_pStatus->Get_Shield();
	m_eStatus.fMaxHp = (_float)m_pStatus->Get_MaxShield();
	RELEASE_INSTANCE(CGameInstance);
	/* Kena Status */

	_float4 vPos;
	XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));

	m_pTriggerDAta = Create_PxTriggerData(m_szCloneObjectTag, this, TRIGGER_PULSE, CUtile::Float_4to3(vPos), 1.f);
	CPhysX_Manager::GetInstance()->Create_Trigger(m_pTriggerDAta);

	return S_OK;
}

void CE_KenaPulse::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

#pragma region PulseStatus test
//	ImGui::Begin("pulse");
//
//	if (ImGui::Button("recompile"))
//		m_pShaderCom->ReCompile();
//
//	/* State test */
//	static int iType = 0;
//	iType = m_eStatus.eState;
//	ImGui::RadioButton("State_Default", &iType, 0);
//	ImGui::RadioButton("State_Damage", &iType, 1); 
//	ImGui::RadioButton("State_Default2Damage", &iType, 2); 
//
//	_float fRatio = m_eStatus.fCurHp / m_eStatus.fMaxHp;
//	ImGui::InputFloat("Ratio", &fRatio);
//
//	_float2 fHp = { m_eStatus.fCurHp, (_float)m_eStatus.fMaxHp };
//	ImGui::DragFloat2("Hp", (_float*)&fHp, 1.0f, 0.0f, m_eStatus.fMaxHp);
//	m_pStatus->Set_Shield((_int)fHp.x);
//
//	if(iType == 0)
//		m_eStatus.eState = CE_KenaPulse::tagMyStatus::STATE_DEFAULT;
//	else if(iType==1)
//	{
//		m_eStatus.eState = CE_KenaPulse::tagMyStatus::STATE_DAMAGE;
//
//		m_eStatus.fStateDurationTime += fTimeDelta * 1.5f;
//		if (m_eStatus.fStateDurationTime > 2.f)
//		{
//			m_eStatus.eState = CE_KenaPulse::tagMyStatus::STATE_DEFAULT;
//			m_eStatus.fStateDurationTime = 0.0f;
//		}
//	}
//	/* State test */
//
//	static bool alpha_preview = true;
//	static bool alpha_half_preview = false;
//	static bool drag_and_drop = true;
//	static bool options_menu = true;
//	static bool hdr = false;
//
//	ImGuiColorEditFlags misc_flags = (hdr ? ImGuiColorEditFlags_HDR : 0) | (drag_and_drop ? 0 : ImGuiColorEditFlags_NoDragDrop) | (alpha_half_preview ? ImGuiColorEditFlags_AlphaPreviewHalf : (alpha_preview ? ImGuiColorEditFlags_AlphaPreview : 0)) | (options_menu ? 0 : ImGuiColorEditFlags_NoOptions);
//
//	static bool   ref_color = false;
//	static ImVec4 ref_color_v(1.0f, 1.0f, 1.0f, 1.0f);
//
//	static _float4 vSelectColor = { 1.0f, 1.0f, 1.0f, 1.0f };
//	vSelectColor = m_eEFfectDesc.vColor;
//
//	ImGui::ColorPicker4("CurColor##6", (float*)&vSelectColor, ImGuiColorEditFlags_NoInputs | misc_flags, ref_color ? &ref_color_v.x : NULL);
//	ImGui::ColorEdit4("Diffuse##5f", (float*)&vSelectColor, ImGuiColorEditFlags_DisplayRGB | misc_flags);
//	m_eEFfectDesc.vColor = vSelectColor;
//
//	ImGui::End();
#pragma endregion PulseStatus test

  	if (m_eEFfectDesc.bActive == false)
  		return;

	m_fTimeDelta += fTimeDelta;
	Set_Status();

	_float3 vScaled = m_pTransformCom->Get_Scaled();

	switch (m_ePulseType)
	{
	case Client::CE_KenaPulse::PULSE_DEFAULT:

		if (m_bNoActive == true) // Pulse ��
		{
			for (auto& pChild : m_vecChild)
				pChild->Set_Active(false);

			m_fDissolveTime += fTimeDelta;
			_float3 vScale = Get_InitMatrixScaled();
			Set_InitMatrixScaled(vScale * 1.3f);

			_float4 vPos;
			XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
			CPhysX_Manager::GetInstance()->Set_ActorPosition(m_pTriggerDAta->pTriggerStatic, CUtile::Float_4to3(vPos));

			if (vScale.x <= 8.f)
				CPhysX_Manager::GetInstance()->Set_ScalingSphere(m_pTriggerDAta->pTriggerStatic, vScale.x *3.5f);

			if (m_fDissolveTime > 1.f)
			{
				m_eEFfectDesc.bActive = false;
				m_bNoActive = false;
				// m_bDesolve = true;
				memcpy(&m_InitWorldMatrix, &m_SaveInitWorldMatrix, sizeof(_float4x4));
				m_fDissolveTime = 0.0f;
				CPhysX_Manager::GetInstance()->Set_ScalingSphere(m_pTriggerDAta->pTriggerStatic, 0.f);
			}
		}

		if (m_bNoActive == false && m_eEFfectDesc.bActive == true)
		{
			for (auto& pChild : m_vecChild)
				pChild->Set_Active(true);
			// m_bDesolve = true;
		}
		break;

	case Client::CE_KenaPulse::PULSE_PARRY:
		m_eEFfectDesc.iPassCnt = 0;
		m_fTimeDelta += fTimeDelta;

		for (auto& pChild : m_vecChild)
			pChild->Set_Active(false);

		if (vScaled.x > 5.f)
		{
			m_eEFfectDesc.bActive = false;
			m_eEFfectDesc.iPassCnt = 1;

			m_ePulseType = CE_KenaPulse::PULSE_DEFAULT;
			m_pTransformCom->Set_WorldMatrix_float4x4(m_SaveInitWorldMatrix);
		}
		else
		{
			vScaled *= fTimeDelta + 1.1f;
			m_pTransformCom->Set_Scaled(vScaled);
		}
		break;
	}
}

void CE_KenaPulse::Late_Tick(_float fTimeDelta)
{
 	if (m_eEFfectDesc.bActive == false)
 		return;

	if (m_ePulseType == CE_KenaPulse::PULSE_DEFAULT && m_pParent != nullptr)
		Set_Matrix();
	
	__super::Late_Tick(fTimeDelta);

	if (nullptr != m_pRendererCom)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);

		if(m_ePulseType != CE_KenaPulse::PULSE_DEFAULT)
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_EFFECT, this);
	}
}

HRESULT CE_KenaPulse::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	if (m_pModelCom != nullptr && m_pShaderCom != nullptr)
		m_pModelCom->Render(m_pShaderCom, 0, nullptr, m_eEFfectDesc.iPassCnt);

	return S_OK;
}

void CE_KenaPulse::Reset()
{
	dynamic_cast<CE_KenaPulseCloud*>(m_vecChild.front())->ResetSprite();
	dynamic_cast<CE_KenaPulseDot*>(m_vecChild.back())->Set_ShapePosition();

	m_bNoActive = false;
	m_bDesolve = true;
	m_fDissolveTime = 0.f;

	memcpy(&m_InitWorldMatrix, &m_SaveInitWorldMatrix, sizeof(_float4x4));
	Set_InitMatrixScaled(Get_InitMatrixScaled());

	CPhysX_Manager::GetInstance()->Set_ScalingSphere(m_pTriggerDAta->pTriggerStatic, 0.f);
}

_int CE_KenaPulse::Execute_Collision(CGameObject * pTarget, _float3 vCollisionPos, _int iColliderIndex)
{
	_bool bRealAttack = false;
	if (iColliderIndex == (_uint)COL_MONSTER_WEAPON && (bRealAttack = ((CMonster*)pTarget)->IsRealAttack()))
	{
		if (m_pKena->Get_State(CKena::STATE_PULSE) == false)
			return 0;

		// KenaPulse ���ݷ� ���
		m_pStatus->Under_Shield(((CMonster*)pTarget)->Get_MonsterStatusPtr());
	}
	return 0;
}

HRESULT CE_KenaPulse::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	/* Kena Status */
	_float fRatio = m_eStatus.fCurHp / m_eStatus.fMaxHp;
	if (FAILED(m_pShaderCom->Set_RawValue("g_HpRatio", &fRatio, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_PulseState", &m_eStatus.eState, sizeof(_uint))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_DamageDurationTime", &m_eStatus.fStateDurationTime, sizeof(_float))))
		return E_FAIL;
	/* Kena Status */

	//if (FAILED(m_pShaderCom->Set_RawValue("g_bDissolve", &m_bDesolve, sizeof(_bool))))
	//	return E_FAIL;

	//if (m_bDesolve)
	//{
	//	if (FAILED(m_pShaderCom->Set_RawValue("g_fDissolveTime", &m_fDissolveTime, sizeof(_float))))
	//		return E_FAIL;

	//	_float fTextureIdx = m_pDissolveTexture->Get_TextureIdx();
	//	if (FAILED(m_pDissolveTexture->Bind_ShaderResource(m_pShaderCom, "g_DissolveTexture", fTextureIdx)))
	//		return E_FAIL;
	//}
	return S_OK;
}

void CE_KenaPulse::Imgui_RenderProperty()
{
	static _int iType = 0;
	ImGui::RadioButton("Default", &iType, 0);
	ImGui::RadioButton("Pulse", &iType, 1);
	ImGui::RadioButton("Bomb", &iType, 2);

	if (iType == 0)
		m_ePulseType = CE_KenaPulse::PULSE_DEFAULT;
	else if (iType == 1)
		m_ePulseType = CE_KenaPulse::PULSE_PARRY;

	if (ImGui::Button("Active"))
		m_eEFfectDesc.bActive = !m_eEFfectDesc.bActive;

}

void CE_KenaPulse::Set_Status()
{
	m_eStatus.fCurHp = (_float)m_pStatus->Get_Shield();
	m_eStatus.fMaxHp = (_float)m_pStatus->Get_MaxShield();
}

CE_KenaPulse * CE_KenaPulse::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar* pFilePath)
{
	CE_KenaPulse * pInstance = new CE_KenaPulse(pDevice,pContext);

	if (FAILED(pInstance->Initialize_Prototype(pFilePath)))
	{
		MSG_BOX("CE_KenaPulse Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CE_KenaPulse::Clone(void * pArg)
{
	CE_KenaPulse * pInstance = new CE_KenaPulse(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CE_KenaPulse Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CE_KenaPulse::Free()
{
	__super::Free();

	Safe_Release(m_pDissolveTexture);
	
}
