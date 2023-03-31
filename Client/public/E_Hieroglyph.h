#pragma once
#include "Effect.h"
#include "PhysX_Defines.h"

BEGIN(Client)

class CE_Hieroglyph final : public CEffect
{
private:
	CE_Hieroglyph(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CE_Hieroglyph(const CE_Hieroglyph& rhs);
	virtual ~CE_Hieroglyph() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pFilePath = nullptr);
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Late_Initialize(void* pArg = nullptr)override;
	virtual void    Tick(_float fTimeDelta) override;
	virtual void    Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	void			Reset();

	virtual _int	Execute_Collision(CGameObject* pTarget, _float3 vCollisionPos, _int iColliderIndex) override;
	virtual void	ImGui_PhysXValueProperty()override;

public:
	void			Set_TexRandomPrint(_float fHeight);

private:
	HRESULT			SetUp_ShaderResources();
	HRESULT			SetUp_Components();
	void			Imgui_RenderProperty() override;

private:
	_float4x4				m_SaveInitWorldMatrix;
	
public:
	static  CE_Hieroglyph* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pFilePath = nullptr);
	virtual CGameObject*	   Clone(void* pArg = nullptr) override;
	virtual void               Free() override;

};

END