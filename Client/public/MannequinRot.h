#pragma once
#include "GameObject.h"
#include "Client_Defines.h"
#include "RotHat.h"
#include "Rot.h"

BEGIN(Engine)
class CRenderer;
class CShader;
class CModel;
class CBone;
class CFSMComponent;
END

BEGIN(Client)
class CMannequinRot : public CGameObject
{
private:
	CRenderer* m_pRendererCom = nullptr;
	CShader* m_pShaderCom = nullptr;
	CModel* m_pModelCom = nullptr;
	CFSMComponent* m_pFSM = nullptr;

	class CE_TeleportRot* m_pTeleportRot = nullptr;		
	class CCameraForRot* m_pMyCam = nullptr;
	class CRotHat* m_pRotHats[CRotHat::HAT_TYPE_END] = { nullptr, };
	
	_uint m_iNumMeshes = 0;
	_int m_iCurrentHatIndex = -1;

	_bool m_bShowFlag = false;	
	_bool m_bRender = false;
	_bool m_bEndShow = false;

	_float m_fIdleTimeCheck = 0.f;
	_uint m_iPerchaseAnimIndex = CRot::PURCHASEHAT3;

private:
	CMannequinRot(ID3D11Device* pDevice, ID3D11DeviceContext* p_context);
	CMannequinRot(const CMannequinRot& rhs);
	virtual ~CMannequinRot() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Late_Initialize(void * pArg);
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT RenderShadow() override;
	virtual void Imgui_RenderProperty() override;
	virtual void ImGui_AnimationProperty() override;
	virtual void ImGui_ShaderValueProperty() override;
	
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();
	HRESULT SetUp_ShadowShaderResources();
	HRESULT SetUp_State();
	HRESULT SetUp_Effects();

public:
	static CMannequinRot* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
	
	void Create_Hats();
	void Change_Hat(_int iHatIndex);
	void Start_FashiomShow();
	void End_FashiomShow();
};

END