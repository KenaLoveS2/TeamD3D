#pragma once
#include "Effect_Mesh.h"

BEGIN(Engine)
class CTexture;
END

BEGIN(Client)

class CE_ShamanSummons final : public CEffect_Mesh
{
private:
	CE_ShamanSummons(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CE_ShamanSummons(const CE_ShamanSummons& rhs);
	virtual ~CE_ShamanSummons() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar * pFilePath = nullptr);
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Late_Initialize(void* pArg = nullptr)override;
	virtual void    Tick(_float fTimeDelta) override;
	virtual void    Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual void	Imgui_RenderProperty() override;

	void			Reset();
	void			Set_Dissolve(_bool bDissolve) { m_bDissolve = bDissolve; }

private:
	HRESULT SetUp_ShaderResources();
	HRESULT SetUp_Components();

	HRESULT SetUp_Effects();

private:
	class CTexture* m_pShamanTextureCom = nullptr;
	_uint			m_iShamanTexture = 0;

	_bool			m_bTurnInto = false;
	_bool			m_bDissolve = false;
	_float			m_fDissolveTime = 1.0f;

public:
	static  CE_ShamanSummons* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar * pFilePath = nullptr);
	virtual CGameObject*	  Clone(void* pArg = nullptr) override;
	virtual void			  Free() override;

};

END