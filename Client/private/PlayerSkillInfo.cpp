#include "stdafx.h"
#include "..\public\PlayerSkillInfo.h"
#include "GameInstance.h"	
#include "Json/json.hpp"
#include <fstream>
#include <codecvt>
#include <locale>
#include "Utile.h"

CPlayerSkillInfo::CPlayerSkillInfo()
	:m_pTextureProtoTag(nullptr)
{
}

HRESULT CPlayerSkillInfo::Load_File(ID3D11Device* pDevice, ID3D11DeviceContext*	pContext, wstring filePath)
{
	Json	jLoad;

	string strFilePath;
	strFilePath.assign(filePath.begin(), filePath.end());

	ifstream file(filePath);
	if (file.fail())
		return E_FAIL;
	file >> jLoad;
	file.close();

	/* 1) Texture File Load -> Make Texture ProtoComponent */
	string imageFile;
	jLoad["ImagePath"].get_to<string>(imageFile);

	jLoad["Category"].get_to<string>(m_strType);
	wstring wtype;
	wtype.assign(m_strType.begin(), m_strType.end());

	wstring wstrImageFilePath;
	wstrImageFilePath.assign(imageFile.begin(), imageFile.end());

	wstring protoTag = L"Prototype_Component_Texture_SkillIcon_";
	protoTag += wtype;
	m_pTextureProtoTag = CUtile::Create_String(protoTag.c_str());

	/* Create Texture Component */
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), 
		m_pTextureProtoTag, CTexture::Create(pDevice, pContext, wstrImageFilePath.c_str(), 5))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	/* 2. Name, Description, Condition ,State */
	using convert_type = codecvt_utf8<wchar_t>;
	wstring_convert<convert_type> utf8_conv;
	
	for (_uint i = 0; i < 5; ++i)
	{
		string strLevel = "Level" + to_string(i);
		for (auto jLevel : jLoad[strLevel])
		{
			string strName, strDesc;
			jLevel["Name"].get_to<string>(strName);
			jLevel["Desc"].get_to<string>(strDesc);

			m_tDesc[i].wstrName = utf8_conv.from_bytes(strName);
			m_tDesc[i].wstrDesc = utf8_conv.from_bytes(strDesc);

			_int iState;
			jLevel["State"].get_to<int>(iState);
			m_tDesc[i].eState = (STATE)(iState);

			for (auto jCondition : jLevel["Condition"])
			{
				jCondition["PrevSkill"].get_to<_int>(m_tDesc[i].conditions[CONDITION_PREVSKILL]);
				jCondition["RotLevel"].get_to<_int>(m_tDesc[i].conditions[CONDITION_ROTLEVEL]);
				jCondition["Karma"].get_to<_int>(m_tDesc[i].conditions[CONDITION_KARMA]);
			}

		}
	}

	return S_OK;
}

CPlayerSkillInfo * CPlayerSkillInfo::Create(ID3D11Device* pDevice, ID3D11DeviceContext*	pContext, wstring filePath)
{
	CPlayerSkillInfo* pInstance = new CPlayerSkillInfo();
	if (FAILED(pInstance->Load_File(pDevice, pContext, filePath)))
	{
		MSG_BOX("Failed To Load : PlayerSkillInfo");
	}
	return pInstance;
}

void CPlayerSkillInfo::Free()
{
	Safe_Delete_Array(m_pTextureProtoTag);
}


