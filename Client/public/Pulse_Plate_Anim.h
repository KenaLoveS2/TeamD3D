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
class CControlRoom;
class CPulse_Plate_Anim final : public CEnviromentObj
{
private:
	enum COLLIDERTYPE
	{
		COLL_PLAYER = 0,
		COLL_END,
	};


private:
	CPulse_Plate_Anim(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPulse_Plate_Anim(const CPulse_Plate_Anim& rhs);
	virtual ~CPulse_Plate_Anim() = default;

public:
	const _bool Get_PlayerColl()const { return m_bPlayerColl; }

public:
	virtual HRESULT		Initialize_Prototype() override;
	virtual HRESULT		Initialize(void* pArg) override;
	virtual HRESULT		Late_Initialize(void* pArg =nullptr/* = nullptr */)override;
	virtual void				Tick(_float fTimeDelta) override;
	virtual void				Late_Tick(_float fTimeDelta) override;
	virtual HRESULT		Render() override;

public:
	virtual void					ImGui_PhysXValueProperty() override;
	virtual void					ImGui_AnimationProperty() override;
private: 
	CShader*				m_pShaderCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CModel*				m_pModelCom = nullptr;
	class CInteraction_Com*			m_pInteractionCom = nullptr;
	class CControlMove*				m_pControlMoveCom = nullptr;
	class		CKena *m_pKena = nullptr;
	CTransform* m_pKenaTransform = nullptr;
	CControlRoom*					m_pControlRoom = nullptr;

private:
	_bool							m_bPlayerColl = false;
	_bool							m_bFirstCinema = false;
	_bool							m_bSecondCinema = false;

public:
	virtual HRESULT		Add_AdditionalComponent(_uint iLevelIndex, const _tchar* pComTag, COMPONENTS_OPTION eComponentOption)override;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();

private:
	void	Pulse_Plate_AnimControl(_float fTimeDelta);

public:
	virtual _int Execute_Collision(CGameObject * pTarget, _float3 vCollisionPos, _int iColliderIndex) override;
	virtual _int Execute_TriggerTouchFound(CGameObject* pTarget, _uint iTriggerIndex, _int iColliderIndex) override;
	virtual _int Execute_TriggerTouchLost(CGameObject* pTarget, _uint iTriggerIndex, _int iColliderIndex) override;

public:
	static  CPulse_Plate_Anim*	   Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};
END
