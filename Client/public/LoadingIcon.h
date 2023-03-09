#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CVIBuffer_Rect;
END

BEGIN(Client)
class CLoadingIcon final : public CGameObject
{
private:
	CLoadingIcon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLoadingIcon(const CLoadingIcon& rhs);
	virtual ~CLoadingIcon() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CVIBuffer_Rect*			m_pVIBufferCom = nullptr;

	/* Test */
	_float					m_fAngle;
	_float					m_fTime;
	_float					m_fTimeAcc;

private:
	_float4x4				m_ViewMatrix;
	_float4x4				m_ProjMatrix;
	_float					m_fX, m_fY, m_fSizeX, m_fSizeY;

private:
	HRESULT			SetUp_Components();
	HRESULT			SetUp_ShaderResources();

public:
	static CLoadingIcon*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg = nullptr) override;
	virtual void			Free() override;
};

END