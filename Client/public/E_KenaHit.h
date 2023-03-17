#pragma once
#include "Client_Defines.h"
#include "Effect_Mesh.h"

BEGIN(Client)

class CE_KenaHit final : public CEffect_Mesh
{
private:
	CE_KenaHit(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CE_KenaHit(const CE_KenaHit& rhs);
	virtual ~CE_KenaHit() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pFilePath = nullptr);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void    Tick(_float fTimeDelta) override;
	virtual void    Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual void    Imgui_RenderProperty();

private:
	_float			m_fScaleTime = 0.0f;
	_float			m_fAddValue = 0.0f;

public:
	void			Set_Child();

public:
	static  CE_KenaHit* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pFilePath = nullptr);
	virtual CGameObject*  Clone(void* pArg = nullptr) override;
	virtual void          Free() override;
};

END