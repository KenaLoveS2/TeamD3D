#include "stdafx.h"
#include "..\public\E_Hieroglyph.h"
#include "GameInstance.h"
#include "Monster.h"

CE_Hieroglyph::CE_Hieroglyph(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEffect(pDevice, pContext)
{
}

CE_Hieroglyph::CE_Hieroglyph(const CE_Hieroglyph & rhs)
	:CEffect(rhs)
{
	
}

HRESULT CE_Hieroglyph::Initialize_Prototype(const _tchar* pFilePath)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	if (FAILED(__super::Load_E_Desc(pFilePath)))
		return E_FAIL;

	return S_OK;
}

HRESULT CE_Hieroglyph::Initialize(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 2.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	m_eEFfectDesc.bActive = false;
	m_eEFfectDesc.vScale = XMVectorSet(0.7f, 0.7f, 0.7f, 1.f);
	return S_OK;
}

HRESULT CE_Hieroglyph::Late_Initialize(void * pArg)
{	
	return S_OK;
}

void CE_Hieroglyph::Tick(_float fTimeDelta)
{
 	if (m_eEFfectDesc.bActive == false)
 		return;

	__super::Tick(fTimeDelta);

	/* TurnOff System */
	if (m_fShaderBindTime > 3.f)
	{
		m_pTransformCom->Set_PositionY(-1000.f);
		m_eEFfectDesc.bActive = false;
		m_fShaderBindTime = 0.0f;
	}
}

void CE_Hieroglyph::Late_Tick(_float fTimeDelta)
{
	if (m_eEFfectDesc.bActive == false)
		return;

	__super::Late_Tick(fTimeDelta);
}

HRESULT CE_Hieroglyph::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_ShaderResources(), E_FAIL);

	return S_OK;
}

_int CE_Hieroglyph::Execute_Collision(CGameObject* pTarget, _float3 vCollisionPos, _int iColliderIndex)
{
	return 0;
}

void CE_Hieroglyph::ImGui_PhysXValueProperty()
{
	__super::ImGui_PhysXValueProperty();
}

void CE_Hieroglyph::Set_TexRandomPrint(_float fHeight)
{
	m_eEFfectDesc.fWidthFrame = floor(CUtile::Get_RandomFloat(0.0f, m_eEFfectDesc.iSeparateWidth * 1.0f));

	if (fHeight != -1)
		m_eEFfectDesc.fHeightFrame = fHeight;
	else
		m_eEFfectDesc.fHeightFrame = floor(CUtile::Get_RandomFloat(0.0f, m_eEFfectDesc.iSeparateHeight * 1.0f));
}

HRESULT CE_Hieroglyph::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	return S_OK;
}

HRESULT CE_Hieroglyph::SetUp_Components()
{	
	return S_OK;
}

void CE_Hieroglyph::Imgui_RenderProperty()
{
	ImGui::Checkbox("Active", &m_eEFfectDesc.bActive);

	/* 이거 바꾸면 됨 */
	ImGui::InputFloat("g_WidthFrame", &m_eEFfectDesc.fWidthFrame);
	ImGui::InputFloat("g_HeightFrame", &m_eEFfectDesc.fHeightFrame);
	/* 이거 바꾸면 됨 */

	ImGui::InputInt("g_SeparateWidth", &m_eEFfectDesc.iSeparateWidth);
	ImGui::InputInt("g_SeparateHeight", &m_eEFfectDesc.iSeparateHeight);
}

CE_Hieroglyph * CE_Hieroglyph::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar* pFilePath)
{
	CE_Hieroglyph * pInstance = new CE_Hieroglyph(pDevice,pContext);

	if (FAILED(pInstance->Initialize_Prototype(pFilePath)))
	{
		MSG_BOX("CE_Hieroglyph Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CE_Hieroglyph::Clone(void * pArg)
{
	CE_Hieroglyph * pInstance = new CE_Hieroglyph(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CE_Hieroglyph Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CE_Hieroglyph::Free()
{
	__super::Free();
}
