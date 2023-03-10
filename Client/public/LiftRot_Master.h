#pragma once
#include "GameInstance.h"
#include "GameObject.h"
#include "Client_Defines.h"
#include "LiftRot.h"

BEGIN(Engine)
class CRenderer;
class CShader;
class CModel;
class CRenderer;
class CFSMComponent;
END

BEGIN(Client)
class CLiftRot_Master final : public CGameObject
{
private:			
	class CGameInstance* m_pGameInstacne = nullptr;
	class CLiftRot* m_pLiftRotArr[LIFT_ROT_COUNT] = { nullptr, };
	
private:
	CLiftRot_Master(ID3D11Device* pDevice, ID3D11DeviceContext* p_context);
	CLiftRot_Master(const CLiftRot_Master& rhs);
	virtual ~CLiftRot_Master() = default;

public:
	static CLiftRot_Master* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
	
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Late_Initialize(void * pArg);
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;

	void Execute_WakeUp(_float4 vCenterPos, _float3* pCreatePosOffsetArr, _float3* pLiftPosOffsetArr);
	void Execute_LiftStart();
	void Execute_LiftMoveStart();
	void Execute_Move(_float4 vCenterPos, _float3 *pOffsetPosArr);
	void Execute_LiftMoveEnd();
	void Execute_LiftDownEnd();

	_bool Is_LiftReady();
	_bool Is_LiftEnd();
};

END