#include "stdafx.h"
#include "..\public\UI_NodeQuest.h"
#include "GameInstance.h"
#include "Json/json.hpp"
#include <fstream>
#include <codecvt>
#include <locale>
#include "Utile.h"

CUI_NodeQuest::CUI_NodeQuest(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CUI_Node(pDevice, pContext)
	, m_szQuest(nullptr)
	, m_fTime(0.f)
	, m_fTimeAcc(0.f)
	, m_fSpeed(0.f)
	, m_fAlpha(0.f)
	, m_bOpening(false)
	, m_bClear(false)
	, m_iReward(10)
{
}

CUI_NodeQuest::CUI_NodeQuest(const CUI_NodeQuest & rhs)
	: CUI_Node(rhs)
	, m_szQuest(nullptr)
	, m_fTime(0.f)
	, m_fTimeAcc(0.f)
	, m_fSpeed(0.f)
	, m_fAlpha(0.f)
	, m_bOpening(false)
	, m_bClear(false)
	, m_iReward(10)
{
}

void CUI_NodeQuest::Set_QuestString(wstring str)
{

	//wcscpy_s()
	m_szQuest = CUtile::Create_String(str.c_str());
	//m_szQuest = new _tchar[str.length()+1];
	//_int i = 0;
	//for (auto c : str)
	//{
	//	m_szQuest[i] = c;
	//	i++;
	//}
	//m_szQuest[i] = '\0';
}

void CUI_NodeQuest::Set_Clear()
{
	m_bClear = true;
	m_fTimeAcc = 0.0f;
}

HRESULT CUI_NodeQuest::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_NodeQuest::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
	{
		m_pTransformCom->Set_Scaled(_float3(400.f, 24.f, 1.f));
		XMStoreFloat4x4(&m_matLocal, m_pTransformCom->Get_WorldMatrix());
	}

	if (FAILED(SetUp_Components()))
	{
		MSG_BOX("Failed To SetUp Components");
		return E_FAIL;
	}

	//m_bActive = false;
	return S_OK;
}

void CUI_NodeQuest::Tick(_float fTimeDelta)
{
	if (!m_bActive)
		return;

	if (!m_bOpening)
	{
		m_fSpeed = 0.3f;
		m_fTimeAcc += fTimeDelta;
		if (m_fAlpha < 1.f)
		{
			m_fAlpha += m_fSpeed * m_fTimeAcc;
			//m_matLocal._11 += m_fSpeed * m_fTimeAcc;
			//Set_LocalMatrix(m_matLocal);
		}
		else
		{
			m_bOpening = true;
			m_fAlpha = 1.f;
			m_fTimeAcc = 0.f;
		}
	}

	if (m_bOpening && m_bClear)
	{
		m_fSpeed = 0.3f;
		m_fTimeAcc += fTimeDelta;
		if (m_fAlpha <= 0.f)
			m_bActive = false;
		else
		{
			m_fAlpha -= m_fSpeed * m_fTimeAcc;
			m_bClear = false;
		}
	}

	__super::Tick(fTimeDelta);

}

void CUI_NodeQuest::Late_Tick(_float fTimeDelta)
{
	if (!m_bActive)
		return;

	__super::Late_Tick(fTimeDelta);
}

HRESULT CUI_NodeQuest::Render()
{
	if (nullptr == m_pTextureCom[TEXTURE_DIFFUSE])
		return E_FAIL;

	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
	{
		MSG_BOX("Failed To Setup ShaderResources : UI_HUDHPBar");
		return E_FAIL;
	}

	m_pShaderCom->Begin(m_iRenderPass);
	m_pVIBufferCom->Render();

	_float4 vPos;
	XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
	_float2 vNewPos = { vPos.x + g_iWinSizeX*0.5f + 25.f, g_iWinSizeY*0.5f - vPos.y -20.f};

	CGameInstance::GetInstance()->Render_Font(TEXT("Font_Jangmi0"), m_szQuest,
		vNewPos /* position */,
		0.f, _float2(1.f, 1.f)/* size */, 
		XMVectorSet(1.f, 1.f, 1.f, m_fAlpha)/* color */);

	return S_OK;
}

HRESULT CUI_NodeQuest::SetUp_Components()
{
	/* Renderer */
	if (__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom))
		return E_FAIL;

	/* Shader */
	if (__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_VtxTex"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom))
		return E_FAIL;

	/* VIBuffer_Rect */
	if (__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom))
		return E_FAIL;


	return S_OK;
}

HRESULT CUI_NodeQuest::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &m_tDesc.ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &m_tDesc.ProjMatrix)))
		return E_FAIL;

	if (m_pTextureCom[TEXTURE_DIFFUSE] != nullptr)
	{
		if (FAILED(m_pTextureCom[TEXTURE_DIFFUSE]->Bind_ShaderResource(m_pShaderCom, "g_Texture")))
			return E_FAIL;
	}

	if (m_pTextureCom[TEXTURE_MASK] != nullptr)
	{
		if (FAILED(m_pTextureCom[TEXTURE_MASK]->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture")))
			return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Set_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

//HRESULT CUI_NodeQuest::Save_Data()
//{
//	Json	json;

	//std::string std = "한글테스트";
	//std::wstring_convert<codecvt_utf8_utf16<int16_t>, int16_t> convert;
	//auto p = reinterpret_cast<const int16_t*>(std.data());
	//json["test"] = convert.to_bytes(p, p + std.size());

	//Json	jString = {
	//	{"name", "John"},
	//	{"what", "fckyou"}
	//};

	/* convert wstring to string */
	//using convert_type = codecvt_utf8<wchar_t>;
	//wstring wstr = L"한글테스트";
	//wstring_convert<convert_type> utf8_conv;
	//json["test"] = utf8_conv.to_bytes(wstr);
	//json.dump();
	/* ~ convert wstring to string */


	//using convert_type = codecvt_utf8<wchar_t>;

	//wstring main = L"타로를 구하세요";
	//wstring_convert<convert_type> utf8_conv;
	//json["mainQuest"]; // = utf8_conv.to_bytes(main);
	//				   //json.dump();

	//Json jSub;
	//wstring sub1 = L"신사를 정화하세요.";
	//wstring sub2 = L"오염된 숲을 정화하세요.";
	//json["sub1"] = utf8_conv.to_bytes(sub1);
	//json.dump();
	//json["sub2"] = utf8_conv.to_bytes(sub2);
	//json.dump();
	//json["mainQuest"].push_back(jSub);

//}

//HRESULT CUI_NodeQuest::Load_Data(wstring fileName)
//{
//	Json	jLoad;
//
//	wstring name = L"../Bin/Data/UI/";
//	name += fileName;
//	name += L"_Property.json";
//	string filePath;
//	filePath.assign(name.begin(), name.end());
//
//	ifstream file(filePath);
//	if (file.fail())
//		return E_FAIL;
//	file >> jLoad;
//	file.close();
//
//
//
//	/* convert string to wstring */
//	//string test;
//	//jLoad["test"].get_to<string>(test);
//	//using convert_type = codecvt_utf8<wchar_t>;
//	//wstring_convert<convert_type> utf8_conv;
//	//wstring wstr = utf8_conv.from_bytes(test);
//	/* ~ convert string to wstring */
//}



CUI_NodeQuest * CUI_NodeQuest::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CUI_NodeQuest*	pInstance = new CUI_NodeQuest(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_NodeQuest");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_NodeQuest::Clone(void * pArg)
{
	CUI_NodeQuest*	pInstance = new CUI_NodeQuest(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_NodeQuestMain");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_NodeQuest::Free()
{
	__super::Free();

	if (m_isCloned)
	{
		delete [] m_szQuest;
		m_szQuest = nullptr;
	}
}
