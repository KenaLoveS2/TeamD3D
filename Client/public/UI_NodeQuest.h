#pragma once
#include "Client_Defines.h"
#include "UI_Node.h"

BEGIN(Client)
class CUI_NodeQuest final : public CUI_Node
{
public:
	enum EVENT {EVENT_FADE, EVENT_END };

private:
	CUI_NodeQuest(ID3D11Device*	pDevice, ID3D11DeviceContext* pContext);
	CUI_NodeQuest(const CUI_NodeQuest& rhs);
	virtual ~CUI_NodeQuest() = default;

public:
	void	Set_HeightByIndex(_float fY) { m_matLocal._42 = fY; }
	void	Set_QuestString(wstring str);
	void	Appear(_float fTerm);
	void	Disappear(_float fTerm);
	void	Move_Up();
	_bool	Is_Finished() { return m_bFinished; }
	_float4	Get_ComputePosition();

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
	_float					m_fTime;
	_float					m_fTimeAcc;
	_float					m_fTerm;

	_bool					m_bAppear;
	_bool					m_bDisappear;

	_bool					m_bFinished;
	_bool					m_bMoveUp;



public:
	static	CUI_NodeQuest*		Create(ID3D11Device* pDevice, ID3D11DeviceContext*	pContext);
	virtual CGameObject*		Clone(void* pArg = nullptr);
	virtual void				Free() override;
};
END