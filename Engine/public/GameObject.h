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
	_float					Get_CamDistance() const { return m_fCamDistance; }
	const _int&			Get_AnimationIndex() const { return m_iAnimationIndex; }
	void					Set_AnimationIndex(_uint iAnimationIndex) { m_iAnimationIndex = iAnimationIndex; }

public:
	CTransform*			Get_TransformCom() { return m_pTransformCom; }
	_matrix				Get_WorldMatrix() { return m_pTransformCom->Get_WorldMatrix(); }

public:
	class CComponent*	Find_Component(const _tchar* pComponentTag);
	const _tchar*		Get_ObjectCloneName() { return m_szCloneObjectTag; }
	const _tchar*		Get_ProtoObjectName() { return m_szProtoObjectTag; }

	void					Set_CloneTag(const _tchar* pCloneObjectTag) { m_szCloneObjectTag = pCloneObjectTag; }
	void					Set_ProtoTag(const _tchar* pProtoObjectTag) { m_szProtoObjectTag = pProtoObjectTag; }

	void					Delete_Component(const _tchar* pComponentTag);

protected:
	CGameObject(ID3D11Device*	pDevice, ID3D11DeviceContext* pContext);
	CGameObject(const CGameObject& rhs);
	virtual ~CGameObject() = default;

public:
	static const _tchar*			m_pTransformComTag;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);

	virtual HRESULT Late_Initialize(void* pArg = nullptr) { return S_OK; };

	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual HRESULT RenderShadow();

	/* For Animation */
	virtual void				Update_Child() {}
	virtual HRESULT			Call_EventFunction(const string& strFuncName) { return S_OK; }
	virtual void				Push_EventFunctions() {};
	virtual void				Calc_RootBoneDisplacement(_fvector vDisplacement) {}

protected:
	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pContext = { nullptr };
	_bool					m_isCloned = { false };
	_float					m_fCamDistance = { 0.0 };
	_bool					m_bShadow = true;

	const _tchar*		m_szCloneObjectTag = TEXT("");
	const _tchar*		m_szProtoObjectTag = TEXT("");
	_int				m_iAnimationIndex = 0;

	_float3			m_vPhysXScale;
	_float3			m_vPhysXPos;

protected:
	map<const _tchar*, class CComponent*>			m_Components;

	class CTransform*									m_pTransformCom = nullptr;

protected:	
	HRESULT Add_Component(_uint iLevelIndex, const _tchar* pPrototypeTag, const _tchar* pComponentTag, class CComponent** ppOut, void* pArg = nullptr, CGameObject* pOwner = nullptr);

public:
	void Compute_CamDistance();

public:	
	virtual CGameObject* Clone(void* pArg = nullptr) = 0;
	virtual void Free() override;

	void	SwitchOnOff_Shadow(_bool bSwitch) { m_bShadow = bSwitch; }

public: /* imgui */
	virtual  void Imgui_RenderComponentProperties();

	virtual void Imgui_RenderProperty() {}
	virtual void ImGui_AnimationProperty() {}
	virtual void ImGui_ShaderValueProperty() {};
	virtual void ImGui_PhysXValueProperty() {};

	void Set_Position(_float4& vPosition);
	void Set_Position(_float3& vPosition);
	void Set_WorldMatrix(_float4x4& vPosition);

public:
	virtual _int Execute_Collision() { return 0; }
};
END