#pragma once
#include "MonsterWeapon.h"

BEGIN(Engine)
class CRenderer;
class CShader;
class CModel;
class CTexture;
END


BEGIN(Client)
class CBranchTosser_Tree : public CGameObject
{
private:
	CRenderer* m_pRendererCom = nullptr;
	CShader* m_pShaderCom = nullptr;
	CModel* m_pModelCom = nullptr;
	CTexture *m_pDissolveTextureCom = nullptr;

	class CBranchTosser* m_pBranchTosser = nullptr;

	_uint m_iNumMeshes = 0; 
	_uint m_iShaderPass = 0;

private:
	CBranchTosser_Tree(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBranchTosser_Tree(const CBranchTosser_Tree& rhs);
	virtual ~CBranchTosser_Tree() = default;

public:
	static CBranchTosser_Tree* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Late_Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT RenderShadow() override;
		
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();
	HRESULT SetUp_ShadowShaderResources();

	void Set_ShaderPassIndex(_uint iIndex ){ m_iShaderPass = iIndex; }
	void Set_OwnerBranchTosser(class CBranchTosser* pOwner) { m_pBranchTosser = pOwner; }
	void Clear();
};
END
