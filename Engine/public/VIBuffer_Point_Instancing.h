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

		_bool	    bSpread = true; 
		_bool		bIsAlive;		
		_float		fTimeDelta = 0.0f;
		_bool 		bMoveY = false;
		_bool 		bRotation = false;
		_float      fMoveY = 0.0f;

		_bool		bGravity = false;
		_bool		bUseGravity = false;

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
		_float4		vOriginPos = { 0.0f,0.0f,0.0f,1.0f }; 
		_vector		vExplosionDir;
		_float		fRangeOffset, fRangeY;
		// ~Shape_Explosion //

	}POINTDESC;

	typedef struct tagInstnaceData
	{
		_double		pSpeeds;
		_vector		fPos;
		_float2		SpeedMinMax;
		_float2		fPSize;
	}INSTANCEDATA;

public:
	CVIBuffer_Point_Instancing(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Point_Instancing(const CVIBuffer_Point_Instancing& rhs);
	virtual ~CVIBuffer_Point_Instancing() = default;

public:
	POINTDESC*     Get_PointDesc() { return m_ePointDesc; }
	void	       Set_PointDesc(POINTDESC* ePointDesc) {
		memcpy(m_ePointDesc, ePointDesc, sizeof(POINTDESC) * m_iNumInstance);
	}

	INSTANCEDATA*   Get_InstanceData() { return m_InstanceData; }
	INSTANCEDATA    Get_InstanceData_Idx(_int iIndex) { return m_InstanceData[iIndex]; }
	void			Set_InstanceData(INSTANCEDATA* eInstanceData) {
		memcpy(m_InstanceData, eInstanceData, sizeof(INSTANCEDATA) * m_iNumInstance);
	}

	HRESULT			Set_ShapePosition();
public:
	HRESULT			Set_Pos(_float3 fMin, _float3 fMax);
	HRESULT			Set_PSize(_float2 PSize);
	HRESULT			Set_RandomPSize(_float2 PSizeMinMax);
	void			Set_Speeds(_double pSpeed);
	void			Set_RandomSpeeds(_float fmin, _float fmax);
	void			Set_Gravity(_bool bUseGravity) { m_ePointDesc->bUseGravity = bUseGravity; }
	void			Set_GravityTimeZero() { m_fGravity = 0.0f; }
	void			Set_ResetOriginPos();

public:
	HRESULT			Tick_Box(_float fTimeDelta);
	HRESULT			Tick_Stright(_float fTimeDelta);
	HRESULT			Tick_PlaneCircle(_float fTimeDelta);
	HRESULT			Tick_Cone(_float fTimeDelta);
	HRESULT			Tick_Explosion(_float fTimeDelta);

public:
	virtual HRESULT Initialize_Prototype(_uint iNumInstance);
	virtual HRESULT Initialize(void* pArg, class CGameObject* pOwner) override;
	virtual HRESULT Tick(_float fTimeDelta) override;
	virtual HRESULT Render();

private:
	POINTDESC*		m_ePointDesc;
	INSTANCEDATA*   m_InstanceData;

	_float			m_fGravity = 9.8f;
	_float			m_fAngle = 0.0f;

public:
	static CVIBuffer_Point_Instancing* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iNumInstance = 1);
	virtual CComponent* Clone(void* pArg = nullptr, class CGameObject* pOwner = nullptr) override;
	virtual void Free() override;
};

END