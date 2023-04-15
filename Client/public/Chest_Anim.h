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

class CChest_Anim final : public CEnviromentObj
{
public:
	enum ANIMATION { CURSED_ACTIVATE, CURSED_CLEARED, OPEN, ANIMATION_END };

private:
	CChest_Anim(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CChest_Anim(const CChest_Anim& rhs);
	virtual ~CChest_Anim() = default;

public:
	virtual HRESULT				Initialize_Prototype() override;
	virtual HRESULT				Initialize(void* pArg) override;
	virtual HRESULT				Late_Initialize(void* pArg) override;
	virtual void						Tick(_float fTimeDelta) override;
	virtual void						Late_Tick(_float fTimeDelta) override;
	virtual HRESULT				Render() override;
	virtual HRESULT				RenderShadow() override;

public:
	virtual void				Imgui_RenderProperty() override;
	virtual void				ImGui_AnimationProperty() override;
	virtual void				ImGui_PhysXValueProperty() override;
	
	virtual _int				Execute_Collision(CGameObject* pTarget, _float3 vCollisionPos, _int iColliderIndex) override;
	virtual _int				Execute_TriggerTouchFound(CGameObject* pTarget, _uint iTriggerIndex, _int iColliderIndex) override;
	virtual _int				Execute_TriggerTouchLost(CGameObject* pTarget, _uint iTriggerIndex, _int iColliderIndex) override;

	virtual void			Push_EventFunctions() override;
	void					Chest_FirstMeetSound(_bool bIsInit, _float fTimeDelta);
	void					Chest_OpenSound(_bool bIsInit, _float fTimeDelta);
	void					Chest_CloseSound(_bool bIsInit, _float fTimeDelta);
	void					Chest_Clear(_bool bIsInit, _float fTimeDelta);
private:
	CModel*								m_pModelCom = nullptr;


private:
	class CKena*			m_pKena = nullptr;
	CTransform*				m_pKenaTransform = nullptr;
	_bool						m_bRenderCheck = false;
private:
	_bool						m_bKenaDetected = false;
	_bool						m_bOpened = false;

	ANIMATION				m_eCurState = CURSED_CLEARED;
	ANIMATION				m_ePreState = CURSED_CLEARED;

	_bool					m_bTestOnce = false;

private:
	ANIMATION					Check_State();
	void						Update_State(_float fTimeDelta);

private:
	HRESULT						SetUp_Components();
	HRESULT						SetUp_ShaderResources();
	HRESULT						SetUp_ShadowShaderResources() override;

private:
	/* Effects */
	class CE_Chest* m_pChestEffect = nullptr;
	class CE_P_Chest* m_pChestEffect_P = nullptr;

private:
	HRESULT						Ready_Effect();

public:
	static  CChest_Anim*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg = nullptr) override;
	virtual void				Free() override;
};

END