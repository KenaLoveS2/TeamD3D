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
class CBone;
END

BEGIN(Client)
class CHealthFlower_Anim final : public CEnviromentObj
{
public:
	enum ANIMATION{ BIND, CLOSE, CLOSE_REACT, CLOSE_LOOP, OPEN, OPEN_LOOP, ANIMATION_END };

private:
	CHealthFlower_Anim(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CHealthFlower_Anim(const CHealthFlower_Anim& rhs);
	virtual ~CHealthFlower_Anim() = default;
public:
	virtual HRESULT		Initialize_Prototype() override;
	virtual HRESULT		Initialize(void* pArg) override;
	virtual HRESULT		Late_Initialize(void* pArg) override;
	virtual void		Tick(_float fTimeDelta) override;
	virtual void		Late_Tick(_float fTimeDelta) override;
	virtual HRESULT		Render() override;
	virtual HRESULT       RenderShadow() override;
	virtual void		ImGui_AnimationProperty() override;
	virtual void		ImGui_PhysXValueProperty() override;
	virtual HRESULT		Add_AdditionalComponent(_uint iLevelIndex, const _tchar* pComTag, COMPONENTS_OPTION eComponentOption) override;
	virtual _int		Execute_TriggerTouchFound(CGameObject* pTarget, _uint iTriggerIndex, _int iColliderIndex) override;
	virtual _int		Execute_TriggerTouchLost(CGameObject* pTarget, _uint iTriggerIndex, _int iColliderIndex) override;

public:
	HRESULT				SetUp_Effects();

public:
	Delegator<CUI_ClientManager::UI_PRESENT, _float>	m_FlowerDelegator;

private:
	CShader*			m_pShaderCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CModel*				m_pModelCom = nullptr;
	class CInteraction_Com* m_pInteractionCom = nullptr;
	class CControlMove* m_pControlMoveCom = nullptr;

private:
	class CKena*		m_pKena = nullptr;
	class CCamera_Player* m_pCamera = nullptr;
	class CE_P_ExplosionGravity* m_pExplosionGravity = nullptr;

private:
	_bool				m_bKenaDetected = false;
	_bool				m_bUsed = false;
	_bool				m_bInteractable = false;

	_bool				m_bRenderCheck = false;
	ANIMATION			m_eCurState = OPEN_LOOP;
	ANIMATION			m_ePreState = OPEN_LOOP;

	_float				m_fDissolveTime = 0.f;
	CTexture*			m_pDissolveTexture = nullptr;
			
	class CRotForMonster* m_pBindRots[8] = { nullptr, };

private:
	ANIMATION			Check_State();
	void				Update_State(_float fTimeDelta);
	_bool				Check_CameraRay();

private:
	HRESULT				SetUp_Components();
	HRESULT				SetUp_ShaderResources();
	HRESULT				SetUp_ShadowShaderResources() override;
	HRESULT				Bind_Dissolve(class CShader* pShader);

public:
	static  CHealthFlower_Anim* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void		Free() override;

	void Start_Bind();
	void End_Bind();

	virtual CModel* Get_Model() { return m_pModelCom; }
};
END
