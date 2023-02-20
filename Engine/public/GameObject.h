#pragma once

#include "Transform.h"

BEGIN(Engine)

class ENGINE_DLL CGameObject abstract : public CBase
{
public:
	typedef struct tagGameObjectDesc
	{	
		CTransform::TRANSFORMDESC		TransformDesc;
	}GAMEOBJECTDESC;

public:
	_float Get_CamDistance() const {
		return m_fCamDistance;
	}

public:
	class CComponent* Find_Component(const _tchar* pComponentTag);


protected:
	CGameObject(ID3D11Device*	pDevice, ID3D11DeviceContext* pContext);
	CGameObject(const CGameObject& rhs);
	virtual ~CGameObject() = default;

public:
	static const _tchar*			m_pTransformComTag;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual HRESULT RenderShadow();

protected:
	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pContext = { nullptr };
	_bool					m_isCloned = { false };
	_float					m_fCamDistance = { 0.0 };	

	_tchar m_szName[64] = { 0, };

protected:
	/* 객체들이 사용해야 할 컴포넌트들을 보관한다. */
	map<const _tchar*, class CComponent*>			m_Components;

	class CTransform*								m_pTransformCom = nullptr;

protected:	
	HRESULT Add_Component(_uint iLevelIndex, const _tchar* pPrototypeTag, const _tchar* pComponentTag, class CComponent** ppOut, void* pArg = nullptr);

	void Compute_CamDistance();

public:	
	virtual CGameObject* Clone(void* pArg = nullptr) = 0;
	virtual void Free() override;

	_tchar* Get_ObjectName() { return m_szName; }

public: /* imgui */
		// 이 오브젝트가 가지고 있는 component의 Imgui_RenderProtpery함수를 실행하는 함수.
	void Imgui_RenderComponentProperties();

	// 이 오브젝트에서 보여줄 데이터를 imgui로 작성한다.
	virtual void Imgui_RenderProperty() {}
};
END