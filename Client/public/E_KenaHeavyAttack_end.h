#pragma once
#include "Effect.h"
#include "Client_Defines.h"

BEGIN(Client)

class CE_KenaHeavyAttack_end final : public CEffect
{
private:
	CE_KenaHeavyAttack_end(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CE_KenaHeavyAttack_end(const CE_KenaHeavyAttack_end& rhs);
	virtual ~CE_KenaHeavyAttack_end() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pFilePath = nullptr);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void    Tick(_float fTimeDelta) override;
	virtual void    Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void	Imgui_RenderProperty();

public:
	void	Set_Child();
	_float  m_fScale = 1.0f;

public:
	static  CE_KenaHeavyAttack_end* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pFilePath = nullptr);
	virtual CGameObject*  Clone(void* pArg = nullptr) override;
	virtual void          Free() override;

};

END