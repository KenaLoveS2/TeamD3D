#pragma once
#include "Effect.h"

BEGIN(Client)

class CE_KenaDashRing final : public CEffect
{
private:
	CE_KenaDashRing(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CE_KenaDashRing(const CE_KenaDashRing& rhs);
	virtual ~CE_KenaDashRing() = default;

public:
	void			Set_NextRing(CE_KenaDashRing* pRing) { m_pNextRing = pRing; }
	void			Set_InitScale(_float fScale) { m_fInitScale = fScale; }
	void			Set_CurrentScale(_float fScale) { m_fCurScale = fScale; }

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pFilePath = nullptr);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void    Tick(_float fTimeDelta) override;
	virtual void    Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	void			Imgui_RenderProperty();

private:
	CE_KenaDashRing* m_pNextRing = nullptr;
	_float			m_fCurScale = 0.01f;
	_float			m_fInitScale = 0.01f;

public:
	static  CE_KenaDashRing* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pFilePath = nullptr);
	virtual CGameObject*  Clone(void* pArg = nullptr) override;
	virtual void          Free() override;

};

END