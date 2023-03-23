#pragma once
#include "MonsterWeapon.h"

BEGIN(Engine)
class CTexture;
END

BEGIN(Client)
class CBranchTosser_Weapon final : public Client::CMonsterWeapon
{
private:
	_bool m_bGoStraight = false;
	class CBranchTosser* m_pBranchTosser = nullptr;
	CTexture* m_pDissolveTextureCom = nullptr;
	_uint m_iShaderPass = 1;

private:
	CBranchTosser_Weapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBranchTosser_Weapon(const CBranchTosser_Weapon& rhs);
	virtual ~CBranchTosser_Weapon() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* pArg) override;
	virtual HRESULT			Late_Initialize(void* pArg) override;
	virtual void					Tick(_float fTimeDelta) override;
	virtual void					Late_Tick(_float fTimeDelta) override;
	virtual HRESULT			Render() override;
	virtual HRESULT			RenderShadow() override;
	virtual void					Imgui_RenderProperty() override;
	virtual void					ImGui_ShaderValueProperty() override;
	virtual void					ImGui_PhysXValueProperty() override;

private:
	virtual  void		 	Update_Collider(_float fTimeDelta) override;
	virtual  HRESULT  SetUp_Components() override;
	virtual  HRESULT  SetUp_ShaderResources() override;
	virtual	HRESULT	SetUp_ShadowShaderResources() override;

public:
	static CBranchTosser_Weapon*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

	void Set_GoStraight(_bool bFlag) { m_bGoStraight = bFlag; }

	void Set_OwnerBranchTosser(class CBranchTosser* pOwner) { m_pBranchTosser = pOwner; }

	void Set_ShaderPassIndex(_uint iIndex) { m_iShaderPass = iIndex; }
	void Clear() { m_pTransformCom->Clear_Actor(); }
};
END
