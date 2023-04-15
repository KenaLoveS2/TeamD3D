#pragma once

#include "GameInstance.h"
#include "Client_Defines.h"
#include "UI_ClientManager.h"
#include "Delegator.h"

BEGIN(Engine)
class CRenderer;
END

BEGIN(Client)

class CWorldTrigger final : public CGameObject
{
private:
	CWorldTrigger(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWorldTrigger(const CWorldTrigger& rhs);
	virtual ~CWorldTrigger() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* pArg) override;
	virtual HRESULT			Late_Initialize(void* pArg) override;
	virtual void					Tick(_float fTimeDelta) override;
	virtual void					Late_Tick(_float fTimeDelta) override;
	virtual void					Imgui_RenderProperty() override;
	virtual void					ImGui_PhysXValueProperty() override;
	virtual _int					Execute_Collision(CGameObject* pTarget, _float3 vCollisionPos, _int iColliderIndex) override;

public:
	Delegator<CUI_ClientManager::UI_PRESENT, _float> m_WorldTriggerDelegator;

private:
	HRESULT						Save();
	HRESULT						Load();

	CRenderer* m_pRendererCom = nullptr;

private:
	class CGameInstance* m_pGameInstance = nullptr;
	_bool							m_bChange = false;
	_uint								m_iVectorSize = 0;
	vector<_smatrix>        m_vecWorldMatrix;
	_uint								m_nMatNum = 0;

public:
	static  CWorldTrigger* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END