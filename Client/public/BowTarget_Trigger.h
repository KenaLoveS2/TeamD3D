#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Client)

class CBowTarget_Trigger final : public CGameObject
{
private:
	CBowTarget_Trigger(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBowTarget_Trigger(const CBowTarget_Trigger& rhs);
	virtual ~CBowTarget_Trigger() = default;

public:
	virtual HRESULT					Initialize_Prototype() override;
	virtual HRESULT					Initialize(void* pArg) override;
	virtual HRESULT					Late_Initialize(void* pArg) override;

public:
	virtual void					ImGui_PhysXValueProperty() override;
	virtual _int					Execute_TriggerTouchFound(CGameObject* pTarget, _uint iTriggerIndex, _int iColliderIndex) override;
	virtual _int					Execute_TriggerTouchLost(CGameObject* pTarget, _uint iTriggerIndex, _int iColliderIndex) override;

private:
	wstring							m_wstrTargetGroup = L"";

public:
	static  CBowTarget_Trigger*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*			Clone(void* pArg = nullptr) override;
	virtual void					Free() override;
};

END