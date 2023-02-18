#pragma once
#include "Client_Defines.h"
#include "Base.h"

/* UI 관리 매니저. 컴포넌트 및 오브젝트 생성 관리. */

BEGIN(Client)
class CUI_Manager final : public CBase
{
	DECLARE_SINGLETON(CUI_Manager);

private:
	CUI_Manager();
	virtual ~CUI_Manager() = default;

public:
	HRESULT			Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void			Tick(_float fTimeDelta);
	void			Late_Tick(_float fTimeDelta);
	HRESULT			Render();

public: /* Client 제공 함수 */
	HRESULT			Ready_Proto_TextureComponent(const _tchar* pPrototypeTag, class CTexture* pTexture);
	HRESULT			Ready_Proto_GameObject();
	HRESULT			Clone_GameObject();

private:
	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pContext = nullptr;

public:
	virtual void Free() override;
};
END

