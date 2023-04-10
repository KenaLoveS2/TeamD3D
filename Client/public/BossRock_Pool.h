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
class CBossRock_Pool final : public CGameObject
{
public:
	typedef struct tagBossRockPoolDesc {
		_float4 vCenterPos;
		_uint iRockCount;
	} DESC;

private:			
	class CGameInstance* m_pGameInstance = nullptr;
	vector<class CBossRock*> m_Rocks;

	DESC m_Desc;

private:
	CBossRock_Pool(ID3D11Device* pDevice, ID3D11DeviceContext* p_context);
	CBossRock_Pool(const CBossRock_Pool& rhs);
	virtual ~CBossRock_Pool() = default;

public:
	static CBossRock_Pool* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
	
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Late_Initialize(void * pArg);
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;

	void Execute_UpRocks();
};

END