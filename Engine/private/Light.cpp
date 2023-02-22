#include "stdafx.h"
#include "..\public\Light.h"
#include "Shader.h"
#include "VIBuffer_Rect.h"
#include "Utile.h"

CLight::CLight(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CLight::Initialize(const LIGHTDESC & LightDesc)
{
	m_LightDesc = LightDesc;

	return S_OK;
}

HRESULT CLight::Render(CVIBuffer_Rect * pVIBuffer, CShader * pShader)
{
	_uint		iPassIndex = 1;

	if (LIGHTDESC::TYPE_DIRECTIONAL == m_LightDesc.eType)
	{
		if (FAILED(pShader->Set_RawValue("g_vLightDir", &m_LightDesc.vDirection, sizeof(_float4))))
			return E_FAIL;

		iPassIndex = 1;
	}

	else if (LIGHTDESC::TYPE_POINT == m_LightDesc.eType)
	{
		if (FAILED(pShader->Set_RawValue("g_vLightPos", &m_LightDesc.vPosition, sizeof(_float4))))
			return E_FAIL;
		if (FAILED(pShader->Set_RawValue("g_fLightRange", &m_LightDesc.fRange, sizeof(_float))))
			return E_FAIL;

		iPassIndex = 2;
	}
	
	if (FAILED(pShader->Set_RawValue("g_vLightDiffuse", &m_LightDesc.vDiffuse, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(pShader->Set_RawValue("g_vLightAmbient", &m_LightDesc.vAmbient, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(pShader->Set_RawValue("g_vLightSpecular", &m_LightDesc.vSpecular, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(pShader->Set_RawValue("g_vLightEmissive", &m_LightDesc.vEmissive, sizeof(_float4))))
		return E_FAIL;

	pShader->Begin(iPassIndex);

	pVIBuffer->Render();

	return S_OK;
}

CLight * CLight::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const LIGHTDESC & LightDesc)
{
	CLight*		pInstance = new CLight(pDevice, pContext);

	if (FAILED(pInstance->Initialize(LightDesc)))
	{
		MSG_BOX("CLight Create Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLight::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}

void CLight::Set_LigthDesc(LIGHTDESC & LightDesc)
{
	assert(m_LightDesc.eType == LightDesc.eType && "CLight::Set_LigthDesc");

	memcpy(&m_LightDesc, &LightDesc, sizeof(LIGHTDESC));
}

void CLight::Set_Enable(_bool bFlag)
{
	m_LightDesc.isEnable = bFlag;
}

_float4 CLight::Set_Direction(_float4 & vDirection)
{
	assert(m_LightDesc.eType != LIGHTDESC::TYPE_POINT && "CLight::Set_Direction");

	_float4 vOldDir = m_LightDesc.vDirection;
	m_LightDesc.vDirection = vDirection;

	return vOldDir;
}

_float CLight::Set_Range(_float & vRange)
{
	assert(m_LightDesc.eType != LIGHTDESC::TYPE_DIRECTIONAL && "CLight::Set_Range");

	_float fOldRange = m_LightDesc.fRange;
	m_LightDesc.fRange = vRange;

	return fOldRange;
}

_float4 CLight::Set_Position(_float4 & vPosition)
{	
	_float4 vOldPos = m_LightDesc.vPosition;
	m_LightDesc.vPosition = vPosition;

	return vOldPos;
}

_float4 CLight::Set_Diffuse(_float4 & vColor)
{
	_float4 vOldColor = m_LightDesc.vDiffuse;
	m_LightDesc.vDiffuse = vColor;
		
	return vOldColor;
}

_float4 CLight::Set_Ambient(_float4 & vColor)
{
	_float4 vOldColor = m_LightDesc.vAmbient;
	m_LightDesc.vAmbient = vColor;

	return vOldColor;
}

_float4 CLight::Set_Spectular(_float4 & vColor)
{
	_float4 vOldColor = m_LightDesc.vSpecular;
	m_LightDesc.vSpecular = vColor;

	return vOldColor;
}

_float4 CLight::Set_Emissive(_float4 & vColor)
{
	_float4 vOldColor = m_LightDesc.vEmissive;
	m_LightDesc.vEmissive = vColor;

	return vOldColor;
}
