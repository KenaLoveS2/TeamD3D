#pragma once
#include "Client_Defines.h"
#include "Base.h"


/* Only for Preparing Components and GameObjects */
/* UIs are in static Level, so this class should be in MainApp Class, 
and all works are done before Imgui_UIEditor Starts. */

BEGIN(Client)
class CUI_ClientManager final : public CBase
{
	DECLARE_SINGLETON(CUI_ClientManager);

public:
	CUI_ClientManager();
	virtual ~CUI_ClientManager() = default;

public:
	const vector<wstring>*	Get_TextureProtoTag() { return &m_vecTextureProtoTag; }
	const vector<wstring>*	Get_CanvasProtoTag() { return &m_vecCanvasProtoTag; }
	const vector<wstring>*	Get_NodeProtoTag() { return &m_vecNodeProtoTag; }


public:
	/* Ready_UIs : Ready_Proto_TextureComponent & Ready_Proto_GameObject & Ready_Clone_GameObject */
	HRESULT			Ready_UIs(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	HRESULT			Ready_Proto_TextureComponent(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	HRESULT			Ready_InformationList();
	HRESULT			Ready_Proto_GameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	HRESULT			Ready_Clone_GameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

private:
	vector<wstring>		m_vecTextureProtoTag;
	vector<wstring>		m_vecCanvasProtoTag;
	vector<wstring>		m_vecNodeProtoTag;


public:
	virtual void Free() override;
};
END