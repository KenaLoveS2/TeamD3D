#pragma once
#include "Base.h"

BEGIN(Engine)
class ENGINE_DLL CLight final : public CBase
{
public:
	CLight(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLight() = default;

public:
	const LIGHTDESC* Get_LightDesc() const { return &m_LightDesc; }

public:
	HRESULT Initialize(const LIGHTDESC& LightDesc);
	HRESULT Render(class CVIBuffer_Rect* pVIBuffer, class CShader* pShader);

private:
	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pContext = nullptr;

private:
	LIGHTDESC			m_LightDesc;

public:
	static CLight* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const LIGHTDESC& LightDesc);
	virtual void Free() override;

	// 다른 타입으로의 변경은 불가능
	void Set_LigthDesc(LIGHTDESC &LightDesc); 
	void Set_Enable(_bool bFlag);
	
	// 방향성만 가능
	_float4 Set_Direction(_float4& vDirection); 
	// 점광원만 가능
	_float Set_Range(_float& fRange); 

	_float4 Set_Position(_float4& vPosition);
	_float4 Set_Diffuse(_float4& vColor);
	_float4 Set_Ambient(_float4& vColor);
	_float4 Set_Spectular(_float4& vColor);
};
END