#pragma once
#include "Effect_Mesh.h"
#include "Client_Defines.h"
#include "PhysX_Defines.h"
BEGIN(Client)

class CE_Swipes_Charged final : public CEffect_Mesh
{
private:
	CE_Swipes_Charged(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CE_Swipes_Charged(const CE_Swipes_Charged& rhs);
	virtual ~CE_Swipes_Charged() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pFilePath = nullptr);
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Late_Initialize(void* pArg = nullptr)override;
	virtual void    Tick(_float fTimeDelta) override;
	virtual void    Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	void			Reset();

	virtual _int	Execute_Collision(CGameObject* pTarget, _float3 vCollisionPos, _int iColliderIndex) override;
	virtual void	ImGui_PhysXValueProperty()override;

private:
	HRESULT			SetUp_ShaderResources();
	void			Imgui_RenderProperty() override;

private:
	PX_TRIGGER_DATA*		m_pTriggerData = nullptr;

public:
	static  CE_Swipes_Charged* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pFilePath = nullptr);
	virtual CGameObject*	   Clone(void* pArg = nullptr) override;
	virtual void               Free() override;

};

END