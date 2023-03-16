#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CShader;
class CModel;
class CFSMComponent;
END

BEGIN(Client)

class CNpc : public CGameObject
{
protected:
	enum NpcShaderPass
	{
		DEFAULT,
		SHADOW,
		BENI_EYE,
		AORM,
		SAIYA_EYE,
		PASS_END
	};

protected:
	CNpc(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNpc(const CNpc& rhs);
	virtual ~CNpc() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* pArg) override;
	virtual HRESULT			Late_Initialize(void* pArg) override;
	virtual void					Tick(_float fTimeDelta) override;
	virtual void					Late_Tick(_float fTimeDelta) override;
	virtual HRESULT			Render() override;
	virtual HRESULT			RenderShadow() override;
	virtual void					Imgui_RenderProperty() override;
	virtual void					ImGui_AnimationProperty() override;
	virtual void					ImGui_ShaderValueProperty() override;
	virtual void					ImGui_PhysXValueProperty() override;
	virtual void					Calc_RootBoneDisplacement(_fvector vDisplacement) override;

public:
	_bool							AnimFinishChecker(_uint eAnim, _double FinishRate = 0.95);
	_bool							AnimIntervalChecker(_uint eAnim, _double StartRate, _double FinishRate);
	_bool							DistanceTrigger(_float distance);
	virtual void					AdditiveAnim(_float fTimeDelta);

protected:
	CRenderer*						m_pRendererCom = nullptr;
	CShader*							m_pShaderCom = nullptr;
	CModel*							m_pModelCom = nullptr;
	CFSMComponent*			m_pFSM = nullptr;
	class CKena*					m_pKena = nullptr;
	class CCameraForNpc*		m_pMyCam = nullptr;

protected:
	virtual void					Update_Collider(_float fTimeDelta) PURE;
	virtual	HRESULT			SetUp_State() PURE;
	virtual	HRESULT			SetUp_Components();
	virtual	HRESULT			SetUp_ShaderResources() PURE;
	virtual HRESULT			SetUp_ShadowShaderResources() PURE;
	virtual HRESULT			SetUp_UI();

public:
	virtual CGameObject*	Clone(void* pArg = nullptr)PURE;
	virtual void					Free() override;
};

END