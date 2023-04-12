#pragma once
#include "Effect_Point_Instancing.h"

BEGIN(Client)

class CE_P_CommonBox final : public CEffect_Point_Instancing
{
private:
	CE_P_CommonBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CE_P_CommonBox(const CE_P_CommonBox& rhs);
	virtual ~CE_P_CommonBox() = default;

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

public:
	void	Update_Particle(_float fTimeDelta);
	void	Reset();

	void	TurnSystem_CommonBox(_bool bTurnoff) {
		m_bTurnoff = bTurnoff;
	}

private:
	_bool   m_bTurnoff = false;
	
private:
	CVIBuffer_Point_Instancing::POINTDESC* m_ePointDesc;
	_float	m_fDurationTime = 0.0f;
	_float	m_fMinusTime = 0.0f;
	_uint	m_iNumInstance = 0;

	_int	m_fOverInstanceCnt = 0;

public:
	static  CE_P_CommonBox* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pFilePath = nullptr);
	virtual CGameObject*  Clone(void* pArg = nullptr) override;
	virtual void          Free() override;
};

END