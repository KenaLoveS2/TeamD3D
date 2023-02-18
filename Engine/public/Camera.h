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

	/* 행렬을 바인딩하여 알아서(고정기능렌더링파이프라인) 정점의 변환에 사용되도록 처리. dx11:x*/
	/* 필요하면 얻어오는 기능까지도. */
	//m_pDevice->SetTransform();
	//m_pDevice->GetTransform();

private:
	class CPipeLine*			m_pPipeLine = nullptr;
	CAMERADESC					m_CameraDesc; 

public:		
	virtual CGameObject* Clone(void* pArg = nullptr) = 0;
	virtual void Free() override;
};

END