#pragma once
#include "Enviroment_Interaction.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CRenderer;
class CShader;
class CModel;
class CRenderer;
END

BEGIN(Client)
class CRope_RotRock : public CEnviroment_Interaction
{
private:
	_float3 m_vInitPosition;
	_float4 m_vMoveTargetPosition;
	

	_bool m_bChoiceFlag = false;
	_bool m_bMoveFlag = false;

	class CKena *m_pKena = nullptr;
	
	class CLiftRot* m_pCuteLiftRot = nullptr;
	class CLiftRot_Master * m_pLiftRotMaster = nullptr;	
	
	_float3 m_vRotCreatePosOffset[LIFT_ROT_COUNT] = {
		_float3(2.f, 0.f, 0.f),
		_float3(2.f, 0.f, -2.f),
		_float3(2.f, 0.f, 2.f ),
		_float3(-2.f, 0.f, 0.f), 
		_float3(-2.f, 0.f, -2.f ),
		_float3(-2.f, 0.f, 2.f ),
	};
	
	_float3 m_vRotLiftPosOffset[LIFT_ROT_COUNT] = {
		_float3(1.2f, 0.f, 0.f), 
		_float3(1.2f, 0.f, -1.2f),
		_float3(1.2f, 0.f, 1.2f),
		_float3(-1.2f, 0.f, 0.f),
		_float3(-1.2f, 0.f, -1.2f),
		_float3(-1.2f, 0.f, 1.2f),
	};

	_float3 m_vPxPivotDist;

public:
	CRope_RotRock(ID3D11Device* pDevice, ID3D11DeviceContext* p_context);
	CRope_RotRock(const CRope_RotRock& rhs);
	virtual ~CRope_RotRock() = default;

	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Late_Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	
	HRESULT SetUp_Components();

	virtual void Imgui_RenderProperty() override;
	virtual HRESULT Call_EventFunction(const string& strFuncName) override;
	virtual void Push_EventFunctions() override;
	
public:
	static CRope_RotRock* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

	virtual	HRESULT SetUp_State() override;


	void Set_MoveTargetPosition(_fvector vPos) { 
		m_vMoveTargetPosition = vPos; 
		m_vMoveTargetPosition.w = 1.f;
	}	

	void Set_ChoiceFlag(_bool bFlag) { m_bChoiceFlag = bFlag; }
	void Set_MoveFlag(_bool bFlag) { m_bMoveFlag = bFlag; }
	_bool  Get_MoveFlag() { return m_bMoveFlag; }

};
END