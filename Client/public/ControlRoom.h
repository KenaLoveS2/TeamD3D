#pragma once

#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CRenderer;
END

BEGIN(Client)

class CControlRoom final : public CGameObject
{
private:
	CControlRoom(ID3D11Device* pDevice, ID3D11DeviceContext* p_context);
	CControlRoom(const CControlRoom& rhs);
	virtual ~CControlRoom() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Late_Initialize(void * pArg);
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual void Imgui_RenderProperty() override;
	virtual void ImGui_PhysXValueProperty() override;

private:
	HRESULT SetUp_Components();
	CRenderer* m_pRendererCom = nullptr;

	_bool		m_bInitRender = false;

public:
	static CControlRoom*				Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*				Clone(void* pArg = nullptr)  override;
	virtual void								Free() override;
};

END