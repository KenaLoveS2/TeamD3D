#pragma once
#include "Effect_Mesh.h"

BEGIN(Client)

class CE_RotBombExplosion final : public CEffect_Mesh
{
private:
	CE_RotBombExplosion(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CE_RotBombExplosion(const CE_RotBombExplosion& rhs);
	virtual ~CE_RotBombExplosion() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pFilePath = nullptr);
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Late_Initialize(void* pArg = nullptr)override;
	virtual void    Tick(_float fTimeDelta) override;
	virtual void    Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	void			Reset();


private:
	HRESULT SetUp_ShaderResources();
	void	Imgui_RenderProperty() override;

private:
	_float4x4 m_SaveInitWorldMatrix;

public:
	static  CE_RotBombExplosion* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pFilePath = nullptr);
	virtual CGameObject*  Clone(void* pArg = nullptr) override;
	virtual void          Free() override;

};

END