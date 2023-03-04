#pragma once
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CStatus abstract : public CComponent
{
protected:
	CStatus(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CStatus(const CStatus& rhs);
	virtual ~CStatus() = default;

public:
	

public:
	virtual HRESULT Initialize_Prototype(const wstring& wstrFilePath);
	virtual HRESULT Initialize(void* pArg, class CGameObject* pOwner) override;
	virtual void		Tick(_float fTimeDelta);
	virtual void		Imgui_RenderProperty() override;

protected:
	_int				m_iMaxHP = 0;
	_int				m_iHP = 0;
	_int				m_iAttack = 0;	

protected:
	virtual HRESULT			Save_Status(const _tchar* pFilePath);
	virtual HRESULT			Load_Status(const _tchar* pFilePath) { return S_OK; }

public:
	virtual CComponent*	Clone(void* pArg = nullptr, class CGameObject* pOwner = nullptr) PURE;
	virtual void				Free() override;
};

END