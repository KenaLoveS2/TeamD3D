#include "stdafx.h"
#include "..\public\HatCart.h"
#include "GameInstance.h"
#include "ControlMove.h"
#include "Interaction_Com.h"
#include "Kena.h"
#include "MannequinRot.h"

CHatCart::CHatCart(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CEnviromentObj(pDevice, pContext)
	, m_pPlayer(nullptr)
	, m_pUIShown(nullptr)
{
}

CHatCart::CHatCart(const CHatCart& rhs)
	: CEnviromentObj(rhs)
	, m_pPlayer(nullptr)
	, m_pUIShown(nullptr)
{
}

HRESULT CHatCart::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CHatCart::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_bRenderActive = true;
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_STATIC_SHADOW, this);

	m_pUIShown = new _bool;
	*m_pUIShown = false;

	return S_OK;
}

HRESULT CHatCart::Late_Initialize(void* pArg)
{
	//return S_OK; // 나중에 주석해제하셈

	/*Player_Need*/
	m_pPlayer = dynamic_cast<CKena*>(CGameInstance::GetInstance()->Get_GameObjectPtr(g_LEVEL, L"Layer_Player", L"Kena"));
	if (m_pPlayer == nullptr) return E_FAIL;

	m_pPlayer->Set_HatCartPtr(this);

	Create_MannequinRot();

	return S_OK;
}

void CHatCart::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	/*Culling*/
	//_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	//_float4 vCamPos = CGameInstance::GetInstance()->Get_CamPosition();
	//_vector camPos = XMLoadFloat4(&vCamPos);
	//const _vector	 vDir = camPos - vPos;
	//
	//_float f = XMVectorGetX(XMVector4Length(vDir));
	//if (100.f <= XMVectorGetX(XMVector4Length(vDir)))
	//	m_bRenderCheck = false;
	//if(m_bRenderCheck ==true)
	//	m_bRenderCheck = CGameInstance::GetInstance()->isInFrustum_WorldSpace(vPos, 15.f);
	//if (m_bRenderCheck)
	/*~Culling*/

	if(m_pMannequinRot != nullptr)
		m_pMannequinRot->Tick(fTimeDelta);
}

void CHatCart::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	//_matrix  WolrdMat = m_pTransformCom->Get_WorldMatrix();

	/* compare distance */
	if (m_pPlayer != nullptr)
	{
		_float fDist = m_pTransformCom->Calc_Distance_XZ(m_pPlayer->Get_TransformCom());
		//_float4 vPlayerPos = m_pPlayer->Get_TransformCom()->Get_Position();
		//_float4 vPos = m_pTransformCom->Get_Position();
		//_float	fDist = (vPlayerPos - vPos).Length();

		if (fDist <= 10.f)
		{
			if (CGameInstance::GetInstance()->Key_Down(DIK_Q))
			{
				CUI_ClientManager::UI_PRESENT eCart = CUI_ClientManager::HATCART_;
				m_CartDelegator.broadcast(eCart, m_pPlayer, m_pUIShown);

				if (*m_pUIShown == true) /* Cart Open */
				{
					Update_MannequinRotMatrix();
					m_pMannequinRot->Start_FashiomShow();
				}
				else /* Cart Close */
				{
					m_pMannequinRot->End_FashiomShow();
				}
				
			}
		}
	}

	if (m_pRendererCom) //&& m_bRenderActive && m_bRenderCheck)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);

	if (m_pMannequinRot != nullptr)
		m_pMannequinRot->Late_Tick(fTimeDelta);
}

HRESULT CHatCart::Render()
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

		//if (i == 0 || i == 1)
		//{
		//	FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_AMBIENT_OCCLUSION, "g_MRAOTexture"), E_FAIL);
		//	FAILED_CHECK_RETURN(m_pModelCom->Render(m_pShaderCom, i, nullptr, 6), E_FAIL);
		//}
		//else if (i == 2)
		//{

		//	FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_ROUGHNESS, "g_RoughnessTexture"), E_FAIL);
		//	FAILED_CHECK_RETURN(m_pModelCom->Render(m_pShaderCom, i, nullptr, 14), E_FAIL);
		//}
		//else if (i == 3 || i == 4)
	//	{
		//	FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_COMP_H_R_AO, "g_HRAOTexture"), E_FAIL);
			FAILED_CHECK_RETURN(m_pModelCom->Render(m_pShaderCom, i, nullptr, 15), E_FAIL);
		//}
	}

	return S_OK;
}

HRESULT CHatCart::RenderShadow()
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

void CHatCart::Imgui_RenderProperty()
{

	float fTrans[3] = { m_vRotPivotTranlation.x, m_vRotPivotTranlation.y, m_vRotPivotTranlation.z };
	ImGui::DragFloat3("Pivot Pos", fTrans, 0.01f, -100.f, 100.0f);
	memcpy(&m_vRotPivotTranlation, fTrans, sizeof(_float3));

	float fRot[3] = { m_vRotPivotRotation.x, m_vRotPivotRotation.y, m_vRotPivotRotation.z };
	ImGui::DragFloat3("Pivot Rot", fRot, 0.01f, -100.f, 100.0f);
	memcpy(&m_vRotPivotRotation, fRot, sizeof(_float3));
}

void CHatCart::ImGui_AnimationProperty()
{
}

void CHatCart::ImGui_PhysXValueProperty()
{
}

HRESULT CHatCart::Add_AdditionalComponent(_uint iLevelIndex, const _tchar* pComTag, COMPONENTS_OPTION eComponentOption)
{
	__super::Add_AdditionalComponent(iLevelIndex, pComTag, eComponentOption);

	/* For.Com_CtrlMove */
	if (eComponentOption == COMPONENTS_CONTROL_MOVE)
	{
		if (FAILED(__super::Add_Component(iLevelIndex, TEXT("Prototype_Component_ControlMove"), pComTag,
			(CComponent**)&m_pControlMoveCom)))
			return E_FAIL;
	}
	/* For.Com_Interaction */
	else if (eComponentOption == COMPONENTS_INTERACTION)
	{
		if (FAILED(__super::Add_Component(iLevelIndex, TEXT("Prototype_Component_Interaction_Com"), pComTag,
			(CComponent**)&m_pInteractionCom)))
			return E_FAIL;
	}
	else
		return S_OK;

	return S_OK;

}

_int CHatCart::Execute_Collision(CGameObject* pTarget, _float3 vCollisionPos, _int iColliderIndex)
{
	return _int();
}

_int CHatCart::Execute_TriggerTouchFound(CGameObject* pTarget, _uint iTriggerIndex, _int iColliderIndex)
{
	return _int();
}

_int CHatCart::Execute_TriggerTouchLost(CGameObject* pTarget, _uint iTriggerIndex, _int iColliderIndex)
{
	return _int();
}



HRESULT CHatCart::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (m_EnviromentDesc.iCurLevel == 0)
		m_EnviromentDesc.iCurLevel = g_LEVEL;

	/* For.Com_Model */ 	
	if (FAILED(__super::Add_Component(g_LEVEL, m_EnviromentDesc.szModelTag.c_str(), TEXT("Com_Model"),
		(CComponent**)&m_pModelCom, nullptr, this)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_VtxModelInstance"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CHatCart::SetUp_ShaderResources()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);
	FAILED_CHECK_RETURN(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"), E_FAIL);
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_fFar", pGameInstance->Get_CameraFar(), sizeof(float)), E_FAIL);
	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CHatCart::SetUp_ShadowShaderResources()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);
	FAILED_CHECK_RETURN(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"), E_FAIL);
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_LIGHTVIEW)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_fFar", pGameInstance->Get_CameraFar(), sizeof(float)), E_FAIL);
	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

CHatCart* CHatCart::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CHatCart* pInstance = new CHatCart(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CHatCart");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CHatCart::Clone(void* pArg)
{
	CHatCart* pInstance = new CHatCart(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CHatCart");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CHatCart::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);

	Safe_Release(m_pControlMoveCom);
	Safe_Release(m_pInteractionCom);

	Safe_Release(m_pMannequinRot);	

	Safe_Delete(m_pUIShown);
}

void CHatCart::Create_MannequinRot()
{
	

	// m_pTransformCom->Set_Position(_float4(2.f, -1.f, 2.f, 1.f));

	

	m_pMannequinRot = (CMannequinRot*)CGameInstance::GetInstance()->Clone_GameObject(TEXT("Prototype_GameObject_MannequinRot"), TEXT("MannequinRot"));
	assert(m_pMannequinRot && "CHatCart::Initialize()");
	
	Update_MannequinRotMatrix();
}

void CHatCart::Update_MannequinRotMatrix()
{
	_float4x4 Matrix;
	XMStoreFloat4x4(&Matrix, XMMatrixRotationX(m_vRotPivotRotation.x) * XMMatrixRotationY(m_vRotPivotRotation.y) * XMMatrixRotationZ(m_vRotPivotRotation.z) *
		XMMatrixTranslation(m_vRotPivotTranlation.x, m_vRotPivotTranlation.y, m_vRotPivotTranlation.z) * m_pTransformCom->Get_WorldMatrix());
	m_pMannequinRot->Set_WorldMatrix(Matrix);
}

void CHatCart::Change_MannequinHat(_uint iHatIndex)
{
	if (*m_pUIShown == false) return;

	m_pMannequinRot->Change_Hat(iHatIndex);
}
