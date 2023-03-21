#pragma once
#include "Client_Defines.h"
#include "UI_Node.h"

BEGIN(Client)
class CUI_NodeItemBox final : public CUI_Node
{
private:
	CUI_NodeItemBox(ID3D11Device*	pDevice, ID3D11DeviceContext* pContext);
	CUI_NodeItemBox(const CUI_NodeItemBox& rhs);
	virtual ~CUI_NodeItemBox() = default;

public:
	void	Set_String(_int iCount);

public:
	virtual HRESULT			Initialize_Prototype()			override;
	virtual HRESULT			Initialize(void* pArg)			override;
	virtual void			Tick(_float fTimeDelta)			override;
	virtual void			Late_Tick(_float fTimeDelta)	override;
	virtual HRESULT			Render()						override;

private:
	virtual HRESULT			SetUp_Components() override;
	virtual HRESULT			SetUp_ShaderResources() override;

private:
	_uint					m_iCount;
	_tchar*					m_szCount;
	_float2					m_vFontPos;
	_float					m_fFontSize;
	_float					m_fTextureScale;

public:
	static	CUI_NodeItemBox*	Create(ID3D11Device* pDevice, ID3D11DeviceContext*	pContext);
	virtual CGameObject*		Clone(void* pArg = nullptr);
	virtual void				Free() override;
};
END