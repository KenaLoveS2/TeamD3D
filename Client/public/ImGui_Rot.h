#pragma once

#include "Client_Defines.h"
#include "ImguiObject.h"

BEGIN(Engine)
class CGameInstance;
class CTransform;
END

BEGIN(Client)

class CImGui_Rot final : public CImguiObject
{
private:
	CImGui_Rot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

public:
	virtual  HRESULT Initialize(void* pArg) override;
	virtual void Imgui_FreeRender() override;
	void RotList();

public:
	static void Load_RotObjects(_uint iLevel, string JsonFileName, _bool isDynamic = false);

private:
	void				Save();
	HRESULT		Load();

private:
	CGameInstance*   m_pGameInstance = nullptr;
	class CRot*			 m_pRot = nullptr;
	_bool					 m_bSaveWrite = false;
	string					 m_strFileName;

public:
	static	CImGui_Rot*			Create(ID3D11Device* pDevice, ID3D11DeviceContext*	pContext, void* pArg = nullptr);
	virtual void							Free() override;
};

END