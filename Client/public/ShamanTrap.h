#pragma once

#include "Effect_Mesh.h"
#include "Client_Defines.h"

BEGIN(Client)

class CShamanTrap final : public CEffect_Mesh
{
private:
	CShamanTrap(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CShamanTrap(const CShamanTrap& rhs);
	virtual ~CShamanTrap() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* pArg) override;
	virtual void					Tick(_float fTimeDelta) override;
	virtual void					Late_Tick(_float fTimeDelta) override;
	virtual HRESULT			Render() override;

	_bool	IsActiveState();

public:
	virtual void			ImGui_AnimationProperty() override;
	virtual void			Imgui_RenderProperty() override;

private:
	HRESULT					SetUp_Components();
	HRESULT					SetUp_ShaderResources();

private:
	_uint							m_iNumMeshes = 0;

public:
	static CShamanTrap*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	    Clone(void* pArg = nullptr)  override;
	virtual void					    Free() override;
};

END