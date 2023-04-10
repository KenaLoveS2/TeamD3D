#pragma once
#include "Effect.h"
#include "PhysX_Defines.h"

BEGIN(Client)

class CE_ShamanLazer final : public CEffect
{
public:
	enum CHILD
	{
		CHILD_LINE,
		CHILD_SP,
		CHILD_P,
		CHILD_END
	};

private:
	CE_ShamanLazer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CE_ShamanLazer(const CE_ShamanLazer& rhs);
	virtual ~CE_ShamanLazer() = default;

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
	HRESULT			SetUp_Components();
	HRESULT			SetUp_Effects();

	void			Imgui_RenderProperty() override;

public:
	void			TurnOnLazer(_float fTimeDelta);
	
public:
	static  CE_ShamanLazer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pFilePath = nullptr);
	virtual CGameObject*	   Clone(void* pArg = nullptr) override;
	virtual void               Free() override;

};

END