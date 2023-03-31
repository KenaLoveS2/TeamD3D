#pragma once
#include "Effect_Mesh.h"
#include "PhysX_Defines.h"

BEGIN(Client)

class CE_Warrior_FireSwipe final : public CEffect_Mesh
{
private:
	CE_Warrior_FireSwipe(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CE_Warrior_FireSwipe(const CE_Warrior_FireSwipe& rhs);
	virtual ~CE_Warrior_FireSwipe() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pFilePath = nullptr);
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Late_Initialize(void* pArg = nullptr)override;
	virtual void    Tick(_float fTimeDelta) override;
	virtual void    Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	void			Reset();

public:
	virtual _int	Execute_Collision(CGameObject* pTarget, _float3 vCollisionPos, _int iColliderIndex) override;
	virtual void	Imgui_RenderProperty() override;
	virtual void	ImGui_PhysXValueProperty()override;

public:
	HRESULT SetUp_SwipeTexture();

private:
	HRESULT SetUp_ShaderResources();
	HRESULT SetUp_Components();

private:
	_float	m_fDurationTime = 0.0f;
	
public:
	static  CE_Warrior_FireSwipe* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pFilePath = nullptr);
	virtual CGameObject*		  Clone(void* pArg = nullptr) override;
	virtual void				  Free() override;
};

END