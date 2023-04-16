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
	void	Set_Alpha(_float fAlpha) { 	m_fAlpha = fAlpha; }
	void	Set_Clear();
	_int	Get_Reward() { return m_iReward;  }
	void	QuestOn() {
		if (m_bActive == false)
		{
			m_bActive = true; m_bOpening = false; m_fTimeAcc = 0.f;
		}
	}
	void	QuestOff() {
		if (m_bActive == true)
		{
			m_bOpening = true; m_bClear = true; m_fTimeAcc = 0.f;
		}

	}
	_float	Get_Alpha() { return m_fAlpha; }

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

	_bool					m_bClear;
	_int					m_iReward;

public:
	static	CUI_NodeQuest*		Create(ID3D11Device* pDevice, ID3D11DeviceContext*	pContext);
	virtual CGameObject*		Clone(void* pArg = nullptr);
	virtual void				Free() override;
};
END