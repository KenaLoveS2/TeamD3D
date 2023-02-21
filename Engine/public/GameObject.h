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
	CTransform* Get_TransformCom() { return m_pTransformCom; }

public:
	class CComponent*	Find_Component(const _tchar* pComponentTag);
	const _tchar*		Get_ObjectCloneName() { return m_szCloneObjectTag; }
	void				Set_CloneTag(const _tchar* pCloneObjectTag) { m_szCloneObjectTag = pCloneObjectTag; }

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
	ID3D11Device*				m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pContext = { nullptr };
	_bool							m_isCloned = { false };
	_float							m_fCamDistance = { 0.0 };	
	_bool							m_bShadow = true;

	const _tchar*				m_szName = TEXT("");
	const _tchar*					m_szCloneObjectTag = TEXT("");

protected:
	/* ��ü���� ����ؾ�?�� ������Ʈ���� �����Ѵ�. */
	map<const _tchar*, class CComponent*>			m_Components;

	class CTransform*									m_pTransformCom = nullptr;

protected:	
	HRESULT Add_Component(_uint iLevelIndex, const _tchar* pPrototypeTag, const _tchar* pComponentTag, class CComponent** ppOut, void* pArg = nullptr);

	void Compute_CamDistance();

public:	
	virtual CGameObject* Clone(void* pArg = nullptr) = 0;
	virtual void Free() override;

	const _tchar* Get_ObjectName() { return m_szName; }
	void	SwitchOnOff_Shadow(_bool bSwitch) { m_bShadow = bSwitch; }

public: /* imgui */
		// �� ������Ʈ�� ������ �ִ� component�� Imgui_RenderProtpery�Լ��� �����ϴ� �Լ�.
	void Imgui_RenderComponentProperties();

	// �� ������Ʈ���� ������ �����͸� imgui�� �ۼ��Ѵ�.
	virtual void Imgui_RenderProperty() {}
};
END