#pragma once

#include "GameObject.h"

/* 렌더링에 있어 필수적으로 필요한 요소인 뷰스페이스 변환과 투영변환. */

BEGIN(Engine)

class ENGINE_DLL CCamera abstract : public CGameObject
{
public:
	typedef struct tagCameraDesc : public CGameObject::GAMEOBJECTDESC		
	{
		_float4		vEye, vAt, vUp;	
		_float		fFovy, fAspect, fNear, fFar;
	} CAMERADESC;

protected:
	CCamera(ID3D11Device*	pDevice, ID3D11DeviceContext* pContext);
	CCamera(const CCamera& rhs);
	virtual ~CCamera() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

	_float* Get_Near() { return &m_CameraDesc.fNear; }
	_float* Get_Far()  { return &m_CameraDesc.fFar; }
	_float* Get_Fov() { return &m_CameraDesc.fFovy; }

protected:
	class CPipeLine*			m_pPipeLine = nullptr;
	CAMERADESC				m_CameraDesc;


public:		
	virtual CGameObject* Clone(void* pArg = nullptr) = 0;
	virtual void Free() override;
};

END