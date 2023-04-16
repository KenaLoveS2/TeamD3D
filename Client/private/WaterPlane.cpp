#include "stdafx.h"
#include "..\public\WaterPlane.h"
#include "GameInstance.h"
#include "Kena.h"

#define SND_WATER_1 L"Water_Lake_1.ogg"
#define SND_WATER_2 L"Water_Lake_Rocks_1.ogg"

CWaterPlane::CWaterPlane(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CEnviromentObj(pDevice, pContext)
{
}

CWaterPlane::CWaterPlane(const CWaterPlane& rhs)
	: CEnviromentObj(rhs)
{
}

HRESULT CWaterPlane::Initialize_Prototype()
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);
	return S_OK;
}

HRESULT CWaterPlane::Initialize(void* pArg)
{

	FAILED_CHECK_RETURN(__super::Initialize(pArg), E_FAIL);

	m_EnviromentDesc.ObjectDesc.TransformDesc.fSpeedPerSec = 4.f;
	m_EnviromentDesc.ObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.f);

	FAILED_CHECK_RETURN(SetUp_Components(), E_FAIL);
	m_pTransformCom->Set_Position(_float4(15.f, 0.f, 15.f, 1.f));
	return S_OK;
}

HRESULT CWaterPlane::Late_Initialize(void* pArg)
{
	_float3 vPos;
	XMStoreFloat3(&vPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));

	_float	fMinX, fMaxX, fMinY, fMaxY, fMinZ, fMaxZ;
	m_pModelCom->Calc_MinMax(&fMinX, &fMaxX, &fMinY, &fMaxY, &fMinZ, &fMaxZ);

	_float3	vSize = _float3(fMaxX - fMinX, fMaxY - fMinY, fMaxZ - fMinZ);
	vSize *= 0.5f;
	_float3 vScale = m_pTransformCom->Get_Scaled();

	vSize.x *= vScale.x;
	vSize.y *= vScale.y;
	vSize.z *= vScale.z;

	CPhysX_Manager* pPhysX = CPhysX_Manager::GetInstance();
	CPhysX_Manager::PX_BOX_DESC BoxDesc;
	BoxDesc.pActortag = m_szCloneObjectTag;
	BoxDesc.eType = BOX_STATIC;
	BoxDesc.vPos = vPos;
	BoxDesc.vSize = vSize;
	BoxDesc.vRotationAxis = _float3(0.f, 0.f, 0.f);
	BoxDesc.fDegree = 0.f;
	BoxDesc.isGravity = false;
	BoxDesc.eFilterType = PX_FILTER_TYPE::FILTER_DEFULAT;
	BoxDesc.vVelocity = _float3(0.f, 0.f, 0.f);
	BoxDesc.fDensity = 0.2f;
	BoxDesc.fMass = 150.f;
	BoxDesc.fLinearDamping = 10.f;
	BoxDesc.fAngularDamping = 5.f;
	BoxDesc.bCCD = false;
	BoxDesc.fDynamicFriction = 0.5f;
	BoxDesc.fStaticFriction = 0.5f;
	BoxDesc.fRestitution = 0.1f;
	BoxDesc.isTrigger = true;

	pPhysX->Create_Box(BoxDesc, Create_PxUserData(this, false, COL_WATER));

	_smatrix	matPivot = XMMatrixIdentity();
	m_pTransformCom->Connect_PxActor_Static(m_szCloneObjectTag);
	m_pTransformCom->Set_WorldMatrix(m_pTransformCom->Get_WorldMatrix());

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance)
	m_pKena = (CKena*)pGameInstance->Get_GameObjectPtr(g_LEVEL, TEXT("Layer_Player"), TEXT("Kena"));
	RELEASE_INSTANCE(CGameInstance)

	if (m_pKena == nullptr)
		return E_FAIL;

	if (g_LEVEL != LEVEL_GIMMICK)
	{
		Setting_Sound();
		CGameInstance::GetInstance()->Play_Sound(m_szCopySoundKey_Water_1, 1.f, true);
	}

	return S_OK;
}

void CWaterPlane::Tick(_float fTimeDelta)
{
	CGameObject::Tick(fTimeDelta);
	m_fTimeDelta += fTimeDelta;
}

void CWaterPlane::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	/*NonCulling*/
	if (m_pRendererCom && m_bRenderActive)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_EFFECT, this);
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
	}
}

HRESULT CWaterPlane::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_ShaderResources(), E_FAIL);

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		FAILED_CHECK_RETURN(m_pTextureCom[NORMAL]->Bind_ShaderResource(m_pShaderCom, "g_NormalTexture", m_iTexNum), E_FAIL);
		FAILED_CHECK_RETURN(m_pTextureCom[NOISE]->Bind_ShaderResource(m_pShaderCom, "g_NoiseTexture"), E_FAIL);
		FAILED_CHECK_RETURN(m_pTextureCom[MASK]->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture", m_iMaskTexNum), E_FAIL);
		FAILED_CHECK_RETURN(m_pShaderCom->Set_ShaderResourceView("g_DepthTexture", CGameInstance::GetInstance()->Get_DepthTargetSRV()), E_FAIL);
		FAILED_CHECK_RETURN(m_pModelCom->Render(m_pShaderCom, i, nullptr), E_FAIL);
	}

	return S_OK;
}

void CWaterPlane::Imgui_RenderProperty()
{
	CGameObject::Imgui_RenderProperty();

	if(ImGui::Button("Recompile"))
	m_pShaderCom->ReCompile();
}

void CWaterPlane::ImGui_ShaderValueProperty()
{
	CGameObject::ImGui_ShaderValueProperty();
}

void CWaterPlane::ImGui_PhysXValueProperty()
{
	CGameObject::ImGui_PhysXValueProperty();
}

HRESULT CWaterPlane::SetUp_Components()
{
	FAILED_CHECK_RETURN(__super::Add_Component(g_LEVEL_FOR_COMPONENT, L"Prototype_Component_Model_WaterPlane", L"Com_Model", (CComponent**)&m_pModelCom, nullptr, this), E_FAIL);
	FAILED_CHECK_RETURN(__super::Add_Component(g_LEVEL_FOR_COMPONENT, L"Prototype_GameObject_WaterNormalTexture", L"Com_Texture", (CComponent**)&m_pTextureCom[NORMAL], nullptr, this), E_FAIL);
	FAILED_CHECK_RETURN(__super::Add_Component(g_LEVEL_FOR_COMPONENT, L"Prototype_GameObject_WaterNoiseTexture", L"Com_Texture2", (CComponent**)&m_pTextureCom[NOISE], nullptr, this), E_FAIL);
	FAILED_CHECK_RETURN(__super::Add_Component(g_LEVEL_FOR_COMPONENT, L"Prototype_GameObject_WaterMaskTexture", L"Com_Texture3", (CComponent**)&m_pTextureCom[MASK], nullptr, this), E_FAIL);
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Shader_Water", L"Com_Shader", (CComponent**)&m_pShaderCom), E_FAIL);
	return S_OK;
}

HRESULT CWaterPlane::SetUp_ShaderResources()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);
	FAILED_CHECK_RETURN(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ViewMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ProjMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_vCamPosition", &CGameInstance::GetInstance()->Get_CamPosition(), sizeof(_float4)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_fTime", &m_fTimeDelta, sizeof(_float)), E_FAIL);
	return S_OK;
}

void CWaterPlane::Setting_Sound()
{
	CGameInstance::GetInstance()->Copy_Sound(SND_WATER_1, m_szCopySoundKey_Water_1);
	CGameInstance::GetInstance()->Copy_Sound(SND_WATER_2, m_szCopySoundKey_Water_2);

	CSound::SOUND_DESC desc;
	ZeroMemory(&desc, sizeof(CSound::SOUND_DESC));
	desc.bIs3D = true;
	desc.fRange = 150.f;
	desc.pStartTransform = m_pTransformCom;
	desc.pTargetTransform = m_pKena->Get_TransformCom();
	CGameInstance::GetInstance()->Set_SoundDesc(m_szCopySoundKey_Water_1, desc);
	CGameInstance::GetInstance()->Set_SoundDesc(m_szCopySoundKey_Water_2, desc);
}

CWaterPlane* CWaterPlane::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CWaterPlane*	pInstance = new CWaterPlane(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CWaterPlane");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CWaterPlane::Clone(void* pArg)
{
	CWaterPlane*	pInstance = new CWaterPlane(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CWaterPlane");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWaterPlane::Free()
{
	__super::Free();
	Safe_Release(m_pModelCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	for(_uint i = 0; i < (_uint)TYPE_END; ++i)
		Safe_Release(m_pTextureCom[i]);
}
