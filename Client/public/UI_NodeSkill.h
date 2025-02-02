#pragma once
#include "Client_Defines.h"
#include "UI_Node.h"

BEGIN(Client)
class CUI_NodeEffect;
class CUI_NodeSkill final : public CUI_Node
{
public:
	enum STATE { STATE_BLOCKED, STATE_LOCKED, STATE_UNLOCKED, STATE_END };

private:
	CUI_NodeSkill(ID3D11Device*	pDevice, ID3D11DeviceContext* pContext);
	CUI_NodeSkill(const CUI_NodeSkill& rhs);
	virtual ~CUI_NodeSkill() = default;

public:
	_uint	Get_Level() { return m_iLevel; }

public:
	virtual HRESULT			Initialize_Prototype()			override;
	virtual HRESULT			Initialize(void* pArg)			override;
	virtual HRESULT			Late_Initialize(void* pArg = nullptr) override;
	virtual void			Tick(_float fTimeDelta)			override;
	virtual void			Late_Tick(_float fTimeDelta)	override;
	virtual HRESULT			Render()						override;

public:
	void	BackToOriginal();
	void	Picked(_float fRatio);
	void	State_Change(_uint eState);
	void	Set_LockEffect(CUI_NodeEffect* pEffect);


public:
	virtual	HRESULT			Save_Data();
	virtual HRESULT			Load_Data(wstring fileName);
public:
	HRESULT	Setting(_tchar* textureProtoTag, _uint iLevel);
private:
	virtual HRESULT			SetUp_Components() override;
	virtual HRESULT			SetUp_ShaderResources() override;
	virtual void			Imgui_RenderProperty() override;

private:
	CUI_NodeEffect*			m_pLock;
	_uint					m_iLevel;
	STATE					m_eState;
	_float					m_fDiffuseAlpha;
	_float					m_fMaskAlpha;
	_int					m_iCheck;

public:
	static	CUI_NodeSkill*		Create(ID3D11Device* pDevice, ID3D11DeviceContext*	pContext);
	virtual CGameObject*		Clone(void* pArg = nullptr);
	virtual void				Free() override;
};
END