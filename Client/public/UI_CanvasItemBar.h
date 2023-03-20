#pragma once
#include "Client_Defines.h"
#include "UI_Canvas.h"

BEGIN(Client)
class CUI_CanvasItemBar final : public CUI_Canvas
{
public:
	typedef struct tagItemDesc : public UIDESC
	{
		wstring		wstrName;
		_int		iPrice;
		_int		iTextureIndex;
		CUI*		pParent;
	}ITEMDESC;

private:
	CUI_CanvasItemBar(ID3D11Device*	pDevice, ID3D11DeviceContext* pContext);
	CUI_CanvasItemBar(const CUI_CanvasItemBar& rhs);
	virtual ~CUI_CanvasItemBar() = default;

public:
	virtual HRESULT			Initialize_Prototype()			override;
	virtual HRESULT			Initialize(void* pArg)			override;
	virtual void			Tick(_float fTimeDelta)			override;
	virtual void			Late_Tick(_float fTimeDelta)	override;
	virtual HRESULT			Render()						override;

private:
	virtual HRESULT			Bind()							override;
	virtual HRESULT			Ready_Nodes()					override;
	virtual HRESULT			SetUp_Components()				override;
	virtual HRESULT			SetUp_ShaderResources()			override;
	virtual HRESULT			Save_Data()						override;
	//virtual HRESULT			Load_Data(wstring fileName)		override;

public: /* comment out when modify nodes .*/
	virtual void			Imgui_RenderProperty()			override {};

private:
	ITEMDESC				m_tItemDesc;
	_float					m_fCorrectY;
	_tchar*					m_szName;

public:
	static	CUI_CanvasItemBar*	Create(ID3D11Device* pDevice, ID3D11DeviceContext*	pContext);
	virtual CGameObject*		Clone(void* pArg = nullptr);
	virtual void				Free() override;
};
END