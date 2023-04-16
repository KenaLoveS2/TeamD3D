#include "stdafx.h"
#include "..\public\UI_NodeEffect.h"
#include "GameInstance.h"
#include "UI_Event_Animation.h"
#include "Json/json.hpp"
#include <fstream>
#include "Effect_Particle_Base.h"

CUI_NodeEffect::CUI_NodeEffect(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CUI_Node(pDevice, pContext)
	, m_pTarget(nullptr)
	, m_eType(TYPE_END)
	, m_fTime(0.f)
	, m_fAlpha(1.f)
	, m_vColor(1.f, 1.f, 1.f, 1.f)
	, m_iParticleIndex(0)
	, m_fParticleTimeAcc(0.f)
{
}

CUI_NodeEffect::CUI_NodeEffect(const CUI_NodeEffect & rhs)
	: CUI_Node(rhs)
	, m_pTarget(nullptr)
	, m_eType(TYPE_END)
	, m_fTime(0.f)
	, m_fAlpha(1.f)
	, m_vColor(1.f, 1.f, 1.f, 1.f)
	, m_iParticleIndex(0)
	, m_fParticleTimeAcc(0.f)
{
}

void CUI_NodeEffect::Start_Effect(CUI * pTarget, _float fX, _float fY)
{
	if (nullptr == pTarget)
		return;

	m_pTarget = pTarget;
	m_bActive = true;

	_float4 vPos = pTarget->Get_LocalMatrix().r[3];
	vPos.x += fX;
	vPos.y += fY;

	this->Set_LocalTranslation(vPos);

	//_float4 vPos = pTarget->Get_TransformCom()->Get_State(CTransform::STATE_TRANSLATION);
	//vPos.x += fX;
	//vPos.y += fY;
	//
	//m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION,vPos);

	if(!m_vecEvents.empty())
		m_vecEvents[0]->Call_Event((_uint)0);

	switch (m_eType)
	{
	case TYPE_SEPERATOR:
		m_fTime = 0.f;

		//m_iParticleIndex = 0;
		m_fParticleTimeAcc = 0.f;
		for (auto& p : m_vecParticles)
			p->DeActivate();
		m_iParticleIndex = 0;

		if (m_pParent != nullptr)
		{
			_float4x4 matWorldParent;
			XMStoreFloat4x4(&matWorldParent, m_pParent->Get_WorldMatrix());

			_matrix matParentTrans = XMMatrixTranslation(matWorldParent._41, matWorldParent._42, matWorldParent._43);

			float fRatioX = matWorldParent._11 / m_matParentInit._11;
			float fRatioY = matWorldParent._22 / m_matParentInit._22;
			_matrix matParentScale = XMMatrixScaling(fRatioX, fRatioY, 1.f);

			_smatrix matWorld = m_matLocal * matParentScale * matParentTrans;
			
			m_vOriginalSettingScale = { matWorld.Right().Length(), matWorld.Up().Length(), matWorld.Forward().Length() };
		}

		break;
	case TYPE_RING:
		m_fTime = 0.f;
		break;
	}
		
}

void CUI_NodeEffect::Change_Scale(_float fData)
{
	m_bActive = true;
	if (m_eType == TYPE_NONEANIM || m_eType == TYPE_RING)
	{
		m_matLocal._11 = m_matLocalOriginal._11 * fData;
		m_matLocal._22 = m_matLocalOriginal._22 * fData;

	}
}

void CUI_NodeEffect::BackToOriginalScale()
{
	m_matLocal = m_matLocalOriginal;
}

HRESULT CUI_NodeEffect::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_NodeEffect::Initialize(void * pArg)
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

	//m_bActive = true;

	switch (m_eType)
	{
	case TYPE_ANIM:
	{
		/* Events */
		/* 이미지가 변경되도록 하는 이벤트 */
		UIDESC* tDesc = (UIDESC*)pArg;
		m_vecEvents.push_back(CUI_Event_Animation::Create(tDesc->fileName, this));
		break;
	}
	case TYPE_SEPERATOR:
	{
		/* Ready Particle */
		for (_uint i = 0; i < MAX_PARTICLES; ++i)
		{
			_tchar* fileName = CUtile::Create_StringAuto(L"Particle_UISeparator");
			wstring tag = L"Particle_UISeparator" + to_wstring(i);
			_tchar* cloneTag = CUtile::Create_StringAuto(tag.c_str());

			CEffect_Particle_Base* pEffect =
				static_cast<CEffect_Particle_Base*>(CGameInstance::GetInstance()->Clone_GameObject(L"Prototype_GameObject_Effect_Particle_Base", cloneTag, fileName));
			if (pEffect == nullptr)
			{
				MSG_BOX("failed to create effect : bossHP");
				return E_FAIL;
			}
			m_vecParticles.push_back(pEffect);
		}
	}
	break;
	}


	return S_OK;
}

void CUI_NodeEffect::Tick(_float fTimeDelta)
{
	if (!m_bActive)
		return;

	switch (m_eType)
	{
	case TYPE_SEPERATOR:
	{
		m_fTime += 4.f * fTimeDelta;
		if (m_fTime > 1.f)
			m_fTime = 1.000001f;
	}
		break;
	case TYPE_ANIM:
		if (static_cast<CUI_Event_Animation*>(m_vecEvents[0])->Is_Finished())
			m_bActive = false;
		break;
	case TYPE_RING:
		m_fTime += fTimeDelta;
		m_pTransformCom->Rotation(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(-m_fTime));
		break;
	default:
		break;
	}

	__super::Tick(fTimeDelta);

	for (auto& p : m_vecParticles)
		p->Tick(fTimeDelta);

}

void CUI_NodeEffect::Late_Tick(_float fTimeDelta)
{
	if (!m_bActive)
		return;

	__super::Late_Tick(fTimeDelta);

		/* Test */
	if (m_eType == TYPE_RING)
	{
		m_fTime += 10.f* fTimeDelta;
		m_pTransformCom->Rotation(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(-m_fTime));
	}
	else if (m_eType == TYPE_SEPERATOR)
	{
		m_fParticleTimeAcc += fTimeDelta;
		if (m_fParticleTimeAcc > 0.01f && m_fTime <= 1.f)
		{
			//_float	fXPos = (m_matLocal._41 - m_matLocal._11 * 0.5f) /* Start position */
			//	+ m_matLocal._11 * m_fTime; /* Last Position */
			//_float4	vPos = { fXPos, m_matLocal._42, 0.f, 1.f };

			_float4 vPos = m_pTransformCom->Get_Position();
			_float3 vScale = m_pTransformCom->Get_Scaled();
			_float	fXPos = (vPos.x - vScale.x * 0.5f /* Start position */
				+ vScale.x * m_fTime); /* Last Position */
			_float4	vResult = { fXPos, vPos.y, 0.f, 1.f };

			m_vecParticles[m_iParticleIndex]->Activate(vResult);

			m_iParticleIndex = (m_iParticleIndex + 1) % MAX_PARTICLES;
			m_fParticleTimeAcc = 0.f;
		}
	}


	for (auto& p : m_vecParticles)
		p->Late_Tick(fTimeDelta);

	/* think it's old one but keep it */
	/*if (m_pParent != nullptr)
	{
		_float4x4 matWorldParent;
		XMStoreFloat4x4(&matWorldParent, m_pParent->Get_WorldMatrix());

		_matrix matParentTrans = XMMatrixTranslation(matWorldParent._41, matWorldParent._42, matWorldParent._43);

		float fRatioX = matWorldParent._11 / m_matParentInit._11;
		float fRatioY = matWorldParent._22 / m_matParentInit._22;
		_matrix matParentScale = XMMatrixScaling(fRatioX, fRatioY, 1.f);

		_matrix matWorld = m_matLocal*matParentScale*matParentTrans;
		m_pTransformCom->Set_WorldMatrix(matWorld);
	}

	for (auto e : m_vecEvents)
		e->Late_Tick(fTimeDelta);

	if (nullptr != m_pRendererCom && m_bActive)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
*/
}

HRESULT CUI_NodeEffect::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

void CUI_NodeEffect::Imgui_RenderProperty()
{
	ImGui::Separator();
	ImGui::Text("Effect Specific Property");
	ImGui::Text("0TYPE_NONANIM, 1TYPE_ANIM, 2TYPE_SEPERATOR, 3TYPE_RING, 4TYPE_ALPHA");

	static int eType;
	eType = m_eType;
	if (ImGui::InputInt("EffectType", &eType))
	{
		m_eType = (TYPE)eType;
		m_bActive = true;
	}

	ImGui::InputFloat("Effect Alpha", &m_fAlpha);

	static float vColor[3];
	vColor[0] = m_vColor.x;
	vColor[1] = m_vColor.y;
	vColor[2] = m_vColor.z;
	if (ImGui::InputFloat3("Effect Color", vColor))
	{
		m_vColor.x = vColor[0];
		m_vColor.y = vColor[1];
		m_vColor.z = vColor[2];
	}

	if (m_eType == TYPE_SEPERATOR)
	{
		if (ImGui::Button("Particle Refresh"))
		{
			for (auto& pEffect : m_vecParticles)
				Safe_Release(pEffect);
			m_vecParticles.clear();

			/* Ready Particle */
			for (_uint i = 0; i < MAX_PARTICLES; ++i)
			{
				_tchar* fileName = CUtile::Create_StringAuto(L"Particle_UISeparator");
				wstring tag = L"Particle_UISeparator" + to_wstring(i);
				_tchar* cloneTag = CUtile::Create_StringAuto(tag.c_str());

				CEffect_Particle_Base* pEffect =
					static_cast<CEffect_Particle_Base*>(CGameInstance::GetInstance()->Clone_GameObject(L"Prototype_GameObject_Effect_Particle_Base", cloneTag, fileName));
				if (pEffect == nullptr)
					MSG_BOX("failed to create effect : bossHP");
				m_vecParticles.push_back(pEffect);
			}
		}
	}


	__super::Imgui_RenderProperty();
}

HRESULT CUI_NodeEffect::Save_Data()
{
	Json	json;

	json["Alpha"] = m_fAlpha;
	
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

	_uint eType = m_eType;
	json["AnimType"] = eType;

	for (auto e : m_vecEvents)
		e->Save_Data(&json);


	wstring filePath = L"../Bin/Data/UI/";
	wstring name = Get_ObjectCloneName();
	_uint i = 0;
	for (auto c : name)
	{
		if (c == L'_')
			i++;

		if (i > 1)
			break;
		else
			filePath += c;
	}

	//filePath += this->Get_ObjectCloneName();
	filePath += L"_Property.json";

	string fileName = CUtile::wstring_to_utf8(filePath);
	//fileName = fileName.assign(filePath.begin(), filePath.end());

	ofstream	file(fileName);
	file << json;
	file.close();

	return S_OK;
}

HRESULT CUI_NodeEffect::Load_Data(wstring fileName)
{
	Json	jLoad;

	wstring name = L"../Bin/Data/UI/";
	//name += fileName;

	_uint cnt = 0;
	for (auto c : fileName)
	{
		if (c == L'_')
			cnt++;

		if (cnt > 1)
			break;
		else
			name += c;
	}

	name += L"_Property.json";
	string filePath = CUtile::wstring_to_utf8(name);
	//filePath.assign(name.begin(), name.end());

	ifstream file(filePath);
	if (file.fail())
		return E_FAIL;
	file >> jLoad;
	file.close();

	jLoad["renderPass"].get_to<_uint>(m_iRenderPass);
	m_iOriginalRenderPass = m_iRenderPass;

	_uint eType;
	jLoad["AnimType"].get_to<_uint>(eType);
	m_eType = (TYPE)eType;

	jLoad["DiffuseTextureIndex"].get_to<_int>(m_TextureListIndices[TEXTURE_DIFFUSE]);
	jLoad["MaskTextureIndex"].get_to<_int>(m_TextureListIndices[TEXTURE_MASK]);

	if (jLoad.contains("Alpha"))
	{
		jLoad["Alpha"].get_to<_float>(m_fAlpha);
	}

	vector<wstring>* pTags = CGameInstance::GetInstance()->Get_UIWString(CUI_Manager::WSTRKEY_TEXTURE_PROTOTAG);

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

HRESULT CUI_NodeEffect::SetUp_Components()
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

HRESULT CUI_NodeEffect::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	CUI::SetUp_ShaderResources(); /* Events Resourece Setting */

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

	switch (m_eType)
	{
	case TYPE_SEPERATOR:
		if (FAILED(m_pShaderCom->Set_RawValue("g_Time", &m_fTime, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float))))
			return E_FAIL;
		break;
	case TYPE_ALPHA:
		if (FAILED(m_pShaderCom->Set_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float))))
			return E_FAIL;
		break;
	}

	if (m_iRenderPass == 22)
	{
		if (FAILED(m_pShaderCom->Set_RawValue("g_vColor", &m_vColor, sizeof(_float4))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float))))
			return E_FAIL;

	}

	return S_OK;
}

CUI_NodeEffect * CUI_NodeEffect::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CUI_NodeEffect*	pInstance = new CUI_NodeEffect(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create : CUI_NodeEffect");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_NodeEffect::Clone(void * pArg)
{
	CUI_NodeEffect*	pInstance = new CUI_NodeEffect(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Clone : CUI_NodeEffect");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_NodeEffect::Free()
{
	__super::Free();

	for (auto& p : m_vecParticles)
		Safe_Release(p);
	m_vecParticles.clear();
}
