#pragma once
#include "GameInstance.h"
#include "GameObject.h"
#include "Client_Defines.h"


BEGIN(Engine)
class CRenderer;
class CShader;
class CModel;
END

BEGIN(Client)
class CBossRock final : public CGameObject
{
public:
	enum ROCK_TPYE { SMALL_3, SMALL_4, SMALL_5, SMALL_6, ROCK_TPYE_END, };

	typedef struct tagBossRockDesc {
		ROCK_TPYE eType;
		_float4 vPosition;
	} DESC;

private:			
	class CGameInstance* m_pGameInstance = nullptr;
	CRenderer* m_pRendererCom = nullptr;
	CShader* m_pShaderCom = nullptr;
	CModel* m_pModelCom = nullptr;

	_uint iNumMeshes = 0;
	DESC m_Desc;

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
};

END