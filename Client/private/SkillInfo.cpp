#include "stdafx.h"
#include "..\public\SkillInfo.h"
#include "GameInstance.h"	
#include "Json/json.hpp"
#include <fstream>
#include <codecvt>
#include <locale>
#include "Utile.h"
#include "Kena.h"
#include "Kena_Status.h"

CSkillInfo::CSkillInfo()
	:m_pTextureProtoTag(nullptr)
{
}

//void CSkillInfo::UnLock(_uint iLevel)
//{
//	/* return it can be unlocked. For Confirm window */
	//CHECK eResult = Check(iLevel);
	//wstring msg;
	//switch (eResult)
	//{
	//case CHECK_PREVSKILL:
	//	msg = L"���� �ɷ� �ʿ�";
	//	break;
	//case CHECK_ROTLEVEL:
	//	msg = L"�νķ� ���� " + to_wstring(m_tDesc[iLevel].conditions[CHECK_ROTLEVEL]) + L" �ʿ�";
	//	break;
	//case  CHECK_KARMA:
	//	msg = L"ī���� ����";
	//	break;
	//case CHECK_UNLOCKED_AVAILABLE:
	//	msg = L"���׷��̵� ����";
	//	break;
	//case CHECK_UNLOCKED_ALREADY:
	//	msg = L"�߰� ī���� �ʿ�";
	//	break;
	//default:
	//	msg = L"���ư�";
	//	break;
	//}
	//return msg;
//}

CSkillInfo::CHECK CSkillInfo::Check(_uint iLevel, CKena* pPlayer)
{
	_int iRotLevel, iKarma;

	/* Current Data */
	if (pPlayer == nullptr)
		return CHECK_END;

	iRotLevel = pPlayer->Get_Status()->Get_RotLevel();
	iKarma = pPlayer->Get_Status()->Get_Karma();

	if (m_tDesc[iLevel].eState == STATE_UNLOCKED)
		return CHECK_UNLOCKED_ALREADY;
	
	_int iPrevLevel = m_tDesc[iLevel].conditions[CONDITION_PREVSKILL];
	if (iPrevLevel != -1 && m_tDesc[iPrevLevel].eState != STATE_UNLOCKED)
		return CHECK_PREVSKILL;

	if (iRotLevel < m_tDesc[iLevel].conditions[CONDITION_ROTLEVEL])
		return CHECK_ROTLEVEL;
	
	if (iKarma < m_tDesc[iLevel].conditions[CONDITION_KARMA])
		return CHECK_KARMA;

	return CHECK_UNLOCKED_AVAILABLE;

}

HRESULT CSkillInfo::Load_File(ID3D11Device* pDevice, ID3D11DeviceContext*	pContext, wstring filePath)
{
	Json	jLoad;

	string strFilePath;
	//strFilePath.assign(filePath.begin(), filePath.end());
	strFilePath = CUtile::wstring_to_utf8(filePath);

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

	if (g_LEVEL == LEVEL_TESTPLAY)
	{
		/* Create Texture Component */
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

		if (FAILED(pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(),
			m_pTextureProtoTag, CTexture::Create(pDevice, pContext, CUtile::Create_StringAuto(wstrImageFilePath.c_str()), 5))))
			return E_FAIL;

		RELEASE_INSTANCE(CGameInstance);
	}

	/* 2. Name, Description, Condition ,State */
	//using convert_type = codecvt_utf8<wchar_t>;
	//wstring_convert<convert_type> utf8_conv;
	
	for (_uint i = 0; i < 5; ++i)
	{
		string strLevel = "Level" + to_string(i);
		for (auto jLevel : jLoad[strLevel])
		{
			string strName, strDesc;
			jLevel["Name"].get_to<string>(strName);
			jLevel["Desc"].get_to<string>(strDesc);

			m_tDesc[i].wstrName = CUtile::utf8_to_wstring(strName);
			m_tDesc[i].wstrDesc = CUtile::utf8_to_wstring(strDesc);

			_int iState;
			jLevel["State"].get_to<int>(iState);
			m_tDesc[i].eState = (STATE)(iState);

			for (auto jCondition : jLevel["Condition"])
			{
				jCondition["PrevSkill"].get_to<_int>(m_tDesc[i].conditions[CONDITION_PREVSKILL]);
				jCondition["RotLevel"].get_to<_int>(m_tDesc[i].conditions[CONDITION_ROTLEVEL]);
				jCondition["Karma"].get_to<_int>(m_tDesc[i].conditions[CONDITION_KARMA]);
			}

			string strVideo;
			jLevel["Video"].get_to<string>(strVideo);
			m_tDesc[i].wstrVideo = CUtile::utf8_to_wstring(strVideo);

		}
	}

	return S_OK;
}

CSkillInfo * CSkillInfo::Create(ID3D11Device* pDevice, ID3D11DeviceContext*	pContext, wstring filePath)
{
	CSkillInfo* pInstance = new CSkillInfo();
	if (FAILED(pInstance->Load_File(pDevice, pContext, filePath)))
	{
		MSG_BOX("Failed To Load : SkillInfo");
	}
	return pInstance;
}

void CSkillInfo::Free()
{
	Safe_Delete_Array(m_pTextureProtoTag);
}


