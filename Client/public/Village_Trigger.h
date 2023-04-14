#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Client)

class CVillage_Trigger final : public CGameObject
{
private:
	CVillage_Trigger(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVillage_Trigger(const CVillage_Trigger& rhs);
	virtual ~CVillage_Trigger() = default;

public:
	virtual HRESULT					Initialize_Prototype() override;
	virtual HRESULT					Initialize(void* pArg) override;
	virtual HRESULT					Late_Initialize(void* pArg) override;

public:
	virtual void					ImGui_PhysXValueProperty() override;
	virtual _int					Execute_TriggerTouchFound(CGameObject * pTarget, _uint iTriggerIndex, _int iColliderIndex) override;
	virtual _int					Execute_TriggerTouchLost(CGameObject * pTarget, _uint iTriggerIndex, _int iColliderIndex) override;

private:
	_bool							m_bIntoVillage = true;

public:
	static  CVillage_Trigger*		Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CGameObject*			Clone(void* pArg = nullptr) override;
	virtual void					Free() override;
};

END