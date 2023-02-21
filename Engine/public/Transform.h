#pragma once
#include "Component.h"

/* 객체의 월드 스페이스 상에서의 상태를 저장해준다.(m_WorldMatrix) */
/* 상태변환을 위한 인터페이스를 제공해준다.(Go_Straight, Turn, Chase, LookAt) */

BEGIN(Engine)

class ENGINE_DLL CTransform final : public CComponent
{
public:
	/* STATE_TRANSLATION : (원점 또는 부모) 기준으로부터 얼마나 떨어져있어. */
	/* Position : 절대적인 월드공간에서의 위치. */
	enum STATE { STATE_RIGHT, STATE_UP, STATE_LOOK, STATE_TRANSLATION, STATE_END };

	typedef struct TRANSFORMDESC
	{
		/* 초당 움직여야하는 속도. */
		float		fSpeedPerSec;

		/* 초당 회전해야하는 속도. */
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

	_vector Get_State(STATE eState) const {
		return XMLoadFloat4x4(&m_WorldMatrix).r[eState];
	}

	_float3 Get_Scaled() const {
		return _float3(XMVectorGetX(XMVector3Length(Get_State(STATE_RIGHT))), 
			XMVectorGetX(XMVector3Length(Get_State(STATE_UP))), 
			XMVectorGetX(XMVector3Length(Get_State(STATE_LOOK))));
	}

	void Set_State(STATE eState, _fvector vState) {
		_float4		vTmp;
		XMStoreFloat4(&vTmp, vState);
		memcpy(&m_WorldMatrix.m[eState][0], &vTmp, sizeof vTmp);			
	}

	void Set_Scaled(STATE eState, _float fScale); /* fScale값으로 길이를 변형한다. */
	void Set_Scaled(_float3 vScale); /* fScale값으로 길이를 변형한다. */
	void Scaling(STATE eState, _float fScale); /* fScale배수로 늘린다. */

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg, class CGameObject* pOwner) override;
	virtual void	Imgui_RenderProperty() override;

public:
	void Go_Straight(_float fTimeDelta, class CNavigation* pNaviCom = nullptr);
	void Go_Backward(_float fTimeDelta);
	void Go_Left(_float fTimeDelta);
	void Go_Right(_float fTimeDelta);

	void Go_AxisY(_float fTimeDelta);
	void Go_AxisNegY(_float fTimeDelta);
	void Speed_Boost(_bool bKeyState, _float fValue);

	// Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta);
	void Turn(_fvector vAxis, _float fTimeDelta); /* Dynamic */
	void Rotation(_fvector vAxis, _float fRadian); /* Static */

	/* 쳐다본다. */
	/*void LookAt(const CTransform* pTarget);*/
	void LookAt(_fvector vTargetPos);

	/* 추적한다 .*/
	void Chase(_fvector vTargetPos, _float fTimeDelta, _float fLimit = 0.1f);

public:
	HRESULT Bind_ShaderResource(class CShader* pShaderCom, const char* pConstantName);
	
private:	
	_float4x4				m_WorldMatrix;
	TRANSFORMDESC	m_TransformDesc;
	_float					m_fInitSpeed = 0.f;


public:
	static CTransform* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg = nullptr, class CGameObject* pOwner = nullptr) override;
	virtual void Free() override;

	void Set_OnTerrain(class CVIBuffer_Terrain* pTerrainBuffer, _float fPlusValue);
	_float HeightOnTerrain(_fvector vPos, _float3* pTerrainVtxPos, _uint iNumVerticesX, _uint iNumVerticesZ);
};


END