#include "stdafx.h"
#include "..\public\AnimationState.h"
#include "Json/json.hpp"
#include <fstream>
#include "Utile.h"
#include "GameObject.h"
#include "Model.h"

CAnimationState::CAnimationState()
{
}

const _uint CAnimationState::Get_CurrentAnimIndex() const
{
	if (m_pCurAnim != nullptr)
	{
		if (m_pCurAnim->m_vecAdditiveAnim.empty() == true)
			return m_pCurAnim->m_pMainAnim->Get_AnimIndex();
		else
			return m_pCurAnim->m_vecAdditiveAnim.front()->m_pAdditiveAnim->Get_AnimIndex();
	}
	else
		return 10000;
}

const _uint CAnimationState::Get_PreAnimIndex() const
{
	if (m_pPreAnim != nullptr)
	{
		if (m_pPreAnim->m_vecAdditiveAnim.empty() == true)
			return m_pPreAnim->m_pMainAnim->Get_AnimIndex();
		else
			return m_pPreAnim->m_vecAdditiveAnim.front()->m_pAdditiveAnim->Get_AnimIndex();
	}
	else
		return 10000;
}

const _bool & CAnimationState::Get_AnimationFinish()
{
	if (m_pCurAnim->m_vecAdditiveAnim.empty() == true)
		return m_pCurAnim->m_pMainAnim->IsFinished();
	else
		return m_pCurAnim->m_vecAdditiveAnim.front()->m_pAdditiveAnim->IsFinished();
}

const _float CAnimationState::Get_AnimationProgress() const
{
	if (m_pCurAnim->m_vecAdditiveAnim.empty() == true)
		return m_pCurAnim->m_pMainAnim->Get_AnimationProgress();
	else
		return m_pCurAnim->m_vecAdditiveAnim.front()->m_pAdditiveAnim->Get_AnimationProgress();
}

HRESULT CAnimationState::Initialize(CGameObject * pOwner, CModel * pModelCom, const string & strRootBone, const string & strFilePath)
{
	NULL_CHECK_RETURN(pOwner, E_FAIL);
	NULL_CHECK_RETURN(pModelCom, E_FAIL);

	m_pOwner = pOwner;
	m_pModel = pModelCom;
	m_strRootBone = strRootBone;

	if (strFilePath != "")
		return S_FALSE;

	return S_OK;
}

HRESULT CAnimationState::Initialize_FromFile(const string & strFilePath)
{
	Json	jStates;

	ifstream file(strFilePath.c_str());
	file >> jStates;
	file.close();

	CAnimState*	pAnimState = nullptr;
	CAnimation*	pAnim = nullptr;
	string			strMainAnim = "";
	string			strBlendAnim = "";
	string			strJoint = "";
	string			strLockTo = "";

	for (auto jAnimState : jStates)
	{
		pAnimState = new CAnimState;

		jAnimState["0. State Name"].get_to<string>(pAnimState->m_strStateName);

		jAnimState["1. Looping"].get_to<_bool>(pAnimState->m_bLoop);

		jAnimState["2. Lerp Duration"].get_to<_float>(pAnimState->m_fLerpDuration);

		jAnimState["3. Main Animation"].get_to<string>(strMainAnim);
		pAnim = m_pModel->Find_Animation(strMainAnim);
		NULL_CHECK_RETURN(pAnim, E_FAIL);
		pAnimState->m_pMainAnim = pAnim;

		jAnimState["4. Blend Animation"].get_to<string>(strBlendAnim);
		if (strBlendAnim != "N/A")
		{
			pAnim = m_pModel->Find_Animation(strBlendAnim);
			NULL_CHECK_RETURN(pAnim, E_FAIL);
			pAnimState->m_pBlendAnim = pAnim;
		}

		if (jAnimState["5. Joint Index"].is_array() == true)
		{
			for (auto jJoint : jAnimState["5. Joint Index"])
			{
				jJoint["0. Bone Name"].get_to<string>(strJoint);
				jJoint["1. Lock To"].get_to<string>(strLockTo);
				CBone::LOCKTO	eLockto = CBone::LOCKTO_END;

				if (strLockTo == "Lock Child")
					eLockto = CBone::LOCKTO_CHILD;
				else if (strLockTo == "Lock Parent")
					eLockto = CBone::LOCKTO_PARENT;
				else if (strLockTo == "Lock Alone")
					eLockto = CBone::LOCKTO_ALONE;
				else if (strLockTo == "Unlock Child")
					eLockto = CBone::UNLOCKTO_CHILD;
				else if (strLockTo == "Unlock Parent")
					eLockto = CBone::UNLOCKTO_PARENT;
				else if (strLockTo == "Unlock Alone")
					eLockto = CBone::UNLOCKTO_ALONE;
				else
					return E_FAIL;

				pAnimState->m_listLockedJoint.push_back(pair<string, CBone::LOCKTO>{strJoint, eLockto});
			}
		}

		if (jAnimState["6. Additive Animation"].is_array() == true)
		{
			CAdditiveAnimation*		pAdditive = nullptr;
			for (auto jAdditive : jAnimState["6. Additive Animation"])
			{
				pAdditive = new CAdditiveAnimation;

				_int	iRatioControl = 0;
				jAdditive["0. Ratio Control"].get_to<_int>(iRatioControl);
				pAdditive->m_eControlRatio = (CAdditiveAnimation::RATIOTYPE)iRatioControl;

				jAdditive["1. Max Additive Ratio"].get_to<_float>(pAdditive->m_fMaxAdditiveRatio);

				jAdditive["2. Reference Animation"].get_to<string>(strMainAnim);
				pAnim = m_pModel->Find_Animation(strMainAnim);
				NULL_CHECK_RETURN(pAnim, E_FAIL);
				pAdditive->m_pRefAnim = pAnim;

				jAdditive["3. Additive Animation"].get_to<string>(strBlendAnim);
				pAnim = m_pModel->Find_Animation(strBlendAnim);
				NULL_CHECK_RETURN(pAnim, E_FAIL);
				pAdditive->m_pAdditiveAnim = pAnim;

				if (jAdditive["4. Joint Index"].is_array() == true)
				{
					for (auto jJoint : jAdditive["4. Joint Index"])
					{
						jJoint["0. Bone Name"].get_to<string>(strJoint);
						jJoint["1. Lock To"].get_to<string>(strLockTo);
						CBone::LOCKTO	eLockto = CBone::LOCKTO_END;

						if (strLockTo == "Lock Child")
							eLockto = CBone::LOCKTO_CHILD;
						else if (strLockTo == "Lock Parent")
							eLockto = CBone::LOCKTO_PARENT;
						else if (strLockTo == "Lock Alone")
							eLockto = CBone::LOCKTO_ALONE;
						else if (strLockTo == "Unlock Child")
							eLockto = CBone::UNLOCKTO_CHILD;
						else if (strLockTo == "Unlock Parent")
							eLockto = CBone::UNLOCKTO_PARENT;
						else if (strLockTo == "Unlock Alone")
							eLockto = CBone::UNLOCKTO_ALONE;
						else
							return E_FAIL;

						pAdditive->m_listLockedJoint.push_back(pair<string, CBone::LOCKTO>{strJoint, eLockto});
					}
				}
				pAnimState->m_vecAdditiveAnim.push_back(pAdditive);
			}
		}
		m_mapAnimState.emplace(pAnimState->m_strStateName, pAnimState);
	}

	return S_OK;
}

void CAnimationState::Tick(_float fTimeDelta)
{
	/* TODO : Additive Ratio Controller */
}

HRESULT CAnimationState::State_Animation(const string & strStateName, _float fLerpDuration)
{
	const auto	iter = find_if(m_mapAnimState.begin(), m_mapAnimState.end(), [&strStateName](const pair<const string, CAnimState*>& Pair) {
		return Pair.first == strStateName;
	});

	if (iter == m_mapAnimState.end())
		return E_FAIL;

	CAnimState*		pAnim = iter->second;
	NULL_CHECK_RETURN(pAnim->m_pMainAnim, E_FAIL);

	if (fLerpDuration < 0.f)
		m_fLerpDuration = pAnim->m_fLerpDuration;
	else
		m_fLerpDuration = fLerpDuration;

	if (pAnim->m_listLockedJoint.empty() == false)
	{
		CBone*	pBone = nullptr;
		for (auto& Joint : pAnim->m_listLockedJoint)
		{
			pBone = m_pModel->Get_BonePtr(Joint.first.c_str());
			NULL_CHECK_RETURN(pBone, E_FAIL);
			pBone->Set_BoneLocked(Joint.second);
		}
	}

	if (m_pCurAnim != nullptr)
	{
		m_pPreAnim = m_pCurAnim;
		m_fCurLerpTime = 0.f;

		if (pAnim->m_pMainAnim != m_pPreAnim->m_pMainAnim)
		{
			pAnim->m_pMainAnim->Reset_Animation();
			if (pAnim->m_pBlendAnim != nullptr)
			{
				if (pAnim->m_pBlendAnim == m_pPreAnim->m_pMainAnim)
					pAnim->m_pMainAnim->Set_PlayTime(pAnim->m_pBlendAnim->Get_PlayTime());
				else
					pAnim->m_pBlendAnim->Reset_Animation();
			}
		}
		else
		{
			if (pAnim->m_pBlendAnim != nullptr)
				pAnim->m_pBlendAnim->Set_PlayTime(m_pPreAnim->m_pMainAnim->Get_PlayTime());
		}
	}
	else
		m_fCurLerpTime = m_fLerpDuration;

	m_pCurAnim = pAnim;

	if (m_pCurAnim->m_vecAdditiveAnim.empty() == false)
	{
		for (auto pAdditiveAnim : m_pCurAnim->m_vecAdditiveAnim)
		{
			pAdditiveAnim->m_pRefAnim->Reset_Animation();
			pAdditiveAnim->m_pAdditiveAnim->Reset_Animation();

			if (pAdditiveAnim->m_eControlRatio == CAdditiveAnimation::RATIOTYPE_AUTO)
				pAdditiveAnim->m_fAdditiveRatio = 0.f;
		}
	}

	return S_OK;
}

void CAnimationState::Play_Animation(_float fTimeDelta)
{
	//NULL_CHECK_RETURN(m_pCurAnim, );

	if (m_pCurAnim == nullptr)
		return;

	CAnimation*	pPreAnim = nullptr;
	CAnimation*	pPreBlendAnim = nullptr;
	CAnimation*	pMainAnim = m_pCurAnim->m_pMainAnim;
	CAnimation*	pBlendAnim = m_pCurAnim->m_pBlendAnim;
	CBone*		pJoint = nullptr;

	/* Lerp */
	if (m_fCurLerpTime < m_fLerpDuration)
	{
		/* Pre State Update */
		m_pModel->Set_AllBonesUnlock();
		if (m_pPreAnim->m_listLockedJoint.empty() == false)
		{
			for (auto& Pair : m_pPreAnim->m_listLockedJoint)
			{
				pJoint = m_pModel->Get_BonePtr(Pair.first.c_str());
				assert(pJoint != nullptr);
				pJoint->Set_BoneLocked(Pair.second);
			}
		}

		pPreAnim = m_pPreAnim->m_pMainAnim;
		pPreBlendAnim = m_pPreAnim->m_pBlendAnim;
		pPreAnim->Update_Bones_ReturnMat(fTimeDelta, m_matBonesTransformation, m_strRootBone, pPreBlendAnim);

		if (pPreAnim == pMainAnim || pPreAnim == pBlendAnim)
			pPreAnim->Reverse_Play(fTimeDelta);

		if (pPreBlendAnim != nullptr)
		{
			if (pPreBlendAnim == pMainAnim || pPreBlendAnim == pBlendAnim)
				pPreBlendAnim->Reverse_Play(fTimeDelta);
		}

		if (m_pPreAnim->m_vecAdditiveAnim.empty() == false)
		{
			for (auto pAdditiveAnim : m_pPreAnim->m_vecAdditiveAnim)
			{
				m_pModel->Set_AllBonesUnlock();

				if (pAdditiveAnim->m_listLockedJoint.empty() == false)
				{
					for (auto& Pair : pAdditiveAnim->m_listLockedJoint)
					{
						pJoint = m_pModel->Get_BonePtr(Pair.first.c_str());
						assert(pJoint != nullptr);
						pJoint->Set_BoneLocked(Pair.second);
					}
				}

				pAdditiveAnim->m_pAdditiveAnim->Update_Bones_Additive_ReturnMat(fTimeDelta, pAdditiveAnim->m_fAdditiveRatio, m_matBonesTransformation, m_strRootBone);
			}
		}

		/* Current State Blend */
		m_pModel->Set_AllBonesUnlock();

		if (m_pCurAnim->m_listLockedJoint.empty() == false)
		{
			for (auto& Pair : m_pCurAnim->m_listLockedJoint)
			{
				pJoint = m_pModel->Get_BonePtr(Pair.first.c_str());
				assert(pJoint != nullptr);
				pJoint->Set_BoneLocked(Pair.second);
			}
		}

		//if (pBlendAnim != nullptr && pBlendAnim == pPreAnim)
		//	pBlendAnim->Reverse_Play(fTimeDelta);

		pMainAnim->Update_Bones_Blend_ReturnMat(fTimeDelta, m_fCurLerpTime / m_fLerpDuration, m_matBonesTransformation, m_strRootBone, pBlendAnim);
		/* TODO : Check 'Is Additive Process needs Lerp' */		

		m_fCurLerpTime += fTimeDelta;
	}
	/* Common */
	else
	{
		/* Current State Update*/
		m_pModel->Set_AllBonesUnlock();
		if (m_pCurAnim->m_listLockedJoint.empty() == false)
		{
			for (auto& Pair : m_pCurAnim->m_listLockedJoint)
			{
				pJoint = m_pModel->Get_BonePtr(Pair.first.c_str());
				assert(pJoint != nullptr);
				pJoint->Set_BoneLocked(Pair.second);
			}
		}
		pMainAnim->Update_Bones_ReturnMat(fTimeDelta, m_matBonesTransformation, m_strRootBone, pBlendAnim);
	}
	//m_pModel->Compute_CombindTransformationMatrix();

	/* Additive */
	if (m_pCurAnim->m_vecAdditiveAnim.empty() == false)
	{
		//m_pCurAnim->m_vecAdditiveAnim.front()->m_pRefAnim->Update_Bones_ReturnMat(0.f, m_matBonesTransformation);

		/* TODO : Check 'Is Additive Process needs Lerp' */
		for (auto pAdditiveAnim : m_pCurAnim->m_vecAdditiveAnim)
		{
			m_pModel->Set_AllBonesUnlock();

			if (pAdditiveAnim->m_listLockedJoint.empty() == false)
			{
				for (auto& Pair : pAdditiveAnim->m_listLockedJoint)
				{
					pJoint = m_pModel->Get_BonePtr(Pair.first.c_str());
					assert(pJoint != nullptr);
					pJoint->Set_BoneLocked(Pair.second);
				}
			}

			CUtile::Saturate<_float>(pAdditiveAnim->m_fAdditiveRatio, 0.f, pAdditiveAnim->m_fMaxAdditiveRatio);

			if (pAdditiveAnim->m_eControlRatio == CAdditiveAnimation::RATIOTYPE_MAX)
				pAdditiveAnim->m_pAdditiveAnim->Update_Bones_Additive_ReturnMat(fTimeDelta, 1.f, m_matBonesTransformation, m_strRootBone);
			else if (pAdditiveAnim->m_eControlRatio == CAdditiveAnimation::RATIOTYPE_AUTO)
			{
				if (pAdditiveAnim->m_bPlayReverse == false)
				{
					if (pAdditiveAnim->m_fAdditiveRatio < pAdditiveAnim->m_fMaxAdditiveRatio)
						pAdditiveAnim->m_fAdditiveRatio += fTimeDelta * 0.05f;
					else
						pAdditiveAnim->m_bPlayReverse = true;
				}
				else
				{
					if (pAdditiveAnim->m_fAdditiveRatio > 0.f)
						pAdditiveAnim->m_fAdditiveRatio -= fTimeDelta * 0.05f;
					else
					{
						pAdditiveAnim->m_fAdditiveRatio = 0.f;
						pAdditiveAnim->m_bPlayReverse = false;
					}
				}

				pAdditiveAnim->m_pAdditiveAnim->Update_Bones_Additive_ReturnMat(fTimeDelta, pAdditiveAnim->m_fAdditiveRatio, m_matBonesTransformation, m_strRootBone);
			}
			else if (pAdditiveAnim->m_eControlRatio == CAdditiveAnimation::RATIOTYPE_CONTROL)
				pAdditiveAnim->m_pAdditiveAnim->Update_Bones_Additive_ReturnMat(fTimeDelta, pAdditiveAnim->m_fAdditiveRatio, m_matBonesTransformation, m_strRootBone);
		}
	}
	m_pModel->Compute_CombindTransformationMatrix();
	
 	for (auto pModel : m_vecNonSyncPart)
 		pModel->Update_BonesMatrix(m_pModel);
}

void CAnimationState::ImGui_RenderProperty()
{
	static _bool	bSave = false;
	static char	szFilePath[MAX_PATH] = "";
	static _int	iSelectState = -1;
	char**			ppStateName = nullptr;

	if (m_pCurAnim != nullptr)
	{
		ImGui::Text("Current AnimState : %s", m_pCurAnim->m_strStateName.c_str());
		ImGui::Text("Loop : %d", m_pCurAnim->m_bLoop);
		ImGui::Text("Lerp Duration : %f", m_pCurAnim->m_fLerpDuration);
		ImGui::Text("Main Animation : %s", m_pCurAnim->m_pMainAnim->Get_Name());
		if (m_pCurAnim->m_pBlendAnim != nullptr)
			ImGui::Text("Blend Animation : %s", m_pCurAnim->m_pBlendAnim->Get_Name());
		else
			ImGui::Text("Blend Animation : N/A");
		if (m_pCurAnim->m_vecAdditiveAnim.empty() == false)
		{
			_uint j = 0;
			for (auto pAdditiveAnim : m_pCurAnim->m_vecAdditiveAnim)
			{
				ImGui::Text("Reference Animation[%d] : %s", j, pAdditiveAnim->m_pRefAnim->Get_Name());
				ImGui::Text("Additive Animation[%d] : %s", j++, pAdditiveAnim->m_pAdditiveAnim->Get_Name());
			}
		}
	}
	else
		ImGui::Text("Current AnimState : N/A");

	ImGui::Separator();
	
	if (m_mapAnimState.empty() == false)
	{
		ppStateName = new char*[(_int)m_mapAnimState.size()];
		_uint k = 0;
		for (auto& Pair : m_mapAnimState)
		{
			_uint	iLength = (_uint)Pair.first.length() + 1;
			ppStateName[k] = new char[iLength];
			strcpy_s(ppStateName[k++], iLength, Pair.first.c_str());
		}

		ImGui::ListBox("State List", &iSelectState, ppStateName, (_int)m_mapAnimState.size());

		if (iSelectState != -1)
		{
			CAnimState*		pSelectState = m_mapAnimState[string(ppStateName[iSelectState])];
			if (pSelectState != nullptr)
			{
				ImGui::Text("State Name : %s", pSelectState->m_strStateName.c_str());
				ImGui::Text("Loop : %d", pSelectState->m_bLoop);
				ImGui::InputFloat("Lerp Duration", &pSelectState->m_fLerpDuration, 0.005f, 0.01f);
				ImGui::Text("Main Animation : %s", pSelectState->m_pMainAnim->Get_Name());
				if (pSelectState->m_pBlendAnim != nullptr)
					ImGui::Text("Blend Animation : %s", pSelectState->m_pBlendAnim->Get_Name());
				else
					ImGui::Text("Blend Animation : N/A");
				if (pSelectState->m_vecAdditiveAnim.empty() == false)
				{
					_uint j = 0;
					for (auto pAdditiveAnim : pSelectState->m_vecAdditiveAnim)
					{
						ImGui::Text("Reference Animation[%d] : %s", j, pAdditiveAnim->m_pRefAnim->Get_Name());
						ImGui::Text("Additive Animation[%d] : %s", j, pAdditiveAnim->m_pAdditiveAnim->Get_Name());
						
						ImGui::Text("Control Ratio[%d]", j);
						ImGui::SameLine();
						ImGui::RadioButton("MAX", (_int*)&pAdditiveAnim->m_eControlRatio, CAdditiveAnimation::RATIOTYPE_MAX);
						ImGui::SameLine();
						ImGui::RadioButton("AUTO", (_int*)&pAdditiveAnim->m_eControlRatio, CAdditiveAnimation::RATIOTYPE_AUTO);
						ImGui::SameLine();
						ImGui::RadioButton("CONTROL", (_int*)&pAdditiveAnim->m_eControlRatio, CAdditiveAnimation::RATIOTYPE_CONTROL);

						char	szMaxRatio[32] = "";
						sprintf_s(szMaxRatio, "Max Additive Ratio[%d]", j);
						ImGui::DragFloat(szMaxRatio, &pAdditiveAnim->m_fMaxAdditiveRatio, 0.002f, 0.f, 1.f);

						char	szRatio[32] = "";
						sprintf_s(szRatio, "Additive Ratio[%d]", j);
						ImGui::DragFloat(szRatio, &pAdditiveAnim->m_fAdditiveRatio, 0.002f, 0.f, pAdditiveAnim->m_fMaxAdditiveRatio);
						//CUtile::Saturate<_float>(pAdditiveAnim->m_fAdditiveRatio, 0.f, 1.f);

						j++;
					}
				}
				if (ImGui::Button("Delete"))
				{
					for (_uint i = 0; i < (_uint)m_mapAnimState.size(); ++i)
						Safe_Delete_Array(ppStateName[i]);
					Safe_Delete_Array(ppStateName);

					auto iter = m_mapAnimState.begin();
					for (_int x = 0; x < iSelectState; ++x)
						++iter;

					Safe_Release(iter->second);
					iter = m_mapAnimState.erase(iter);

					iSelectState = -1;

					return;
				}
			}
		}

		for (_uint i = 0; i < (_uint)m_mapAnimState.size(); ++i)
			Safe_Delete_Array(ppStateName[i]);
		Safe_Delete_Array(ppStateName);
	}

	ImGui::Separator();
	/* TODO : Joint Select */
	if (ImGui::CollapsingHeader("Add State"))
	{
		static _bool		bLoop = true;
		static _float		fLerpDuration = 0.2f;
		static	string		strStateName = "";
		static	string		strMainAnim = "";
		static	string		strBlendAnim = "";
		static _int		iAddCnt = 0;
		static	string		strRefAnim[5] = { "", "", "", "", "" };
		static	string		strAddAnim[5] = { "", "", "", "", "" };
		static CAdditiveAnimation::RATIOTYPE		eControlRatio[5] = { CAdditiveAnimation::RATIOTYPE_MAX,
			CAdditiveAnimation::RATIOTYPE_MAX,
			CAdditiveAnimation::RATIOTYPE_MAX,
			CAdditiveAnimation::RATIOTYPE_MAX,
			CAdditiveAnimation::RATIOTYPE_MAX };
		static _float		fMaxRatio[5] = { 0.f, 0.f, 0.f, 0.f, 0.f };

		ImGui::Text("Loop");
		ImGui::SameLine();
		if (ImGui::RadioButton("TRUE", bLoop))
			bLoop = true;
		ImGui::SameLine();
		if (ImGui::RadioButton("FALSE", !bLoop))
			bLoop = false;
		ImGui::InputFloat("Lerp Duration", &fLerpDuration, 0.01f, 0.05f);
		ImGui::InputText("State Name", &strStateName);
		ImGui::InputText("Main Animation", &strMainAnim);
		ImGui::InputText("Blend Animation", &strBlendAnim);
		ImGui::InputInt("Additive Animation Count", &iAddCnt, 1, 1);
		CUtile::Saturate<_int>(iAddCnt, 0, 5);
		for (_int i = 0; i < iAddCnt; ++i)
		{
			char	szRef[32] = "";
			char	szAdd[32] = "";
			char	szMaxAdd[32] = "";
			sprintf_s(szRef, "Reference Animation[%d]", i);
			sprintf_s(szAdd, "Additive Animation[%d]", i);
			ImGui::InputText(szRef, &strRefAnim[i]);
			ImGui::InputText(szAdd, &strAddAnim[i]);
			ImGui::DragFloat(szMaxAdd, &fMaxRatio[i], 0.002f, 0.f, 1.f);
			ImGui::Text("Constrol Ratio[%d]", i);
			ImGui::SameLine();
			if (ImGui::RadioButton("max", (_int*)&eControlRatio[i], (_int)CAdditiveAnimation::RATIOTYPE_MAX))
				eControlRatio[i] = CAdditiveAnimation::RATIOTYPE_MAX;
			ImGui::SameLine();
			if (ImGui::RadioButton("auto", (_int*)&eControlRatio[i], (_int)CAdditiveAnimation::RATIOTYPE_AUTO))
				eControlRatio[i] = CAdditiveAnimation::RATIOTYPE_AUTO;
			ImGui::SameLine();
			if (ImGui::RadioButton("control", (_int*)&eControlRatio[i], (_int)CAdditiveAnimation::RATIOTYPE_CONTROL))
				eControlRatio[i] = CAdditiveAnimation::RATIOTYPE_CONTROL;
			sprintf_s(szMaxAdd, "Max Additive Ratio[%d]", i);
		}
		
		if (ImGui::Button("Add"))
		{
			_bool			bFalseFlag = false;
			CAnimation*	pAnim = nullptr;
			CAnimState*	pAnimState = new CAnimState;
			pAnimState->m_bLoop = bLoop;
			pAnimState->m_fLerpDuration = fLerpDuration;
			pAnimState->m_strStateName = strStateName;

			pAnim = m_pModel->Find_Animation(strMainAnim);
			if (pAnim != nullptr)
				pAnimState->m_pMainAnim = pAnim;
			else
				bFalseFlag = true;

			if (strBlendAnim != "")
			{
				pAnim = m_pModel->Find_Animation(strBlendAnim);
				if (pAnim != nullptr)
					pAnimState->m_pBlendAnim = pAnim;
				else
					bFalseFlag = true;
			}

			if (iAddCnt != 0 && bFalseFlag == false)
			{
				CAdditiveAnimation*		pAdditiveAnim = new CAdditiveAnimation;
				for (_int i = 0; i < iAddCnt; ++i)
				{
					pAnim = m_pModel->Find_Animation(strRefAnim[i]);
					if (pAnim != nullptr)
						pAdditiveAnim->m_pRefAnim = pAnim;
					else
						bFalseFlag = true;

					pAnim = m_pModel->Find_Animation(strAddAnim[i]);
					if (pAnim != nullptr)
						pAdditiveAnim->m_pAdditiveAnim = pAnim;
					else
						bFalseFlag = true;

					pAdditiveAnim->m_eControlRatio = eControlRatio[i];
					pAdditiveAnim->m_fMaxAdditiveRatio = fMaxRatio[i];

					if (bFalseFlag == true)
						Safe_Release(pAdditiveAnim);
					else
						pAnimState->m_vecAdditiveAnim.push_back(pAdditiveAnim);
				}
			}

			if (bFalseFlag == true)
				Safe_Release(pAnimState);
			else
				m_mapAnimState.emplace(strStateName, pAnimState);
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel"))
		{
			bLoop = true;
			fLerpDuration = 0.2f;
			strStateName = "";
			strMainAnim = "";
			strBlendAnim = "";
			for (_int i = 0; i < 5; ++i)
			{
				strRefAnim[i] = "";
				strAddAnim[i] = "";
				eControlRatio[i] = CAdditiveAnimation::RATIOTYPE_MAX;
				fMaxRatio[i] = 0.f;
			}
		}
	}

	if (ImGui::CollapsingHeader("Save & Load"))
	{
		if (ImGui::Button("Save"))
		{
			bSave = true;
			iSelectState = -1;
		}
		ImGui::SameLine();
		if (ImGui::Button("Load"))
		{
			bSave = false;
			iSelectState = -1;
			strcpy_s(szFilePath, "");
			ImGuiFileDialog::Instance()->OpenDialog("Load Directory", "Load Directory", ".json", "../Bin/Data/Animation/", ".json", 1, nullptr, ImGuiFileDialogFlags_Modal);
		}
		ImGui::SameLine();
		if (ImGui::Button("Print Animation Names"))
			ImGuiFileDialog::Instance()->OpenDialog("Print Animation Names", "Save Directory", ".json", "../Bin/Data/Animation/", ".", 0, nullptr, ImGuiFileDialogFlags_Modal);

		if (bSave == true)
		{
			ImGui::InputTextWithHint("File Path", "Click Here for Choose Directory", szFilePath, MAX_PATH, ImGuiInputTextFlags_AutoSelectAll);
			if (ImGui::Button("Choose Directory"))
				ImGuiFileDialog::Instance()->OpenDialog("Save Directory", "Save Directory", ".json", "../Bin/Data/Animation/", ".", 0, nullptr, ImGuiFileDialogFlags_Modal);
			ImGui::SameLine();
			if (ImGui::Button("Confirm"))
			{
				Save(string(szFilePath));
				strcpy_s(szFilePath, "");
			}
		}
		if (ImGuiFileDialog::Instance()->Display("Save Directory"))
		{
			if (ImGuiFileDialog::Instance()->IsOk())
			{
				string		strDirectory = ImGuiFileDialog::Instance()->GetCurrentPath();
				wstring	wstrObjectTag = m_pOwner->Get_ObjectCloneName();
				string		strObjectTag = "";
				strObjectTag.assign(wstrObjectTag.begin(), wstrObjectTag.end());

				strDirectory = strDirectory + "\\" + strObjectTag + ".json";

				strcpy_s(szFilePath, strDirectory.c_str());

				ImGuiFileDialog::Instance()->Close();
			}
			else if (!ImGuiFileDialog::Instance()->IsOk())
				ImGuiFileDialog::Instance()->Close();
		}
		if (ImGuiFileDialog::Instance()->Display("Load Directory"))
		{
			if (ImGuiFileDialog::Instance()->IsOk())
			{
				string		strFilePath = ImGuiFileDialog::Instance()->GetFilePathName();

				Load(strFilePath);
				ImGuiFileDialog::Instance()->Close();
			}
			else if (!ImGuiFileDialog::Instance()->IsOk())
				ImGuiFileDialog::Instance()->Close();
		}
		if (ImGuiFileDialog::Instance()->Display("Print Animation Names"))
		{
			if (ImGuiFileDialog::Instance()->IsOk())
			{
				string		strDirectory = ImGuiFileDialog::Instance()->GetCurrentPath();
				wstring	wstrObjectTag = m_pOwner->Get_ObjectCloneName();
				string		strObjectTag = "";
				strObjectTag.assign(wstrObjectTag.begin(), wstrObjectTag.end());

				strDirectory = strDirectory + "\\" + strObjectTag + " Animation Names.json";

				m_pModel->Print_Animation_Names(strDirectory);

				ImGuiFileDialog::Instance()->Close();
			}
			else if (!ImGuiFileDialog::Instance()->IsOk())
				ImGuiFileDialog::Instance()->Close();
		}
	}
}

HRESULT CAnimationState::Generate_Animation(const string & strFilePath)
{
	if (strFilePath == "")
		return E_FAIL;

	return S_OK;
}

HRESULT CAnimationState::Add_State(CAnimState * pAnim)
{
	NULL_CHECK_RETURN(pAnim, E_FAIL);

	if (pAnim->m_vecAdditiveAnim.empty() == true)
		pAnim->m_pMainAnim->IsLooping() = pAnim->m_bLoop;
	else
		pAnim->m_vecAdditiveAnim.front()->m_pAdditiveAnim->IsLooping() = pAnim->m_bLoop;

	m_mapAnimState.emplace(pAnim->m_strStateName, pAnim);

	return S_OK;
}

HRESULT CAnimationState::Add_AnimSharingPart(CModel * pModel, _bool bMeshSync)
{
	NULL_CHECK_RETURN(pModel, E_FAIL);

	if (bMeshSync == true)
	{
		m_pModel->Synchronization_MeshBone(pModel);

		m_vecSyncPart.push_back(pModel);
	}
	else
	{
		m_vecNonSyncPart.push_back(pModel);
	}

	return S_OK;
}

HRESULT CAnimationState::Save(const string & strFilePath)
{
	Json	jStates;
	Json	jAnimState;
	Json	jAdditiveAnim;
	Json	jJoint;

	for (auto& Pair : m_mapAnimState)
	{
		jAnimState["0. State Name"] = Pair.first;
		jAnimState["1. Looping"] = Pair.second->m_bLoop;
		jAnimState["2. Lerp Duration"] = Pair.second->m_fLerpDuration;
		jAnimState["3. Main Animation"] = string(Pair.second->m_pMainAnim->Get_Name());

		if (Pair.second->m_pBlendAnim != nullptr)
			jAnimState["4. Blend Animation"] = string(Pair.second->m_pBlendAnim->Get_Name());
		else
			jAnimState["4. Blend Animation"] = string("N/A");

		if (Pair.second->m_listLockedJoint.empty() == true)
			jAnimState["5. Joint Index"] = string("N/A");

		for (auto& Joint : Pair.second->m_listLockedJoint)
		{
			jJoint["0. Bone Name"] = Joint.first;

			if (Joint.second == CBone::LOCKTO_CHILD)
				jJoint["1. Lock To"] = string("Lock Child");
			else if (Joint.second == CBone::LOCKTO_PARENT)
				jJoint["1. Lock To"] = string("Lock Parent");
			else if (Joint.second == CBone::LOCKTO_ALONE)
				jJoint["1. Lock To"] = string("Lock Alone");
			else if (Joint.second == CBone::UNLOCKTO_CHILD)
				jJoint["1. Lock To"] = string("Unlock Child");
			else if (Joint.second == CBone::UNLOCKTO_PARENT)
				jJoint["1. Lock To"] = string("Unlock Parent");
			else if (Joint.second == CBone::UNLOCKTO_ALONE)
				jJoint["1. Lock To"] = string("Unlock Alone");
			else
				return E_FAIL;

			jAnimState["5. Joint Index"].push_back(jJoint);
			jJoint.clear();
		}

		if (Pair.second->m_vecAdditiveAnim.empty() == true)
			jAnimState["6. Additive Animation"] = string("N/A");

		for (auto pAdditiveAnim : Pair.second->m_vecAdditiveAnim)
		{
			jAdditiveAnim["0. Ratio Control"] = (_int)pAdditiveAnim->m_eControlRatio;
			jAdditiveAnim["1. Max Additive Ratio"] = pAdditiveAnim->m_fMaxAdditiveRatio;
			jAdditiveAnim["2. Reference Animation"] = string(pAdditiveAnim->m_pRefAnim->Get_Name());
			jAdditiveAnim["3. Additive Animation"] = string(pAdditiveAnim->m_pAdditiveAnim->Get_Name());

			if (pAdditiveAnim->m_listLockedJoint.empty() == true)
				jAdditiveAnim["4. Joint Index"] = string("N/A");

			for (auto& Joint : pAdditiveAnim->m_listLockedJoint)
			{
				jJoint["0. Bone Name"] = Joint.first;

				if (Joint.second == CBone::LOCKTO_CHILD)
					jJoint["1. Lock To"] = string("Lock Child");
				else if (Joint.second == CBone::LOCKTO_PARENT)
					jJoint["1. Lock To"] = string("Lock Parent");
				else if (Joint.second == CBone::LOCKTO_ALONE)
					jJoint["1. Lock To"] = string("Lock Alone");
				else if (Joint.second == CBone::UNLOCKTO_CHILD)
					jJoint["1. Lock To"] = string("Unlock Child");
				else if (Joint.second == CBone::UNLOCKTO_PARENT)
					jJoint["1. Lock To"] = string("Unlock Parent");
				else if (Joint.second == CBone::UNLOCKTO_ALONE)
					jJoint["1. Lock To"] = string("Unlock Alone");
				else
					return E_FAIL;

				jAdditiveAnim["4. Joint Index"].push_back(jJoint);
				jJoint.clear();
			}

			jAnimState["6. Additive Animation"].push_back(jAdditiveAnim);
			jAdditiveAnim.clear();
		}

		jStates.push_back(jAnimState);
		jAnimState.clear();
	}

	ofstream	file(strFilePath.c_str());
	file << jStates;
	file.close();

	return S_OK;
}

HRESULT CAnimationState::Load(const string & strFilePath)
{
	m_pCurAnim = nullptr;
	m_pPreAnim = nullptr;

	//m_vecSyncPart.clear();
	//m_vecNonSyncPart.clear();

	for (auto& pAnimState : m_mapAnimState)
		Safe_Release(pAnimState.second);
	m_mapAnimState.clear();

	FAILED_CHECK_RETURN(Initialize_FromFile(strFilePath), E_FAIL);

	m_pCurAnim = m_mapAnimState["IDLE"];

	return S_OK;
}

CAnimationState * CAnimationState::Create(CGameObject * pOwner, CModel * pModelCom, const string & strRootBone, const string & strFilePath)
{
	CAnimationState*	pInstance = new CAnimationState;
	HRESULT				hr = pInstance->Initialize(pOwner, pModelCom, strRootBone, strFilePath);

	if (hr == E_FAIL)
	{
		MSG_BOX("Failed to Create : CAnimationState");
		Safe_Release(pInstance);
	}
	else if (hr == S_FALSE)
		pInstance->Initialize_FromFile(strFilePath);

	return pInstance;
}
