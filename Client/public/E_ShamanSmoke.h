#pragma once
#include "Effect.h"

BEGIN(Client)
class CE_ShamanSmoke final : public CEffect
{
public:
	enum STATE
	{
		STATE_IDLE, 
		STATE_TELEPORT, 
		STATE_END
	};
private:
	CE_ShamanSmoke(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CE_ShamanSmoke(const CE_ShamanSmoke& rhs);
	virtual ~CE_ShamanSmoke() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pFilePath = nullptr);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void    Tick(_float fTimeDelta) override;
	virtual void    Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual void	Imgui_RenderProperty() override;

public:
	void			Set_State(STATE eState, _float4 vPos, _float4 vLeftHandPos = _float4(0.0f, 0.0f, 0.0f, 1.0f));

private:
	HRESULT			SetUp_Components();
	HRESULT			SetUp_ShaderResources();

private:
	STATE			m_eState = STATE_IDLE;
	_float			m_fChildDurationTime = 0.0f;

public:
	static CE_ShamanSmoke*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pFilePath =nullptr);
	virtual CGameObject*	Clone(void* pArg = nullptr)  override;
	virtual void			Free() override;
};

END