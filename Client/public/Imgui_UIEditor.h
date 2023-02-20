#pragma once
#include "Client_Defines.h"
#include "ImguiObject.h"

BEGIN(Engine)
class CUI_Canvas;
class CUI_Node;
END

BEGIN(Client)
class CImgui_UIEditor final : public CImguiObject
{
private:
	CImgui_UIEditor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

public:
	virtual HRESULT Initialize(void* pArg = nullptr);
	virtual void Imgui_FreeRender();

private:
	HRESULT	Ready_TextureList();
	HRESULT Ready_CanvasProtoList();
	HRESULT Ready_CloneCanvasList();

private:
	vector<string>					m_vecTextureTag;
	vector<string>					m_vecCanvasProtoTag;
	vector<CUI_Canvas*>		m_vecCanvas;
	vector<string>					m_vecRenderPass;
	
	CUI_Canvas*						m_pCanvas;
	CUI_Node*						m_pNode;

public:
	static	CImgui_UIEditor*	Create(ID3D11Device* pDevice, ID3D11DeviceContext*	pContext, void* pArg = nullptr);
	virtual void				Free() override;
};
END