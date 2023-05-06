#include "stdafx.h"
#include "WorldTrigger.h"
#include "GameInstance.h"
#include "PostFX.h"

CWorldTrigger::CWorldTrigger(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CGameObject(pDevice, pContext)
{
}

CWorldTrigger::CWorldTrigger(const CWorldTrigger& rhs)
	:CGameObject(rhs)
{
}

HRESULT CWorldTrigger::Initialize_Prototype()
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);
	return S_OK;
}

HRESULT CWorldTrigger::Initialize(void* pArg)
{
	CGameObject::GAMEOBJECTDESC desc;
	ZeroMemory(&desc, sizeof(CGameObject::GAMEOBJECTDESC));
	desc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.f);
	desc.TransformDesc.fSpeedPerSec = 4.f;
	FAILED_CHECK_RETURN(__super::Initialize(&desc), E_FAIL);
	m_pGameInstance = CGameInstance::GetInstance();

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CWorldTrigger::Late_Initialize(void* pArg)
{
	_float3 vPos;
	XMStoreFloat3(&vPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
	_float3 vSize = _float3(5.f, 5.f, 0.5f);
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
	pPhysX->Create_Box(BoxDesc, Create_PxUserData(this, false, COL_UI_TRIGGER));
	m_pTransformCom->Connect_PxActor_Static(m_szCloneObjectTag);
	Load();
	_smatrix mat;
	m_vecWorldMatrix.push_back(mat);
	return S_OK;
}

void CWorldTrigger::Tick(_float fTimeDelta)
{
	CGameObject::Tick(fTimeDelta);

	m_iVectorSize = static_cast<_uint>(m_vecWorldMatrix.size());
	if (!m_vecWorldMatrix.empty() && m_nMatNum < m_iVectorSize)
		m_pTransformCom->Set_WorldMatrix_float4x4(m_vecWorldMatrix[m_nMatNum]);

	m_pTransformCom->Tick(fTimeDelta);

	if(CPostFX::GetInstance()->Get_Capture())
	{
		CGameInstance::GetInstance()->Set_SingleLayer(g_LEVEL, L"Layer_Canvas");
		m_pRendererCom->Set_CaptureMode(true);
	}
}

void CWorldTrigger::Late_Tick(_float fTimeDelta)
{
	CGameObject::Late_Tick(fTimeDelta);
}

void CWorldTrigger::Imgui_RenderProperty()
{
	CGameObject::Imgui_RenderProperty();

	m_iVectorSize = static_cast<_uint>(m_vecWorldMatrix.size());

	if (ImGui::Button("Push_WorldPos"))
		m_vecWorldMatrix.push_back(m_pTransformCom->Get_WorldMatrixFloat4x4());

	char** ppKeyFrameNum = new char* [m_iVectorSize];

	for (_uint i = 0; i < m_iVectorSize; ++i)
	{
		ppKeyFrameNum[i] = new char[16];
		sprintf_s(ppKeyFrameNum[i], sizeof(char) * 16, "%u", i);
	}

	static _int iSelectKeyFrame = -1;

	if (ImGui::Button("Reset"))
		iSelectKeyFrame = -1;
	ImGui::SameLine();
	ImGui::Checkbox("IsChange", &m_bChange);

	ImGui::ListBox("KeyFrameList", &iSelectKeyFrame, ppKeyFrameNum, (int)m_iVectorSize);

	if (iSelectKeyFrame != -1 && iSelectKeyFrame < (int)m_iVectorSize)
	{
		if(!m_bChange)
			m_pTransformCom->Set_WorldMatrix_float4x4(m_vecWorldMatrix[iSelectKeyFrame]);

		if (ImGui::Button("Change"))
			m_vecWorldMatrix[iSelectKeyFrame] = m_pTransformCom->Get_WorldMatrixFloat4x4();

		if (ImGui::Button("Erase"))
		{
			m_vecWorldMatrix.erase(m_vecWorldMatrix.begin() + iSelectKeyFrame);
			iSelectKeyFrame = -1;
		}
	}

	if (ImGui::Button("Clear"))
		m_vecWorldMatrix.clear();

	for (_uint i = 0; i < m_iVectorSize; ++i)
		Safe_Delete_Array(ppKeyFrameNum[i]);
	Safe_Delete_Array(ppKeyFrameNum);

	if (ImGui::Button("Save"))
		Save();

	if (ImGui::Button("Load"))
		Load();
}

void CWorldTrigger::ImGui_PhysXValueProperty()
{
	CGameObject::ImGui_PhysXValueProperty();
}

_int CWorldTrigger::Execute_Collision(CGameObject* pTarget, _float3 vCollisionPos, _int iColliderIndex)
{
	if (pTarget == nullptr)
		return 0;

	if (iColliderIndex == static_cast<_int>(COL_PLAYER_WEAPON))
	{
 		CPostFX::GetInstance()->BlurCapture();
		CUI_ClientManager::UI_PRESENT present = CUI_ClientManager::INFO_;
		if(g_LEVEL == LEVEL_GIMMICK)
		{
			_float f = 4.f;
			m_WorldTriggerDelegator.broadcast(present, f);
			m_nMatNum++;
		}
		else
		{
			_float fValue = static_cast<_float>(m_nMatNum);
			m_WorldTriggerDelegator.broadcast(present, fValue);
			m_nMatNum++;
		}
	}

	return 0;
}

void CWorldTrigger::BroadCast_WorldTrigger(_uint iValue)
{
	CPostFX::GetInstance()->BlurCapture();
	CUI_ClientManager::UI_PRESENT present = CUI_ClientManager::INFO_;
	m_nMatNum = iValue;
	_float fValue = static_cast<_float>(m_nMatNum);
	m_WorldTriggerDelegator.broadcast(present, fValue);
	m_nMatNum++;
}

HRESULT CWorldTrigger::Save()
{
	string      strSaveDirectory;
	if (g_LEVEL == LEVEL_FINAL)
	{
		strSaveDirectory = "../Bin/Data/UITrigger/WorldTriggerFinal.json";
	}
	else
	{
		strSaveDirectory = "../Bin/Data/UITrigger/WorldTrigger.json";
	}

	ofstream      file(strSaveDirectory.c_str());
	Json   jWorldTriggerMatrixList;

	jWorldTriggerMatrixList["0_KeyFrameSize"] = m_iVectorSize;

	_float      fElement = 0.f;

	for (_uint i = 0; i < m_iVectorSize; ++i)
	{
		Json jChild;

		_float4x4 mat = m_vecWorldMatrix[i];

		for (int i = 0; i < 16; ++i)
		{
			fElement = 0.f;
			memcpy(&fElement, (float*)&mat + i, sizeof(float));
			jChild["0_Transform State"].push_back(fElement);
		}

		jWorldTriggerMatrixList["1_Data"].push_back(jChild);
	}

	file << jWorldTriggerMatrixList;
	file.close();

	return S_OK;
}

HRESULT CWorldTrigger::Load()
{
	string      strLoadDirectory;

	 if(g_LEVEL == LEVEL_GIMMICK)
	{
		strLoadDirectory = "../Bin/Data/UITrigger/WorldTriggerGimmick.json";
	}
	else
	{
		 strLoadDirectory = "../Bin/Data/UITrigger/WorldTrigger.json";
	}

	ifstream      file(strLoadDirectory.c_str());
	Json   jLoadCineCamKeyFrameList;
	file >> jLoadCineCamKeyFrameList;
	file.close();

	m_vecWorldMatrix.clear();

	jLoadCineCamKeyFrameList["0_KeyFrameSize"].get_to<_uint>(m_iVectorSize);

	_float4x4 mat;

	for (auto jLoadChild : jLoadCineCamKeyFrameList["1_Data"])
	{
		int j = 0;
		for (float fElement : jLoadChild["0_Transform State"])
			memcpy(((float*)&mat) + (j++), &fElement, sizeof(float));
		m_vecWorldMatrix.push_back(mat);
	}

	return S_OK;
}

CWorldTrigger* CWorldTrigger::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CWorldTrigger* pInstance = new CWorldTrigger(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CWorldTrigger");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CWorldTrigger::Clone(void* pArg)
{
	CWorldTrigger* pInstance = new CWorldTrigger(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CWorldTrigger");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWorldTrigger::Free()
{
	CGameObject::Free();
	Safe_Release(m_pRendererCom);
}
