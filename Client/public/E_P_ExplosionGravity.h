#pragma once
#include "Effect_Point_Instancing.h"

BEGIN(Client)

class CE_P_ExplosionGravity final : public CEffect_Point_Instancing
{
public:
	enum TYPE { TYPE_DAMAGE_MONSTER, TYPE_END };

private:
	CE_P_ExplosionGravity(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CE_P_ExplosionGravity(const CE_P_ExplosionGravity& rhs);
	virtual ~CE_P_ExplosionGravity() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pFilePath = nullptr);
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Late_Initialize(void* pArg) override;
	virtual void    Tick(_float fTimeDelta) override;
	virtual void    Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	
public:
	HRESULT SetUp_ShaderResources();
	HRESULT SetUp_Components();
	
public:
	void	Set_Type(TYPE eType) { m_eType = eType;}
	TYPE	Get_Type() { return m_eType; }

	void	Set_Option();

private:
	TYPE	m_eType = TYPE_END;

public:
	static  CE_P_ExplosionGravity* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pFilePath = nullptr);
	virtual CGameObject*  Clone(void* pArg = nullptr) override;
	virtual void          Free() override;
};

END