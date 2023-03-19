#pragma once

#include "Client_Defines.h"
#include "Base.h"

/* 로딩레벨에서 처리해야할 자원들을 로드하낟. : 실제 할당하고자하는 레벨을 위한 준비. */


BEGIN(Client)
class CLoader final : public CBase
{
private:
	CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLoader() = default;

public:
	LEVEL Get_NextLevelID() const {
		return m_eNextLevelID;
	}

	const _tchar* Get_LoadingText() const {
		return m_szLoadingText;
	}

	const _float Get_Cur()  const { return m_fCur; }
	const _float Get_Max() const { return m_fMax; }

	CRITICAL_SECTION Get_CriticalSection() const {
		return m_Critical_Section;
	}


	_bool isFinished() const {
		return m_isFinished;
	}

public:
	HRESULT Initialize(LEVEL eNextLevelID);

	HRESULT Loading_ForLogo();
	HRESULT Loading_ForGamePlay();
	HRESULT Loading_ForMapTool();
	HRESULT Loading_ForTestPlay();
	HRESULT Loading_ForTestEffect();

	HRESULT Loading_ForWJ(_uint iLevelIndex);
	HRESULT Loading_ForJH(_uint iLevelIndex);
	HRESULT Loading_ForSY(_uint iLevelIndex);
	HRESULT Loading_ForBJ(_uint iLevelIndex);
	HRESULT Loading_ForHO(_uint iLevelIndex);
	HRESULT Loading_ForHW(_uint iLevelIndex);

private:
	ID3D11Device*					m_pDevice = nullptr;
	ID3D11DeviceContext*		m_pContext = nullptr;
	CRITICAL_SECTION			m_Critical_Section;

private:
	LEVEL				m_eNextLevelID = LEVEL_END;
	HANDLE			m_hThread;
	_bool				m_isFinished = false;
	_tchar				m_szLoadingText[MAX_PATH] = TEXT("");

	_float				m_fCur = 1.f;
	_float				m_fMax = 0.f;

public:
	static CLoader* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevelID);
	virtual void Free() override;

	HRESULT LoadNonAnimModel(_uint iLevelIndex);
	HRESULT LoadNonAnimFolderModel(_uint iLevelIndex , string strFolderName,_bool bIsLod=false,_bool bIsInstancing =false
	, _bool bIsJsonMatarial=false,_bool bPivotScale=false);

};

END