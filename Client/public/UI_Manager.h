#pragma once
#include "Client_Defines.h"
#include "Base.h"

/* UI ���� �Ŵ���. ������Ʈ(���ҽ�����) �� ������Ʈ �������� �����Ѵ�. */
/* �ٸ� �Ŵ���ó�� �� Ŭ������ ���� UI�� �������� �ʴ´�.(delegator ���) */

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

public: /* Client ���� �Լ� */
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

