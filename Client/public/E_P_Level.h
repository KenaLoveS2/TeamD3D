#pragma once
#include "Effect_Point_Instancing.h"

BEGIN(Client)

class CE_P_Level final : public CEffect_Point_Instancing
{
private:
	CE_P_Level(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CE_P_Level(const CE_P_Level& rhs);
	virtual ~CE_P_Level() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pFilePath = nullptr);
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Late_Initialize(void* pArg  = nullptr );
	virtual void    Tick(_float fTimeDelta) override;
	virtual void    Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	
public:
	void	Tick_Rotation(_float fTimeDelta);
	void	Reset();

private:
	_bool	m_bReset = false;

	_float	m_fDurationTime = 0.0f;
	CVIBuffer_Point_Instancing::POINTDESC* m_ePointDesc;

public:
	static  CE_P_Level*   Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pFilePath = nullptr);
	virtual CGameObject*  Clone(void* pArg = nullptr) override;
	virtual void          Free() override;
};

END