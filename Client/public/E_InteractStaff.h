#pragma once
#include "Effect.h"
#include "Client_Defines.h"

BEGIN(Client)

class CE_InteractStaff final : public CEffect
{
public:
	enum TYPE { TYPE_KENASTAFF, TYPE_ROT, TYPE_END };

private:
	CE_InteractStaff(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CE_InteractStaff(const CE_InteractStaff& rhs);
	virtual ~CE_InteractStaff() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pFilePath = nullptr);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void    Tick(_float fTimeDelta) override;
	virtual void    Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void	Set_Child();
	void	Set_Type(TYPE eType) { m_eType = eType; }

private:
	TYPE    m_eType = TYPE_KENASTAFF;

public:
	static  CE_InteractStaff* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pFilePath = nullptr);
	virtual CGameObject*  Clone(void* pArg = nullptr) override;
	virtual void          Free() override;
};

END