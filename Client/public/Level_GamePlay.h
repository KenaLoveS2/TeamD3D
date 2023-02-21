#pragma once
#include "Client_Defines.h"
#include "Level.h"

#define THREAD_COUNT		10

BEGIN(Engine)
class CGameObject;
END

BEGIN(Client)
class CLevel_GamePlay final : public CLevel
{
public:
	struct CloneObjectDesc
	{
		_tchar* pLayerTag;
		_tchar* pPrototypeTag;
		_tchar* pCloneTag;
		void* pArg;

		CGameObject* pObjectOut;
	};

	typedef struct tagThreadData
	{
		CRITICAL_SECTION* pCriticalSection;
		HANDLE hThread;
		_uint iIndex;

		CloneObjectDesc* pCloneObjectDesc;
		_uint iNumCopyObjects;
	} THREAD_DATA;

private:
	THREAD_DATA m_ThreadData[THREAD_COUNT];
	CRITICAL_SECTION m_Critical_Section;

private:
	CLevel_GamePlay(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_GamePlay() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Lights();
	HRESULT Ready_Layer_BackGround(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Camera(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Player(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Monster(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Effect(const _tchar* pLayerTag);


	HRESULT Ready_Thread_CloneObject(const _tchar* pLayerTag);
	HRESULT Initialize_ThreadData();
	HRESULT Clone_GameObjectsByThreadData();
	void Clear_ThreadData();

public:
	static CLevel_GamePlay* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END