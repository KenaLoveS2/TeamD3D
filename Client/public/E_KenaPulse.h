#pragma once
#include "Effect_Mesh.h"
#include "Client_Defines.h"
#include "PhysX_Defines.h"
BEGIN(Client)

class CE_KenaPulse final : public CEffect_Mesh
{
public:
	enum PULSETYPE { PULSE_DEFAULT, PULSE_PARRY, PULSE_END };

private:
	CE_KenaPulse(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CE_KenaPulse(const CE_KenaPulse& rhs);
	virtual ~CE_KenaPulse() = default;

public:
	_float3			Get_InitMatrixScaled();
	void			Set_NoActive(_bool bActive) { m_bNoActive = bActive; }
	void			Set_InitMatrixScaled(_float3 vScale);
	void			Set_Child();

	void			Set_Type(PULSETYPE eType) { m_ePulseType = eType; }

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pFilePath = nullptr);
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Late_Initialize(void* pArg = nullptr)override;
	virtual void    Tick(_float fTimeDelta) override;
	virtual void    Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	void				Reset();

private:
	HRESULT SetUp_ShaderResources();
	void	Imgui_RenderProperty() override;

private:
	class CTexture* m_pDissolveTexture = nullptr;

	PX_TRIGGER_DATA*		m_pTriggerDAta = nullptr;
	PULSETYPE				m_ePulseType = PULSE_DEFAULT;

private:
	_bool	m_bDesolve = true;
	_bool	m_bNoActive = false;

	_float	m_fActivePlusScale = 1.3f;
	_float	m_fDissolveTime = 0.0f;

	_float4x4 m_SaveInitWorldMatrix;

public:
	static  CE_KenaPulse* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pFilePath = nullptr);
	virtual CGameObject*  Clone(void* pArg = nullptr) override;
	virtual void          Free() override;

};

END