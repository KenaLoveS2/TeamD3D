#pragma once
#include "Component.h"

/* ��ü�� ���� �����̽� �󿡼��� ���¸� �������ش�.(m_WorldMatrix) */
/* ���º�ȯ�� ���� �������̽��� �������ش�.(Go_Straight, Turn, Chase, LookAt) */

BEGIN(Engine)

class ENGINE_DLL CTransform final : public CComponent
{
public:
	/* STATE_TRANSLATION : (���� �Ǵ� �θ�) �������κ��� �󸶳� �������־�. */
	/* Position : �������� ������������� ��ġ. */
	enum STATE { STATE_RIGHT, STATE_UP, STATE_LOOK, STATE_TRANSLATION, STATE_END };
	enum DIRECTION { DIR_W, DIR_A, DIR_S, DIR_D, DIR_WA, DIR_WD, DIR_SA, DIR_SD, DIR_LOOK, DIR_END };

	typedef struct TRANSFORMDESC
	{
		/* �ʴ� ���������ϴ� �ӵ�. */
		float		fSpeedPerSec;

		/* �ʴ� ȸ���ؾ��ϴ� �ӵ�. */
		float		fRotationPerSec;

	}TRANSFORMDESC;	

protected:
	CTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTransform(const CTransform& rhs);
	virtual ~CTransform() = default;

public:
	_matrix Get_WorldMatrix_Inverse() {
		return XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_WorldMatrix));
	}

	_matrix Get_WorldMatrix() {
		return XMLoadFloat4x4(&m_WorldMatrix);
	}
	_float4x4 Get_WorldMatrixFloat4x4() {
		return m_WorldMatrix;
	}


	_vector Get_State(STATE eState) const {
		return XMLoadFloat4x4(&m_WorldMatrix).r[eState];
	}

	_float3 Get_Scaled() const {
		return _float3(XMVectorGetX(XMVector3Length(Get_State(STATE_RIGHT))), 
			XMVectorGetX(XMVector3Length(Get_State(STATE_UP))), 
			XMVectorGetX(XMVector3Length(Get_State(STATE_LOOK))));
	}

	TRANSFORMDESC Get_TransformDesc() { return m_TransformDesc; }
	void		  Set_TransformDesc(TRANSFORMDESC eTransformDesc) { 
		memcpy(&m_TransformDesc, &eTransformDesc, sizeof(TRANSFORMDESC));
	}
	void			Set_Speed(_float fSpeedPerSec) { m_TransformDesc.fSpeedPerSec = fSpeedPerSec; }
	void			Set_RotatePerSecond(_float fRotatePerSec) { m_TransformDesc.fRotationPerSec = fRotatePerSec; }

	void Set_WorldMatrix_float4x4(_float4x4& fWorldMatrix);
	void Set_WorldMatrix(_fmatrix WorldMatrix);

	void Set_State(STATE eState, _fvector vState) {
		_float4 vTmp;
		XMStoreFloat4(&vTmp, vState);
		memcpy(&m_WorldMatrix.m[eState][0], &vTmp, sizeof vTmp);
	}

	void Set_Scaled(STATE eState, _float fScale); /* fScale������ ���̸� �����Ѵ�. */
	void Set_Scaled(_float3 vScale); /* fScale������ ���̸� �����Ѵ�. */
	void Scaling(STATE eState, _float fScale); /* fScale����� �ø���. */

	/* ReCalculate Other Vector Automatically */
	void Set_Look(_fvector vLook);
	void Set_Right(_fvector vRight);
	void Set_Up(_fvector vUp);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg, class CGameObject* pOwner) override;
	virtual void	Imgui_RenderProperty() override;
	virtual void	Imgui_RenderProperty_ForJH();

public:
	void Go_Straight(_float fTimeDelta);
	void Go_Backward(_float fTimeDelta);
	void Go_Left(_float fTimeDelta);
	void Go_Right(_float fTimeDelta);
	void Go_Direction(_fvector vDirection, _float fTimeDelta, class CNavigation* pNavigCom = nullptr);
	void Go_DirectionNoY(_fvector vDirection, _float fTimeDelta, class CNavigation* pNavigCom = nullptr);

	/* Camera Movement */
	void Go_AxisY(_float fTimeDelta);
	void Go_AxisNegY(_float fTimeDelta);
	void Speed_Boost(_bool bKeyState, _float fValue);
	void Speed_Down(_bool bKeyState, _float fValue);
	void Orbit(_fvector vTargetPos, _fvector vAxis, const _float& fDistance, _float fTimeDelta);

	/* Arrow */
	void Arrow(_fvector vTargetPos, _fvector vFirePosition, _float fMaxAngle, _float fTimeDelta, _bool& bReach);
	void Projectile_Motion(_float fMaxAngle, _float fTimeDelta);

	// Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta);
	void Turn(_fvector vAxis, _float fTimeDelta); /* Dynamic */
	void Rotation(_fvector vAxis, _float fRadian); /* Static */
	void RotationFromNow(_fvector vAxis, _float fRadian);

	/*void LookAt(const CTransform* pTarget);*/
	void LookAt(_fvector vTargetPos);
	void LookAt_NoUpDown(_fvector vTargetPos);

	void Chase(_fvector vTargetPos, _float fTimeDelta, _float fLimit = 0.1f, _bool bChaseY = false);
	//void Jump(_float fTimeDelta);

public:
	HRESULT Bind_ShaderResource(class CShader* pShaderCom, const char* pConstantName);

	struct ActorData
	{
		PxRigidActor* pActor;
		 _tchar pActorTag[MAX_PATH];
		_float4x4 PivotMatrix;
	};

	list<ActorData>* Get_ActorList() { return &m_ActorList; }
	ActorData* FindActorData(const _tchar* pActorTag);

private:	
	_float4x4							m_WorldMatrix;
	TRANSFORMDESC			m_TransformDesc;
	_float								m_fInitSpeed = 0.f;

	class CPhysX_Manager* m_pPhysX_Manager = nullptr;
	_bool							   m_bIsStaticPxActor = false;
	PxRigidActor*				   m_pPxActor = nullptr;

	_float3 m_vPxPivot = { 0.f, 0.f, 0.f };
	_float3 m_vPxPivotScale = { 1.f,1.f,1.f };

	list<ActorData> m_ActorList;
	
public:
	static CTransform* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg = nullptr, class CGameObject* pOwner = nullptr) override;
	virtual void Free() override;

	void Set_OnTerrain(class CVIBuffer_Terrain* pTerrainBuffer, _float fPlusValue);
	_float HeightOnTerrain(_fvector vPos, _float3* pTerrainVtxPos, _uint iNumVerticesX, _uint iNumVerticesZ);

	_float Calc_Distance_XYZ(_float4 &vTargetPos);	
	_float Calc_Distance_XZ(_float4 &vTargetPos);
	_float Calc_Distance_XY(_float4 &vTargetPos);
	_float Calc_Distance_YZ(_float4 &vTargetPos);

	_float Calc_Distance_XYZ(_float3 &vTargetPos);
	_float Calc_Distance_XZ(_float3 &vTargetPos);
	_float Calc_Distance_XY(_float3 &vTargetPos);
	_float Calc_Distance_YZ(_float3 &vTargetPos);
	
	_float Calc_Distance_XYZ(CTransform* pTransform);
	_float Calc_Distance_XZ(CTransform* pTransform);	
	_float Calc_Distance_XY(CTransform* pTransform);
	_float Calc_Distance_YZ(CTransform* pTransform);

	/* Look �������� �¿�� Radian / 2 ������ ���� �ȿ� Ÿ���� �ִ��� �������� ����մϴ�. */
	_bool  Calc_InRange(_float fRadian, CTransform* pTargetTransformCom);
	_float	Calc_Pitch();

	void Connect_PxActor_Static(const _tchar * pActorTag, _float3 vPivotDist = _float3(0.f, 0.f, 0.f));
	void Connect_PxActor_Gravity(const _tchar * pActorTag, _float3 vPivotDist = _float3(0.f, 0.f, 0.f));
	void Add_Collider(const _tchar * pActorTag, _float4x4 PivotMatrix);
	void Update_Collider(const _tchar * pActorTag, _float4x4 PivotMatrix);

	void Set_Translation(_fvector vPosition, _fvector vDist);

	void Set_PxPivot(_float3 vPivot) { m_vPxPivot = vPivot; }
	_float3 Get_vPxPivot() { return m_vPxPivot; }

	void Set_PxPivotScale(_float3 vPivotScale) { m_vPxPivotScale = vPivotScale; }
	_float3 Get_vPxPivotScale() { return m_vPxPivotScale; }

	void Tick(_float fTimeDelta);

	_bool IsClosed_XYZ(_float4& vTargetPos, _float fDIstance = 0.1f);
	_bool IsClosed_XZ(_float4& vTargetPos, _float fDIstance = 0.1f);
	_bool IsClosed_XYZ(_float3& vTargetPos, _float fDIstance = 0.1f);
	_bool IsClosed_XZ(_float3& vTargetPos, _float fDIstance = 0.1f);

	void Set_Position(_fvector vPos);
	void Clear_Actor();
	void Update_AllCollider(_float4x4 PivotMatrix);
};

END



