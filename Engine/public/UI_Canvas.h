#pragma once
#include "UI.h"

BEGIN(Engine)
class ENGINE_DLL CUI_Canvas abstract : public CUI
{
public:
	CUI_Canvas(ID3D11Device*	pDevice, ID3D11DeviceContext* pContext);
	CUI_Canvas(const CUI_Canvas& rhs);
	virtual ~CUI_Canvas() = default;
public:
	CUI*	Get_SelectedNode() {
		if (m_iSelectedNode < 0 || m_iSelectedNode >= m_vecNode.size())
			return nullptr;
		return m_vecNode[m_iSelectedNode];
	}

public:
	virtual HRESULT			Initialize_Prototype()			override;
	virtual HRESULT			Initialize(void* pArg)			override;
	virtual void			Tick(_float fTimeDelta)			override;
	virtual void			Late_Tick(_float fTimeDelta)	override;
	virtual HRESULT			Render()						override;

protected: /* For Clones. */
	virtual HRESULT			Bind() { return S_OK; }
	virtual HRESULT			Ready_Nodes() { return S_OK; }

public:
	virtual void			Imgui_RenderProperty();


public:
	virtual HRESULT					Save_Data() override;
	virtual HRESULT					Load_Data(wstring fileName) override;

public:
	/* Add Sub UI(Canvas Or Node)*/
	virtual	HRESULT	Add_Node(CUI* pUI)sealed;

//public: /* Provide Functions */
//	virtual	void	Call_CanvasFunction(_int iParam1, _int iParam2) {};

protected:
	vector<string>		m_vecNodeCloneTag;
	vector<CUI*>		m_vecNode;
	_bool				m_bBindFinished;

	/* For Tool */
	_int				m_iSelectedNode;


public:
	virtual CGameObject* Clone(void* pArg = nullptr) = 0;
	virtual void Free() override;
};
END