#include "stdafx.h"
#include "..\public\E_P_ExplosionGravity.h"
#include "GameInstance.h"
#include "E_ShamanLazer.h"

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

	// 200 => 100
	m_pVIInstancingBufferCom = CVIBuffer_Point_Instancing::Create(m_pDevice, m_pContext, 100);

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
	//if (!lstrcmp(Get_ObjectCloneName(), L"Explosion"))
	if (m_eType == TYPE_DEFAULT)
 		Set_OptionTool();
 	else
		m_fLife += fTimeDelta;

	if (m_eEFfectDesc.eTextureRenderType == CEffect_Base::tagEffectDesc::TEX_SPRITE)
		Tick_Sprite(m_fDurationTime, fTimeDelta);

	__super::Tick(fTimeDelta);
	if (m_eEFfectDesc.bActive == false)
	{
		m_fLife = 0.0f;
		return;
	}
	else
		m_pTransformCom->Set_Position(m_vFixPos);

	//m_fLife += fTimeDelta;
	/*m_eType != CE_P_ExplosionGravity::TYPE_DEFAULT && */
	if (m_eType != CE_P_ExplosionGravity::TYPE_DEFAULT && m_eEFfectDesc.bActive == true && m_pVIInstancingBufferCom->Get_Finish() == true)
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

	m_fRandomColor = _float4(rand() % 1, rand() % 1, rand() % 1, rand() % 1) * 1.f;
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_RandomColor", &m_fRandomColor, sizeof(_float4)), E_FAIL);

	return S_OK;
}

HRESULT CE_P_ExplosionGravity::SetUp_Components()
{
	return S_OK;
}

HRESULT CE_P_ExplosionGravity::SetUp_ChangeBuffer()
{
	return S_OK;
}

HRESULT CE_P_ExplosionGravity::Save_Desc(const char* pFileTag)
{
	string strFilePath = "../Bin/Data/Effect/Particle/";
	strFilePath += pFileTag;
	strFilePath += ".json";

	Json	jTag;
	Json	jDesc;

	CVIBuffer_Point_Instancing::POINTDESC* ePointDesc = m_pVIInstancingBufferCom->Get_PointDesc();
	CVIBuffer_Point_Instancing::INSTANCEDATA* eInstanceData = m_pVIInstancingBufferCom->Get_InstanceData();

	jTag["0. FileTag"] = pFileTag;

	jDesc["A. Type"] = ePointDesc->eShapeType;
	jDesc["A-1. TextureRenderType"] = m_eEFfectDesc.eTextureRenderType;
	jDesc["A-1_0. Sprite_Width"] = m_eEFfectDesc.iWidthCnt;
	jDesc["A-1_1. Sprite_Height"] = m_eEFfectDesc.iHeightCnt;
	jDesc["A-1_2. Sprite_DurationeTime"] = m_eEFfectDesc.fTimeDelta;
	jDesc["B. Pass"] = m_eEFfectDesc.iPassCnt;
	jDesc["C. HDRValue"] = m_fHDRValue;

	_float	vColor = 0.f, vScale = 0.f;
	for (_int i = 0; i < 4; ++i)
	{
		vColor = 0.0f;
		memcpy(&vColor, (_float*)&m_eEFfectDesc.vColor + i, sizeof(_float));
		jDesc["D. Color"].push_back(vColor);
	}

	_float fFrame = 0.0f;
	for (_int i = 0; i < MAX_TEXTURECNT; ++i) 
	{
		fFrame = 0.0f;
		memcpy(&fFrame, &m_eEFfectDesc.fFrame[i], sizeof(_float));
		jDesc["E. DiffuseTex"].push_back(fFrame);
	}

	jDesc["F. Spread"] = ePointDesc->bSpread;

	_float fMin = 0.0f, fMax = 0.0f;
	for (_int k = 0; k < 3; k++)
	{
		fMin = 0.0f;
		memcpy(&fMin, (_float*)&ePointDesc->fMin + k, sizeof(_float));
		jDesc["G. Min"].push_back(fMin);
	}
	for (_int k = 0; k < 3; k++)
	{
		fMax = 0.0f;
		memcpy(&fMin, (_float*)&ePointDesc->fMax + k, sizeof(_float));
		jDesc["G. Max"].push_back(fMax);
	}
	jDesc["H. InstanceSpeed"] = eInstanceData->pSpeeds;
	jDesc["I. CreateRange"] = ePointDesc->fCreateRange;
	jDesc["J. Range"] = ePointDesc->fRange;
	jDesc["K. Term"] = ePointDesc->fTerm;
	jDesc["L. PowValue"] = ePointDesc->fPowValue;

	_float fSize = 0.0f;
	for (_int k = 0; k < 2; k++)
	{
		fSize = 0.0f;
		memcpy(&fSize, (_float*)&eInstanceData->fPSize + k, sizeof(_float));
		jDesc["M. Size"].push_back(fSize);
	}

	jTag["1. Data"].push_back(jDesc);
	ofstream	file(strFilePath.c_str());
	file << jTag;
	file.close();

	return S_OK;
}

HRESULT CE_P_ExplosionGravity::Load_Desc(const char* pFilePath)
{
	string strFilePath = "../Bin/Data/Effect/Particle/";
	strFilePath += pFilePath;
	strFilePath += ".json";

	ifstream	file(strFilePath.c_str());

	Json	jTag;
	file >> jTag;
	file.close();

	CVIBuffer_Point_Instancing::POINTDESC* ePointDesc = m_pVIInstancingBufferCom->Get_PointDesc();
	CVIBuffer_Point_Instancing::INSTANCEDATA* eInstanceData = m_pVIInstancingBufferCom->Get_InstanceData();

	for (auto jDesc : jTag["1. Data"])
	{
		_uint iShapeType = 0;
		jDesc["A. Type"].get_to<_uint>(iShapeType);
		memcpy(&ePointDesc->eShapeType, &iShapeType, sizeof(_int));

		_uint iTextureRenderType = 0;
		jDesc["A-1. TextureRenderType"].get_to<_uint>(iTextureRenderType);
		memcpy(&m_eEFfectDesc.eTextureRenderType, &iTextureRenderType, sizeof(_int));

		jDesc["A-1_0. Sprite_Width"].get_to<_int>(m_eEFfectDesc.iWidthCnt);
		jDesc["A-1_1. Sprite_Height"].get_to<_int>(m_eEFfectDesc.iHeightCnt);
		jDesc["A-1_2. Sprite_DurationeTime"].get_to<_float>(m_eEFfectDesc.fTimeDelta);

		jDesc["B. Pass"].get_to<_int>(m_eEFfectDesc.iPassCnt);
		jDesc["C. HDRValue"].get_to<_float>(m_fHDRValue);

		_int iColor = 0;
		for (_float vColor : jDesc["D. Color"])
			memcpy(((_float*)&m_eEFfectDesc.vColor) + (iColor++), &vColor, sizeof(_float));

		_int iDiffuse = 0;
		for (_float fFrame : jDesc["E. DiffuseTex"])
			memcpy(((_float*)&m_eEFfectDesc.fFrame) + (iDiffuse++), &fFrame, sizeof(_float));

		jDesc["F. Spread"].get_to<_bool>(ePointDesc->bSpread);

		_int k = 0;
		for (_float fMin : jDesc["G. Min"])
			memcpy((_float*)&ePointDesc->fMin + (k++), &fMin, sizeof(_float));

		k = 0;
		for (_float fMax : jDesc["G. Max"])
			memcpy((_float*)&ePointDesc->fMax + (k++), &fMax, sizeof(_float));

		jDesc["H. InstanceSpeed"].get_to<_double>(eInstanceData->pSpeeds);
		jDesc["I. CreateRange"].get_to<_float>(ePointDesc->fCreateRange);
		jDesc["J. Range"].get_to<_float>(ePointDesc->fRange);
		jDesc["K. Term"].get_to<_float>(ePointDesc->fTerm);
		jDesc["L. PowValue"].get_to<_float>(ePointDesc->fPowValue);

		k = 0;
		for (_float fSize : jDesc["M. Size"])
			memcpy((_float*)&eInstanceData->fPSize + (k++), &fSize, sizeof(_float));
	}

	switch (ePointDesc->eShapeType)
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
	m_pVIInstancingBufferCom->Set_ShapePosition();

	m_pVIInstancingBufferCom->Set_RandomPSize(eInstanceData->fPSize);
	m_pVIInstancingBufferCom->Set_RandomSpeeds(0.1f, (_float)eInstanceData->pSpeeds);
	m_pVIInstancingBufferCom->SetRandomDir();
	m_pVIInstancingBufferCom->Set_Position(ePointDesc->fMin, ePointDesc->fMax);
	ePointDesc->fRange = ePointDesc->fRange;
	ePointDesc->fPowValue = ePointDesc->fPowValue;
	ePointDesc->fTerm = ePointDesc->fTerm;
	ePointDesc->fCreateRange = ePointDesc->fCreateRange;

	m_eEFfectDesc.iSeparateWidth = m_eEFfectDesc.iWidthCnt;
	m_eEFfectDesc.iSeparateHeight = m_eEFfectDesc.iHeightCnt;

	return S_OK;
}

void CE_P_ExplosionGravity::Set_Option(TYPE eType, _vector vSetDir)
{
	m_eType = eType;
	CVIBuffer_Point_Instancing::POINTDESC* ePointDesc = m_pVIInstancingBufferCom->Get_PointDesc();
	_float3 fMin = _float3(-1.f, -1.f, -1.f);
	_float3 fMax = _float3(1.f, 1.f, 1.f);
	_float fTerm = 1.f;

	switch (eType)
	{
	case TYPE::TYPE_DEFAULT:
		/* 위로 터지는거 */
		m_pVIInstancingBufferCom->Set_Position(fMin, fMax);
		ParticleOption_Parabola(ePointDesc, 53.f, XMVectorSet(1.f, 1.f, 1.f, 0.2f), 0.1f,
			fTerm,	_float2(0.05f, 0.2f), false);
		break;

	case TYPE::TYPE_DEAD_MONSTER:
		m_pVIInstancingBufferCom->Set_Position(fMin, fMax);
		ParticleOption_Parabola(ePointDesc, 53.f, XMVectorSet(235.f, 97.f, 140.f, 255.f) / 255.f, 0.1f,
			fTerm, _float2(0.05f, 0.1f), false);
		break;

	case CE_P_ExplosionGravity::TYPE_BOSS_WEAPON:
		m_pVIInstancingBufferCom->Set_Position(fMin, fMax);
		ParticleOption_Parabola(ePointDesc, 53.f, XMVectorSet(255.f, 127.f, 255.f, 255.f) / 255.f, 0.4f,
			fTerm, _float2(0.05f, 0.2f), false);
		break;

	case CE_P_ExplosionGravity::TYPE_BOSS_ATTACK:
		m_pVIInstancingBufferCom->Set_Position(fMin, fMax);
		ParticleOption_Parabola(ePointDesc, 53.f, XMVectorSet(255.f, 127.f, 255.f, 255.f) / 255.f, 0.2f,
			fTerm, _float2(0.05f, 0.2f), true, vSetDir);
		break;

	case CE_P_ExplosionGravity::TYPE_BOSS_PARRY:
		m_pVIInstancingBufferCom->Set_Position(fMin, fMax);
		ParticleOption_Parabola(ePointDesc, 10.f, XMVectorSet(255.f, 127.f, 255.f, 255.f) / 255.f, 0.2f,
			fTerm, _float2(0.05f, 0.2f), false);
		break;

	case CE_P_ExplosionGravity::TYPE_BOSS_GATHER:
	{
		m_eEFfectDesc.fFrame[0] = 53.f;
		m_eEFfectDesc.iPassCnt = 0;
		m_eEFfectDesc.vColor = XMVectorSet(1.f, 0.2f, 1.f, 0.4f);

		/* Point Instance Option */
		m_pVIInstancingBufferCom->Set_Speeds(0.02f);
		ePointDesc->eShapeType = CVIBuffer_Point_Instancing::POINTDESC::SHAPETYPE::VIBUFFER_GATHER;
		ePointDesc->bSpread = false;

		ePointDesc->fCreateRange = 1.f;
		ePointDesc->fRange = 1.f;
		ePointDesc->fTerm = 1.f;
		ePointDesc->bSetDir = false;

		m_pVIInstancingBufferCom->Set_Position(fMin, fMax);
		m_pVIInstancingBufferCom->Set_RandomPSize(_float2(0.05f, 0.1f));
		break;
	}

	case CE_P_ExplosionGravity::TYPE_DAMAGE_PULSE:
		m_pVIInstancingBufferCom->Set_Position(fMin, fMax);
		ParticleOption_Parabola(ePointDesc, 53.f, XMVectorSet(1.f, 0.5f, 1.f, 0.2f), 0.2f,
			fTerm, _float2(0.05f, 0.1f), true, vSetDir);
		break;

	case CE_P_ExplosionGravity::TYPE_KENA_ATTACK:
		m_pVIInstancingBufferCom->Set_Position(fMin, fMax);
// 		ParticleOption_Parabola(ePointDesc, 58.f, XMVectorSet(114.f, 227.f, 255.f, 255.f) / 255.f, 0.1f,
// 			fTerm, _float2(0.05f, 0.15f), false);
		ParticleOption_Parabola(ePointDesc, 58.f, XMVectorSet(114.f, 227.f, 255.f, 255.f) / 255.f, 0.1f,
			fTerm, _float2(0.05f, 0.15f), true, vSetDir);
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
		m_pVIInstancingBufferCom->Set_RandomPSize(_float2(0.05f, 0.1f));
		break;
	}

	case CE_P_ExplosionGravity::TYPE_HEALTHFLOWER:
		fTerm = 1.5f;
		m_pVIInstancingBufferCom->Set_Position(fMin, fMax);
		ParticleOption_Parabola(ePointDesc, 53.f, XMVectorSet(0.f, 220.f, 255.f, 255.f)/ 255.f, 0.03f,
			fTerm, _float2(0.05f, 0.2f), false);
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

	m_pVIInstancingBufferCom->Set_RandomPSize(pSize);
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
		m_pVIInstancingBufferCom->Set_RandomPSize(_float2(0.1f, 0.1f));
	ImGui::SameLine();
	if (ImGui::Button("ResetTime"))
		m_fLife = 0.0f;
	ImGui::SameLine();
	if (ImGui::Button("Spread"))
		ePointDesc->bSpread = !ePointDesc->bSpread;
	ImGui::SameLine();
	if (ImGui::Button("Dir"))
		m_pVIInstancingBufferCom->SetRandomDir();

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

	ImGui::ColorPicker4("CurColor##4", (float*)&vSelectColor, ImGuiColorEditFlags_NoInputs | misc_flags, ref_color ? &ref_color_v.x : NULL);
	ImGui::ColorEdit4("Diffuse##2f", (float*)&vSelectColor, ImGuiColorEditFlags_DisplayRGB | misc_flags);
	m_eEFfectDesc.vColor = vSelectColor;

	ImGui::InputFloat("HDRValue", &m_fHDRValue);

	ImGui::InputInt("Pass", &m_eEFfectDesc.iPassCnt);
	ImGui::InputFloat4("DiffuseTexture", (_float*)&m_eEFfectDesc.fFrame);

	_float fSpeed = m_pTransformCom->Get_TransformDesc().fSpeedPerSec;
	_float fRotation = m_pTransformCom->Get_TransformDesc().fRotationPerSec;
	ImGui::InputFloat("Speed", &fSpeed);
	ImGui::InputFloat("Rotation", &fRotation);

	static _float fRange = ePointDesc->fRange;
	static _float fTerm = ePointDesc->fTerm;
	static _float fCreateRange = ePointDesc->fCreateRange;
	static _float fPowValue = ePointDesc->fPowValue;
	static _float2 fSize = eInstanceData->fPSize;
	static _int    iType = (_int)ePointDesc->eShapeType;
	static _float3 fMin = _float3(0.0f, 0.0f, 0.0f);
	static _float3 fMax = _float3(0.0f, 0.0f, 0.0f);
	static _double fInstanceSpeed = eInstanceData->pSpeeds;

	ImGui::InputFloat("Range", &fRange);
	ImGui::InputFloat("Term", &fTerm);
	ImGui::InputFloat("fCreateRange", &fCreateRange);
	ImGui::InputFloat("fPowValue", &fPowValue);
	ImGui::InputFloat2("fSize", (_float*)&fSize);

	ImGui::InputInt("TextureRenderType", (_int*)&m_eEFfectDesc.eTextureRenderType);
	ImGui::InputFloat("g_WidthFrame", &m_eEFfectDesc.fWidthFrame);
	ImGui::InputFloat("g_HeightFrame", &m_eEFfectDesc.fHeightFrame);
	ImGui::InputInt("g_SeparateWidth", &m_eEFfectDesc.iSeparateWidth);
	ImGui::InputInt("g_SeparateHeight", &m_eEFfectDesc.iSeparateHeight);
	ImGui::InputFloat("DurationTime", &m_eEFfectDesc.fTimeDelta);

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
		m_pVIInstancingBufferCom->Set_RandomPSize(_float2(fSize.x, fSize.y));
		m_pVIInstancingBufferCom->Set_RandomSpeeds(0.1f, (_float)fInstanceSpeed);
		m_pVIInstancingBufferCom->SetRandomDir();
		m_pVIInstancingBufferCom->Set_Position(fMin, fMax);
		ePointDesc->fRange = fRange;
		ePointDesc->fPowValue = fPowValue;
		ePointDesc->fTerm = fTerm;
		ePointDesc->fCreateRange = fCreateRange;

		m_eEFfectDesc.iWidthCnt = m_eEFfectDesc.iSeparateWidth;
		m_eEFfectDesc.iHeightCnt = m_eEFfectDesc.iSeparateHeight;
	}
	ImGui::SameLine();
	if (ImGui::Button("Reset")) {
		fRange = ePointDesc->fRange;
		fTerm = ePointDesc->fTerm;
		fCreateRange = ePointDesc->fCreateRange;
		fPowValue = ePointDesc->fPowValue;
		fSize = eInstanceData->fPSize;
		iType = (_int)ePointDesc->eShapeType;
		fMin = ePointDesc->fMin;
		fMax = ePointDesc->fMax;
		fInstanceSpeed = eInstanceData->pSpeeds;
		
		m_pVIInstancingBufferCom->Set_RandomPSize(_float2(fSize.x, fSize.y));
		m_pVIInstancingBufferCom->Set_RandomSpeeds(0.1f, (_float)fInstanceSpeed);
		m_pVIInstancingBufferCom->SetRandomDir();
		m_pVIInstancingBufferCom->Set_Position(fMin, fMax);
	}

	ImGui::SameLine();
	ImGui::Checkbox("Active", &m_eEFfectDesc.bActive);

	static	char szSaveFileName[MAX_PATH] = "";
	ImGui::InputTextWithHint("##SaveData", "Input Save Data File Name", szSaveFileName, MAX_PATH); ImGui::SameLine();
	if (ImGui::Button("Save_Particle Desc"))
		Save_Desc(szSaveFileName); 

	static	char szLoadFileName[MAX_PATH] = "";
	ImGui::InputTextWithHint("##LoadData", "Input Load Data File Name", szLoadFileName, MAX_PATH); ImGui::SameLine();
	if (ImGui::Button("Load_Particle Desc"))
		Load_Desc(szLoadFileName);

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
