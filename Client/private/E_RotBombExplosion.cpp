#include "stdafx.h"
#include "..\public\E_RotBombExplosion.h"
#include "GameInstance.h"
#include "Effect_Mesh_T.h"

CE_RotBombExplosion::CE_RotBombExplosion(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEffect_Mesh(pDevice, pContext)
{
}

CE_RotBombExplosion::CE_RotBombExplosion(const CE_RotBombExplosion & rhs)
	:CEffect_Mesh(rhs)
{
	
}

HRESULT CE_RotBombExplosion::Initialize_Prototype(const _tchar* pFilePath)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	if (FAILED(Load_E_Desc(pFilePath)))
		return E_FAIL;

	return S_OK;
}

HRESULT CE_RotBombExplosion::Initialize(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 2.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	/* Component */
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(),
		TEXT("Prototype_Component_Shader_VtxEffectModel"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;

	Set_ModelCom(m_eEFfectDesc.eMeshType);
	/* ~Component */

	for (auto& pChild : m_vecChild)
		pChild->Set_Parent(this);

	m_eEFfectDesc.bActive = false;
	memcpy(&m_SaveInitWorldMatrix, &m_InitWorldMatrix, sizeof(_float4x4));
	return S_OK;
}

HRESULT CE_RotBombExplosion::Late_Initialize(void * pArg)
{	

	return S_OK;
}

void CE_RotBombExplosion::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (m_eEFfectDesc.bActive == true)
	{
		for (auto& pChild : m_vecChild)
			pChild->Set_Active(true);

		m_fTimeDelta += fTimeDelta;
		_float3 vScaled = m_pTransformCom->Get_Scaled();

		if (vScaled.x > 3.f) // 내 스케일이 3보다 커지면 현재 크기 유지
		{
			m_fBombTime += fTimeDelta;
			m_vecChild[0]->Set_Active(false); // 자식은 끄고 내 크기 유지

			if (m_fBombTime > 4.f) // 현재 상태에서 4초가 지나면 
			{
				m_bBomb = true;	   // Bomb 상태전환
				dynamic_cast<CEffect_Mesh_T*>(m_vecChild[0])->Set_Dissolve(true);
			}
		}
		else
		{
			vScaled *= fTimeDelta + 1.3f;
			m_pTransformCom->Set_Scaled(vScaled);
			m_vecChild[0]->Set_AddScale(fTimeDelta + 1.3f);
		}
	}

	if (m_bBomb) // 사라짐
	{
		m_fDissolveTime += fTimeDelta;
		dynamic_cast<CEffect_Mesh_T*>(m_vecChild[0])->Set_DissolveTime(m_fDissolveTime);
		if (m_fDissolveTime > 1.f)
		{
			m_bBomb = false;
			dynamic_cast<CEffect_Mesh_T*>(m_vecChild[0])->Set_Dissolve(false);
			dynamic_cast<CEffect_Mesh_T*>(m_vecChild[0])->Set_DissolveTime(0.0f);
			m_eEFfectDesc.bActive = false;
			m_fDissolveTime = 0.0f;
			m_fBombTime = 0.0f;

			m_pTransformCom->Set_Scaled(_float3(0.9f, 0.9f, 0.9f));
			for (auto& pChild : m_vecChild)
				pChild->Set_Scale(_float3(1.f, 1.f, 1.f));
		}
	}
}

void CE_RotBombExplosion::Late_Tick(_float fTimeDelta)
{
	if (m_eEFfectDesc.bActive == false)
		return;

 	//if (m_pParent != nullptr)
 	//	Set_Matrix();

	__super::Late_Tick(fTimeDelta);

	if (nullptr != m_pRendererCom)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_EFFECT, this);
	}
}

HRESULT CE_RotBombExplosion::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	if (m_pModelCom != nullptr && m_pShaderCom != nullptr)
		m_pModelCom->Render(m_pShaderCom, 0, nullptr, m_eEFfectDesc.iPassCnt);

	return S_OK;
}

HRESULT CE_RotBombExplosion::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_bDissolve", &m_bBomb, sizeof(_bool))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_fDissolveTime", &m_fDissolveTime, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

void CE_RotBombExplosion::Imgui_RenderProperty()
{
	if (ImGui::Button("Active"))
		m_eEFfectDesc.bActive = !m_eEFfectDesc.bActive;
}

CE_RotBombExplosion * CE_RotBombExplosion::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar* pFilePath)
{
	CE_RotBombExplosion * pInstance = new CE_RotBombExplosion(pDevice,pContext);

	if (FAILED(pInstance->Initialize_Prototype(pFilePath)))
	{
		MSG_BOX("CE_RotBombExplosion Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CE_RotBombExplosion::Clone(void * pArg)
{
	CE_RotBombExplosion * pInstance = new CE_RotBombExplosion(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CE_RotBombExplosion Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CE_RotBombExplosion::Free()
{
	__super::Free();
}
