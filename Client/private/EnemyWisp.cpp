#include "stdafx.h"
#include "..\public\EnemyWisp.h"
#include "GameInstance.h"
#include "Bone.h"
#include "Effect_Trail.h"

CEnemyWisp::CEnemyWisp(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEffect_Mesh(pDevice, pContext)
{
}

CEnemyWisp::CEnemyWisp(const CEnemyWisp & rhs)
	: CEffect_Mesh(rhs)
{
}

HRESULT CEnemyWisp::Initialize_Prototype()
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

	return S_OK;
}

HRESULT CEnemyWisp::Initialize(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(CGameObject::GAMEOBJECTDESC));

	if (pArg == nullptr)
	{
		GameObjectDesc.TransformDesc.fSpeedPerSec = 7.f;
		GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.f);
	}
	else
		memcpy(&GameObjectDesc, pArg, sizeof(CGameObject::GAMEOBJECTDESC));

	/* 모델을 별도로 설정해 주기 때문에 Desc 일부 변경 해줌 */
	m_eEFfectDesc.eMeshType = CEffect_Base::tagEffectDesc::MESH_END; // Mesh 생성 안함
	m_iTotalDTextureComCnt = 0;	m_iTotalMTextureComCnt = 0;

	FAILED_CHECK_RETURN(__super::Initialize(&GameObjectDesc), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_Components(), E_FAIL);
	m_pModelCom->Set_AnimIndex(0);

	FAILED_CHECK_RETURN(Set_WispTrail(), E_FAIL);
	/* matrix 이상하게 들어감 확인 해봐야함 */
	_matrix matiden = XMMatrixIdentity();
	m_pTransformCom->Set_WorldMatrix(matiden);

	return S_OK;
}

void CEnemyWisp::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	static _bool bPlay = false;

	ImGui::Begin("EnemyWisp Option");
	ImGui::Checkbox("Play", &bPlay);

	ImGui::DragFloat("UV_X", (_float*)&m_fUV.x, 0.01f, 0.f, 10.f);
	ImGui::DragFloat("UV_Y", (_float*)&m_fUV.y, 0.01f, 0.f, 10.f);

	if(bPlay)
	{
		m_pModelCom->Play_Animation(fTimeDelta);

		if (m_pModelCom->Get_AnimationFinish() == true)
			dynamic_cast<CEffect_Trail*>(m_pEffectTrail)->ResetInfo();

		m_pEffectTrail->Tick(fTimeDelta);
		m_pEffectTrail->Set_Active(true);
	}

	if (!bPlay)
		m_pEffectTrail->Set_Active(false);

	if (ImGui::Button("ReCompile"))
		m_pShaderCom->ReCompile();

	static bool alpha_preview = true;
	static bool alpha_half_preview = false;
	static bool drag_and_drop = true;
	static bool options_menu = true;
	static bool hdr = false;

	ImGuiColorEditFlags misc_flags = (hdr ? ImGuiColorEditFlags_HDR : 0) | (drag_and_drop ? 0 : ImGuiColorEditFlags_NoDragDrop) | (alpha_half_preview ? ImGuiColorEditFlags_AlphaPreviewHalf : (alpha_preview ? ImGuiColorEditFlags_AlphaPreview : 0)) | (options_menu ? 0 : ImGuiColorEditFlags_NoOptions);

	static bool   ref_color = false;
	static ImVec4 ref_color_v(1.0f, 1.0f, 1.0f, 1.0f);

	static _float4 vSelectColor = { 1.0f, 1.0f, 1.0f, 1.0f };
	vSelectColor = m_eEFfectDesc.vColor;

	ImGui::ColorPicker4("CurColor##6", (float*)&vSelectColor, ImGuiColorEditFlags_NoInputs | misc_flags, ref_color ? &ref_color_v.x : NULL);
	ImGui::ColorEdit4("Diffuse##5f", (float*)&vSelectColor, ImGuiColorEditFlags_DisplayRGB | misc_flags);
	m_eEFfectDesc.vColor = vSelectColor;

	ImGui::End();
}

void CEnemyWisp::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	/* Trail */
	if (m_pEffectTrail != nullptr)
	{
		CBone*	pWispBonePtr = m_pModelCom->Get_BonePtr("EnemyWisp_Jnt2");
		_matrix SocketMatrix = pWispBonePtr->Get_CombindMatrix() * m_pModelCom->Get_PivotMatrix();
		_matrix matWorldSocket = SocketMatrix * m_pTransformCom->Get_WorldMatrix();

		m_pEffectTrail->Get_TransformCom()->Set_WorldMatrix(matWorldSocket);
		m_pEffectTrail->Late_Tick(fTimeDelta);
	}

	if (m_pRendererCom != nullptr)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CEnemyWisp::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_ShaderResources(), E_FAIL);

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();
	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture");
		m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_SPECULAR, "g_ReamTexture");
		m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_AMBIENT, "g_LineTexture");
		m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_MASK, "g_ShapeTexture");
		m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_ALPHA, "g_SmoothTexture");

		m_pModelCom->Render(m_pShaderCom, i, "g_BoneMatrices", 0);
	}

	return S_OK;
}

HRESULT CEnemyWisp::SetUp_Components()
{
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Shader_VtxEffectAnimModel", L"Com_Shader", (CComponent**)&m_pShaderCom), E_FAIL);

	FAILED_CHECK_RETURN(__super::Add_Component(g_LEVEL, L"Prototype_Component_Model_EnemyWisp", L"Com_Model", (CComponent**)&m_pModelCom, nullptr, this), E_FAIL);

	FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(0, WJTextureType_DIFFUSE, TEXT("../Bin/Resources/Anim/Enemy/EnemyWisp/Texture/Noise_cloudsmed.png")), E_FAIL);
	FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(0, WJTextureType_SPECULAR, TEXT("../Bin/Resources/Anim/Enemy/EnemyWisp/Texture/T_Deadzone_REAM.png")), E_FAIL);
	FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(0, WJTextureType_AMBIENT, TEXT("../Bin/Resources/Anim/Enemy/EnemyWisp/Texture/T_Black_Linear.png")), E_FAIL);
	FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(0, WJTextureType_MASK, TEXT("../Bin/Resources/Anim/Enemy/EnemyWisp/Texture/E_Effect_0.png")), E_FAIL);
	FAILED_CHECK_RETURN(m_pModelCom->SetUp_Material(0, WJTextureType_ALPHA, TEXT("../Bin/Resources/Anim/Enemy/EnemyWisp/Texture/T_GR_Noise_Smooth_A.png")), E_FAIL);

	return S_OK;
}

HRESULT CEnemyWisp::SetUp_ShaderResources()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);

	if (FAILED(m_pShaderCom->Set_RawValue("g_UV", &m_fUV, sizeof(_float2))))
		return E_FAIL;

	return S_OK;
}

HRESULT CEnemyWisp::Set_WispTrail()
{
	/* Set Trail */
	CEffect_Base* pEffectBase = nullptr;
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	pEffectBase = dynamic_cast<CEffect_Base*>(pGameInstance->Clone_GameObject(L"Prototype_GameObject_EnemyWispTrail", L"EnemyWispTrail"));
	NULL_CHECK_RETURN(pEffectBase, E_FAIL);
	pEffectBase->Set_Parent(this);
	m_pEffectTrail = pEffectBase;

	RELEASE_INSTANCE(CGameInstance);
	/* Set Trail */

	return S_OK;
}

CEnemyWisp * CEnemyWisp::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CEnemyWisp * pInstance = new CEnemyWisp(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CEnemyWisp Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CEnemyWisp::Clone(void * pArg)
{
	CEnemyWisp * pInstance = new CEnemyWisp(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CEnemyWisp Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEnemyWisp::Free()
{
	__super::Free();
}
