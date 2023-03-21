#include "stdafx.h"
#include "..\public\UI_NodeItemBar.h"
#include "GameInstance.h"

CUI_NodeItemBar::CUI_NodeItemBar(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CUI_Node(pDevice, pContext)
{
}

CUI_NodeItemBar::CUI_NodeItemBar(const CUI_NodeItemBar & rhs)
	:CUI_Node(rhs)
{
}

HRESULT CUI_NodeItemBar::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_NodeItemBar::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
	{
		m_pTransformCom->Set_Scaled(_float3(100.f, 100.f, 1.f));
		XMStoreFloat4x4(&m_matLocal, m_pTransformCom->Get_WorldMatrix());
	}

	if (FAILED(SetUp_Components()))
	{
		MSG_BOX("Failed To SetUp Components");
		return E_FAIL;
	}

	m_bActive = true;
	return S_OK;
}

void CUI_NodeItemBar::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CUI_NodeItemBar::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CUI_NodeItemBar::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	_float4 vPos;
	XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
	_float2 vNewPos = { vPos.x + g_iWinSizeX*0.5f + m_vOffset.x, g_iWinSizeY*0.5f - vPos.y + m_vOffset.y };

	if (nullptr != m_szName)
	{
		CGameInstance::GetInstance()->Render_Font(m_szFont, m_szName,
			vNewPos /* position */,
			0.f, m_vFontSize/* size */,
			XMLoadFloat4(&m_vColor)/* color */);
	}

	return S_OK;
}

HRESULT CUI_NodeItemBar::Save_Data()
{
	Json	json;

	_float fColor = 0.f;
	for (int i = 0; i < 4; ++i)
	{
		fColor = 0.f;
		memcpy(&fColor, (float*)&m_vColor + i, sizeof(_float));
		json["color"].push_back(fColor);
	}

	_smatrix matWorld = m_matLocal; // m_matLocalOriginal;
	_float fValue = 0.f;
	for (int i = 0; i < 16; ++i)
	{
		fValue = 0.f;
		memcpy(&fValue, (float*)&matWorld + i, sizeof(float));
		json["localMatrix"].push_back(fValue);
	}

	json["renderPass"] = m_iOriginalRenderPass;

	_int iIndex;
	if (m_pTextureCom[TEXTURE_DIFFUSE] != nullptr)
		iIndex = m_TextureListIndices[TEXTURE_DIFFUSE];
	else
		iIndex = -1;
	json["DiffuseTextureIndex"] = iIndex;

	if (m_pTextureCom[TEXTURE_MASK] != nullptr)
		iIndex = m_TextureListIndices[TEXTURE_MASK];
	else
		iIndex = -1;
	json["MaskTextureIndex"] = iIndex;

	for (auto e : m_vecEvents)
		e->Save_Data(&json);

	string fileName = "../Bin/Data/UI/Node_HatCartItemBG_Property.json";

	ofstream	file(fileName);
	file << json;
	file.close();

	return S_OK;
}

HRESULT CUI_NodeItemBar::Load_Data(wstring fileName)
{
	Json	jLoad;
	string filePath = "../Bin/Data/UI/Node_HatCartItemBG_Property.json";
	ifstream file(filePath);
	if (file.fail())
		return E_FAIL;
	file >> jLoad;
	file.close();

	jLoad["renderPass"].get_to<_uint>(m_iRenderPass);
	m_iOriginalRenderPass = m_iRenderPass;


	jLoad["DiffuseTextureIndex"].get_to<_int>(m_TextureListIndices[TEXTURE_DIFFUSE]);
	jLoad["MaskTextureIndex"].get_to<_int>(m_TextureListIndices[TEXTURE_MASK]);

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	vector<wstring>* pTags = pGameInstance->Get_UIWString(CUI_Manager::WSTRKEY_TEXTURE_PROTOTAG);
	RELEASE_INSTANCE(CGameInstance);

	if (-1 != m_TextureListIndices[TEXTURE_DIFFUSE])
	{
		if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(),
			(*pTags)[m_TextureListIndices[TEXTURE_DIFFUSE]].c_str(), TEXT("Com_DiffuseTexture"),
			(CComponent**)&m_pTextureCom[0])))
			return S_OK;
	}
	if (-1 != m_TextureListIndices[TEXTURE_MASK])
	{
		if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(),
			(*pTags)[m_TextureListIndices[TEXTURE_MASK]].c_str(), TEXT("Com_MaskTexture"),
			(CComponent**)&m_pTextureCom[1])))
			return S_OK;
	}

	int i = 0;
	_float4x4	matLocal;
	for (float fElement : jLoad["localMatrix"])
		memcpy(((float*)&matLocal) + (i++), &fElement, sizeof(float));

	if (jLoad.contains("color"))
	{
		int i = 0;
		for (float fElement : jLoad["color"])
			memcpy(((float*)&m_vColor) + (i++), &fElement, sizeof(float));
	}

	this->Set_LocalMatrix(matLocal);
	m_matLocalOriginal = matLocal;

	m_vOriginalSettingScale = m_pTransformCom->Get_Scaled();

	return S_OK;
}

_bool CUI_NodeItemBar::MouseOverEvent()
{
	return _bool();
}

void CUI_NodeItemBar::BackToNormal()
{
}

HRESULT CUI_NodeItemBar::SetUp_Components()
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

HRESULT CUI_NodeItemBar::SetUp_ShaderResources()
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
		if (FAILED(m_pTextureCom[TEXTURE_DIFFUSE]->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iTextureIdx)))
			return E_FAIL;
	}

	if (m_pTextureCom[TEXTURE_MASK] != nullptr)
	{
		if (FAILED(m_pTextureCom[TEXTURE_MASK]->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture")))
			return E_FAIL;
	}

	return S_OK;
}

CUI_NodeItemBar * CUI_NodeItemBar::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CUI_NodeItemBar*	pInstance = new CUI_NodeItemBar(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_NodeItemBar");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_NodeItemBar::Clone(void * pArg)
{
	CUI_NodeItemBar*	pInstance = new CUI_NodeItemBar(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_NodeItemBar");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_NodeItemBar::Free()
{
	__super::Free();
}
