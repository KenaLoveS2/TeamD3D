#pragma once
#include "Effect_Mesh.h"
#include "PhysX_Defines.h"

BEGIN(Client)

class CE_EnrageInto final : public CEffect_Mesh
{
private:
	CE_EnrageInto(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CE_EnrageInto(const CE_EnrageInto& rhs);
	virtual ~CE_EnrageInto() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pFilePath = nullptr);
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Late_Initialize(void* pArg = nullptr)override;
	virtual void    Tick(_float fTimeDelta) override;
	virtual void    Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual void	Imgui_RenderProperty() override;
	void			Reset();

private:
	HRESULT SetUp_ShaderResources();
	HRESULT SetUp_Components();

private:
	_float	m_fDurationTime = 0.0f;

public:
	static  CE_EnrageInto* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pFilePath = nullptr);
	virtual CGameObject*		 Clone(void* pArg = nullptr) override;
	virtual void				 Free() override;

};

END