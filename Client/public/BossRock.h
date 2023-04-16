#pragma once
#include "GameInstance.h"
#include "GameObject.h"
#include "Client_Defines.h"


BEGIN(Engine)
class CRenderer;
class CShader;
class CModel;
class CGameInstance;
END

BEGIN(Client)
class CBossRock final : public CGameObject
{
public:
	enum ROCK_TPYE { SMALL_3, SMALL_4, SMALL_5, SMALL_6, ROCK_TPYE_END, };
	enum STATE { UP, DOWN, STATE_END };

	typedef struct tagBossRockDesc {
		ROCK_TPYE eType;
		_float4 vPosition;
		_float fUpTime;
		_float fSpeedY;
	} DESC;

private:			
	CGameInstance* m_pGameInstance = nullptr;
	CRenderer* m_pRendererCom = nullptr;
	CShader* m_pShaderCom = nullptr;
	CModel* m_pModelCom = nullptr;

	_uint m_iNumMeshes = 0;
	DESC m_Desc;
	STATE m_eState = STATE_END;

	_float m_fUpTimeCheck = 0.f;
	_tchar* m_pCopySoundkey_Throw = nullptr;
	_tchar* m_pCopySoundkey_Impact = nullptr;
	_bool m_bGroundCollision = false;

	_int m_iThisRockIndex = -1;
	
private:
	CBossRock(ID3D11Device* pDevice, ID3D11DeviceContext* p_context);
	CBossRock(const CBossRock& rhs);
	virtual ~CBossRock() = default;

public:
	static CBossRock* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
	
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Late_Initialize(void * pArg);
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();
	
	void BossRockProc(_float fTimeDelta);
	void Exectue_Up();

	HRESULT Setup_RockSounds();
	_int Execute_Collision(CGameObject* pTarget, _float3 vCollisionPos, _int iColliderIndex);
	void Set_RockIndex(_uint iIndex);
};

END