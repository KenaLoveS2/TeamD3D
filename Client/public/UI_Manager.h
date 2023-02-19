#pragma once
#include "Client_Defines.h"
#include "Base.h"

/* UI 관리 매니저. 컴포넌트(리소스관리) 및 오브젝트 생성에만 관여한다. */
/* 다른 매니저처럼 이 클래스를 통해 UI를 제어하지 않는다.(delegator 사용) */

BEGIN(Client)
class CUI_Manager final : public CBase
{
	DECLARE_SINGLETON(CUI_Manager);

private:
	CUI_Manager();
	virtual ~CUI_Manager() = default;

public:
	HRESULT			Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	//void			Tick(_float fTimeDelta);
	//void			Late_Tick(_float fTimeDelta);
	//HRESULT			Render();

public: /* Client 제공 함수 */
	HRESULT			Ready_Proto_TextureComponent();
	HRESULT			Ready_Proto_GameObject();
	HRESULT			Clone_GameObject();

private:
	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pContext = nullptr;

public:
	virtual void Free() override;
};
END

