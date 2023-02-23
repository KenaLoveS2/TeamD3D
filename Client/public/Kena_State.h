#pragma once
#include "Base.h"
#include "Client_Defines.h"
#include "Transform.h"

BEGIN(Engine)
class CGameInstance;
class CStateMachine;
class CModel;
END

BEGIN(Client)

class CKena_State final : public CBase
{
public:
	enum ANIMATION {

	};

private:
	CKena_State();
	virtual ~CKena_State() = default;

public:
	HRESULT					Initialize(class CKena* pKena, CStateMachine* pStateMachine, CModel* pModel, CTransform* pTransform, class CCamera_Player* pCamera);
	void						Tick(_double dTimeDelta);
	void						Late_Tick(_double dTimeDelta);
	
private:

private:
	CGameInstance*			m_pGameInstance = nullptr;

private:
	class CKena*				m_pKena = nullptr;
	CStateMachine*			m_pStateMachine = nullptr;
	CModel*					m_pModel = nullptr;
	CTransform*				m_pTransform = nullptr;
	class CCamera_Player*	m_pCamera = nullptr;

private:

private:

private:

private:

public:
	static	CKena_State*	Create(class CKena* pKena, CStateMachine* pStateMachine, CModel* pModel, CTransform* pTransform, class CCamera_Player* pCamera);
	virtual void				Free() override;
};

END