#pragma once
#include "Effect_Point_Instancing.h"

BEGIN(Client)

class CE_P_DeadZoneDust final : public CEffect_Point_Instancing
{
public:
	enum TYPE
	{
		TYPE_DUST_A, // Black
		TYPE_DUST_B, // White
		TYPE_DUST_END
	};

private:
	CE_P_DeadZoneDust(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CE_P_DeadZoneDust(const CE_P_DeadZoneDust& rhs);
	virtual ~CE_P_DeadZoneDust() = default;

public:
	
	virtual HRESULT Initialize_Prototype(const _tchar* pFilePath = nullptr);
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Late_Initialize(void* pArg = nullptr);
	virtual void    Tick(_float fTimeDelta) override;
	virtual void    Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	HRESULT			SetUp_Components();
	HRESULT			SetUp_ShaderResources();

	void			Set_Type(TYPE eType);

public:
	void			Reset();

private:
	TYPE			m_eType = TYPE_DUST_A;
	_bool			m_bDissolve = false;
	class CBossShaman* m_pShaman = nullptr;

public:
	static  CE_P_DeadZoneDust* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pFilePath = nullptr);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void          Free() override;
};

END