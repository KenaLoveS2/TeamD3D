#pragma once
#include "Effect_Mesh.h"

BEGIN(Client)

class CE_RotBombExplosion final : public CEffect_Mesh
{
public:
	enum CHILD
	{
		CHILD_COVER,
		CHILD_P,
		CHILD_END
	};

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

public:
	void			Reset();
	void			TurnonBomb(_float fTimeDelta);
	void			TurnoffBomb(_float fTimeDelta);

private:
	HRESULT SetUp_ShaderResources();
	void	Imgui_RenderProperty() override;
	void	ImGui_PhysXValueProperty() override;
	HRESULT SetUp_Effects();

private:
	_float4x4 m_SaveInitWorldMatrix;

	_bool		m_bBomb = false;
	_bool		m_bTurnOn = false;
	_float		m_fBombTime = 0.0f;
	_float		m_fDissolveTime = 0.0f;

public:
	static  CE_RotBombExplosion* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pFilePath = nullptr);
	virtual CGameObject*  Clone(void* pArg = nullptr) override;
	virtual void          Free() override;

};

END