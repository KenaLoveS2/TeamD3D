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
class CGimmick_EnviObj  final : public CEnviromentObj
{
private:
	CGimmick_EnviObj(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGimmick_EnviObj(const CGimmick_EnviObj& rhs);
	virtual ~CGimmick_EnviObj() = default;

public:
	void				Set_Gimmick_Active(_int iRoomIndex, _bool bGimmick_Active);
public:
	virtual HRESULT		Initialize_Prototype() override;
	virtual HRESULT		Initialize(void* pArg) override;
	virtual HRESULT		Late_Initialize(void* pArg=nullptr)override;
	virtual void				Tick(_float fTimeDelta) override;
	virtual void				Late_Tick(_float fTimeDelta) override;
	virtual HRESULT		Render() override;
	virtual HRESULT		RenderCine() override;
	virtual HRESULT		RenderShadow() override;

public:
	_bool				Gimmik_Start(_float fTimeDelta);
	void				SetUp_GimmicKEffect_Pos();
private:
	_bool				Gimmick_Go_up(_float fTimeDelta);
	HRESULT				Ready_Effect();
private:
	CShader*						m_pShaderCom = nullptr;
	CRenderer*						m_pRendererCom = nullptr;
	CModel*							m_pModelCom = nullptr;

	class CInteraction_Com*			m_pInteractionCom = nullptr;
	class CControlMove*				m_pControlMoveCom = nullptr;
	class CControlRoom*					m_pControlRoom = nullptr;

	class CE_P_EnvironmentDust*		m_pGimmickObjEffect = nullptr;

private:
	_float4							m_vOriginPos;
	_bool							m_bGimmick_Active = false;
	_bool							m_bColliderOn = false;
	_bool							m_bTestOnce = false;
public:
	
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();
	HRESULT SetUp_ShadowShaderResources();
	HRESULT SetUp_CineShaderResources();

public:
	static  CGimmick_EnviObj*	   Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};
END
