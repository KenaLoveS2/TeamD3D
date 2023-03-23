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
	enum ADDITIVETYPE { ADDITIVE, REPLACE, ADDITIVETYPE_END };

	RATIOTYPE	m_eControlRatio = RATIOTYPE_MAX;
	ADDITIVETYPE	m_eAdditiveType = ADDITIVE;
	_bool			m_bPlayReverse = false;
	_float			m_fAdditiveRatio = 0.f;
	_float			m_fMaxAdditiveRatio = 1.f;
	CAnimation*	m_pRefAnim = nullptr;
	CAnimation*	m_pAdditiveAnim = nullptr;
	list<pair<string, CBone::LOCKTO>>	m_listLockedJoint;
	list<_uint>								m_listLockedBoneIndex;

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
	list<_uint>								m_listLockedBoneIndex;

	//_bool			m_bHasLookAnim = false;
	//vector<CAdditiveAnimation*>		m_vecLookAnim;

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

private:
	CAnimationState();
	virtual ~CAnimationState() = default;

public:
	CAnimState*		Get_CurrentAnim() { return m_pCurAnim; }
	CAnimState*		Get_PreAnim() { return m_pPreAnim; }
	const string&		Get_CurrentAnimName() const { return m_pCurAnim->m_strStateName; }
	const string&		Get_PreAnimName() const { return m_pPreAnim->m_strStateName; }
	const _uint		Get_CurrentAnimIndex() const;
	const _uint		Get_PreAnimIndex() const;
	const _bool&		Get_AnimationFinish();
	const _bool		Get_AnimationFinish(const string& strStateName);
	const _float		Get_AnimationProgress() const;
	const _bool&		Get_Preview() const { return m_bPreview; }

public:
	HRESULT			Initialize(CGameObject* pOwner, CModel* pModelCom, const string& strRootBone, const string& strFilePath);
	HRESULT			Initialize_FromFile(const string& strFilePath);
	void				Tick(_float fTimeDelta);
	HRESULT			State_Animation(const string& strStateName, _float fLerpDuration = -1.f);
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

	_bool				m_bPreview = false;

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