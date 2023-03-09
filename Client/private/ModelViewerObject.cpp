#include "stdafx.h"
#include "..\public\ModelViewerObject.h"
#include "GameInstance.h"


CModelViewerObject::CModelViewerObject(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CEnviromentObj(pDevice, pContext)
{
}

CModelViewerObject::CModelViewerObject(const CModelViewerObject & rhs)
	: CEnviromentObj(rhs)
{
}

void CModelViewerObject::Set_ViewerCamZPos(_float fZPos)
{
		m_fViewerCamZPos = fZPos;
	
		_matrix		matWorld, matScale, matRotX, matRotY, matRotZ, matTrans;

		matScale = XMMatrixScaling(1.f, 1.f, 1.f);
		matRotX = XMMatrixRotationX(XMConvertToRadians(m_fViewerCame_XAngle));
		matRotY = XMMatrixRotationY(XMConvertToRadians(0.f));
		matRotZ = XMMatrixRotationZ(XMConvertToRadians(0.f));
		matTrans = XMMatrixTranslation(0.f, m_fViewerCamYPos, m_fViewerCamZPos);
		matWorld = matScale * matRotX * matRotY * matRotZ * matTrans;

		_vector vTargetPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
		_matrix matProj;

		XMStoreFloat4x4(&m_ViewerCamView, XMMatrixInverse(nullptr, matWorld));
		matProj = XMMatrixPerspectiveFovLH(XMConvertToRadians(60.f), g_iWinSizeX / _float(g_iWinSizeY), 0.2f, 300.f);

		XMStoreFloat4x4(&m_ViewerCamProv, XMMatrixInverse(nullptr, matProj));

}

HRESULT CModelViewerObject::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CModelViewerObject::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_bRenderActive = true;

	_matrix		matWorld, matScale, matRotX, matRotY, matRotZ, matTrans;

	matScale = XMMatrixScaling(1.f, 1.f, 1.f);
	matRotX = XMMatrixRotationX(XMConvertToRadians(60.f));
	matRotY = XMMatrixRotationY(XMConvertToRadians(0.f));
	matRotZ = XMMatrixRotationZ(XMConvertToRadians(0.f));
	matTrans = XMMatrixTranslation(0.f, 40.f, -12.f);

	matWorld = matScale * matRotX * matRotY * matRotZ * matTrans;

	_matrix matProj;

	XMStoreFloat4x4(&m_ViewerCamView, XMMatrixInverse(nullptr, matWorld));
	matProj =XMMatrixPerspectiveFovLH(XMConvertToRadians(60.0f), g_iWinSizeX / _float(g_iWinSizeY) ,0.2f, 300.f);

	XMStoreFloat4x4(&m_ViewerCamProv, XMMatrixInverse(nullptr, matProj));


	return S_OK;
}

void CModelViewerObject::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (CGameInstance::GetInstance()->Key_Pressing(DIK_Q))
	{
		m_fViewerCamYPos -= m_fViewerCamYPos_Num;
		m_fViewerCame_XAngle -= m_fViewerCamXAngle_Num;
		_matrix		matWorld, matScale, matRotX, matRotY, matRotZ, matTrans;

		matScale = XMMatrixScaling(1.f, 1.f, 1.f);
		matRotX = XMMatrixRotationX(XMConvertToRadians(m_fViewerCame_XAngle));
		matRotY = XMMatrixRotationY(XMConvertToRadians(0.f));
		matRotZ = XMMatrixRotationZ(XMConvertToRadians(0.f));
		matTrans = XMMatrixTranslation(0.f, m_fViewerCamYPos, m_fViewerCamZPos);
		matWorld = matScale * matRotX * matRotY * matRotZ * matTrans;

		_vector vTargetPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
		_matrix matProj;

		XMStoreFloat4x4(&m_ViewerCamView, XMMatrixInverse(nullptr, matWorld));
		matProj = XMMatrixPerspectiveFovLH(XMConvertToRadians(60.f), g_iWinSizeX / _float(g_iWinSizeY), 0.2f, 300.f);

		XMStoreFloat4x4(&m_ViewerCamProv, XMMatrixInverse(nullptr, matProj));
	}

	if (CGameInstance::GetInstance()->Key_Pressing(DIK_E))
	{
		m_fViewerCamYPos += m_fViewerCamYPos_Num;
		m_fViewerCame_XAngle += m_fViewerCamXAngle_Num;
		_matrix		matWorld, matScale, matRotX, matRotY, matRotZ, matTrans;

		matScale = XMMatrixScaling(1.f, 1.f, 1.f);
		matRotX = XMMatrixRotationX(XMConvertToRadians(m_fViewerCame_XAngle));
		matRotY = XMMatrixRotationY(XMConvertToRadians(0.f));
		matRotZ = XMMatrixRotationZ(XMConvertToRadians(0.f));
		matTrans = XMMatrixTranslation(0.f, m_fViewerCamYPos, m_fViewerCamZPos);
		matWorld = matScale * matRotX * matRotY * matRotZ * matTrans;

		_vector vTargetPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
		_matrix matProj;

		XMStoreFloat4x4(&m_ViewerCamView, XMMatrixInverse(nullptr, matWorld));
		matProj = XMMatrixPerspectiveFovLH(XMConvertToRadians(60.f), g_iWinSizeX / _float(g_iWinSizeY), 0.2f, 300.f);

		XMStoreFloat4x4(&m_ViewerCamProv, XMMatrixInverse(nullptr, matProj));
	}

	if (CGameInstance::GetInstance()->Key_Pressing(DIK_R))
	{
		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta *0.05f);
	}
	if (CGameInstance::GetInstance()->Key_Pressing(DIK_1))
	{
		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta *-0.05f);
	}

	//static float vAngle[3] = { 50.f,0.f,0.f };
	//static float vPos[3] = { 0.f,25.f,-12.f };
	//ImGui::InputFloat3("Angle", vAngle);
	//ImGui::InputFloat3("Pos", vPos);

	//_matrix		matWorld, matScale, matRotX, matRotY, matRotZ, matTrans;

	//matScale = XMMatrixScaling(1.f, 1.f, 1.f);
	//matRotX = XMMatrixRotationX(XMConvertToRadians(vAngle[0]));
	//matRotY = XMMatrixRotationY(XMConvertToRadians(vAngle[1]));
	//matRotZ = XMMatrixRotationZ(XMConvertToRadians(vAngle[2]));
	//matTrans = XMMatrixTranslation(vPos[0], vPos[1], vPos[2]);
	//matWorld = matScale * matRotX * matRotY * matRotZ * matTrans;

	//_vector vTargetPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	//_matrix matProj;

	//XMStoreFloat4x4(&m_ViewerCamView, XMMatrixInverse(nullptr, matWorld));
	//matProj = XMMatrixPerspectiveFovLH(XMConvertToRadians(120.0f), g_iWinSizeX / _float(g_iWinSizeY), 0.2f, 300.f);

	//XMStoreFloat4x4(&m_ViewerCamProv, XMMatrixInverse(nullptr, matProj));


}

void CModelViewerObject::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_VIEWER, this);
}

HRESULT CModelViewerObject::Render()
{
	if (m_pModelCom == nullptr)
		return S_OK;

	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		/* 이 모델을 그리기위한 셰이더에 머테리얼 텍스쳐를 전달하낟. */
		m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture");
		m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_NORMALS, "g_NormalTexture");
		//m_pE_R_AoTexCom->Bind_ShaderResource(m_pShaderCom, "g_ERAOTexture");
		m_pModelCom->Render(m_pShaderCom, i, nullptr, 5);
	}
	return S_OK;

}

HRESULT CModelViewerObject::Change_Model(_uint iCurLevel , const _tchar* pModelTag)
{
	assert(pModelTag != nullptr  && "CModelViewerObject::Change_Model");

	if (m_pModelCom != nullptr)
	{
		Delete_Component(TEXT("Com_Model"));
		Safe_Release(m_pModelCom);
	}

	if (FAILED(__super::Add_Component(iCurLevel, pModelTag, TEXT("Com_Model"),
		(CComponent**)&m_pModelCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CModelViewerObject::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_VtxModelTess"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;
	
	return S_OK;
}

HRESULT CModelViewerObject::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)			 // 카메라 위치 설정하기
		return E_FAIL;


	//matWorld *= XMLoadFloat4x4(&m_ViewerCamView);
	//m_pTransformCom->Set_WorldMatrix(matWorld);

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	//if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
	//	return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &m_ViewerCamView)))
		return E_FAIL;
	/*if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &m_ViewerCamProv)))
		return E_FAIL;*/

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CModelViewerObject * CModelViewerObject::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CModelViewerObject*		pInstance = new CModelViewerObject(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CModelViewerObject");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CModelViewerObject::Clone(void * pArg)
{
	CModelViewerObject*		pInstance = new CModelViewerObject(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CModelViewerObject");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CModelViewerObject::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
