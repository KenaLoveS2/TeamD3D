#include "stdafx.h"
#include "..\public\EnviromentObj.h"
#include "GameInstance.h"
#include "Utile.h"
#include "Enviroment_Manager.h"

CEnviromentObj::CEnviromentObj(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
	, m_pEnviroment_Manager(CEnviroment_Manager::GetInstance())
{
}

CEnviromentObj::CEnviromentObj(const CEnviromentObj & rhs)
	: CGameObject(rhs)
	, m_pEnviroment_Manager(rhs.m_pEnviroment_Manager)
{
}

void CEnviromentObj::Add_TexturePath(const _tchar * TexturePath)
{
	m_EnviromentDesc.vecStr_textureFilePath.push_back(TexturePath);
	/*나중에 중복 처리하기*/
}

HRESULT CEnviromentObj::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype())) return E_FAIL;

	return S_OK;
}

HRESULT CEnviromentObj::Initialize(void * pArg)
{
	m_CurComponenteTag.reserve(5);

	if (pArg)
	{
		ENVIROMENT_DESC* Desc = reinterpret_cast<ENVIROMENT_DESC*>(pArg);

		m_EnviromentDesc.szProtoObjTag = Desc->szProtoObjTag;
		m_EnviromentDesc.szModelTag = Desc->szModelTag;
		m_EnviromentDesc.szTextureTag = Desc->szTextureTag;
		m_EnviromentDesc.iRoomIndex = Desc->iRoomIndex;
		m_EnviromentDesc.eChapterType = Desc->eChapterType;
		m_EnviromentDesc.iCurLevel = Desc->iCurLevel;			//일단 툴에서만
		m_EnviromentDesc.vecStr_textureFilePath = Desc->vecStr_textureFilePath;
		m_EnviromentDesc.ObjectDesc.TransformDesc.fRotationPerSec = 90.f;
		m_EnviromentDesc.ObjectDesc.TransformDesc.fSpeedPerSec = 5.f;
	}
	else
	{
		m_EnviromentDesc.ObjectDesc.TransformDesc.fRotationPerSec = 90.f;
		m_EnviromentDesc.ObjectDesc.TransformDesc.fSpeedPerSec = 5.f;
	}

	if (FAILED(__super::Initialize(&m_EnviromentDesc.ObjectDesc))) return E_FAIL;


	HRESULT SetUp_Component();


	return S_OK;
}

void CEnviromentObj::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CEnviromentObj::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	m_bRenderActive = m_pEnviroment_Manager->Is_RenderIndex(m_EnviromentDesc.iRoomIndex);
}

HRESULT CEnviromentObj::Render()
{
	__super::Render();

	return S_OK;
}

HRESULT CEnviromentObj::Add_AdditionalComponent(_uint iLevelIndex, const _tchar * pComTag, COMPONENTS_OPTION eComponentOption)
{
	m_CurComponenteTag.push_back(pComTag);

	return S_OK;
}

void CEnviromentObj::Imgui_RenderComponentProperties()
{
	if (ImGui::CollapsingHeader("Single_Enviroment Obj"))
	{
		ImGui::InputInt(" Option : ", &m_iImgui_ComponentOption);
#pragma region 예외처리
		if (m_iImgui_ComponentOption < 0)
			m_iImgui_ComponentOption = 0;
		else if (m_iImgui_ComponentOption >= static_cast<COMPONENTS_OPTION>(COMPONENTS_END))
			m_iImgui_ComponentOption = COMPONENTS_END;
#pragma endregion

		if (m_iImgui_ComponentOption == 0)
			m_str_Imgui_ComTag = "Com_CtrlMove";
		else if (m_iImgui_ComponentOption == 1)
			m_str_Imgui_ComTag = "Com_Interaction";
		else
			m_str_Imgui_ComTag = "None";

		ImGui::Text("Cur Option : %s", m_str_Imgui_ComTag.c_str());

		if (ImGui::Button("Add Component") && m_str_Imgui_ComTag != "None")
		{
			if (COMPONENTS_END == m_iImgui_ComponentOption)
				return;

			CGameInstance* pGameInstace = GET_INSTANCE(CGameInstance);
			_tchar* pComTag = CUtile::StringToWideChar(m_str_Imgui_ComTag);
			pGameInstace->Add_String(pComTag);
			Add_AdditionalComponent(pGameInstace->Get_CurLevelIndex(), pComTag,
				static_cast<COMPONENTS_OPTION>(m_iImgui_ComponentOption));

			RELEASE_INSTANCE(CGameInstance);
		}


#pragma region 와이어프레임으로 그리기
		ImGui::Checkbox("WireFrame_Render", &m_bWireFrame_Rendering);
		if (m_bWireFrame_Rendering == true)
			m_iShaderOption = 3;
		else
			m_iShaderOption = 0;
	}
#pragma endregion

	__super::Imgui_RenderComponentProperties();
}

void CEnviromentObj::Free()
{
	__super::Free();

}