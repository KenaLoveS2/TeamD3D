#pragma once
#include "VIBuffer_Instancing.h"

BEGIN(Engine)
class ENGINE_DLL CVIBuffer_Point_Instancing_S2 final : public CVIBuffer_Instancing
{
public:
	typedef struct tagPtInfo
	{
		enum TYPE { TYPE_HAZE, TYPE_END };

		TYPE	eType = TYPE_HAZE;

		_float	fTerm = 0.0f;
		_float	fTermAcc = 0.0f;
		_int	iInstanceIndex = 0;
		_int	iNumInstance = 0;

		_float2 vPSize = { 0.0f, 0.2f }; /* min, max */
		_float3	vSpeedMin = { 0.0f, 0.0f, 0.0f };
		_float3 vSpeedMax = { 0.0f, 0.0f, 0.0f };

	}POINTINFO;

private:
	CVIBuffer_Point_Instancing_S2(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Point_Instancing_S2(const CVIBuffer_Point_Instancing_S2& rhs);
	virtual ~CVIBuffer_Point_Instancing_S2() = default;

public:
	virtual HRESULT Initialize_Prototype(POINTINFO* tInfo);
	virtual HRESULT Initialize(void* pArg, class CGameObject* pOwner)	override;
	virtual HRESULT Tick(_double TimeDelta);
	virtual HRESULT Render()						override;

private:
	POINTINFO		m_tInfo;
	_float*			m_pXSpeeds = nullptr;
	_float*			m_pYSpeeds = nullptr;
	_float*			m_pZSpeeds = nullptr;

public:
	static CVIBuffer_Point_Instancing_S2* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, POINTINFO* tInfo);
	virtual CComponent* Clone(void* pArg = nullptr, class CGameObject* pOwner = nullptr) override;
	virtual void Free() override;
};
END