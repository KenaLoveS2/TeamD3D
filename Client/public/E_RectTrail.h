#pragma once
#include "Effect_Trail.h"

BEGIN(Client)

class CE_RectTrail final : public CEffect_Trail
{
private:
	CE_RectTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CE_RectTrail(const CE_RectTrail& rhs);
	virtual ~CE_RectTrail() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pFilePath = nullptr);
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Late_Initialize(void* pArg) override;
	virtual void    Tick(_float fTimeDelta) override;
	virtual void    Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	virtual void Imgui_RenderProperty()override;

private:
	HRESULT SetUp_ShaderResources();

private:
	class CKena* m_pKena = nullptr;

public:
	static  CE_RectTrail* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pFilePath = nullptr);
	virtual CGameObject*  Clone(void* pArg = nullptr) override;
	virtual void          Free() override;

};

END