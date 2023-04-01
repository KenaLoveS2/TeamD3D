#include "stdafx.h"
#include "..\public\UI_CanvasItemBar.h"
#include "GameInstance.h"
#include "UI_NodeCrystal.h"
#include "UI_NodeItemBox.h"

CUI_CanvasItemBar::CUI_CanvasItemBar(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CUI_Canvas(pDevice, pContext)
	, m_fCorrectY(0.f)
	, m_szName(nullptr)
{
}

CUI_CanvasItemBar::CUI_CanvasItemBar(const CUI_CanvasItemBar & rhs)
	: CUI_Canvas(rhs)
	, m_fCorrectY(0.f)
	, m_szName(nullptr)
{
}

HRESULT CUI_CanvasItemBar::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_CanvasItemBar::Initialize(void * pArg)
{
	CUI::Initialize(pArg);

	/* Get a Texture Size, and Make an Initial Matrix */
	XMStoreFloat4x4(&m_matInit, XMMatrixScaling(1212.f, 236.f, 1.f));

	if (pArg == nullptr)
		m_pTransformCom->Set_Scaled({ 1212.f, 236.f, 1.f });
	else
	{
		m_tItemDesc = *(ITEMDESC*)(pArg);

		/* 여기 수정해서 아이템 바의 텍스처가 바뀌어야 함. */
		Load_Data(m_tItemDesc.fileName);

		_float3 vScale = m_pTransformCom->Get_Scaled();
		m_fCorrectY = m_tItemDesc.iTextureIndex*1.1f * vScale.y;
		_float4 vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
		vPos.y -= m_fCorrectY;
		m_pTransformCom->Set_Position(vPos);

		m_szName = CUtile::Create_StringAuto(m_tItemDesc.wstrName.c_str());

		/* Own Matrix */
		m_matInit = m_pTransformCom->Get_WorldMatrixFloat4x4();

		m_pParent = m_tItemDesc.pParent;
		_float4 vParent = m_pParent->Get_TransformCom()->Get_State(CTransform::STATE_TRANSLATION);
		vParent.x += m_matInit._41;
		vParent.y += m_matInit._42;
		m_pTransformCom->Set_Position(vParent);
	}


	if (FAILED(SetUp_Components()))
	{
		MSG_BOX("Failed To SetUp Components : CanvasUpgrade");
		return E_FAIL;
	}

	if (FAILED(Ready_Nodes()))
	{
		MSG_BOX("Failed To Ready Nodes : CanvasUpgrade");
		return E_FAIL;
	}

	m_bActive = true;

	return S_OK;
}

void CUI_CanvasItemBar::Tick(_float fTimeDelta)
{
	if (!m_bBindFinished)
	{
		if (FAILED(Bind()))
			return;
	}

	if (!m_bActive)
		return;

	_float4 vPos = m_pParent->Get_TransformCom()->Get_State(CTransform::STATE_TRANSLATION);
	vPos.x += m_matInit._41;
	vPos.y += m_matInit._42;
	//vPos.z = 0.9f;
	m_pTransformCom->Set_Position(vPos);


	__super::Tick(fTimeDelta);
}

void CUI_CanvasItemBar::Late_Tick(_float fTimeDelta)
{
	if (!m_bActive)
		return;

	__super::Late_Tick(fTimeDelta);
}

HRESULT CUI_CanvasItemBar::Render()
{
	__super::Render();

	_float4 vPos;
	XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
	_float2 vNewPos = { vPos.x + g_iWinSizeX*0.5f - 120.f, g_iWinSizeY*0.5f - vPos.y - 15.f};

	if (nullptr != m_szName)
	{
		CGameInstance::GetInstance()->Render_Font(TEXT("Font_Jangmi0"), m_szName,
			vNewPos /* position */,
			0.f, _float2(0.6f, 0.6f)/* size */,
			XMVectorSet(0.f, 0.f, 0.f, 1.f)/* color */);
	}

	return S_OK;
}

HRESULT CUI_CanvasItemBar::Bind()
{
	m_bBindFinished = true;
	return S_OK;
}

HRESULT CUI_CanvasItemBar::Ready_Nodes()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	/* Box */
	{
		CUI* pUI = nullptr;
		CUI::UIDESC tDesc;

		string strCloneTag = "Node_ItemBox";
		_tchar* wstrCloneTag = CUtile::StringToWideChar(strCloneTag);
		tDesc.fileName = wstrCloneTag;
		pUI = static_cast<CUI*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_UI_Node_ItemBox", wstrCloneTag, &tDesc));
		if (FAILED(Add_Node(pUI)))
			return E_FAIL;
		m_vecNodeCloneTag.push_back(strCloneTag);
		pGameInstance->Add_String(wstrCloneTag);
	}

	/* Hat */
	{
		CUI* pUI = nullptr;
		CUI::UIDESC tDesc;

		string strCloneTag = "Node_ItemHat";
		_tchar* wstrCloneTag = CUtile::StringToWideChar(strCloneTag);
		tDesc.fileName = wstrCloneTag;
		pUI = static_cast<CUI*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_UI_Node_ItemHat", wstrCloneTag, &tDesc));
		if (FAILED(Add_Node(pUI)))
			return E_FAIL;
		m_vecNodeCloneTag.push_back(strCloneTag);
		pGameInstance->Add_String(wstrCloneTag);
		pUI->Set_TextureIndex(m_tItemDesc.iTextureIndex);
	}

	/* Crystal */
	{
		CUI* pUI = nullptr;
		CUI::UIDESC tDesc;

		string strCloneTag = "Node_HatPrice";
		_tchar* wstrCloneTag = CUtile::StringToWideChar(strCloneTag);
		tDesc.fileName = wstrCloneTag;
		pUI = static_cast<CUI*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_UI_Node_Crystal", wstrCloneTag, &tDesc));
		if (FAILED(Add_Node(pUI)))
			return E_FAIL;
		m_vecNodeCloneTag.push_back(strCloneTag);
		pGameInstance->Add_String(wstrCloneTag);
		static_cast<CUI_NodeCrystal*>(pUI)->Set_Crystal(m_tItemDesc.iPrice);
		static_cast<CUI_NodeCrystal*>(pUI)->Set_Font(L"Font_Jangmi0", { 0.f, 0.f, 0.f, 1.f }, { 0.6f, 0.6f }, { 20.f, -10.f });

	}

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CUI_CanvasItemBar::SetUp_Components()
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

HRESULT CUI_CanvasItemBar::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CUI::SetUp_ShaderResources();

	_matrix matWorld = m_pTransformCom->Get_WorldMatrix();
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

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CUI_CanvasItemBar::Save_Data()
{
	MSG_BOX("Not Allowed. Change the data by file.");

	return S_OK;


	Json	json;

	_smatrix matWorld = m_pTransformCom->Get_WorldMatrix();
	_float fValue = 0.f;
	for (int i = 0; i < 16; ++i)
	{
		fValue = 0.f;
		memcpy(&fValue, (float*)&matWorld + i, sizeof(float));
		json["worldMatrix"].push_back(fValue);
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


	string fileName = "../Bin/Data/UI/Canvas_ItemBar_Property.json";
	ofstream	file(fileName);
	file << json;
	file.close();

	for (auto node : m_vecNode)
		node->Save_Data();

	MSG_BOX("Save Complete");

	return S_OK;
}
HRESULT CUI_CanvasItemBar::Load_Data(wstring fileName)
{
	Json	jLoad;

	wstring name = L"../Bin/Data/UI/";
	name += fileName;
	name += L"_Property.json";
	string filePath;
	filePath.assign(name.begin(), name.end());

	ifstream file(filePath);
	if (file.fail())
		return E_FAIL;
	file >> jLoad;
	file.close();

	jLoad["renderPass"].get_to<_uint>(m_iRenderPass);
	m_iOriginalRenderPass = m_iRenderPass;


	switch (m_tItemDesc.iTextureIndex % 4)
	{
	case 0:
		if (FAILED(Add_Component(CGameInstance::Get_StaticLevelIndex(),
			L"Prototype_Component_Texture_ItemBG1", m_TextureComTag[TEXTURE_DIFFUSE].c_str(),
			(CComponent**)&m_pTextureCom[0])))
			return S_OK;
		break;
	case 1:
		if (FAILED(Add_Component(CGameInstance::Get_StaticLevelIndex(),
			L"Prototype_Component_Texture_ItemBG2", m_TextureComTag[TEXTURE_DIFFUSE].c_str(),
			(CComponent**)&m_pTextureCom[0])))
			return S_OK;
		break;
	case 2:
		if (FAILED(Add_Component(CGameInstance::Get_StaticLevelIndex(),
			L"Prototype_Component_Texture_ItemBG3", m_TextureComTag[TEXTURE_DIFFUSE].c_str(),
			(CComponent**)&m_pTextureCom[0])))
			return S_OK;
		break;
	case 3:
		if (FAILED(Add_Component(CGameInstance::Get_StaticLevelIndex(),
			L"Prototype_Component_Texture_ItemBG4", m_TextureComTag[TEXTURE_DIFFUSE].c_str(),
			(CComponent**)&m_pTextureCom[0])))
			return S_OK;
		break;
	}


	int i = 0;
	_float4x4	matWorld;
	for (float fElement : jLoad["worldMatrix"])
		memcpy(((float*)&matWorld) + (i++), &fElement, sizeof(float));
	m_pTransformCom->Set_WorldMatrix_float4x4(matWorld);


	m_vOriginalSettingScale = m_pTransformCom->Get_Scaled();

	return S_OK;
}
void CUI_CanvasItemBar::Clicked()
{
	m_iTextureIdx = 1;
	m_vecNode[UI_BOX]->Set_TextureIndex(1);
	_float3 vScale = m_vOriginalSettingScale;
	vScale.x *= 1.05f;
	vScale.y *= 1.05f;
	m_pTransformCom->Set_Scaled(vScale);

	
}
void CUI_CanvasItemBar::BackToNormal()
{
	m_iTextureIdx = 0;
	m_vecNode[UI_BOX]->Set_TextureIndex(0);
	m_pTransformCom->Set_Scaled(m_vOriginalSettingScale);
}
void CUI_CanvasItemBar::Buy(_int iCount)
{
	static_cast<CUI_NodeItemBox*>(m_vecNode[UI_BOX])->Set_String(iCount);
}

CUI_CanvasItemBar * CUI_CanvasItemBar::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CUI_CanvasItemBar*	pInstance = new CUI_CanvasItemBar(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_CanvasItemBar");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_CanvasItemBar::Clone(void * pArg)
{
	CUI_CanvasItemBar*	pInstance = new CUI_CanvasItemBar(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_CanvasItemBar");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_CanvasItemBar::Free()
{
	__super::Free();

}
