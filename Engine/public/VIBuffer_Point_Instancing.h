#pragma once

#include "VIBuffer_Instancing.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Point_Instancing final : public CVIBuffer_Instancing
{
public:
	typedef struct tagPointDesc
	{
		enum  SHAPETYPE { VIBUFFER_BOX, VIBUFFER_STRIGHT, VIBUFFER_PLANECIRCLE, VIBUFFER_CONE, VIBUFFER_EXPLOSION, VIBUFFER_END };
		enum  DIRXYZ { DIR_X, DIR_Y, DIR_Z, DIR_END };

		SHAPETYPE	eShapeType = VIBUFFER_BOX;
		DIRXYZ		eRotXYZ = DIR_Y;

		_float4		vDir = { 1.0f,0.0f,0.0f,0.0f };
		_float		fRange = 3.0f;
		_int		iCreateInstance = 0;
		_int		iInstanceIndex = 0;

		_bool	    bSpread = true; // if(Spread == true) => Out, 
		_bool		bIsAlive;		// 파티클이 생존한 경우 true, 소멸한 경우 false
		_float		fTimeDelta = 0.0f;

		// Shape_Box  //
		_float3     fMin = { 1.f,1.f,1.f };
		_float3     fMax = { 1.f,1.f,1.f };
		// ~Shape_Box //

		// Shape_Sprite  //
		_float      fCreateRange = 1.0f;
		_float      fDurationTime = 0.0f;
		_float      fMaxTime = 0.0f;
		_float4		vCirclePos = { 0.0f,0.0f,0.0f,1.0f };
		// ~Shape_Sprite  //

		// Shape_Cone  //
		_float2     fConeRange = { 1.0f, 3.0f };
		// ~Shape_Cone //

		// Shape_Explosion  //
		_float4		vOriginPos = { 0.0f,0.0f,0.0f,1.0f }; // 생성 위치 
		_vector		vExplosionDir;// 움직일 Dir
		_float		fRangeOffset, fRangeY;
		// ~Shape_Explosion //
		
	}POINTDESC;

public:
	CVIBuffer_Point_Instancing(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Point_Instancing(const CVIBuffer_Point_Instancing& rhs);
	virtual ~CVIBuffer_Point_Instancing() = default;

public:
	POINTDESC*   Get_PointDesc() { return m_ePointDesc; }
	void	     Set_PointDesc(POINTDESC* ePointDesc) {
		memcpy(m_ePointDesc, ePointDesc, sizeof(POINTDESC) * m_iNumInstance);
	}

	HRESULT			Set_ShapePosition();

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

private:
	POINTDESC*		m_ePointDesc;

public:
	static CVIBuffer_Point_Instancing* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iNumInstance = 1);
	virtual CComponent* Clone(void* pArg = nullptr, class CGameObject* pOwner = nullptr) override;
	virtual void Free() override;
};

END