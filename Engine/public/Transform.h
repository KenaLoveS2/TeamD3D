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

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg, class CGameObject* pOwner) override;
	virtual void	Imgui_RenderProperty() override;

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

	// Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta);
	void Turn(_fvector vAxis, _float fTimeDelta); /* Dynamic */
	void Rotation(_fvector vAxis, _float fRadian); /* Static */
	void RotationFromNow(_fvector vAxis, _float fRadian);

	/* �Ĵٺ���. */
	/*void LookAt(const CTransform* pTarget);*/
	void LookAt(_fvector vTargetPos);

	/* �����Ѵ� .*/
	void Chase(_fvector vTargetPos, _float fTimeDelta, _float fLimit = 0.1f);

public:
	HRESULT Bind_ShaderResource(class CShader* pShaderCom, const char* pConstantName);

private:	
	_float4x4				m_WorldMatrix;

	TRANSFORMDESC			m_TransformDesc;
	_float					m_fInitSpeed = 0.f;

	
	class CPhysX_Manager* m_pPhysX_Manager = nullptr;
	_bool m_bIsStaticPxActor = false;
	PxRigidActor* m_pPxActor = nullptr;	
	_float3 m_vPxPivot = { 0.f, 0.f, 0.f };
	
	struct StaticActorData
	{
		PxRigidActor* pActor;
		_float4x4 PivotMatrix;
	};

	list<StaticActorData> m_StaticActorList;
	
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
	
	_float Calc_Distance_XYZ(CTransform* pTransform);
	_float Calc_Distance_XZ(CTransform* pTransform);	
	_float Calc_Distance_XY(CTransform* pTransform);
	_float Calc_Distance_YZ(CTransform* pTransform);

	void Connect_PxActorDynamic(const _tchar* pActorTag, _float3 vPivot = {0.f, 0.f, 0.f});
	void Add_PxActorStatic(const _tchar * pActorTag, _float4x4 PivotMatrix);

	void Set_Translation(_fvector vPosition, _fvector vDist);
	void Set_PxPivot(_float3 vPivot) { m_vPxPivot = vPivot; }
	void Tick(_float fTimeDelta);
};

END