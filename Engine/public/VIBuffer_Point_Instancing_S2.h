#pragma once
#include "VIBuffer_Instancing.h"

BEGIN(Engine)
class ENGINE_DLL CVIBuffer_Point_Instancing_S2 final : public CVIBuffer_Instancing
{
public:
	typedef struct tagPtInfo
	{
		enum TYPE { TYPE_HAZE, TYPE_GATHER, TYPE_PARABOLA , TYPE_SPREAD, TYPE_END };

		TYPE	eType = TYPE_HAZE;

		_float	fTerm = 0.0f;
		_float	fTermAcc = 0.0f;
		_float	fPlaySpeed = 1.0f; /* Diffrenst usage by Type */
		_int	iInstanceIndex = 0;
		_int	iNumInstance = 0;

		_float2 vPSize	  = { 0.0f, 0.0f };
		_float3	vSpeedMin = { 0.0f, 0.0f, 0.0f };
		_float3 vSpeedMax = { 0.0f, 0.0f, 0.0f };
		_float3 vMinPos = { 0.0f, 0.0f, 0.0f };
		_float3 vMaxPos = { 0.0f, 0.0f, 0.0f };
	}POINTINFO;

private:
	CVIBuffer_Point_Instancing_S2(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Point_Instancing_S2(const CVIBuffer_Point_Instancing_S2& rhs);
	virtual ~CVIBuffer_Point_Instancing_S2() = default;

public:
	POINTINFO		Get_Info() { return m_tInfo; }
	_bool			Is_Finished() { return m_bFinished; }

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg, class CGameObject* pOwner)	override;
	virtual HRESULT Tick(_float TimeDelta)			override;
	virtual HRESULT Render()						override;

public:
	HRESULT		Update_Buffer(POINTINFO* pInfo);

private: /* Tick Function By Type */
	HRESULT		Tick_Haze(_float TimeDelta);
	HRESULT		Tick_Gather(_float TimeDelta);
	HRESULT		Tick_Parabola(_float TimeDelta);
	HRESULT		Tick_Spread(_float TimeDelta);
private:
	void		Safe_Delete_Arrays();
	void		Reset();

private:
	POINTINFO		m_tInfo;
	_float*			m_pXSpeeds		= nullptr;
	_float*			m_pYSpeeds		= nullptr;
	_float*			m_pZSpeeds		= nullptr;
	_float3*		m_pPositions	= nullptr;

	_bool			m_bFinished		= false;

public:
	static CVIBuffer_Point_Instancing_S2* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg = nullptr, class CGameObject* pOwner = nullptr) override;
	virtual void Free() override;
};
END