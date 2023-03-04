#pragma once
#include "Status.h"
#include "Client_Defines.h"

BEGIN(Client)

class CMonster_Status final : public CStatus
{
private:
	CMonster_Status(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonster_Status(const CMonster_Status& rhs);
	virtual ~CMonster_Status() = default;

public:
	virtual HRESULT Initialize_Prototype(const wstring& wstrFilePath) override;
	virtual HRESULT Initialize(void* pArg, class CGameObject* pOwner) override;
	virtual void		Tick(_float fTimeDelta) override;
	virtual void		Imgui_RenderProperty() override;

private:
	_float				m_fInitAttackCoolTime = 0.f;
	_float				m_fCurAttackCoolTime = 0.f;

	_int				m_iNeedPIP = 0;

private:
	virtual HRESULT	Save_Status(const _tchar* pFilePath) override;
	virtual HRESULT	Load_Status(const _tchar* pFilePath) override;

public:
	static CMonster_Status*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& wstrFilePath = L"");
	virtual CComponent*	Clone(void* pArg = nullptr, class CGameObject* pOwner = nullptr) override;
	virtual void				Free() override;
};

END