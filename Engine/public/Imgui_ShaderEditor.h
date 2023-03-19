#pragma once

#include "ImguiObject.h"

BEGIN(Engine)

class ENGINE_DLL CImgui_ShaderEditor final : public CImguiObject
{
private:
	CImgui_ShaderEditor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

public:
	virtual  HRESULT Initialize(void* pArg) override;
	virtual void Imgui_FreeRender() override;

	class CGameObject*				Find_GameObject(_int iIndex);
	void										Update_Level();

	static CImgui_ShaderEditor* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg = nullptr);
	virtual void Free() override;

private:
	class CGameInstance* m_pGameInstance = nullptr;
	class CRenderer*			m_pRendererCom = nullptr;

	_bool	m_bInit = false;
	_bool	m_bPhysXRender = false;

	_uint	 m_iCurrentLevel = 0;
	map	<const _tchar*, class CGameObject*>*		m_mapShaderValueObject;
};

END