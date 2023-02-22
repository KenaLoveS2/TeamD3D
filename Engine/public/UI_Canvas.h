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
	virtual HRESULT			Initialize_Prototype()			override;
	virtual HRESULT			Initialize(void* pArg)			override;
	virtual void			Tick(_float fTimeDelta)			override;
	virtual void			Late_Tick(_float fTimeDelta)	override;
	virtual HRESULT			Render()						override;
public:
	virtual void			Imgui_RenderProperty();
public:
	virtual		HRESULT	Add_Node(CUI* pUI)sealed;		/* 캔버스에 하위 UI(Node 또는 Canvas) 추가 */

protected:
	vector<string>		m_vecNodeCloneTag;
	vector<CUI*>		m_vecNode; 



public:
	virtual CGameObject* Clone(void* pArg = nullptr) = 0;
	virtual void Free() override;
};
END