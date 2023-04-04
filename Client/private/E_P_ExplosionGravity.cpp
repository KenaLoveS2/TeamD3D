#include "stdafx.h"
#include "..\public\E_P_ExplosionGravity.h"
#include "GameInstance.h"

CE_P_ExplosionGravity::CE_P_ExplosionGravity(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEffect_Point_Instancing(pDevice, pContext)
{
}

CE_P_ExplosionGravity::CE_P_ExplosionGravity(const CE_P_ExplosionGravity & rhs)
	: CEffect_Point_Instancing(rhs)
{
}

HRESULT CE_P_ExplosionGravity::Initialize_Prototype(const _tchar * pFilePath)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	if(pFilePath != nullptr)
	{
		if (FAILED(Load_E_Desc(pFilePath)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CE_P_ExplosionGravity::Initialize(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 4.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	m_pVIInstancingBufferCom = CVIBuffer_Point_Instancing::Create(m_pDevice, m_pContext, 200);

	FAILED_CHECK_RETURN(__super::Initialize(&GameObjectDesc), E_FAIL);
	
	m_eEFfectDesc.bActive = false;
	return S_OK;
}

HRESULT CE_P_ExplosionGravity::Late_Initialize(void* pArg)
{
	// Set_Option(CE_P_ExplosionGravity::TYPE_DEFAULT, XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f));
	return S_OK;
}

void CE_P_ExplosionGravity::Tick(_float fTimeDelta)
{
	//if (!lstrcmp(Get_ObjectCloneName(), L"Test"))
 	if (m_eType == TYPE_HEALTHFLOWER)
 		Set_OptionTool();
 	else
		m_fLife += fTimeDelta;

	__super::Tick(fTimeDelta);
	if (m_eEFfectDesc.bActive == false)
	{
		m_fLife = 0.0f;
		return;
	}
	else
		m_pTransformCom->Set_Position(m_vFixPos);

//	m_fLife += fTimeDelta;
	/*m_eType != CE_P_ExplosionGravity::TYPE_DEFAULT && */
	if (m_eType == TYPE_HEALTHFLOWER && m_eEFfectDesc.bActive == true &&  m_pVIInstancingBufferCom->Get_Finish() == true)
		m_eEFfectDesc.bActive = false;
}

void CE_P_ExplosionGravity::Late_Tick(_float fTimeDelta)
{
	if (m_eEFfectDesc.bActive == true)
		__super::Late_Tick(fTimeDelta);
}

HRESULT CE_P_ExplosionGravity::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CE_P_ExplosionGravity::SetUp_ShaderResources()
{
	if (m_pShaderCom == nullptr)
		return E_FAIL;

	return S_OK;
}

HRESULT CE_P_ExplosionGravity::SetUp_Components()
{
	return S_OK;
}

void CE_P_ExplosionGravity::Set_Option(TYPE eType, _vector vSetDir)
{
	CVIBuffer_Point_Instancing::POINTDESC* ePointDesc = m_pVIInstancingBufferCom->Get_PointDesc();
	m_eType = eType;
	_float3 fMin = _float3(-1.f, -1.f, -1.f);
	_float3 fMax = _float3(1.f, 1.f, 1.f);
	_float fTerm = 1.f;

	switch (eType)
	{
	case TYPE::TYPE_DEFAULT:
		/* 위로 터지는거 */
		m_pVIInstancingBufferCom->Set_Position(fMin, fMax);
		ParticleOption_Parabola(ePointDesc, 53.f, XMVectorSet(1.f, 1.f, 1.f, 0.2f), 0.1f,
			fTerm,	_float2(0.2f, 0.2f), false);
		break;

	case TYPE::TYPE_DEAD_MONSTER:
		m_pVIInstancingBufferCom->Set_Position(fMin, fMax);
		ParticleOption_Parabola(ePointDesc, 53.f, XMVectorSet(235.f, 97.f, 140.f, 255.f) / 255.f, 0.1f,
			fTerm, _float2(0.1f, 0.1f), false);
		break;

	case CE_P_ExplosionGravity::TYPE_BOSS_WEAPON:
		m_pVIInstancingBufferCom->Set_Position(fMin, fMax);
		ParticleOption_Parabola(ePointDesc, 53.f, XMVectorSet(255.f, 127.f, 255.f, 255.f) / 255.f, 0.4f,
			fTerm, _float2(0.2f, 0.2f), false);
		break;

	case CE_P_ExplosionGravity::TYPE_BOSS_ATTACK:
		m_pVIInstancingBufferCom->Set_Position(fMin, fMax);
		ParticleOption_Parabola(ePointDesc, 53.f, XMVectorSet(255.f, 127.f, 255.f, 255.f) / 255.f, 0.2f,
			fTerm, _float2(0.2f, 0.2f), true, vSetDir);
		break;

	case CE_P_ExplosionGravity::TYPE_BOSS_PARRY:
		m_pVIInstancingBufferCom->Set_Position(fMin, fMax);
		ParticleOption_Parabola(ePointDesc, 10.f, XMVectorSet(255.f, 127.f, 255.f, 255.f) / 255.f, 0.2f,
			fTerm, _float2(0.2f, 0.2f), false);
		break;

	case CE_P_ExplosionGravity::TYPE_DAMAGE_PULSE:
		m_pVIInstancingBufferCom->Set_Position(fMin, fMax);
		ParticleOption_Parabola(ePointDesc, 53.f, XMVectorSet(1.f, 0.5f, 1.f, 0.2f), 0.2f,
			fTerm, _float2(0.1f, 0.1f), true, vSetDir);
		break;

	case CE_P_ExplosionGravity::TYPE_KENA_ATTACK:
		m_pVIInstancingBufferCom->Set_Position(fMin, fMax);
		ParticleOption_Parabola(ePointDesc, 58.f, XMVectorSet(114.f, 227.f, 255.f, 255.f) / 255.f, 0.1f,
			fTerm, _float2(0.15f, 0.15f), false);
		break;

	case CE_P_ExplosionGravity::TYPE_KENA_ATTACK2:
	{
		m_eEFfectDesc.fFrame[0] = 28.f;
		m_eEFfectDesc.iPassCnt = 3;
		m_eEFfectDesc.vColor = XMVectorSet(1.f, 2.f, 4.f, 0.4f);

		/* Point Instance Option */
		m_pVIInstancingBufferCom->Set_Speeds(0.1f);
		ePointDesc->eShapeType = CVIBuffer_Point_Instancing::POINTDESC::SHAPETYPE::VIBUFFER_GATHER;
		ePointDesc->bSpread = false;

		ePointDesc->fCreateRange = 1.f;
		ePointDesc->fRange = 3.f;
		ePointDesc->fTerm = 1.f;
		ePointDesc->bSetDir = false;

		m_pVIInstancingBufferCom->Set_Position(fMin, fMax);
		m_pVIInstancingBufferCom->Set_PSize(_float2(0.1f, 0.1f));
		break;
	}

	case CE_P_ExplosionGravity::TYPE_HEALTHFLOWER:
		fTerm = 1.5f;
		m_pVIInstancingBufferCom->Set_Position(fMin, fMax);
		ParticleOption_Parabola(ePointDesc, 53.f, XMVectorSet(0.f, 220.f, 255.f, 255.f)/ 255.f, 0.03f,
			fTerm, _float2(0.1f, 0.1f), false);
		break;

	}
}

void CE_P_ExplosionGravity::ParticleOption_Parabola(CVIBuffer_Point_Instancing::POINTDESC* ePointDesc, _float fDiffuseIdx
	, _fvector vColor, _float pSpeed, _float fTerm, _float2 pSize, _bool bSetDir, _fvector vDir)
{
	m_eEFfectDesc.fFrame[0] = fDiffuseIdx;
	m_eEFfectDesc.iPassCnt = 13;
	m_eEFfectDesc.vColor = vColor;

	/* Point Instance Option */
	m_pVIInstancingBufferCom->Set_Speeds(pSpeed);
	ePointDesc->eShapeType = CVIBuffer_Point_Instancing::POINTDESC::SHAPETYPE::VIBUFFER_PARABOLA;

	ePointDesc->fCreateRange = 1.f;
	ePointDesc->fRange = 1.f;
	ePointDesc->fTerm = fTerm;
	ePointDesc->bSetDir = bSetDir;
	if(bSetDir)
		ePointDesc->SetDir = vDir;

	m_pVIInstancingBufferCom->Set_PSize(pSize);
}

void CE_P_ExplosionGravity::UpdateParticle(_float4 vPos, _vector vDir)
{
	m_eEFfectDesc.bActive = true;
	Reset_Type();

	m_vFixPos = vPos;
	m_pTransformCom->Set_Position(vPos);
	m_pVIInstancingBufferCom->Set_Dir(vDir);
}

void CE_P_ExplosionGravity::UpdateColor(_fvector vColor)
{
	m_eEFfectDesc.vColor = vColor;
}

HRESULT CE_P_ExplosionGravity::Reset_Type()
{
	if (nullptr == m_pVIInstancingBufferCom)
		return E_FAIL;

	m_pVIInstancingBufferCom->Reset_Type();
	return S_OK;
}

void CE_P_ExplosionGravity::Set_OptionTool()
{
	ImGui::Begin("ExplosionGravity");

	CVIBuffer_Point_Instancing::POINTDESC* ePointDesc = m_pVIInstancingBufferCom->Get_PointDesc();
	CVIBuffer_Point_Instancing::INSTANCEDATA* eInstanceData = m_pVIInstancingBufferCom->Get_InstanceData();

	if (ImGui::Button("ReCompile"))
		m_pShaderCom->ReCompile();
	ImGui::SameLine();
	if(ImGui::Button("Resize"))
		m_pVIInstancingBufferCom->Set_PSize(_float2(0.1f, 0.1f));
	ImGui::SameLine();
	if (ImGui::Button("ResetTime"))
		m_fLife = 0.0f;
	ImGui::SameLine();
	if (ImGui::Button("Spread"))
		ePointDesc->bSpread = !ePointDesc->bSpread;
	ImGui::SameLine();
	if (ImGui::Button("Dir"))
		m_pVIInstancingBufferCom->SetRandomDir();

	ImGui::InputFloat4("Color", (_float*)&m_eEFfectDesc.vColor);

	ImGui::InputInt("Pass", &m_eEFfectDesc.iPassCnt);
	ImGui::InputFloat4("DiffuseTexture", (_float*)&m_eEFfectDesc.fFrame);

	_float fSpeed = m_pTransformCom->Get_TransformDesc().fSpeedPerSec;
	_float fRotation = m_pTransformCom->Get_TransformDesc().fRotationPerSec;
	ImGui::InputFloat("Speed", &fSpeed);
	ImGui::InputFloat("Rotation", &fRotation);

	static _float fRange = ePointDesc->fRange;
	static _float fTerm = ePointDesc->fTerm;
	static _float fCreateRange = ePointDesc->fCreateRange;
	ImGui::InputFloat("Range", &fRange);
	ImGui::InputFloat("Term", &fTerm);
	ImGui::InputFloat("fCreateRange", &fCreateRange);

	static _int    iType = (_int)ePointDesc->eShapeType;
	static _float3 fMin = _float3(0.0f, 0.0f, 0.0f);
	static _float3 fMax = _float3(0.0f, 0.0f, 0.0f);
	static _double fInstanceSpeed = eInstanceData->pSpeeds;

	ImGui::RadioButton("VIBUFFER_BOX", &iType, 0); ImGui::SameLine();
	ImGui::RadioButton("VIBUFFER_STRIGHT", &iType, 1); ImGui::SameLine();
	ImGui::RadioButton("VIBUFFER_PLANECIRCLE", &iType, 2); ImGui::SameLine();
	ImGui::RadioButton("VIBUFFER_CONE", &iType, 3); ImGui::SameLine();
	ImGui::RadioButton("VIBUFFER_EXPLOSION", &iType, 4);
	ImGui::RadioButton("VIBUFFER_EXPLOSION_GRAVITY", &iType, 5); ImGui::SameLine();
	ImGui::RadioButton("VIBUFFER_PLANECIRCLE_HAZE", &iType, 6); ImGui::SameLine();
	ImGui::RadioButton("VIBUFFER_HAZE", &iType, 7); ImGui::SameLine();
	ImGui::RadioButton("VIBUFFER_GATHER", &iType, 8); ImGui::SameLine();
	ImGui::RadioButton("VIBUFFER_PARABOLA", &iType, 9);
	ImGui::Separator();
	ImGui::InputDouble("InstanceSpeed", &fInstanceSpeed);
	ImGui::Separator();
	ImGui::InputFloat3("Min", (_float*)&fMin);
	ImGui::InputFloat3("Max", (_float*)&fMax);
	ImGui::Separator();

	if (ImGui::Button("Set Type"))
	{
		switch (iType)
		{
		case 0:
			ePointDesc->eShapeType = CVIBuffer_Point_Instancing::POINTDESC::SHAPETYPE::VIBUFFER_BOX;
			break;

		case 1:
			ePointDesc->eShapeType = CVIBuffer_Point_Instancing::POINTDESC::SHAPETYPE::VIBUFFER_STRIGHT;
			break;

		case 2:
			ePointDesc->eShapeType = CVIBuffer_Point_Instancing::POINTDESC::SHAPETYPE::VIBUFFER_PLANECIRCLE;
			break;

		case 3:
			ePointDesc->eShapeType = CVIBuffer_Point_Instancing::POINTDESC::SHAPETYPE::VIBUFFER_CONE;
			break;

		case 4:
			ePointDesc->eShapeType = CVIBuffer_Point_Instancing::POINTDESC::SHAPETYPE::VIBUFFER_EXPLOSION;
			break;

		case 5:
			ePointDesc->eShapeType = CVIBuffer_Point_Instancing::POINTDESC::SHAPETYPE::VIBUFFER_EXPLOSION_GRAVITY;
			break;

		case 6:
			ePointDesc->eShapeType = CVIBuffer_Point_Instancing::POINTDESC::SHAPETYPE::VIBUFFER_PLANECIRCLE_HAZE;
			m_pVIInstancingBufferCom->Set_ShapePosition();
			break;

		case 7:
			ePointDesc->eShapeType = CVIBuffer_Point_Instancing::POINTDESC::SHAPETYPE::VIBUFFER_HAZE;
			break;

		case 8:
			ePointDesc->eShapeType = CVIBuffer_Point_Instancing::POINTDESC::SHAPETYPE::VIBUFFER_GATHER;
			break;

		case 9:
			ePointDesc->eShapeType = CVIBuffer_Point_Instancing::POINTDESC::SHAPETYPE::VIBUFFER_PARABOLA;
			break;

		default:
			break;
		}
	}

	ImGui::SameLine();
	if (ImGui::Button("Set_Option")) {
		m_pVIInstancingBufferCom->Set_Speeds(fInstanceSpeed);
		m_pVIInstancingBufferCom->SetRandomDir();
		m_pVIInstancingBufferCom->Set_Position(fMin, fMax);
		ePointDesc->fRange = fRange;
		ePointDesc->fTerm = fTerm;
		ePointDesc->fCreateRange = fCreateRange;
	}
	ImGui::SameLine();
	ImGui::Checkbox("Active", &m_eEFfectDesc.bActive);

	ImGui::End();
}

CE_P_ExplosionGravity * CE_P_ExplosionGravity::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar * pFilePath)
{
	CE_P_ExplosionGravity * pInstance = new CE_P_ExplosionGravity(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pFilePath)))
	{
		MSG_BOX("CE_P_ExplosionGravity Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CE_P_ExplosionGravity::Clone(void * pArg)
{
	CE_P_ExplosionGravity * pInstance = new CE_P_ExplosionGravity(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CE_P_ExplosionGravity Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CE_P_ExplosionGravity::Free()
{
	__super::Free();
}
