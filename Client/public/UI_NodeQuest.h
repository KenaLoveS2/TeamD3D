#pragma once
#include "Client_Defines.h"
#include "UI_Node.h"

BEGIN(Client)
class CUI_NodeQuest final : public CUI_Node
{
private:
	CUI_NodeQuest(ID3D11Device*	pDevice, ID3D11DeviceContext* pContext);
	CUI_NodeQuest(const CUI_NodeQuest& rhs);
	virtual ~CUI_NodeQuest() = default;

public:
	void	Set_QuestString(wstring str);
	void	Set_Alpha(_float fAlpha) { m_fAlpha = fAlpha; }
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
	_tchar*					m_szQuest;
private:
	_bool					m_bOpening;
	_float					m_fTime;
	_float					m_fTimeAcc;
	_float					m_fSpeed;
	_float					m_fAlpha;

public:
	static	CUI_NodeQuest*		Create(ID3D11Device* pDevice, ID3D11DeviceContext*	pContext);
	virtual CGameObject*		Clone(void* pArg = nullptr);
	virtual void				Free() override;
};
END