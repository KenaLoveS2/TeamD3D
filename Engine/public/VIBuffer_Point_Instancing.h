#pragma once

#include "VIBuffer_Instancing.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Point_Instancing final : public CVIBuffer_Instancing
{
public:
	CVIBuffer_Point_Instancing(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Point_Instancing(const CVIBuffer_Point_Instancing& rhs);
	virtual ~CVIBuffer_Point_Instancing() = default;

public:
	_double*		Get_Speeds() { return m_pSpeeds;}
	void			Set_Speeds(_double* pSpeed) { m_pSpeeds = pSpeed; }

	HRESULT			Set_Pos(_float3 fMin, _float3 fMax);

	_float2			Get_PSize() { return m_fPSize; }
	HRESULT			Set_PSize(_float2 PSize);
	HRESULT			Set_RandomPSize(_float2 PSizeMinMax);

	_float2			Get_RandomSpeeds() { return m_SpeedMinMax; }
	void			Set_RandomSpeeds(_double fMin, _double fMax);

public:
	virtual HRESULT Initialize_Prototype(_uint iNumInstance);
	virtual HRESULT Initialize(void* pArg, class CGameObject* pOwner) override;
	virtual HRESULT Tick(_float fTimeDelta) override;
	virtual HRESULT Render();

private:
	_double*		m_pSpeeds = nullptr;
	_float2			m_SpeedMinMax = { 0.0f, 0.0f };

	_float4			m_fPos = { 0.0f,0.0f,0.0f,1.0f };
	_float2			m_fPSize = { 0.2f,0.2f };
	VTXPOINT*       m_pVerices = nullptr;

public:
	static CVIBuffer_Point_Instancing* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iNumInstance = 1);
	virtual CComponent* Clone(void* pArg = nullptr, class CGameObject* pOwner = nullptr) override;
	virtual void Free() override;
};

END