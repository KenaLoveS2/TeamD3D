#pragma once

#include "VIBuffer_Instancing.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Point_Instancing final : public CVIBuffer_Instancing
{
public:
	typedef struct tagPointDesc
	{
		enum  SHAPETYPE { VIBUFFER_BOX, VIBUFFER_SPHERE, VIBUFFER_CONE, VIBUFFER_EXPLOSION, VIBUFFER_END };
		enum  MOVEDIR { MOVE_FRONT, MOVE_BACK, MOVE_UP, MOVE_DOWN, MOVE_LEFT, MOVE_RIGHT, MOVE_END };
		enum  ROTXYZ { ROT_X, ROT_Y, ROT_Z, ROT_END };

		SHAPETYPE		eShapeType = VIBUFFER_BOX;
		MOVEDIR			eMoveDir = MOVE_DOWN;
		ROTXYZ			eRotXYZ = ROT_Y;

		_vector vDir = { 1.0f,0.0f,0.0f,0.0f };
		_float4 vInitPos;
		_float  fRange = 3.0f;

		// Shape_Box  //
		_float3 fMin = { 1.f,1.f,1.f };
		_float3 fMax = { 1.f,1.f,1.f };
		// ~Shape_Box //

		// Shape_Sphere  //
		_float fChargeRange;
		_float fCircleRate;
		// ~Shape_Sphere //

		// Shape_Cone  //
		_float fMinY, fRangeY, fRangeOffset;
		_float fStopMinTime, fStopMaxTime;
		// ~Shape_Cone //

		// Shape_Explosion  //
// 		_float3		fMin = { 1.f,1.f,1.f };
// 		_float3		fMax = { 1.f,1.f,1.f };
		_float4		vOriginPos = { 0.0f,0.0f,0.0f,1.0f };		// 생성 위치 
		_vector		vExplosionDir;	// 움직일 Dir

		_float      fExplosionRange = 0.0f;
	//	_float      fRangeOffset = 0.0f;
		_float      fDegree = 0.0f;
		_float      fDegreeOffset = 0.0f;
		_float      fSizeRate = 1.0f;
		_float      fSizeRateOffset = 0.0f;

		_float3		vVelocity;		// 파티클 속도
		_float		fDurationTime;	// 파티클의 현재나이
		_bool		bIsAlive;		// 파티클이 생존한 경우 true, 소멸한 경우 false
		// ~Shape_Explosion //
		
	}POINTDESC;

public:
	CVIBuffer_Point_Instancing(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Point_Instancing(const CVIBuffer_Point_Instancing& rhs);
	virtual ~CVIBuffer_Point_Instancing() = default;

public:
	POINTDESC::SHAPETYPE		Get_ShapeType() { return m_ePointDesc->eShapeType; }
	void						Set_ShapeType(POINTDESC::SHAPETYPE eType) {
		memcpy(&m_ePointDesc->eShapeType, &eType, sizeof(POINTDESC::SHAPETYPE));
	}

	POINTDESC::MOVEDIR			Get_MoveDir() { return m_ePointDesc->eMoveDir; }
	void						Set_MoveDir(POINTDESC::MOVEDIR eType) {
		memcpy(&m_ePointDesc->eMoveDir, &eType, sizeof(POINTDESC::MOVEDIR));
	}

	POINTDESC*  Get_PointDesc() { return m_ePointDesc; }
	void	    Set_PointDesc(POINTDESC* ePointDesc) {
		memcpy(m_ePointDesc, ePointDesc, sizeof(POINTDESC));
	}

	HRESULT			Set_ShapePosition(POINTDESC* ePointDesc);

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

private:
	POINTDESC*		m_ePointDesc;

public:
	static CVIBuffer_Point_Instancing* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iNumInstance = 1);
	virtual CComponent* Clone(void* pArg = nullptr, class CGameObject* pOwner = nullptr) override;
	virtual void Free() override;
};

END