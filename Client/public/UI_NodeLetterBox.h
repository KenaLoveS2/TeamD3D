#pragma once
#include "Client_Defines.h"
#include "UI_Node.h"

BEGIN(Client)
class CUI_NodeLetterBox final : public CUI_Node
{
private:
	CUI_NodeLetterBox(ID3D11Device*	pDevice, ID3D11DeviceContext* pContext);
	CUI_NodeLetterBox(const CUI_NodeLetterBox& rhs);
	virtual ~CUI_NodeLetterBox() = default;

public:
	virtual HRESULT			Initialize_Prototype()			override;
	virtual HRESULT			Initialize(void* pArg)			override;
	virtual HRESULT			Late_Initialize(void* pArg = nullptr) override;
	virtual void			Tick(_float fTimeDelta)			override;
	virtual void			Late_Tick(_float fTimeDelta)	override;
	virtual HRESULT			Render()						override;

public:
	void					Appear();
	void					DisAppear();

private:
	virtual HRESULT			SetUp_Components() override;
	virtual HRESULT			SetUp_ShaderResources() override;

private:
	_bool					m_bStart;
	_float					m_fSpeed;

	_float					m_fMinY;

public:
	static	CUI_NodeLetterBox*		Create(ID3D11Device* pDevice, ID3D11DeviceContext*	pContext);
	virtual CGameObject*			Clone(void* pArg = nullptr);
	virtual void					Free() override;
};
END