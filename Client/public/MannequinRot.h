#pragma once
#include "Rot_Base.h"
#include "RotHat.h"

BEGIN(Client)
class CMannequinRot : public CRot_Base
{
private:		
	class CCameraForRot* m_pMyCam = nullptr;
	class CRotHat* m_pRotHats[CRotHat::HAT_TYPE_END] = { nullptr, };
		
	_int m_iCurrentHatIndex = -1;

	_bool m_bShowFlag = false;	
	_bool m_bRender = false;
	_bool m_bEndShow = false;

	_float m_fIdleTimeCheck = 0.f;
	_uint m_iPerchaseAnimIndex = PURCHASEHAT1;

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
	
	virtual void Imgui_RenderProperty() override;
	virtual void ImGui_AnimationProperty() override;
	virtual void ImGui_ShaderValueProperty() override;
	
private:		
	virtual HRESULT SetUp_State() override;
	
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