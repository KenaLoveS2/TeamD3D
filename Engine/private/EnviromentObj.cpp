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

void CEnviromentObj::Add_TexturePath(const _tchar * TexturePath, aiTextureType Type)
{
	if (WJTextureType_NONE == Type)
	{
		MSG_BOX("NONE");
	}
	else 	if (WJTextureType_DIFFUSE == Type)
	{
		m_EnviromentDesc.AI_textureFilePaths.DIFFUSE_path = TexturePath;
	}
	else 	if (WJTextureType_SPECULAR == Type)
	{
		m_EnviromentDesc.AI_textureFilePaths.SPECULAR_path = TexturePath;
	}
	else 	if (WJTextureType_AMBIENT == Type)
	{
		m_EnviromentDesc.AI_textureFilePaths.AMBIENT_path = TexturePath;
	}
	else 	if (WJTextureType_EMISSIVE == Type)
	{
		m_EnviromentDesc.AI_textureFilePaths.EMISSIVE_path = TexturePath;
	}
	else 	if (WJTextureType_EMISSIVEMASK == Type)
	{
		m_EnviromentDesc.AI_textureFilePaths.EMISSIVEMASK_path = TexturePath;
	}
	else 	if (WJTextureType_NORMALS == Type)
	{
		m_EnviromentDesc.AI_textureFilePaths.NORMALS_path = TexturePath;
	}
	else 	if (WJTextureType_MASK == Type)
	{
		m_EnviromentDesc.AI_textureFilePaths.MASK_path = TexturePath;
	}
	else 	if (WJTextureType_SSS_MASK == Type)
	{
		m_EnviromentDesc.AI_textureFilePaths.SSS_MASK_path = TexturePath;
	}
	else 	if (WJTextureType_SPRINT_EMISSIVE == Type)
	{
		m_EnviromentDesc.AI_textureFilePaths.SPRINT_EMISSIVE_path = TexturePath;
	}
	else 	if (WJTextureType_LIGHTMAP == Type)
	{
		m_EnviromentDesc.AI_textureFilePaths.LIGHTMAP_path = TexturePath;
	}
	else 	if (WJTextureType_REFLECTION == Type)
	{
		m_EnviromentDesc.AI_textureFilePaths.REFLECTION_path = TexturePath;
	}
	else 	if (WJTextureType_BASE_COLOR == Type)
	{
		m_EnviromentDesc.AI_textureFilePaths.BASE_COLOR_path = TexturePath;
	}
	else 	if (WJTextureType_NORMAL_CAMERA == Type)
	{
		m_EnviromentDesc.AI_textureFilePaths.NORMAL_CAMERA_path = TexturePath;
	}
	else 	if (WJTextureType_EMISSION_COLOR == Type)
	{
		m_EnviromentDesc.AI_textureFilePaths.EMISSION_COLOR_path = TexturePath;
	}
	else 	if (WJTextureType_METALNESS == Type)
	{
		m_EnviromentDesc.AI_textureFilePaths.METALNESS_path = TexturePath;
	}
	else 	if (WJTextureType_DIFFUSE_ROUGHNESS == Type)
	{
		m_EnviromentDesc.AI_textureFilePaths.DIFFUSE_ROUGHNESS_path = TexturePath;
	}
	else 	if (WJTextureType_AMBIENT_OCCLUSION == Type)
	{
		m_EnviromentDesc.AI_textureFilePaths.AMBIENT_OCCLUSION_path = TexturePath;
	}
	else
		assert(!  " CEnviromentObj::Add_TexturePath Out_Of_Range");
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
		m_EnviromentDesc.AI_textureFilePaths = Desc->AI_textureFilePaths;
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