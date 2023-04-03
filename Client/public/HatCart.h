#pragma once
#include "Client_Defines.h"
#include "EnviromentObj.h"
#include "UI_ClientManager.h"
#include "Delegator.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
class CTexture;
END

BEGIN(Client)
class CKena;
class CHatCart  final : public CEnviromentObj
{
private:
	CHatCart(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CHatCart(const CHatCart& rhs);
	virtual ~CHatCart() = default;

public:
	virtual HRESULT						Initialize_Prototype() override;
	virtual HRESULT						Initialize(void* pArg) override;
	virtual HRESULT						Late_Initialize(void* pArg = nullptr);
	virtual void						Tick(_float fTimeDelta) override;
	virtual void						Late_Tick(_float fTimeDelta) override;
	virtual HRESULT						Render() override;
	virtual HRESULT						RenderShadow() override;

public:
	virtual void						Imgui_RenderProperty() override;
	virtual void						ImGui_AnimationProperty() override;
	virtual void						ImGui_PhysXValueProperty() override;
	virtual HRESULT						Add_AdditionalComponent(_uint iLevelIndex, const _tchar* pComTag, COMPONENTS_OPTION eComponentOption) override;
	virtual _int						Execute_Collision(CGameObject* pTarget, _float3 vCollisionPos, _int iColliderIndex) override;
	virtual _int						Execute_TriggerTouchFound(CGameObject* pTarget, _uint iTriggerIndex, _int iColliderIndex) override;
	virtual _int						Execute_TriggerTouchLost(CGameObject* pTarget, _uint iTriggerIndex, _int iColliderIndex) override;

public:
	Delegator<CUI_ClientManager::UI_PRESENT, CKena*, _bool*>		m_CartDelegator;

private:
	CModel* m_pModelCom = nullptr;
	class CInteraction_Com* m_pInteractionCom = nullptr;
	class CControlMove* m_pControlMoveCom = nullptr;

	_bool								m_bPulseTest = true;
	_float								m_fEmissivePulse = 0.f;
	_float								m_fNegativeQuantity = 1.f;

	class CMannequinRot* m_pMannequinRot = nullptr;

	_float3 m_vRotPivotTranlation = { -0.05f, 1.f, 1.27f };
	_float3 m_vRotPivotRotation = { 0.f, 0.f, 0.f };

private:
	HRESULT							SetUp_Components();
	HRESULT							SetUp_ShaderResources();
	HRESULT							SetUp_ShadowShaderResources();

private:
	CKena* m_pPlayer;
	_bool m_bShowUI = false;
public:
	static  CHatCart* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void						Free() override;

	void Create_MannequinRot();
	void Update_MannequinRotMatrix();
	void Change_MannequinHat(_uint iHatIndex);
};
END
