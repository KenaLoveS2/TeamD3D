#include "..\public\EnviromentObj.h"
#include "GameInstance.h"
#include "Utile.h"

CEnviromentObj::CEnviromentObj(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CEnviromentObj::CEnviromentObj(const CEnviromentObj & rhs)
	: CGameObject(rhs)
{	
}

HRESULT CEnviromentObj::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype())) return E_FAIL;	

	return S_OK;
}

HRESULT CEnviromentObj::Initialize(void * pArg)
{
	ZeroMemory(&m_Desc, sizeof(DESC));
	if (pArg) 
	{		
		memcpy(&m_Desc, pArg, sizeof(DESC));
	}
	else
	{
		m_Desc.ObjectDesc.TransformDesc.fRotationPerSec = 90.f;
		m_Desc.ObjectDesc.TransformDesc.fSpeedPerSec = 5.f;
	}

	if (FAILED(__super::Initialize(&m_Desc.ObjectDesc))) return E_FAIL;


	HRESULT SetUp_Component();


	return S_OK;
}

void CEnviromentObj::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CEnviromentObj::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CEnviromentObj::Render()
{
	__super::Render();
}

void CEnviromentObj::Free()
{
	__super::Free();
}