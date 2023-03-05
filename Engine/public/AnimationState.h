#pragma once
#include "Base.h"
#include "Bone.h"

BEGIN(Engine)
class CModel;
class CAnimation;
class CGameObject;

struct ENGINE_DLL  CAdditiveAnimation : public CBase
{
	enum RATIOTYPE { RATIOTYPE_MAX, RATIOTYPE_AUTO, RATIOTYPE_CONTROL, RATIOTYPE_END };
	RATIOTYPE	m_eControlRatio = RATIOTYPE_MAX;
	_bool			m_bPlayReverse = false;
	_float			m_fAdditiveRatio = 0.f;
	_float			m_fMaxAdditiveRatio = 1.f;
	CAnimation*	m_pRefAnim = nullptr;
	CAnimation*	m_pAdditiveAnim = nullptr;
	list<pair<string, CBone::LOCKTO>>	m_listLockedJoint;

	virtual void	Free() override {
		m_listLockedJoint.clear();
	}
};

struct ENGINE_DLL CAnimState : public CBase
{
	string			m_strStateName = "";
	_bool			m_bLoop = true;
	_float			m_fLerpDuration = 0.2f;
	CAnimation*	m_pMainAnim = nullptr;
	CAnimation*	m_pBlendAnim = nullptr;
	vector<CAdditiveAnimation*>		m_vecAdditiveAnim;
	list<pair<string, CBone::LOCKTO>>	m_listLockedJoint;
	map<_float, const string>			m_mapEvent;

	virtual void	Free() override {
		for (auto pAdditiveAnim : m_vecAdditiveAnim)
			Safe_Release(pAdditiveAnim);
		m_vecAdditiveAnim.clear();
		m_listLockedJoint.clear();
		m_mapEvent.clear();
	}
};

class ENGINE_DLL CAnimationState : public CBase
{
public:
	using JOINTSET = pair<string, CBone::LOCKTO>;
	enum ANIMBODY { ANIMBODY_UPPER, ANIMBODY_LOWER, ANIMBODY_ALL, ANIMBODY_END };

private:
	CAnimationState();
	virtual ~CAnimationState() = default;

public:
	CAnimState*		Get_CurrentAnim() { return m_pCurAnim; }
	CAnimState*		Get_PreAnim() { return m_pPreAnim; }
	const _bool&		Get_AnimationFinish();

public:
	HRESULT			Initialize(CGameObject* pOwner, CModel* pModelCom, const string& strRootBone, const string& strFilePath);
	HRESULT			Initialize_FromFile(const string& strFilePath);
	void				Tick(_float fTimeDelta);
	HRESULT			State_Animation(const string& strStateName, ANIMBODY eAnimBody = ANIMBODY_ALL);
	void				Play_Animation(_float fTimeDelta);
	void				ImGui_RenderProperty();

	HRESULT			Generate_Animation(const string& strFilePath);
	HRESULT			Add_State(CAnimState* pAnim);
	HRESULT			Add_AnimSharingPart(CModel* pModel, _bool bMeshSync);
	HRESULT			Save(const string& strFilePath);
	HRESULT			Load(const string& strFilePath);

private:
	CGameObject*	m_pOwner = nullptr;
	CModel*			m_pModel = nullptr;
	vector<CModel*>	m_vecSyncPart;
	vector<CModel*>	m_vecNonSyncPart;


private:
	map<const string, CAnimState*>		m_mapAnimState;
	CAnimState*		m_pCurAnim = nullptr;
	CAnimState*		m_pPreAnim = nullptr;
	string				m_strRootBone = "";

	_float				m_fCurLerpTime = 0.f;
	_float				m_fLerpDuration = 0.f;

	_smatrix			m_matBonesTransformation[800];

public:
	static CAnimationState*	Create(CGameObject* pOwner, CModel* pModelCom, const string& strRootBone, const string& strFilePath = "");
	virtual void					Free() override {
		m_vecSyncPart.clear();
		m_vecNonSyncPart.clear();

		for (auto& pAnimState : m_mapAnimState)
			Safe_Release(pAnimState.second);
		m_mapAnimState.clear();
	}
};

END