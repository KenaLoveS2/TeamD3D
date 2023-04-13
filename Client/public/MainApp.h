#pragma once
#include "Client_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CRenderer;
class CGameInstance;
END

BEGIN(Client)
class CMainApp final : public CBase
{
private:
	CGameInstance*			m_pGameInstance = nullptr;
	class CBGM_Manager*		m_pBGM_Manager = nullptr;
	CRenderer*				m_pRenderer = nullptr;

	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pContext = nullptr;

#ifdef _DEBUG
private:
	_tchar					m_szFPS[MAX_PATH] = TEXT("");
	_uint					m_iNumCallDraw = 0;
	_float					m_TimeAcc = 0.f;
#endif

private:
	CMainApp();
	virtual ~CMainApp() = default;

	HRESULT Start_Level(LEVEL eLevelID);
	HRESULT Ready_Prototype_Component();
	HRESULT Ready_Prototype_GameObject();

	HRESULT CMainApp::Ready_BufferLock_UnLock();

public:
	HRESULT Initialize();
	void Tick(_float fTimeDelta);
	HRESULT Render();
	HRESULT Resize_BackBuffer();

	static CMainApp* Create();
	virtual void Free() override;	
};	
END