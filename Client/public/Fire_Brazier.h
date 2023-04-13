#pragma once
#include "Client_Defines.h"
#include "EnviromentObj.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
class CTexture;
END

BEGIN(Client)
class CFire_Brazier  final : public CEnviromentObj
{
private:
	CFire_Brazier(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CFire_Brazier(const CFire_Brazier& rhs);
	virtual ~CFire_Brazier() = default;

public:
	virtual HRESULT		Initialize_Prototype() override;
	virtual HRESULT		Initialize(void* pArg) override;
	virtual HRESULT		Late_Initialize(void* pArg = nullptr);
	virtual void				Tick(_float fTimeDelta) override;
	virtual void				Late_Tick(_float fTimeDelta) override;
	virtual HRESULT		Render() override;
	virtual HRESULT		RenderShadow() override;

	virtual HRESULT		RenderCine() override;

public:
	virtual void				Imgui_RenderProperty() override;
	virtual void				ImGui_ShaderValueProperty() override;
	virtual void				ImGui_PhysXValueProperty() override;

private:
	CModel* m_pModelCom = nullptr;
	class CInteraction_Com* m_pInteractionCom = nullptr;
	class CControlMove* m_pControlMoveCom = nullptr;

	class CE_FireBrazier* m_pFireBrazierEffect = nullptr;
	class CE_P_CommonBox* m_pCommonBox = nullptr;

	_bool										m_bPulseTest = true;
	_float										m_fEmissivePulse = 0.f;
	_float										m_fNegativeQuantity = 1.f;


public:
	virtual _int Execute_Collision(CGameObject* pTarget, _float3 vCollisionPos, _int iColliderIndex) override;
	virtual _int Execute_TriggerTouchFound(CGameObject* pTarget, _uint iTriggerIndex, _int iColliderIndex) override;
	virtual _int Execute_TriggerTouchLost(CGameObject* pTarget, _uint iTriggerIndex, _int iColliderIndex) override;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();
	HRESULT SetUp_ShadowShaderResources();
	HRESULT Ready_Effect();

public:
	static  CFire_Brazier* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};
END
