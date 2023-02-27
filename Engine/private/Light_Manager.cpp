#include "stdafx.h"
#include "..\public\Light_Manager.h"
#include "Light.h"

IMPLEMENT_SINGLETON(CLight_Manager)

CLight_Manager::CLight_Manager()
{
}

const LIGHTDESC * CLight_Manager::Get_LightDesc(_uint iIndex)
{
	if (iIndex >= m_Lights.size()) return nullptr;

	return m_Lights[iIndex]->Get_LightDesc();	
}

HRESULT CLight_Manager::Add_Light(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const LIGHTDESC & LightDesc, CLight** ppOut)
{
	CLight* pLight = CLight::Create(pDevice, pContext, LightDesc);
	if (nullptr == pLight) return E_FAIL;

	m_Lights.push_back(pLight);

	if (ppOut) *ppOut = pLight;

	return S_OK;
}

void CLight_Manager::Render_Light(CVIBuffer_Rect * pVIBuffer, CShader * pShader)
{
	for (auto& pLight : m_Lights)
	{
		if(pLight)
			pLight->Get_LightDesc()->isEnable && pLight->Render(pVIBuffer, pShader);
	}
}

void CLight_Manager::Clear()
{
	for (auto& pLight : m_Lights)
		Safe_Release(pLight);

	m_Lights.clear();
}

void CLight_Manager::Imgui_Render()
{
	static _int iSelectLight = -1;
	static _int iSize = (_int)m_Lights.size();
	CLight* pLight = nullptr;

	LIGHTDESC::TYPE eType = LIGHTDESC::TYPE_END;

	char** ppLightTag = new char*[iSize];

	for (int n = 0; n < iSize; n++)
	{
		_uint	iTagLength = _uint(strlen(m_Lights[n]->Get_LightDesc()->szLightName)) + 1;
		ppLightTag[n] = new char[iTagLength];
		sprintf_s(ppLightTag[n], sizeof(char) * iTagLength, m_Lights[n]->Get_LightDesc()->szLightName);
	}

	ImGui::ListBox("Light List", &iSelectLight, ppLightTag, (_int)iSize);

	if (iSelectLight != -1)
	{
		static const char* current_item = NULL;
		pLight = m_Lights[iSelectLight];
		eType = pLight->Get_LightDesc()->eType;
	}

	static bool bEnable = true;

	if (pLight != nullptr)
	{
		ImGui::Checkbox("Enable", &bEnable);
		pLight->Set_Enable(bEnable);

		switch (eType)
		{
		case LIGHTDESC::TYPE_DIRECTIONAL:
			{
				_float  fDirction[3] = { pLight->Get_LightDesc()->vDirection.x,	pLight->Get_LightDesc()->vDirection.y, pLight->Get_LightDesc()->vDirection.z };
				static _float2 dirMinMax{ -100.f, 100.f };
				ImGui::InputFloat2("DirMinMax", (float*)&dirMinMax);
				ImGui::DragFloat3("Dir", fDirction, 0.1f, dirMinMax.x, dirMinMax.y);
				pLight->Set_Direction(_float4(fDirction[0], fDirction[1], fDirction[2], 0.f));

				if(ImGui::CollapsingHeader("Diffuse"))
				{
					_float4 vDiffuseColor = pLight->Get_LightDesc()->vDiffuse;
					Set_ColorValue(COLOR_DIFFUSE, (char*)&pLight->Get_LightDesc()->szLightName, vDiffuseColor);
					pLight->Set_Diffuse(vDiffuseColor);
				}

				if (ImGui::CollapsingHeader("Ambient"))
				{
					_float4 vAmbientColor = pLight->Get_LightDesc()->vAmbient;
					Set_ColorValue(COLOR_AMBIENT, (char*)&pLight->Get_LightDesc()->szLightName, vAmbientColor);
					pLight->Set_Ambient(vAmbientColor);
				}

				if (ImGui::CollapsingHeader("Specular"))
				{
					_float4 vSpecularColor = pLight->Get_LightDesc()->vSpecular;
					Set_ColorValue(COLOR_SPECULAR, (char*)&pLight->Get_LightDesc()->szLightName, vSpecularColor);
					pLight->Set_Spectular(vSpecularColor);
				}
			}
			break;

		case LIGHTDESC::TYPE_POINT:
			{
				_float  fPostion[3] = { pLight->Get_LightDesc()->vPosition.x,	pLight->Get_LightDesc()->vPosition.y, pLight->Get_LightDesc()->vPosition.z };
				static _float2 dirMinMax{ -100.f, 100.f };
				ImGui::InputFloat2("PosMinMax", (float*)&dirMinMax);
				ImGui::DragFloat3("Pos", fPostion, 0.1f, dirMinMax.x, dirMinMax.y);
				pLight->Set_Position(_float4(fPostion[0], fPostion[1], fPostion[2], 1.f));

				_float fRange = pLight->Get_LightDesc()->fRange;
				static _float2 rangeMinMax{ -100.f, 100.f };
				ImGui::InputFloat2("RangeMinMax", (float*)&rangeMinMax);
				ImGui::DragFloat("Range", &fRange, 0.1f, rangeMinMax.x, rangeMinMax.y);
				pLight->Set_Range(fRange);

				if (ImGui::CollapsingHeader("Diffuse"))
				{
					_float4 vDiffuseColor = pLight->Get_LightDesc()->vDiffuse;
					Set_ColorValue(COLOR_DIFFUSE, (char*)&pLight->Get_LightDesc()->szLightName, vDiffuseColor);
					pLight->Set_Diffuse(vDiffuseColor);
				}

				if (ImGui::CollapsingHeader("Ambient"))
				{
					_float4 vAmbientColor = pLight->Get_LightDesc()->vAmbient;
					Set_ColorValue(COLOR_AMBIENT, (char*)&pLight->Get_LightDesc()->szLightName, vAmbientColor);
					pLight->Set_Ambient(vAmbientColor);
				}

				if (ImGui::CollapsingHeader("Specular"))
				{
					_float4 vSpecularColor = pLight->Get_LightDesc()->vSpecular;
					Set_ColorValue(COLOR_SPECULAR, (char*)&pLight->Get_LightDesc()->szLightName, vSpecularColor);
					pLight->Set_Spectular(vSpecularColor);
				}
			}
			break;

		case LIGHTDESC::TYPE_END:
			break;
		}
	}
	
	for (_uint i = 0; i < (_uint)iSize; ++i)
		Safe_Delete_Array(ppLightTag[i]);

	Safe_Delete_Array(ppLightTag);
}

void CLight_Manager::Set_ColorValue(eColor eType, char* pLightTag, OUT _float4& vColor)
{
	static bool alpha_preview = true;
	static bool alpha_half_preview = false;
	static bool drag_and_drop = true;
	static bool options_menu = true;
	static bool hdr = true;

	ImGuiColorEditFlags misc_flags = (hdr ? ImGuiColorEditFlags_HDR : 0) | (drag_and_drop ? 0 : ImGuiColorEditFlags_NoDragDrop) | (alpha_half_preview ? ImGuiColorEditFlags_AlphaPreviewHalf : (alpha_preview ? ImGuiColorEditFlags_AlphaPreview : 0)) | (options_menu ? 0 : ImGuiColorEditFlags_NoOptions);

	static bool   ref_color = false;
	static ImVec4 ref_color_v(1.f, 1.f, 1.f, 1.f);

	char DiffColor[MAX_PATH] = { "DiffuseColor##4_" };
	strcat_s(DiffColor, MAX_PATH, pLightTag);
	char DColor[MAX_PATH] = { "Diffuse##2f_" };
	strcat_s(DColor, MAX_PATH, pLightTag);

	char AmColor[MAX_PATH] = { "AmbientColor##4_" };
	strcat_s(AmColor, MAX_PATH, pLightTag);
	char AColor[MAX_PATH] = { "Ambient##2f_" };
	strcat_s(AColor, MAX_PATH, pLightTag);

	char SpecColor[MAX_PATH] = { "SpecularColor##4_" };
	strcat_s(SpecColor, MAX_PATH, pLightTag);
	char SColor[MAX_PATH] = { "Specular##2f_" };
	strcat_s(SColor, MAX_PATH, pLightTag);


	if(eType == COLOR_DIFFUSE)
	{
		_float4 color = _float4(vColor.x, vColor.y, vColor.z, vColor.w);
		ImGui::ColorPicker4(DiffColor, (float*)&color, ImGuiColorEditFlags_HDR | misc_flags, ref_color ? &ref_color_v.x : NULL);
		ImGui::ColorEdit4(DColor, (float*)&color, ImGuiColorEditFlags_HDR | misc_flags);
		vColor = _float4(color.x, color.y, color.z, color.w);
	}
	else if (eType == COLOR_AMBIENT)
	{
		_float4 color = _float4(vColor.x, vColor.y, vColor.z, vColor.w);
		ImGui::ColorPicker4(AmColor, (float*)&color, ImGuiColorEditFlags_HDR | misc_flags, ref_color ? &ref_color_v.x : NULL);
		ImGui::ColorEdit4(AColor, (float*)&color, ImGuiColorEditFlags_HDR | misc_flags);
		vColor = _float4(color.x, color.y, color.z, color.w);
	}
	else if (eType == COLOR_SPECULAR)
	{
		_float4 color = _float4(vColor.x, vColor.y, vColor.z, vColor.w);
		ImGui::ColorPicker4(SpecColor, (float*)&color, ImGuiColorEditFlags_HDR | misc_flags, ref_color ? &ref_color_v.x : NULL);
		ImGui::ColorEdit4(SColor, (float*)&color, ImGuiColorEditFlags_HDR | misc_flags);
		vColor = _float4(color.x, color.y, color.z, color.w);
	}	
}

void CLight_Manager::Free()
{
	for (auto& pLight : m_Lights)
		Safe_Release(pLight);

	m_Lights.clear();
}
