#pragma once
#include "Effect.h"
#include "Client_Defines.h"

BEGIN(Client)

class CE_SpiritArrowPosition final : public CEffect
{
public:
	enum EFFECTS { EFFECT_SPRITEPOSITION, EFFECT_PARTICLE, EFFECT_END };

private:
	CE_SpiritArrowPosition(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CE_SpiritArrowPosition(const CE_SpiritArrowPosition& rhs);
	virtual ~CE_SpiritArrowPosition() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pFilePath = nullptr);
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT	Late_Initialize(void* pArg) override;
	virtual void    Tick(_float fTimeDelta) override;
	virtual void    Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void	Set_Child();

private:
	_bool	m_bChildActive = false;

public:
	static  CE_SpiritArrowPosition* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pFilePath = nullptr);
	virtual CGameObject*  Clone(void* pArg = nullptr) override;
	virtual void          Free() override;

};

END