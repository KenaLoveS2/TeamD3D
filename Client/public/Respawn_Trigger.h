#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Client)

class CRespawn_Trigger final : public CGameObject
{
private:
	CRespawn_Trigger(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRespawn_Trigger(const CRespawn_Trigger& rhs);
	virtual ~CRespawn_Trigger() = default;

public:
	virtual HRESULT					Initialize_Prototype() override;
	virtual HRESULT					Initialize(void* pArg) override;
	virtual HRESULT					Late_Initialize(void* pArg) override;

public:
	virtual void					ImGui_PhysXValueProperty() override;
	virtual _int					Execute_TriggerTouchFound(CGameObject* pTarget, _uint iTriggerIndex, _int iColliderIndex) override;
	virtual _int					Execute_TriggerTouchLost(CGameObject* pTarget, _uint iTriggerIndex, _int iColliderIndex) override;

private:
	_bool							m_bUsed = false;

public:
	static  CRespawn_Trigger*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*			Clone(void* pArg = nullptr) override;
	virtual void					Free() override;
};

END