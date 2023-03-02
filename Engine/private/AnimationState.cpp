#include "stdafx.h"
#include "..\public\AnimationState.h"
#include "Json/json.hpp"
#include <fstream>
#include "GameObject.h"
#include "Model.h"

CAnimationState::CAnimationState()
{
}

HRESULT CAnimationState::Initialize(CGameObject * pOwner, CModel * pModelCom, const string & strFilePath)
{
	NULL_CHECK_RETURN(pOwner, E_FAIL);
	NULL_CHECK_RETURN(pModelCom, E_FAIL);

	m_pOwner = pOwner;
	m_pModel = pModelCom;

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

		jAnimState["2. Main Animation"].get_to<string>(strMainAnim);
		pAnim = m_pModel->Find_Animation(strMainAnim);
		NULL_CHECK_RETURN(pAnim, E_FAIL);
		pAnimState->m_pMainAnim = pAnim;

		jAnimState["3. Blend Animation"].get_to<string>(strBlendAnim);
		if (strBlendAnim != "N/A")
		{
			pAnim = m_pModel->Find_Animation(strBlendAnim);
			NULL_CHECK_RETURN(pAnim, E_FAIL);
			pAnimState->m_pBlendAnim = pAnim;
		}

		if (jAnimState["4. Joint Index"].is_array() == true)
		{
			for (auto jJoint : jAnimState["4. Joint Index"])
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

		if (jAnimState["5. Additive Animation"].is_array() == true)
		{
			CAdditiveAnimation*		pAdditive = nullptr;
			for (auto jAdditive : jAnimState["5. Additive Animation"])
			{
				pAdditive = new CAdditiveAnimation;

				jAdditive["0. Ratio Control"].get_to<_bool>(pAdditive->m_bControlRatio);

				jAdditive["1. Reference Animation"].get_to<string>(strMainAnim);
				pAnim = m_pModel->Find_Animation(strMainAnim);
				NULL_CHECK_RETURN(pAnim, E_FAIL);
				pAdditive->m_pRefAnim = pAnim;

				jAdditive["2. Additive Animation"].get_to<string>(strBlendAnim);
				pAnim = m_pModel->Find_Animation(strBlendAnim);
				NULL_CHECK_RETURN(pAnim, E_FAIL);
				pAdditive->m_pAdditiveAnim = pAnim;

				for (auto jJoint : jAdditive["3. Joint Index"])
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

HRESULT CAnimationState::State_Animation(const string & strStateName)
{
	const auto	iter = find_if(m_mapAnimState.begin(), m_mapAnimState.end(), [&strStateName](const pair<const string, CAnimState*>& Pair) {
		return Pair.first == strStateName;
	});

	if (iter == m_mapAnimState.end())
		return E_FAIL;

	CAnimState*		pAnim = iter->second;
	NULL_CHECK_RETURN(pAnim->m_pMainAnim, E_FAIL);

	m_fLerpDuration = pAnim->m_fLerpDuration;

	if (pAnim->m_listLockedJoint.empty() == false)
	{
		CBone*	pBone = nullptr;
		_uint		iLockedBoneCnt = (_uint)pAnim->m_listLockedJoint.size();
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
	}
	else
		m_fCurLerpTime = m_fLerpDuration;

	m_pCurAnim = pAnim;
	m_pCurAnim->m_pMainAnim->Reset_Animation();
	if (m_pCurAnim->m_pBlendAnim != nullptr)
		m_pCurAnim->m_pBlendAnim->Reset_Animation();
	if (m_pCurAnim->m_vecAdditiveAnim.empty() != false)
	{
		for (auto pAdditiveAnim : m_pCurAnim->m_vecAdditiveAnim)
		{
			pAdditiveAnim->m_pRefAnim->Reset_Animation();
			pAdditiveAnim->m_pAdditiveAnim->Reset_Animation();
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

	_smatrix		matBonesTransformation[800];

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
		pPreAnim->Update_Bones_ReturnMat(fTimeDelta, matBonesTransformation, pPreBlendAnim);

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

				pAdditiveAnim->m_pAdditiveAnim->Update_Bones_Additive_ReturnMat(fTimeDelta, pAdditiveAnim->m_fAdditiveRatio, matBonesTransformation, pAdditiveAnim->m_pRefAnim);
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

		pMainAnim->Update_Bones_Blend_ReturnMat(fTimeDelta, m_fCurLerpTime / m_fLerpDuration, matBonesTransformation, pBlendAnim);
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
		pMainAnim->Update_Bones_ReturnMat(fTimeDelta, matBonesTransformation, pBlendAnim);
	}
	m_pModel->Compute_CombindTransformationMatrix();

	/* Additive */
	if (m_pCurAnim->m_vecAdditiveAnim.empty() == false)
	{
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
			if (pAdditiveAnim->m_fAdditiveRatio > 1.f)
				pAdditiveAnim->m_fAdditiveRatio = 1.f;

			if (pAdditiveAnim->m_bOneKeyFrame == true)
				pAdditiveAnim->m_pAdditiveAnim->Update_Bones_Additive_ReturnMat(fTimeDelta, 1.f, matBonesTransformation, m_pCurAnim->m_pMainAnim);
			else
				pAdditiveAnim->m_pAdditiveAnim->Update_Bones_Additive_ReturnMat(fTimeDelta, 1.f, matBonesTransformation, pAdditiveAnim->m_pRefAnim);

			pAdditiveAnim->m_fAdditiveRatio += fTimeDelta;
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
	if (ImGui::Button("Save"))
		bSave = true;
	ImGui::SameLine();
	if (ImGui::Button("Load"))
	{
		bSave = false;
		strcpy_s(szFilePath, "");
		ImGuiFileDialog::Instance()->OpenDialog("Load Directory", "Load Directory", ".json", "../Bin/Data/Animation/", ".json", 1, nullptr, ImGuiFileDialogFlags_Modal);
	}

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
		jAnimState["2. Main Animation"] = string(Pair.second->m_pMainAnim->Get_Name());

		if (Pair.second->m_pBlendAnim != nullptr)
			jAnimState["3. Blend Animation"] = string(Pair.second->m_pBlendAnim->Get_Name());
		else
			jAnimState["3. Blend Animation"] = string("N/A");

		if (Pair.second->m_listLockedJoint.empty() == true)
			jAnimState["4. Joint Index"] = string("N/A");

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

			jAnimState["4. Joint Index"].push_back(jJoint);
			jJoint.clear();
		}

		if (Pair.second->m_vecAdditiveAnim.empty() == true)
			jAnimState["5. Additive Animation"] = string("N/A");

		for (auto pAdditiveAnim : Pair.second->m_vecAdditiveAnim)
		{
			jAdditiveAnim["0. Ratio Control"] = pAdditiveAnim->m_bControlRatio;
			jAdditiveAnim["1. Reference Animation"] = string(pAdditiveAnim->m_pRefAnim->Get_Name());
			jAdditiveAnim["2. Additive Animation"] = string(pAdditiveAnim->m_pAdditiveAnim->Get_Name());

			if (pAdditiveAnim->m_listLockedJoint.empty() == true)
				jAdditiveAnim["3. Joint Index"] = string("N/A");

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

				jAdditiveAnim["3. Joint Index"].push_back(jJoint);
				jJoint.clear();
			}

			jAnimState["5. Additive Animation"].push_back(jAdditiveAnim);
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

	m_vecSyncPart.clear();
	m_vecNonSyncPart.clear();

	for (auto& pAnimState : m_mapAnimState)
		Safe_Release(pAnimState.second);
	m_mapAnimState.clear();

	FAILED_CHECK_RETURN(Initialize_FromFile(strFilePath), E_FAIL);

	return S_OK;
}

CAnimationState * CAnimationState::Create(CGameObject * pOwner, CModel * pModelCom, const string & strFilePath)
{
	CAnimationState*	pInstance = new CAnimationState;
	HRESULT				hr = pInstance->Initialize(pOwner, pModelCom, strFilePath);

	if (hr == E_FAIL)
	{
		MSG_BOX("Failed to Create : CAnimationState");
		Safe_Release(pInstance);
	}
	else if (hr == S_FALSE)
		pInstance->Initialize_FromFile(strFilePath);

	return pInstance;
}
