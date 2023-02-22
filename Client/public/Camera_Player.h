#pragma once
#include "Camera.h"
#include "Client_Defines.h"

BEGIN(Client)

class CCamera_Player final : public CCamera
{
private:
	CCamera_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera_Player(const CCamera_Player& rhs);
	virtual ~CCamera_Player() = default;

public:
	void					Set_Player(class CKena* pKena, CTransform* pTransform) { m_pKena = pKena; m_pKenaTransform = pTransform; }

public:
	virtual HRESULT		Initialize_Prototype() override;
	virtual HRESULT		Initialize(void* pArg) override;
	virtual void			Tick(_float fTimeDelta) override;
	virtual void			Late_Tick(_float fTimeDelta) override;
	virtual HRESULT		Render() override;

private:
	class CKena*			m_pKena = nullptr;
	CTransform*			m_pKenaTransform = nullptr;
	
	_float					m_fDistanceFromTarget = 5.f;

	_long					m_MouseMoveX = 0;
	_long					m_MouseMoveY = 0;

public:
	static	CCamera_Player*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg = nullptr) override;
	virtual void					Free() override;
};

END