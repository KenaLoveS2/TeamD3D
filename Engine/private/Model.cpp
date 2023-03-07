#include "stdafx.h"
#include "..\public\Model.h"
#include "Instancing_Mesh.h"
#include "Mesh.h"
#include "Texture.h"
#include "Shader.h"
#include "Bone.h"
#include "Animation.h"
#include "GameObject.h"
#include "Json/json.hpp"
#include <fstream>
#include "Function_Manager.h"
#include "Utile.h"
#include "PipeLine.h"
#include "Transform.h"
#include "GameInstance.h"

CModel::CModel(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent(pDevice, pContext)
{
}

CModel::CModel(const CModel & rhs)
	: CComponent(rhs)
	, m_eType(rhs.m_eType)
	, m_wstrModelFilePath(rhs.m_wstrModelFilePath)
	, m_iNumMeshes(rhs.m_iNumMeshes)
	, m_Materials(rhs.m_Materials)
	, m_iNumMaterials(rhs.m_iNumMaterials)
	, m_iNumBones(rhs.m_iNumBones)
	, m_iNumAnimations(rhs.m_iNumAnimations)
	, m_iCurrentAnimIndex(rhs.m_iCurrentAnimIndex)
	, m_PivotMatrix(rhs.m_PivotMatrix)
	, m_dwBeginBoneData(rhs.m_dwBeginBoneData)
	/*for Instancing*/
	, m_bIsInstancing(rhs.m_bIsInstancing)
	, m_bIsLodModel(rhs.m_bIsLodModel)
{
	for (auto& Material : m_Materials)
	{
		for (_uint i = 0; i < (_uint)WJ_TEXTURE_TYPE_MAX; ++i)
			Safe_AddRef(Material.pTexture[i]);
	}

	for (auto& pBone : rhs.m_Bones)
	{
		m_Bones.push_back((CBone*)pBone->Clone());
	}

	for (auto& pAnimation : rhs.m_Animations)
	{
		m_Animations.push_back((CAnimation*)pAnimation->Clone());
	}

	for (auto& pMesh : rhs.m_Meshes)			/*For.Origin*/
	{
		m_Meshes.push_back((CMesh*)pMesh->Clone());
	}

	for (auto& pInstanceMesh : rhs.m_InstancingMeshes)			/*For.Instacing*/
	{
		m_InstancingMeshes.push_back((CInstancing_Mesh*)pInstanceMesh->Clone());
	}



}

CBone * CModel::Get_BonePtr(const char * pBoneName)
{
	auto	iter = find_if(m_Bones.begin(), m_Bones.end(), [&](CBone* pBone)->_bool
	{
		return !strcmp(pBoneName, pBone->Get_Name());
	});

	if (iter == m_Bones.end())
		return nullptr;

	return *iter;
}

const _double& CModel::Get_PlayTime() const
{
	return m_Animations[m_iCurrentAnimIndex]->Get_PlayTime();
}

const _bool & CModel::Get_AnimationFinish() const
{
	return m_Animations[m_iCurrentAnimIndex]->IsFinished();
}

void CModel::Set_PlayTime(_double dPlayTime)
{
	m_Animations[m_iCurrentAnimIndex]->Set_PlayTime(dPlayTime);
}

HRESULT CModel::Initialize_Prototype(const _tchar *pModelFilePath, _fmatrix PivotMatrix,
	const _tchar * pAdditionalFilePath, _bool bIsLod, _bool bIsInstancing, const char*  JsonMatrialPath)
{
	if (JsonMatrialPath == nullptr)
		JsonMatrialPath = "NULL";

	m_bIsInstancing = bIsInstancing;			/* 현재 모델이 인스턴싱인가?*/
	m_bIsLodModel = bIsLod;
	XMStoreFloat4x4(&m_PivotMatrix, PivotMatrix);

	m_wstrModelFilePath = pModelFilePath;

	_tchar		wszExt[32] = L"";
	_wsplitpath_s(pModelFilePath, nullptr, 0, nullptr, 0, nullptr, 0, wszExt, 32);

	if (!lstrcmp(wszExt, L".mdat"))
	{
		HANDLE hFile = CreateFile(pModelFilePath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
		if (hFile == INVALID_HANDLE_VALUE) return E_FAIL;

		_uint iMaterialIndex = 0;
		_ulong dwByte = 0;
		ReadFile(hFile, &m_eType, sizeof(m_eType), &dwByte, nullptr);
		ReadFile(hFile, &m_iNumMaterials, sizeof(_uint), &dwByte, nullptr);
		if (!strcmp(JsonMatrialPath, "NULL") )
		{
			for (_uint i = 0; i < m_iNumMaterials; i++)
			{
				MODELMATERIAL ModelMatrial;
				for (_uint j = 0; j < (_uint)WJ_TEXTURE_TYPE_MAX; j++)
				{
					_bool bUseFlag = false;
					ReadFile(hFile, &bUseFlag, sizeof(_bool), &dwByte, nullptr);
					if (bUseFlag)
					{
						_uint iLen = 0;
						ReadFile(hFile, &iLen, sizeof(_uint), &dwByte, nullptr);
						_tchar szPath[MAX_PATH] = { 0, };
						ReadFile(hFile, szPath, (iLen + 1) * sizeof(_tchar), &dwByte, nullptr);
						_uint iNumTextures = 0;
						ReadFile(hFile, &iNumTextures, sizeof(_uint), &dwByte, nullptr);
						ModelMatrial.pTexture[j] = CTexture::Create(m_pDevice, m_pContext, szPath, iNumTextures);
					}
					else
						ModelMatrial.pTexture[j] = nullptr;
				}

				iMaterialIndex++;
				m_Materials.push_back(ModelMatrial);			//  MODELMATERIAL  1개는 18개의 텍스쳐를 생성가능하다.
																// 결국에 여기를 손봐야한다.
			}
		}
		else
		{
			  CUtile::MODELMATERIAL_Create(m_pDevice, m_pContext, JsonMatrialPath, m_Materials);
		
			for (_uint i = 0; i < m_iNumMaterials; i++)
			{
				// 여기서 json 로드하기
			
				for (_uint j = 0; j < (_uint)WJ_TEXTURE_TYPE_MAX; j++)
				{
					_bool bUseFlag = false;
					ReadFile(hFile, &bUseFlag, sizeof(_bool), &dwByte, nullptr);
					if (bUseFlag)
					{
						_uint iLen = 0;
						ReadFile(hFile, &iLen, sizeof(_uint), &dwByte, nullptr);
						_tchar szPath[MAX_PATH] = { 0, };
						ReadFile(hFile, szPath, (iLen + 1) * sizeof(_tchar), &dwByte, nullptr);
						_uint iNumTextures = 0;
						ReadFile(hFile, &iNumTextures, sizeof(_uint), &dwByte, nullptr);
					}
					else
						continue;
				}
				iMaterialIndex++;
			}
		

				// 결국에 여기를 손봐야한다.
			
		}

		ReadFile(hFile, &m_iNumBones, sizeof(_uint), &dwByte, nullptr);
		for (_uint i = 0; i < m_iNumBones; i++)
		{
			CBone* pBone = CBone::Create(hFile);
			if (pBone == nullptr)
				return E_FAIL;

			m_Bones.push_back(pBone);
		}

		if (FAILED(SetUp_BonesParentPtr()))
			return E_FAIL;

		ReadFile(hFile, &m_iNumMeshes, sizeof(_uint), &dwByte, nullptr);
		for (_uint i = 0; i < m_iNumMeshes; i++)
		{
			// for. Instaincing
			if (m_bIsInstancing == true)
			{
				CInstancing_Mesh *pMesh = CInstancing_Mesh::Create(m_pDevice, m_pContext, hFile, this, bIsLod);
				assert(nullptr != pMesh && "CModel::Initialize_Prototype _Instancing");
				m_InstancingMeshes.push_back(pMesh);
			}
			else   // For.origin Mesh
			{
				CMesh *pMesh = CMesh::Create(m_pDevice, m_pContext, hFile, this, bIsLod);
				if (pMesh == nullptr) return E_FAIL;

				m_Meshes.push_back(pMesh);
			}
		}

		ReadFile(hFile, &m_iNumAnimations, sizeof(_uint), &dwByte, nullptr);
		for (_uint i = 0; i < m_iNumAnimations; i++)
		{
			CAnimation *pAnimation = CAnimation::Create(hFile, this);
			if (pAnimation == nullptr) return E_FAIL;

			m_Animations.push_back(pAnimation);
		}

		CloseHandle(hFile);
	}
	else if (!lstrcmp(wszExt, L".model"))
		FAILED_CHECK_RETURN(Load_MeshMaterial(m_wstrModelFilePath), E_FAIL);

	return S_OK;
}

HRESULT CModel::Initialize(void * pArg, CGameObject * pOwner)
{
	FAILED_CHECK_RETURN(__super::Initialize(pArg, pOwner), E_FAIL);

	_tchar		wszExt[32] = L"";
	_wsplitpath_s(m_wstrModelFilePath.c_str(), nullptr, 0, nullptr, 0, nullptr, 0, wszExt, 32);

	if (!lstrcmp(wszExt, L".mdat"))
	{
		if (FAILED(SetUp_BonesParentPtr()))
			return E_FAIL;

		if (FAILED(SetUp_ClonedAnimations()))
			return E_FAIL;

		if (FAILED(SetUp_ClonedMeshes()))
			return E_FAIL;
	}
	else if (!lstrcmp(wszExt, L".model"))
	{
		DWORD	dwByte = 0;
		HANDLE	hFile = CreateFileW(m_wstrModelFilePath.c_str(), GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
		SetFilePointer(hFile, m_dwBeginBoneData, nullptr, FILE_BEGIN);

		DWORD	Temp = 0;
		ReadFile(hFile, &Temp, sizeof(DWORD), &dwByte, nullptr);

		FAILED_CHECK_RETURN(Load_BoneAnimation(hFile, dwByte), E_FAIL);

		CloseHandle(hFile);
	}

#ifdef _DEBUG
	/*For.Imgui*/

	m_pInstanceTransform = static_cast<CTransform*>(CGameInstance::GetInstance()->
		Clone_Component(CGameInstance::Get_StaticLevelIndex(), CGameInstance::m_pPrototypeTransformTag));

#endif // _DEBUG


	return S_OK;
}

void CModel::Imgui_RenderProperty()
{
	ImGui::Separator();

	if (ImGui::CollapsingHeader("Meshes"))
	{
		_uint	iMaterialIndex = 0;
		for (size_t i = 0; i < m_iNumMeshes; ++i)
		{
			if (m_bIsInstancing == true)			/*For.Instacing*/
				iMaterialIndex = m_InstancingMeshes[i]->Get_MaterialIndex();
			else												/*For.Origin*/
				iMaterialIndex = m_Meshes[i]->Get_MaterialIndex();
			ImGui::Text("Mesh %d", i);
			ImGui::Separator();

			for (_uint j = 0; j < (_uint)WJ_TEXTURE_TYPE_MAX; j++)
			{
				if (m_Materials[iMaterialIndex].pTexture[j] != nullptr)
				{
					ImGui::SameLine();
					ImGui::Image(m_Materials[iMaterialIndex].pTexture[j]->Get_Texture(), ImVec2(50.f, 50.f));
				}
			}
		}
	}

	if (ImGui::CollapsingHeader("Materials"))
	{
		for (size_t i = 0; i < m_iNumMaterials; ++i)
		{
			ImGui::Text("Material %d", i);

			for (_uint j = 0; j < (_uint)WJ_TEXTURE_TYPE_MAX; ++j)
			{
				if (m_Materials[i].pTexture[j] != nullptr)
				{
					ImGui::Image(m_Materials[i].pTexture[j]->Get_Texture(), ImVec2(50.f, 50.f));
					ImGui::SameLine();
				}
			}
			ImGui::NewLine();
		}
	}

	if (ImGui::CollapsingHeader("Animations"))
	{
		static _int	iSelectAnimation = -1;
		static _int	iSelectFunction = -1;
		static _int	iSelectEvent = -1;
		static _bool	bAddEvent = false;
		CAnimation*	pAnimation = nullptr;
		char**			ppAnimationTag = new char*[m_iNumAnimations];
		static string	strSearchTag = "";
		_bool			bSearchMode = false;
		char**			ppFunctionTags = nullptr;

		string			LastSearchTag = strSearchTag;
		ImGui::InputText("Search", &strSearchTag);
		if (strSearchTag != "")
			bSearchMode = true;
		else
			bSearchMode = false;

		if (LastSearchTag != strSearchTag)
			iSelectAnimation = -1;

		string		strCompareTag = "";
		_uint		iSearchedCount = 0;
		for (_uint i = 0; i < m_iNumAnimations; ++i)
		{
			_uint	iTagLength = 0;

			if (bSearchMode)
			{
				strCompareTag = m_Animations[i]->Get_Name();

				if (strCompareTag.find(strSearchTag) != string::npos)
				{
					iTagLength = _uint(strlen(m_Animations[i]->Get_Name())) + 1;
					ppAnimationTag[iSearchedCount] = new char[iTagLength];
					sprintf_s(ppAnimationTag[iSearchedCount++], sizeof(char) * iTagLength, m_Animations[i]->Get_Name());
				}
				else
					continue;
			}
			else
			{
				iTagLength = _uint(strlen(m_Animations[i]->Get_Name())) + 1;
				ppAnimationTag[i] = new char[iTagLength];
				sprintf_s(ppAnimationTag[i], sizeof(char) * iTagLength, m_Animations[i]->Get_Name());
			}
		}
		if (bSearchMode == true)
			ImGui::ListBox("Animation Search", &iSelectAnimation, ppAnimationTag, (_int)iSearchedCount);
		else
			ImGui::ListBox("Animation List", &iSelectAnimation, ppAnimationTag, (_int)m_iNumAnimations);

		if (ImGui::Button("Split All Animation Tag"))
		{
			if (bSearchMode == true)
			{
				for (_uint i = 0; i < iSearchedCount; ++i)
					Safe_Delete_Array(ppAnimationTag[i]);
				Safe_Delete_Array(ppAnimationTag);
			}
			else
			{
				for (_uint i = 0; i < m_iNumAnimations; ++i)
					Safe_Delete_Array(ppAnimationTag[i]);
				Safe_Delete_Array(ppAnimationTag);
			}

			iSelectAnimation = -1;
			for (_uint i = 0; i < m_iNumAnimations; ++i)
			{
				char* pSlittedTag = CUtile::Split_String(const_cast<char*>(m_Animations[i]->Get_Name()), '|');
				m_Animations[i]->Set_Name(pSlittedTag);
				Safe_Delete_Array(pSlittedTag);
			}

			return;
		}
		static _double	dMasterSpeed = 24.0;
		ImGui::InputDouble("Default Speed = 24.0", &dMasterSpeed, 0.5, 1.0, "%.3f");
		if (ImGui::Button("Change All Animations Speed"))
		{
			for (auto& pAnim : m_Animations)
				pAnim->Get_AnimationTickPerSecond() = dMasterSpeed;
		}

		if (iSelectAnimation != -1)
		{
			static _bool	bReName = false;
			static char	szNewName[MAX_PATH] = "";

			if (bSearchMode == true)
			{
				for (auto& pAnim : m_Animations)
				{
					if (!strcmp(pAnim->Get_Name(), ppAnimationTag[iSelectAnimation]))
					{
						pAnimation = pAnim;
						break;
					}
				}
			}
			else
				pAnimation = m_Animations[iSelectAnimation];

			if (ImGui::Button("Play"))
			{
				m_bPausePlay = false;
				m_bPreview = true;
				pAnimation->Reset_Animation();

				if (bSearchMode == true)
				{
					for (_uint i = 0; i < m_iNumAnimations; ++i)
					{
						if (pAnimation == m_Animations[i])
						{
							Set_AnimIndex(i);
							m_pOwner->Set_AnimationIndex(i);
							break;
						}
					}
				}
				else
				{
					Set_AnimIndex(iSelectAnimation);
					m_pOwner->Set_AnimationIndex(iSelectAnimation);
				}
				m_pOwner->Update_Child();
			}
			ImGui::SameLine();
			if (ImGui::Button("ReName"))
			{
				bReName = true;
			}
			ImGui::SameLine();
			if (ImGui::Button("Delete"))
			{
				for (_uint i = 0; i < m_iNumAnimations; ++i)
					Safe_Delete_Array(ppAnimationTag[i]);
				Safe_Delete_Array(ppAnimationTag);

				auto	iter = m_Animations.begin();
				for (_int i = 0; i < iSelectAnimation; ++i)
					++iter;

				Safe_Release(m_Animations[iSelectAnimation]);
				m_Animations.erase(iter);

				m_iNumAnimations--;
				iSelectAnimation = -1;

				return;
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel"))
			{
				m_bPreview = false;
				iSelectAnimation = -1;
				bReName = false;
				pAnimation = nullptr;

				if (bSearchMode == true)
				{
					for (_uint i = 0; i < iSearchedCount; ++i)
						Safe_Delete_Array(ppAnimationTag[i]);
					Safe_Delete_Array(ppAnimationTag);
				}
				else
				{
					for (_uint i = 0; i < m_iNumAnimations; ++i)
						Safe_Delete_Array(ppAnimationTag[i]);
					Safe_Delete_Array(ppAnimationTag);
				}

				return;
			}
			ImGui::SameLine();
			if (ImGui::Button("Save"))
			{
				m_wstrModelFilePath.erase(m_wstrModelFilePath.find_last_of(L"."), string::npos);
				m_wstrModelFilePath += L".model";

				Save_Model(m_wstrModelFilePath);
			}

			if (bReName)
			{
				ImGui::InputText("Input New Name", szNewName, MAX_PATH);

				if (ImGui::Button("Split Name"))
				{
					char* pSplittedName = CUtile::Split_String(ppAnimationTag[iSelectAnimation], '|');
					strcpy_s(szNewName, pSplittedName);
					Safe_Delete_Array(pSplittedName);
				}
				ImGui::SameLine();
				if (ImGui::Button("Confirm"))
				{
					pAnimation->Set_Name(szNewName);
					sprintf_s(szNewName, "");
					bReName = false;
				}
				ImGui::SameLine();
				if (ImGui::Button("Undo"))
				{
					sprintf_s(szNewName, "");
					bReName = false;
				}
			}

			ImGui::Separator();
			const _uint&		iChannelCount = pAnimation->Get_ChannelCount();
			ImGui::InputInt("Num of Channel", (_int*)&iChannelCount, 0, 0, ImGuiInputTextFlags_ReadOnly);
			ImGui::Separator();
			_double&	dPlayTime = pAnimation->Get_PlayTime();
			ImGui::InputDouble("Current Play Time", &dPlayTime, 0.0, 0.0, "%.3f", ImGuiInputTextFlags_ReadOnly);
			_double&	dDuration = pAnimation->Get_AnimationDuration();
			ImGui::InputDouble("Duration", &dDuration, 0.0, 0.0, "%.3f", ImGuiInputTextFlags_ReadOnly);
			_float		fProgress = _float(dPlayTime / dDuration);
			ImGui::InputFloat("Progress", &fProgress, 0.f, 0.f, "%.3f", ImGuiInputTextFlags_ReadOnly);
			_float&	fBlendDuration = pAnimation->Get_BlendDuration();
			ImGui::InputFloat("Blend Duration", &fBlendDuration, 0.01f, 0.05f, "%.3f");
			ImGui::SameLine();
			if (ImGui::SmallButton("reset"))
				fBlendDuration = 0.2f;

			ImGui::Separator();
			ImGui::BulletText("Animation Type");
			CAnimation::ANIMTYPE&		eAnimType = pAnimation->Get_AnimationType();
			if (ImGui::RadioButton("Common", eAnimType == CAnimation::ANIMTYPE_COMMON))
				eAnimType = CAnimation::ANIMTYPE_COMMON;
			ImGui::SameLine();
			if (ImGui::RadioButton("Additive", eAnimType == CAnimation::ANIMTYPE_ADDITIVE))
				eAnimType = CAnimation::ANIMTYPE_ADDITIVE;

			ImGui::Separator();
			ImGui::BulletText("Loop");
			if (ImGui::RadioButton("Allow", pAnimation->IsLooping()))
				pAnimation->IsLooping() = true;
			ImGui::SameLine();
			if (ImGui::RadioButton("Disallow", !pAnimation->IsLooping()))
				pAnimation->IsLooping() = false;

			ImGui::Separator();
			ImGui::BulletText("Animation Speed");
			_double&	dTickPerSecond = pAnimation->Get_AnimationTickPerSecond();
			ImGui::InputDouble("Input Speed", &dTickPerSecond, 0.5, 1.0);
			ImGui::SameLine();
			if (ImGui::Button("Reset"))
				dTickPerSecond = 24.0;

			ImGui::Separator();
			ImGui::BulletText("Play Progress");
			if (ImGui::SliderFloat("Set Progress", &fProgress, 0.f, 1.f))
			{
				bAddEvent = true;
				m_bPausePlay = true;
				m_pOwner->Update_Child();
				pAnimation->Reset_Animation();
				dPlayTime = (_double)fProgress * dDuration;
			}
			if (bAddEvent == true)
			{
				_uint	iFuncCnt = 0;
				CFunction_Manager::GetInstance()->Get_FunctionNames(m_pOwner, iFuncCnt, ppFunctionTags);

				ImGui::Combo("Select Function", &iSelectFunction, ppFunctionTags, iFuncCnt);
				if (ImGui::Button("Add Event") && iSelectFunction > -1)
				{
					_tchar		wszFunctionTag[128] = L"";
					CUtile::CharToWideChar(ppFunctionTags[iSelectFunction], wszFunctionTag);
					pAnimation->Add_Event(_float(dPlayTime), string(ppFunctionTags[iSelectFunction]));
				}
				ImGui::SameLine();
				if (ImGui::Button("Escape"))
				{
					bAddEvent = false;
					iSelectFunction = -1;
				}

				for (_uint i = 0; i < iFuncCnt; ++i)
					Safe_Delete_Array(ppFunctionTags[i]);
				Safe_Delete_Array(ppFunctionTags);
			}
			ImGui::Separator();
			ImGui::BulletText("Event");
			pAnimation->ImGui_RenderEvents(iSelectEvent);
		}

		if (bSearchMode == true)
		{
			for (_uint i = 0; i < iSearchedCount; ++i)
				Safe_Delete_Array(ppAnimationTag[i]);
			Safe_Delete_Array(ppAnimationTag);
		}
		else
		{
			for (_uint i = 0; i < m_iNumAnimations; ++i)
				Safe_Delete_Array(ppAnimationTag[i]);
			Safe_Delete_Array(ppAnimationTag);
		}
	}

	if (m_bIsInstancing == true)
	{


		for (auto &pInstanceMesh : m_InstancingMeshes)
		{
			//pInstanceMesh->Set_Position(); 
		}
	}

}

HRESULT CModel::SetUp_BonesParentPtr()
{
	for (_uint i = 0; i < m_iNumBones; i++)
	{
		const char* pParentName = m_Bones[i]->Get_ParentName();
		if (!strcmp(pParentName, ""))
			continue;

		_uint j = 0;
		for (; j < m_iNumBones; j++)
		{
			if (i == j)
				continue;

			const char* pName = m_Bones[j]->Get_Name();
			if (!strcmp(pParentName, pName))
			{
				m_Bones[i]->SetParent(m_Bones[j]);
				break;
			}
		}

		if (j == m_iNumBones)
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CModel::SetUp_ClonedAnimations()
{
	for (_uint i = 0; i < m_iNumAnimations; i++)
	{
		if (FAILED(m_Animations[i]->SetUp_ChannelsBonePtr(this)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CModel::SetUp_ClonedMeshes()
{
	for (_uint i = 0; i < m_iNumMeshes; i++)
	{
		/*For. Origin*/
		if (false == m_bIsInstancing  && FAILED(m_Meshes[i]->SetUp_BonePtr(this)))
			return E_FAIL;
		/*For. Instaincing*/
		else if (true == m_bIsInstancing &&FAILED(m_InstancingMeshes[i]->SetUp_BonePtr(this)))
			return E_FAIL;
		else continue;
	}

	return S_OK;
}

CAnimation * CModel::Find_Animation(const string & strAnimName)
{
	const auto	iter = find_if(m_Animations.begin(), m_Animations.end(), [&strAnimName](const CAnimation* pAnim) {
		return !strcmp(pAnim->Get_Name(), strAnimName.c_str());
	});

	if (iter == m_Animations.end())
		return nullptr;

	return (*iter);
}

CAnimation * CModel::Find_Animation(_uint iAnimIndex)
{
	if (iAnimIndex >= m_iNumAnimations)
		return nullptr;

	return m_Animations[iAnimIndex];
}

void CModel::Set_InstancePos(vector<_float4x4> InstanceMatrixVec)
{
	if (m_bIsInstancing == false)
		return;

	size_t InputVecSize = InstanceMatrixVec.size();

	for (size_t i = 0; i < InputVecSize; ++i)
	{
		_float4x4* NewMatrix = new _float4x4;
		*NewMatrix = InstanceMatrixVec[i];
		m_pInstancingMatrix.push_back(NewMatrix);
	}
	for (auto& pInstMesh : m_InstancingMeshes)
		pInstMesh->Add_InstanceModel(m_pInstancingMatrix);

}

HRESULT CModel::Save_Model(const wstring & wstrSaveFileDirectory)
{
	DWORD	dwByte = 0;
	HANDLE	hFile = CreateFileW(wstrSaveFileDirectory.c_str(), GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

	if (hFile == INVALID_HANDLE_VALUE)
		return E_FAIL;

	/* Meshes */
	WriteFile(hFile, &m_iNumMeshes, sizeof(_uint), &dwByte, nullptr);

	if (m_bIsInstancing == true)		/*for. Instancing*/
	{
		for (auto& pInstanceMesh : m_InstancingMeshes)
		{
			if (FAILED(pInstanceMesh->Save_Mesh(hFile, dwByte)))
			{
				MSG_BOX("Failed to Save : Instance_Mesh");
				CloseHandle(hFile);
				return E_FAIL;
			}
		}
	}
	else    /*Origin.Instacing*/
	{
		for (auto& pMesh : m_Meshes)
		{
			if (FAILED(pMesh->Save_Mesh(hFile, dwByte)))
			{
				MSG_BOX("Failed to Save : Mesh");
				CloseHandle(hFile);
				return E_FAIL;
			}
		}
	}


	/* Materials*/
	if (m_iNumMaterials == 0)
		return E_FAIL;

	WriteFile(hFile, &m_iNumMaterials, sizeof(_uint), &dwByte, nullptr);

	for (auto& tMaterial : m_Materials)
	{
		for (_uint i = 0; i < (_uint)WJ_TEXTURE_TYPE_MAX; ++i)
		{
			if (tMaterial.pTexture[i] == nullptr)
			{
				_uint		iTemp = (_uint)WJ_TEXTURE_TYPE_MAX;
				WriteFile(hFile, &iTemp, sizeof(_uint), &dwByte, nullptr);
			}
			else
			{
				WriteFile(hFile, &i, sizeof(_uint), &dwByte, nullptr);

				const _tchar*	wszFilePath = tMaterial.pTexture[i]->Get_FilePath().c_str();
				_uint				iFilePathLength = (_uint)tMaterial.pTexture[i]->Get_FilePath().length() + 1;

				WriteFile(hFile, &iFilePathLength, sizeof(_uint), &dwByte, nullptr);
				WriteFile(hFile, wszFilePath, sizeof(_tchar) * iFilePathLength, &dwByte, nullptr);
			}
		}
	}

	/* 파일 위치 포인터 받아서 저장하기 */
	m_dwBeginBoneData = SetFilePointer(hFile, 0, nullptr, FILE_CURRENT);

	/* Bones */
	WriteFile(hFile, &m_dwBeginBoneData, sizeof(DWORD), &dwByte, nullptr);

	WriteFile(hFile, &m_iNumBones, sizeof(_uint), &dwByte, nullptr);
	for (auto& pBone : m_Bones)
	{
		if (FAILED(pBone->Save_Bone(hFile, dwByte)))
		{
			MSG_BOX("Failed to Save : Bone");
			CloseHandle(hFile);
			return E_FAIL;
		}
	}

	/* Mesh Bones */
	if (true == m_bIsInstancing)			/*For. Instaincing*/
	{
		for (auto& pInstanceMesh : m_InstancingMeshes)
			pInstanceMesh->Save_MeshBones(hFile, dwByte);
	}
	else													/*For. Origin*/
	{
		for (auto& pMesh : m_Meshes)
			pMesh->Save_MeshBones(hFile, dwByte);
	}


	/* Animations */
	WriteFile(hFile, &m_iNumAnimations, sizeof(_uint), &dwByte, nullptr);

	for (auto& pAnimation : m_Animations)
	{
		if (FAILED(pAnimation->Save_Animation(hFile, dwByte)))
		{
			MSG_BOX("Failed to Save : Animation");
			CloseHandle(hFile);
			return E_FAIL;
		}
	}

	CloseHandle(hFile);

	return S_OK;
}

HRESULT CModel::Animation_Synchronization(CModel * pModelCom, const string & strRootNodeName)
{
	NULL_CHECK_RETURN(pModelCom, E_FAIL);

	if (m_eType == TYPE_NONANIM)
		m_eType = TYPE_ANIM;

	for (auto& pAnimation : m_Animations)
		Safe_Release(pAnimation);

	m_Animations.clear();


	for (auto& pAnimation : pModelCom->m_Animations)
		m_Animations.push_back((CAnimation*)pAnimation->Clone());

	for (auto pAnimation : m_Animations)
		FAILED_CHECK_RETURN(pAnimation->Synchronization_ChannelsBonePtr(this, strRootNodeName), E_FAIL);

	m_iNumAnimations = (_uint)m_Animations.size();

	return S_OK;
}

HRESULT CModel::Synchronization_MeshBone(CModel * pModelCom)
{
	NULL_CHECK_RETURN(pModelCom, E_FAIL);

	for (auto pMesh : pModelCom->m_Meshes)
	{
		FAILED_CHECK_RETURN(pMesh->Synchronization_BonePtr(this), E_FAIL);
	}

	char**		ppBoneName = new char*[pModelCom->m_iNumBones];
	_uint		iLength = 0;

	for (_uint i = 0; i < pModelCom->m_iNumBones; ++i)
	{
		iLength = (_uint)strlen(pModelCom->m_Bones[i]->Get_Name()) + 1;
		ppBoneName[i] = new char[iLength];
		strcpy_s(ppBoneName[i], iLength, pModelCom->m_Bones[i]->Get_Name());
	}

	//for (auto pBone : m_Bones)
	//	Safe_Release(pBone);

	_uint j = 0;
	CBone*	pRootBone = nullptr;
	for (auto iter = pModelCom->m_Bones.begin(); iter != pModelCom->m_Bones.end();)
	{
		Safe_Release(*iter);

		CBone*	pBone = Get_BonePtr(ppBoneName[j++]);
		if (pBone == nullptr)
		{
			// 			if (pRootBone != nullptr)
			// 			{
			// 				m_Bones[j - 1]->SetParent(pRootBone);
			// 				pRootBone->Add_Child(m_Bones[j - 1]);
			// 				pRootBone = nullptr;
			// 			}
			iter = pModelCom->m_Bones.erase(iter);
			continue;
		}

		if (iter == pModelCom->m_Bones.begin())
			pRootBone = pBone;

		//Safe_Release(*iter);
		*iter = pBone;
		Safe_AddRef(pBone);

		iter++;
	}

	for (_uint i = 0; i < pModelCom->m_iNumBones; ++i)
		Safe_Delete_Array(ppBoneName[i]);
	Safe_Delete_Array(ppBoneName);

	pModelCom->m_iNumBones = (_uint)pModelCom->m_Bones.size();

	return S_OK;
}

void CModel::Reset_Animation()
{
	m_Animations[m_iCurrentAnimIndex]->Reset_Animation();
}

void CModel::Set_AnimIndex(_uint iAnimIndex)
{
	if (iAnimIndex >= m_iNumAnimations)
		return;

	if (m_iCurrentAnimIndex != iAnimIndex)
	{
		m_iPreAnimIndex = m_iCurrentAnimIndex;

		m_Animations[iAnimIndex]->Reset_Animation();

		m_fBlendDuration = m_Animations[iAnimIndex]->Get_BlendDuration();
		m_fBlendCurTime = 0.f;
	}

	m_iCurrentAnimIndex = iAnimIndex;
}

void CModel::Set_BoneLocked(const char * pBoneName, _bool bLock)
{
	const auto	iter = find_if(m_Bones.begin(), m_Bones.end(), [&](CBone* pBone) {
		return !strcmp(pBone->Get_Name(), pBoneName);
	});

	if (iter == m_Bones.end())
		return;

	(*iter)->Set_BoneLocked(bLock);
}

void CModel::Set_AllBonesUnlock()
{
	for (auto pBone : m_Bones)
		pBone->Set_BoneLocked(false);
}

void CModel::ResetAnimIdx_PlayTime(_uint iAnimIndex)
{
	m_Animations[iAnimIndex]->Reset_Animation();
}

void CModel::Set_AdditiveAnimIndexForMonster(_uint iAnimIndex)
{
	m_iAdditiveAnimIndexForMonster = iAnimIndex;
}

void CModel::Play_AdditiveAnimForMonster(_float fTimeDelta, _float fRatio, const string& strRootBone)
{
	if (TYPE_NONANIM == m_eType)
		return;

	m_Animations[m_iAdditiveAnimIndexForMonster]->Update_Bones_AdditiveForMonster(fTimeDelta, fRatio, strRootBone);

	for (auto& pBone : m_Bones)
	{
		if (nullptr != pBone)
			pBone->Compute_CombindTransformationMatrix();
	}
}

HRESULT CModel::Add_Event(_uint iAnimIndex, _float fPlayTime, const string & strFuncName)
{
	if (iAnimIndex >= m_iNumAnimations)
		return E_FAIL;

	FAILED_CHECK_RETURN(m_Animations[iAnimIndex]->Add_Event(fPlayTime, strFuncName), E_FAIL);

	return S_OK;
}

void CModel::Call_Event(const string & strFuncName)
{
	m_pOwner->Call_EventFunction(strFuncName);
}

void CModel::Compute_CombindTransformationMatrix()
{
	for (auto pBone : m_Bones)
		pBone->Compute_CombindTransformationMatrix();
}

void CModel::Update_BonesMatrix(CModel * pModel)
{
	const char* pBoneName = nullptr;
	for (auto pBone : m_Bones)
	{
		pBoneName = pBone->Get_Name();

		CBone* pOriginBone = pModel->Get_BonePtr(pBoneName);
		if (pOriginBone != nullptr)
		{
			pBone->Set_CombindMatrix(pOriginBone->Get_CombindMatrix());
		}
	}
}

void CModel::Set_AllAnimCommonType()
{
	if (m_Animations.empty())
		return;

	for(auto& pAnim : m_Animations)
	{
		if (pAnim != nullptr)
			pAnim->Set_AnimationType(CAnimation::ANIMTYPE_COMMON);
	}
}

void CModel::Print_Animation_Names(const string & strFilePath)
{
	Json	jAnimName;

	string strAnimName = "";
	for (auto pAnimation : m_Animations)
	{
		strAnimName = pAnimation->Get_Name();
		jAnimName.push_back(strAnimName);
	}

	ofstream	file(strFilePath.c_str());
	file << jAnimName;
	file.close();
}

void CModel::Play_Animation(_float fTimeDelta)
{
	if (TYPE_NONANIM == m_eType)
		return;

	if (m_bPausePlay == true)
		fTimeDelta = 0.f;

	if (m_fBlendCurTime < m_fBlendDuration)
	{
		_float fBlendRatio = m_fBlendCurTime / m_fBlendDuration;

		m_Animations[m_iPreAnimIndex]->Update_Bones(fTimeDelta, m_strRootBone);
		m_Animations[m_iCurrentAnimIndex]->Update_Bones_Blend(fTimeDelta, fBlendRatio, m_strRootBone);

		m_fBlendCurTime += fTimeDelta;
	}
	else
		m_Animations[m_iCurrentAnimIndex]->Update_Bones(fTimeDelta, m_strRootBone);

	for (auto& pBone : m_Bones)
	{
		if (nullptr != pBone)
			pBone->Compute_CombindTransformationMatrix();
	}
}

HRESULT CModel::Bind_Material(CShader * pShader, _uint iMeshIndex, aiTextureType eType, const char * pConstantName)
{
	if (iMeshIndex >= m_iNumMeshes)
		return E_FAIL;
	_uint iMaterialIndex = 0;

	if (m_bIsInstancing == true)			/*for.Instancing*/
		iMaterialIndex = m_InstancingMeshes[iMeshIndex]->Get_MaterialIndex();
	else	                                          /*for.Origin*/
		iMaterialIndex = m_Meshes[iMeshIndex]->Get_MaterialIndex();

	if (iMaterialIndex >= m_iNumMaterials)
		return E_FAIL;

	if (nullptr != m_Materials[iMaterialIndex].pTexture[eType])
	{
		m_Materials[iMaterialIndex].pTexture[eType]->Bind_ShaderResource(pShader, pConstantName);
	}
	else
		return E_FAIL;


	return S_OK;
}

HRESULT CModel::Render(CShader* pShader, _uint iMeshIndex, const char* pBoneConstantName, _uint iPassIndex)
{
	/*For.Instancing*/
	if (true == m_bIsInstancing && nullptr != m_InstancingMeshes[iMeshIndex])
	{
		if (nullptr != pBoneConstantName)
		{
			_float4x4		BoneMatrices[800];
			m_InstancingMeshes[iMeshIndex]->SetUp_BoneMatrices(BoneMatrices, XMLoadFloat4x4(&m_PivotMatrix));
			pShader->Set_MatrixArray(pBoneConstantName, BoneMatrices, 800);
		}
		pShader->Begin(iPassIndex);
		m_InstancingMeshes[iMeshIndex]->Render();
	}
	/*For.Origin*/
	else  if (false == m_bIsInstancing	&& nullptr != m_Meshes[iMeshIndex])
	{
		if (nullptr != pBoneConstantName)
		{
			_float4x4		BoneMatrices[800];
			m_Meshes[iMeshIndex]->SetUp_BoneMatrices(BoneMatrices, XMLoadFloat4x4(&m_PivotMatrix));
			pShader->Set_MatrixArray(pBoneConstantName, BoneMatrices, 800);
		}
		pShader->Begin(iPassIndex);
		m_Meshes[iMeshIndex]->Render();
	}

	return S_OK;
}

HRESULT CModel::Load_MeshMaterial(const wstring & wstrModelFilePath)
{
	DWORD	dwByte = 0;
	HANDLE	hFile = CreateFileW(wstrModelFilePath.c_str(), GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (hFile == INVALID_HANDLE_VALUE)
		return E_FAIL;

	/* Meshes */
	ReadFile(hFile, &m_iNumMeshes, sizeof(_uint), &dwByte, nullptr);
	/*For. Instancing*/
	if (true == m_bIsInstancing)
	{
		m_InstancingMeshes.reserve(m_iNumMeshes);
		for (_uint i = 0; i < m_iNumMeshes; ++i)
		{
			CInstancing_Mesh*		pInstanceMesh = CInstancing_Mesh::Create(m_pDevice, m_pContext, nullptr, this);
			NULL_CHECK_RETURN(pInstanceMesh, E_FAIL);
			pInstanceMesh->Load_Mesh(hFile, dwByte);
			m_InstancingMeshes.push_back(pInstanceMesh);
		}
	}
	/*For. Origin*/
	else
	{
		m_Meshes.reserve(m_iNumMeshes);
		for (_uint i = 0; i < m_iNumMeshes; ++i)
		{
			CMesh*		pMesh = CMesh::Create(m_pDevice, m_pContext, nullptr, this);
			NULL_CHECK_RETURN(pMesh, E_FAIL);

			pMesh->Load_Mesh(hFile, dwByte);

			m_Meshes.push_back(pMesh);
		}
	}

	/* Materials	 여기를 손봐야한다.		*/
	ReadFile(hFile, &m_iNumMaterials, sizeof(_uint), &dwByte, nullptr);
	m_Materials.reserve(m_iNumMaterials);

	for (_uint i = 0; i < m_iNumMaterials; ++i)
	{
		MODELMATERIAL		tMaterial;
		ZeroMemory(&tMaterial, sizeof(MODELMATERIAL));

		for (_uint j = 0; j < (_uint)WJ_TEXTURE_TYPE_MAX; ++j)
		{
			_uint		iTemp = WJ_TEXTURE_TYPE_MAX;
			ReadFile(hFile, &iTemp, sizeof(_uint), &dwByte, nullptr);

			if (iTemp == (_uint)WJ_TEXTURE_TYPE_MAX)
				continue;

			_uint		iFilePathLength = 0;
			ReadFile(hFile, &iFilePathLength, sizeof(_uint), &dwByte, nullptr);

			_tchar	*	pFilePath = new _tchar[iFilePathLength];
			ReadFile(hFile, pFilePath, sizeof(_tchar) * iFilePathLength, &dwByte, nullptr);

			tMaterial.pTexture[j] = CTexture::Create(m_pDevice, m_pContext, pFilePath);
			NULL_CHECK_RETURN(tMaterial.pTexture[j], E_FAIL);

			Safe_Delete_Array(pFilePath);
		}

		m_Materials.push_back(tMaterial);
	}

	ReadFile(hFile, &m_dwBeginBoneData, sizeof(DWORD), &dwByte, nullptr);

	CloseHandle(hFile);

	return S_OK;
}

HRESULT CModel::Load_BoneAnimation(HANDLE & hFile, DWORD & dwByte)
{
	/* Bones */
	ReadFile(hFile, &m_iNumBones, sizeof(_uint), &dwByte, nullptr);
	m_Bones.reserve(m_iNumBones);

	for (_uint i = 0; i < m_iNumBones; ++i)
	{
		m_Bones.push_back(CBone::Create(nullptr));
		NULL_CHECK_RETURN(m_Bones[i], E_FAIL);
	}

	for (_uint i = 0; i < m_iNumBones; ++i)
	{
		m_Bones[i]->Load_Bone(hFile, dwByte);

		const char* pParentName = m_Bones[i]->Get_ParentName();
		if (!strcmp(pParentName, ""))
			continue;

		_uint j = 0;
		for (; j < m_iNumBones; j++)
		{
			if (i == j)
				continue;

			const char* pName = m_Bones[j]->Get_Name();
			if (!strcmp(pParentName, pName))
			{
				m_Bones[i]->SetParent(m_Bones[j]);
				m_Bones[j]->Add_Child(m_Bones[i]);
				break;
			}
		}

		if (j == m_iNumBones)
			return E_FAIL;
	}

	/* SetUp Mesh Bones */
	if (m_bIsInstancing == true)			/*For.Instaicing*/
	{
		for (auto& pInstMesh : m_InstancingMeshes)
			pInstMesh->SetUp_BonePtr(hFile, dwByte, this);

	}
	else												/*For.Origin*/
	{
		for (auto& pMesh : m_Meshes)
			pMesh->SetUp_BonePtr(hFile, dwByte, this);
	}

	/* Animations */
	ReadFile(hFile, &m_iNumAnimations, sizeof(_uint), &dwByte, nullptr);
	m_Animations.reserve(m_iNumAnimations);

	for (_uint i = 0; i < m_iNumAnimations; ++i)
	{
		CAnimation*	pAnimation = CAnimation::Create(nullptr, this);
		NULL_CHECK_RETURN(pAnimation, E_FAIL);
		FAILED_CHECK_RETURN(pAnimation->Load_Animation(hFile, dwByte), E_FAIL);

		m_Animations.push_back(pAnimation);
	}

	return S_OK;
}

CModel* CModel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pModelFilePath, _fmatrix PivotMatrix, const _tchar* pAdditionalFilePath,
	_bool bIsLod, _bool bIsInstancing, const char* JsonMatrial)
{
	CModel* pInstance = new CModel(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype(pModelFilePath, PivotMatrix, pAdditionalFilePath, bIsLod, bIsInstancing, JsonMatrial)))
	{
		MSG_BOX("Failed to Created : CModel");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CModel::Clone(void * pArg, CGameObject * pOwner)
{
	CModel*		pInstance = new CModel(*this);

	if (FAILED(pInstance->Initialize(pArg, pOwner)))
	{
		MSG_BOX("Failed to Cloned : CModel");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CModel::Free()
{
	__super::Free();

	for (auto& pAnimation : m_Animations)
		Safe_Release(pAnimation);
	m_Animations.clear();

	for (auto& pBone : m_Bones)
		Safe_Release(pBone);

	m_Bones.clear();

	for (auto& Material : m_Materials)
	{
		for (_uint i = 0; i < (_uint)WJ_TEXTURE_TYPE_MAX; ++i)
			Safe_Release(Material.pTexture[i]);
	}
	m_Materials.clear();

	for (auto& pMesh : m_Meshes)		/*Origin Mesh*/
		Safe_Release(pMesh);
	m_Meshes.clear();

	for (auto& pInstMesh : m_InstancingMeshes)		/*Instancing Mesh*/
		Safe_Release(pInstMesh);
	m_InstancingMeshes.clear();

	for (auto &pInstMatrix : m_pInstancingMatrix)
	{
		Safe_Delete(pInstMatrix);
	}
	m_pInstancingMatrix.clear();

#ifdef _DEBUG
	Safe_Release(m_pInstanceTransform);
#endif
}

HRESULT CModel::SetUp_Material(_uint iMaterialIndex, aiTextureType eType, const _tchar *pTexturePath)
{
	if (iMaterialIndex >= m_Materials.size()) return E_FAIL;

	CTexture *pTexture = CTexture::Create(m_pDevice, m_pContext, pTexturePath);
	if (pTexture == nullptr) return E_FAIL;

	if (m_Materials[iMaterialIndex].pTexture[eType]) {
		Safe_Release(m_Materials[iMaterialIndex].pTexture[eType]);
	}

	m_Materials[iMaterialIndex].pTexture[eType] = pTexture;

	return S_OK;
}

#ifdef _DEBUG
void CModel::Imgui_MeshInstancingPosControl(_fmatrix parentMatrix, _float4 vPickingPos,_fmatrix TerrainMatrix,_bool bPickingTerrain)
{
	if (ImGui::BeginListBox("##"))			// 내행렬 * 부모행렬(원본 위치)
	{
		_int iIndex = 0;
		for (auto& ProtoPair : m_pInstancingMatrix)
		{
			const bool bSelected = false;

			char szViewName[512];

			sprintf_s(szViewName, sizeof(szViewName), " Instancing_ %d _ Index ", iIndex);

			if (ImGui::Selectable(szViewName, bSelected))
			{
				m_iSelectMeshInstace_Index = iIndex;
			}

			++iIndex;
		}

		ImGui::EndListBox();
	}
	ImGui::Text("Cur Index : %d", m_iSelectMeshInstace_Index);

	if (bPickingTerrain == false)
	{
		if (ImGui::Button("Instancing Num Increase"))
		{
			_float4x4* Temp = new _float4x4;
			XMStoreFloat4x4(Temp, XMMatrixIdentity());
			m_pInstancingMatrix.push_back(Temp);

			for (auto& pInstMesh : m_InstancingMeshes)
				pInstMesh->Add_InstanceModel(m_pInstancingMatrix);
		}
	}
	else
	{
		_float4x4* Temp = new _float4x4;
		XMStoreFloat4x4(Temp, XMMatrixIdentity());
	
		memcpy(&Temp->m[3], &vPickingPos, sizeof(_float4));

		
		m_pInstancingMatrix.push_back(Temp);

		for (auto& pInstMesh : m_InstancingMeshes)
			pInstMesh->Add_InstanceModel(m_pInstancingMatrix);
	}

	if (ImGui::Button("Instancing Num Delete"))
	{
		_int iDeleteIndex = 0;
		for (auto iter = m_pInstancingMatrix.begin(); iter != m_pInstancingMatrix.end();)
		{
			if (iDeleteIndex == m_iSelectMeshInstace_Index)
			{
				Safe_Delete(*iter);
				*iter = nullptr;
				iter = m_pInstancingMatrix.erase(iter);
				m_iSelectMeshInstace_Index = -1;
				for (auto& pInstMesh : m_InstancingMeshes)
					pInstMesh->Add_InstanceModel(m_pInstancingMatrix);
				break;
			}
			else
			{
				++iter;
				++iDeleteIndex;
			}
		}

	}

	if (m_iSelectMeshInstace_Index == -1)
		return;

	_matrix ParentMulChild, InvParentMulChild, ResultMatrix;
	InvParentMulChild = XMMatrixInverse(nullptr, parentMatrix);
	ParentMulChild = XMLoadFloat4x4(m_pInstancingMatrix[m_iSelectMeshInstace_Index]) * parentMatrix;

	m_pInstanceTransform->Set_WorldMatrix(ParentMulChild);

	m_pInstanceTransform->Imgui_RenderProperty();

	ResultMatrix = m_pInstanceTransform->Get_WorldMatrix();

	ResultMatrix *= InvParentMulChild;
	XMStoreFloat4x4(m_pInstancingMatrix[m_iSelectMeshInstace_Index], ResultMatrix);

	for (auto& pInstMesh : m_InstancingMeshes)
		pInstMesh->Add_InstanceModel(m_pInstancingMatrix);
}
#endif


void CModel::Create_PxTriangle()
{
	for (auto &iter : m_Meshes)
	{
		iter->Create_PxTriangleData();
	}
}

void CModel::Set_PxPosition(_float3 vPosition)
{
	for (auto &iter : m_Meshes)
	{
		iter->Set_PxPosition(vPosition);
	}
}

void CModel::Set_PxMatrix(_float4x4& Matrix)
{
	for (auto &iter : m_Meshes)
	{
		iter->Set_PxMatrix(Matrix);
	}
}
