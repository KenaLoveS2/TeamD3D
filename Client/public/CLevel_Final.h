#pragma once

#include "Client_Defines.h"
#include "Level.h"

BEGIN(Client)

class CLevel_Final final : public CLevel
{
private:
	CLevel_Final(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Final() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Lights();
	HRESULT Ready_Layer_BackGround(const _tchar * pLayerTag);
	HRESULT Ready_Layer_Enviroment(const _tchar * pLayerTag);
	HRESULT Ready_Layer_Camera(const _tchar * pLayerTag);
	HRESULT Ready_Layer_CineCamera(const _tchar * pLayerTag);
	HRESULT Ready_Layer_Player(const _tchar * pLayerTag);
	HRESULT Ready_Layer_Monster(const _tchar * pLayerTag);
	HRESULT Ready_Layer_Rot(const _tchar * pLayerTag);
	HRESULT Ready_Layer_Effect(const _tchar * pLayerTag);
	HRESULT Ready_Layer_UI(const _tchar * pLayerTag);
	HRESULT Ready_Layer_NPC(const _tchar * pLayerTag);
	HRESULT Ready_Layer_ControlRoom(const _tchar * pLayerTag);

public:
	static CLevel_Final* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual void Free() override;
};

END