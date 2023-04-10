#include "stdafx.h"
#include "..\public\Dynamic_Stone.h"
#include "GameInstance.h"
#include "Dynamic_StoneCube.h"
#include "Monster.h"
#include "Kena.h"
CDynamic_Stone::CDynamic_Stone(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CEnviromentObj(pDevice, pContext)
{
}

CDynamic_Stone::CDynamic_Stone(const CDynamic_Stone& rhs)
	: CEnviromentObj(rhs)
{
}


HRESULT CDynamic_Stone::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CDynamic_Stone::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_bRenderActive = true;
	return S_OK;
}

HRESULT CDynamic_Stone::Late_Initialize(void* pArg)
{
	CGameInstance* pGameInst = CGameInstance::GetInstance();
	CGameObject* pCreateObject = nullptr;
	_float4 vPos;
	XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
	_float3 vSize = _float3(1.f, 1.f, 1.f);

	CDynamic_StoneCube::tag_Dynamic_StoneCube StoneCubeDesc;

	_int iMaxIndex = 3, jMaxIndex = 3, kMaxIndex = 3;

	StoneCubeDesc.fMass = 50.f;
	StoneCubeDesc.fLinearDamping = 3.f;

	float	boxRadius = 0.20f; // ���� �߽ɰ� ���� �߽� ������ �Ÿ�
	int		boxCountPerLayer = 2; // �� ���̾ ���Ե� �ڽ� ����
	float	layerHeight = 1.f;  //StoneCubeDesc.vSize.y; // ���̾� �� ���� ����
	float	layerRadiusStep = 0.0f; // ���̾� �� ������ ����
	int		layerCount = 10; // ��ü ���̾� ����

	_float3 vColiderSize = { 0.f,0.f,0.f };

	if (m_EnviromentDesc.szModelTag == L"Prototype_Component_Model_Dy_RockSmall03")
	{
		StoneCubeDesc.vSize = _float3(0.7f, 0.29f, 0.7f);
		StoneCubeDesc.fMass = 30.f;
		StoneCubeDesc.fLinearDamping = 1.f;
		iMaxIndex = 3;  jMaxIndex = 4; kMaxIndex = 3;

		boxRadius = 0.8f; // ���� �߽ɰ� ���� �߽� ������ �Ÿ�
		boxCountPerLayer = 2; // �� ���̾ ���Ե� �ڽ� ����
		layerHeight = StoneCubeDesc.vSize.y; // ���̾� �� ���� ��
		layerRadiusStep = 0.0f; // ���̾� �� ������ ����
		layerCount = 10; // ��ü ���̾� ����
		lstrcpy(StoneCubeDesc.pModelName, L"Prototype_Component_Model_Dy_RockSmall03");
		vColiderSize = _float3(0.7f, 2.5f, 0.7f); //��ü�� �ݶ��̴� ������
	}
	else if (m_EnviromentDesc.szModelTag == L"Prototype_Component_Model_Dy_RockSmall04")
	{
		StoneCubeDesc.vSize = _float3(0.38f, 0.25f, 0.45f);
		StoneCubeDesc.fMass = 100.f;
		StoneCubeDesc.fLinearDamping = 2.f;
		iMaxIndex = 3;  jMaxIndex = 4; kMaxIndex = 3;

		boxRadius = 0.5f; // ���� �߽ɰ� ���� �߽� ������ �Ÿ�
		boxCountPerLayer = 2; // �� ���̾ ���Ե� �ڽ� ����
		layerHeight = StoneCubeDesc.vSize.y; // ���̾� �� ���� ��
		layerRadiusStep = 0.0f; // ���̾� �� ������ ����
		layerCount = 10; // ��ü ���̾� ����
		lstrcpy(StoneCubeDesc.pModelName, L"Prototype_Component_Model_Dy_RockSmall04");
		vColiderSize = _float3(1.13f, 2.88f, 1.13f); //��ü�� �ݶ��̴� ������
	}
	else if (m_EnviromentDesc.szModelTag == L"Prototype_Component_Model_Dy_RockSmall05")
	{
		StoneCubeDesc.vSize = _float3(2.5f, 0.315f, 2.5f);
		StoneCubeDesc.fMass = 100.f;
		StoneCubeDesc.fLinearDamping = 2.f;
		iMaxIndex = 3;  jMaxIndex = 4; kMaxIndex = 3;

		boxRadius = 0.7f; // ���� �߽ɰ� ���� �߽� ������ �Ÿ�
		boxCountPerLayer = 2; // �� ���̾ ���Ե� �ڽ� ����
		layerHeight = StoneCubeDesc.vSize.y; // ���̾� �� ���� ��
		layerRadiusStep = 0.0f; // ���̾� �� ������ ����
		layerCount = 10; // ��ü ���̾� ����
		lstrcpy(StoneCubeDesc.pModelName, L"Prototype_Component_Model_Dy_RockSmall05");
		vColiderSize = _float3(0.9f, 3.3f, 0.9f); //��ü�� �ݶ��̴� ������
	}
	else if (m_EnviromentDesc.szModelTag == L"Prototype_Component_Model_Dy_RockSmall06")
	{
		StoneCubeDesc.vSize = _float3(1.3f, 0.28f, 1.3f);
		StoneCubeDesc.fMass = 10.f;
		StoneCubeDesc.fLinearDamping = 1.f;
		iMaxIndex = 3;  jMaxIndex = 4; kMaxIndex = 3;

		boxRadius = 0.8f; // ���� �߽ɰ� ���� �߽� ������ �Ÿ�
		boxCountPerLayer = 2; // �� ���̾ ���Ե� �ڽ� ����
		layerHeight = StoneCubeDesc.vSize.y; // ���̾� �� ���� ��
		layerRadiusStep = 0.0f; // ���̾� �� ������ ����
		layerCount = 10; // ��ü ���̾� ����
		lstrcpy(StoneCubeDesc.pModelName, L"Prototype_Component_Model_Dy_RockSmall06");
		vColiderSize = _float3(1.f, 2.8f, 1.f); //��ü�� �ݶ��̴� ������

	}
	else if (m_EnviromentDesc.szModelTag == L"Prototype_Component_Model_Dy_MaskShrine_Rock_09")
	{
		StoneCubeDesc.vSize = _float3(0.09f, 0.09f, 0.09f);
		StoneCubeDesc.fMass = 2.f;
		StoneCubeDesc.fLinearDamping = 0.5f;
		iMaxIndex = 3;  jMaxIndex = 4; kMaxIndex = 3;

		boxRadius = 0.20f; // ���� �߽ɰ� ���� �߽� ������ �Ÿ�
		boxCountPerLayer = 2; // �� ���̾ ���Ե� �ڽ� ����
		layerHeight = StoneCubeDesc.vSize.y; // ���̾� �� ���� ��
		layerRadiusStep = 0.0f; // ���̾� �� ������ ����
		layerCount = 9; // ��ü ���̾� ����
		lstrcpy(StoneCubeDesc.pModelName, L"Prototype_Component_Model_Dy_MaskShrine_Rock_09");
		vColiderSize = _float3( 0.26f,1.2f,0.26f ); //��ü�� �ݶ��̴� ������

	}
	else if (m_EnviromentDesc.szModelTag == L"Prototype_Component_Model_Dy_RockMedium04")
	{
		StoneCubeDesc.vSize = _float3(0.37f, 0.23f, 0.37f);
		StoneCubeDesc.fMass = 100.f;
		StoneCubeDesc.fLinearDamping = 3.f;
		iMaxIndex = 3;  jMaxIndex = 4; kMaxIndex = 3;


		boxRadius = 0.65f; // ���� �߽ɰ� ���� �߽� ������ �Ÿ�
		boxCountPerLayer = 2; // �� ���̾ ���Ե� �ڽ� ����
		layerHeight = StoneCubeDesc.vSize.y; // ���̾� �� ���� ��
		layerRadiusStep = 0.0f; // ���̾� �� ������ ����
		layerCount = 10; // ��ü ���̾� ����
		lstrcpy(StoneCubeDesc.pModelName, L"Prototype_Component_Model_Dy_RockMedium04");
		vColiderSize = _float3(0.9f, 2.44f, 0.9f); //��ü�� �ݶ��̴� ������
	}
	else if (m_EnviromentDesc.szModelTag == L"Prototype_Component_Model_Dy_RockMedium06")
	{
		StoneCubeDesc.vSize = _float3(0.4f, 0.23f, 0.4f);
		StoneCubeDesc.fMass = 100.f;
		StoneCubeDesc.fLinearDamping = 3.f;
		iMaxIndex = 3;  jMaxIndex = 4; kMaxIndex = 3;
		
		boxRadius = 0.4f; // ���� �߽ɰ� ���� �߽� ������ �Ÿ�
		boxCountPerLayer = 2; // �� ���̾ ���Ե� �ڽ� ����
		layerHeight = StoneCubeDesc.vSize.y; // ���̾� �� ���� ��
		layerRadiusStep = 0.0f; // ���̾� �� ������ ����
		layerCount = 10; // ��ü ���̾� ����
		lstrcpy(StoneCubeDesc.pModelName, L"Prototype_Component_Model_Dy_RockMedium06");
		vColiderSize = _float3(0.67f, 2.4f, 0.67f); //��ü�� �ݶ��̴� ������
	}
	else if (m_EnviromentDesc.szModelTag == L"Prototype_Component_Model_Dy_RockMedium07")
	{
		StoneCubeDesc.vSize = _float3(0.5f, 0.23f, 0.5f);
		StoneCubeDesc.fMass = 100.f;
		StoneCubeDesc.fLinearDamping = 3.f;
		iMaxIndex = 3;  jMaxIndex = 4; kMaxIndex = 3;

		boxRadius = 0.5f; // ���� �߽ɰ� ���� �߽� ������ �Ÿ�
		boxCountPerLayer = 2; // �� ���̾ ���Ե� �ڽ� ����
		layerHeight = StoneCubeDesc.vSize.y; // ���̾� �� ���� ��
		layerRadiusStep = 0.0f; // ���̾� �� ������ ����
		layerCount = 10; // ��ü ���̾� ����
		lstrcpy(StoneCubeDesc.pModelName, L"Prototype_Component_Model_Dy_RockMedium07");
		vColiderSize = _float3(0.67f, 2.4f, 0.67f); //��ü�� �ݶ��̴� ������
	}

	vSize.x += 0.05f;
	vSize.y += 0.05f;
	vSize.z += 0.05f;
	/*for (_int i = 0; i < iMaxIndex; i++)
	{
		for (_int j = 0; j < jMaxIndex; j++)
		{
			for (_int k = 0; k < kMaxIndex; k++)
			{
				_int iIndex = (i * iMaxIndex * jMaxIndex) + (j * jMaxIndex) + k;

				pCreateObject = nullptr;

				const _tchar* NewName  = CUtile::Create_DummyString(m_szCloneObjectTag, iIndex);

				if (FAILED(pGameInst->Clone_GameObject(g_LEVEL,
					TEXT("Layer_Enviroment"),
					TEXT("Prototype_GameObject_Dynamic_StoneCube"),
					NewName, &m_EnviromentDesc, &pCreateObject)))
					assert(!"CImgui_MapEditor::CDynamic_Stone::Late_Initialize");

				pCreateObject->Get_TransformCom()->Set_State(CTransform::STATE_TRANSLATION,
					XMVectorSet(vPos.x + _float(i * vSize.x), vPos.y+ _float(j + vSize.y), vPos.z + _float(k * vSize.z), 1.f));

				static_cast<CDynamic_StoneCube*>(pCreateObject)->Set_StoneCubeDesc(StoneCubeDesc);

			}
		}
	}*/

	/*���� ž*/

	
	_uint iTemp = 0;
	for (int layer = 0; layer < layerCount; ++layer) 
	{
		float layerRadius = boxRadius + layerRadiusStep * layer;
		int boxCount = boxCountPerLayer * (layer + 1);
		float angleStep = PxTwoPi / boxCount;

		for (int i = 0; i < boxCount; ++i)
		{
			PxVec3 Pxpos(layerRadius * std::cos(angleStep * i),
				layerHeight* layer,
				layerRadius * std::sin(angleStep *i ));

			Pxpos.x += vPos.x; StoneCubeDesc.vPos.x = Pxpos.x;
			Pxpos.y += vPos.y; StoneCubeDesc.vPos.y = Pxpos.y;
			Pxpos.z += vPos.z; StoneCubeDesc.vPos.z = Pxpos.z;
			
			pCreateObject = nullptr;

			const _tchar* NewName = CUtile::Create_DummyString(m_szCloneObjectTag, ++iTemp);

			pCreateObject = pGameInst->Clone_GameObject(
				TEXT("Prototype_GameObject_Dynamic_StoneCube"),
				NewName, &StoneCubeDesc);
			assert(nullptr != pCreateObject  && 
							"CImgui_MapEditor::CDynamic_Stone::Late_Initialize");
						
			pCreateObject->Late_Initialize();

			m_pDynamicObj_List.push_back(pCreateObject);
		}
	}

	CPhysX_Manager* pPhysX = CPhysX_Manager::GetInstance();

	CPhysX_Manager::PX_BOX_DESC BoxDesc;
	BoxDesc.pActortag = m_szCloneObjectTag;
	BoxDesc.eType = BOX_STATIC;		
	BoxDesc.vPos = CUtile::Float_4to3(vPos);
	BoxDesc.vSize = vColiderSize;
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
	BoxDesc.bKinematic = true;
	pPhysX->Create_Box(BoxDesc, Create_PxUserData(this, false, COL_ENVIROMENT));
	
	/*���� ���� �����ϰ� �������� �ٲٱ�*/




	return S_OK;
}

void CDynamic_Stone::Tick(_float fTimeDelta)
{

#ifdef FOR_MAP_GIMMICK

#else
	if(m_bTestLateInit == false)
	{
		Late_Initialize();
		m_bTestLateInit = true;
	}
#endif


	for (auto& pObj : m_pDynamicObj_List)
	{
		if (pObj != nullptr)
			pObj->Tick(fTimeDelta);
	}


	//CPhysX_Manager::GetInstance()->Imgui_Render(m_szCloneObjectTag);



	__super::Tick(fTimeDelta);
}

void CDynamic_Stone::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	size_t size = m_pDynamicObj_List.size();
	size_t iIndex = 0;
	for (auto& pObj : m_pDynamicObj_List)
	{
		if (pObj != nullptr)
			pObj->Late_Tick(fTimeDelta);

	}

//	if (m_pRendererCom && m_bRenderActive)
//	{
//#ifdef _DEBUG
//		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_CINE, this);
//#endif
//		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
//	}

}

HRESULT CDynamic_Stone::Render()
{
	//if (FAILED(__super::Render()))
	//	return E_FAIL;

	//if (FAILED(SetUp_ShaderResources()))
	//	return E_FAIL;

	//_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	//if (m_pModelCom->Get_IStancingModel())
	//{
	//	for (_uint i = 0; i < iNumMeshes; ++i)
	//	{
	//		FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture"), E_FAIL);
	//		FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_NORMALS, "g_NormalTexture"), E_FAIL);

	//		if ((*m_pModelCom->Get_Material())[i].pTexture[WJTextureType_COMP_H_R_AO] != nullptr && (*m_pModelCom->Get_Material())[i].pTexture[WJTextureType_EMISSIVE] != nullptr)
	//		{
	//			FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture"), E_FAIL);
	//			FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_NORMALS, "g_NormalTexture"), E_FAIL);
	//			FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_EMISSIVE, "g_EmissiveTexture"), E_FAIL);
	//			FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_COMP_H_R_AO, "g_HRAOTexture"), E_FAIL);
	//			float f = 1.f;
	//			FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_fStonePulseIntensity", &f, sizeof(float)), E_FAIL);
	//			FAILED_CHECK_RETURN(m_pModelCom->Render(m_pShaderCom, i, nullptr, 4), E_FAIL);
	//		}
	//		else if ((*m_pModelCom->Get_Material())[i].pTexture[WJTextureType_COMP_H_R_AO] != nullptr)
	//		{
	//			FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_COMP_H_R_AO, "g_HRAOTexture"), E_FAIL);
	//			FAILED_CHECK_RETURN(m_pModelCom->Render(m_pShaderCom, i, nullptr, 2), E_FAIL);
	//		}
	//		else	if ((*m_pModelCom->Get_Material())[i].pTexture[WJTextureType_COMP_E_R_AO] != nullptr)
	//		{
	//			FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_COMP_E_R_AO, "g_ERAOTexture"), E_FAIL);
	//			FAILED_CHECK_RETURN(m_pModelCom->Render(m_pShaderCom, i, nullptr, 5), E_FAIL);
	//		}
	//		else if ((*m_pModelCom->Get_Material())[i].pTexture[WJTextureType_AMBIENT_OCCLUSION] != nullptr)
	//		{
	//			FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_AMBIENT_OCCLUSION, "g_MRAOTexture"), E_FAIL);
	//			FAILED_CHECK_RETURN(m_pModelCom->Render(m_pShaderCom, i, nullptr, 6), E_FAIL);
	//		}
	//		else
	//		{
	//			FAILED_CHECK_RETURN(m_pModelCom->Render(m_pShaderCom, i, nullptr, 1), E_FAIL);
	//		}
	//	}
	//}
	//else
	//{
	//	for (_uint i = 0; i < iNumMeshes; ++i)
	//	{
	//		FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture"), E_FAIL);
	//		FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_NORMALS, "g_NormalTexture"), E_FAIL);
	//		if ((*m_pModelCom->Get_Material())[i].pTexture[WJTextureType_COMP_H_R_AO] != nullptr)
	//		{
	//			FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_COMP_H_R_AO, "g_HRAOTexture"), E_FAIL);
	//			FAILED_CHECK_RETURN(m_pModelCom->Render(m_pShaderCom, i, nullptr, 17), E_FAIL);
	//		}
	//		else	if ((*m_pModelCom->Get_Material())[i].pTexture[WJTextureType_COMP_E_R_AO] != nullptr)
	//		{
	//			FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_COMP_E_R_AO, "g_ERAOTexture"), E_FAIL);
	//			FAILED_CHECK_RETURN(m_pModelCom->Render(m_pShaderCom, i, nullptr, 18), E_FAIL);
	//		}
	//		else if ((*m_pModelCom->Get_Material())[i].pTexture[WJTextureType_AMBIENT_OCCLUSION] != nullptr)
	//		{
	//			FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_AMBIENT_OCCLUSION, "g_MRAOTexture"), E_FAIL);
	//			FAILED_CHECK_RETURN(m_pModelCom->Render(m_pShaderCom, i, nullptr, 16), E_FAIL);
	//		}
	//		else
	//		{
	//			FAILED_CHECK_RETURN(m_pModelCom->Render(m_pShaderCom, i, nullptr, 15), E_FAIL);
	//		}
	//	}
	//}

	return S_OK;
}

HRESULT CDynamic_Stone::RenderShadow()
{
	//if (FAILED(__super::RenderShadow()))
	//	return E_FAIL;

	//if (FAILED(SetUp_ShadowShaderResources()))
	//	return E_FAIL;

	//_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	//if (m_pModelCom->Get_IStancingModel())
	//{
	//	for (_uint i = 0; i < iNumMeshes; ++i)
	//	{
	//		FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture"), E_FAIL);
	//		FAILED_CHECK_RETURN(m_pModelCom->Render(m_pShaderCom, i, nullptr, 0), E_FAIL);
	//	}
	//}
	//else
	//{
	//	for (_uint i = 0; i < iNumMeshes; ++i)
	//	{
	//		FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture"), E_FAIL);
	//		FAILED_CHECK_RETURN(m_pModelCom->Render(m_pShaderCom, i, nullptr, 2), E_FAIL);
	//	}
	//}

	return S_OK;
}

HRESULT CDynamic_Stone::RenderCine()
{
	//if (FAILED(__super::RenderCine()))
	//	return E_FAIL;

	//if (FAILED(__super::SetUp_CineShaderResources()))
	//	return E_FAIL;

	//_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	//if (m_pModelCom->Get_IStancingModel())
	//{
	//	for (_uint i = 0; i < iNumMeshes; ++i)
	//	{
	//		FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture"), E_FAIL);
	//		FAILED_CHECK_RETURN(m_pModelCom->Render(m_pShaderCom, i, nullptr, 12), E_FAIL);
	//	}
	//}
	//else
	//{
	//	for (_uint i = 0; i < iNumMeshes; ++i)
	//	{
	//		/* �� ���� �׸������� ���̴��� ���׸��� �ؽ��ĸ� �����ϳ�. */
	//		FAILED_CHECK_RETURN(m_pModelCom->Bind_Material(m_pShaderCom, i, WJTextureType_DIFFUSE, "g_DiffuseTexture"), E_FAIL);
	//		FAILED_CHECK_RETURN(m_pModelCom->Render(m_pShaderCom, i, nullptr, 10), E_FAIL);
	//	}
	//}

	return S_OK;
}

_int CDynamic_Stone::Execute_Collision(CGameObject* pTarget, _float3 vCollisionPos, _int iColliderIndex)
{
	if (pTarget == nullptr || COL_DYNAMIC_ENVIOBJ == iColliderIndex)
		return 0;

	if (m_bOnceColl == true)		//�ѹ��� �����Ǳ����ؼ�
		return 0;

	_bool bMonsterRealAttack = false;
	_bool bPlayerWeaponAttack = false;
	if ( ((iColliderIndex == (_int)COL_MONSTER_WEAPON && 
			(bMonsterRealAttack = ((CMonster*)pTarget)->IsRealAttack())))
		||  (iColliderIndex == (_int)COL_PLAYER_WEAPON && 
			(bPlayerWeaponAttack =  dynamic_cast<CKena*>(pTarget)->Get_State(CKena::STATE_ATTACK))))
	{
		for (auto& pObj : m_pDynamicObj_List)
		{
			if (pObj != nullptr)
			{
				static_cast<CDynamic_StoneCube*>(pObj)->Set_CollActive();
				// static_cast<CDynamic_StoneCube*>(pObj)->Execute_SleepEnd();
			}				
		}

		m_bOnceColl = true;
	}

	return 0;
}

void CDynamic_Stone::ImGui_PhysXValueProperty()
{
	//CPhysX_Manager::GetInstance()->Imgui_Render(m_szCloneObjectTag);

	//if(ImGui::Button("TestTest"))
	//{
	//	for (auto& pObj : m_pDynamicObj_List)
	//	{
	//		if (pObj != nullptr)
	//		{
	//			static_cast<CDynamic_StoneCube*>(pObj)->Set_CollActive();
	//			// static_cast<CDynamic_StoneCube*>(pObj)->Execute_SleepEnd();
	//		}
	//	}
	//}

}

HRESULT CDynamic_Stone::Add_AdditionalComponent(_uint iLevelIndex, const _tchar* pComTag, COMPONENTS_OPTION eComponentOption)
{
	//__super::Add_AdditionalComponent(iLevelIndex, pComTag, eComponentOption);

	///* For.Com_CtrlMove */
	//if (eComponentOption == COMPONENTS_CONTROL_MOVE)
	//{
	//	if (FAILED(__super::Add_Component(iLevelIndex, TEXT("Prototype_Component_ControlMove"), pComTag,
	//		(CComponent**)&m_pControlMoveCom)))
	//		return E_FAIL;
	//}
	///* For.Com_Interaction */
	//else if (eComponentOption == COMPONENTS_INTERACTION)
	//{
	//	if (FAILED(__super::Add_Component(iLevelIndex, TEXT("Prototype_Component_Interaction_Com"), pComTag,
	//		(CComponent**)&m_pInteractionCom)))
	//		return E_FAIL;
	//}
	//else
	//	return S_OK;

	return S_OK;

}

HRESULT CDynamic_Stone::SetUp_Components()
{
	///* For.Com_Renderer */
	//if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
	//	(CComponent**)&m_pRendererCom)))
	//	return E_FAIL;

	///* For.Com_Shader */
	///*���߿�  ���� �ε��� �����ؾ߉�*/
	//if (m_EnviromentDesc.iCurLevel == 0)
	//	m_EnviromentDesc.iCurLevel = LEVEL_MAPTOOL;

	///* For.Com_Model */ 	/*���߿�  ���� �ε��� �����ؾ߉�*/
	//if (FAILED(__super::Add_Component(g_LEVEL, m_EnviromentDesc.szModelTag.c_str(), TEXT("Com_Model"),
	//	(CComponent**)&m_pModelCom)))
	//	return E_FAIL;
	///* For.Com_Shader */
	///* For.Com_Shader */
	//if (m_pModelCom->Get_IStancingModel())
	//{
	//	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_VtxModelInstance"), TEXT("Com_Shader"),
	//		(CComponent**)&m_pShaderCom)))
	//		return E_FAIL;
	//	m_iShaderOption = 1;
	//}
	//else
	//{
	//	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_VtxModelTess"), TEXT("Com_Shader"),
	//		(CComponent**)&m_pShaderCom)))
	//		return E_FAIL;
	//	m_iShaderOption = 4;
	//}


	return S_OK;
}

HRESULT CDynamic_Stone::SetUp_ShaderResources()
{
	/*NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);
	FAILED_CHECK_RETURN(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"), E_FAIL);
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_fFar", pGameInstance->Get_CameraFar(), sizeof(float)), E_FAIL);
	RELEASE_INSTANCE(CGameInstance);*/
	return S_OK;
}

HRESULT CDynamic_Stone::SetUp_ShadowShaderResources()
{
	/*NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);
	FAILED_CHECK_RETURN(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"), E_FAIL);
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_LIGHTVIEW)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_fFar", pGameInstance->Get_CameraFar(), sizeof(float)), E_FAIL);
	RELEASE_INSTANCE(CGameInstance);*/
	return S_OK;
}

CDynamic_Stone* CDynamic_Stone::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDynamic_Stone* pInstance = new CDynamic_Stone(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CDynamic_Stone");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CDynamic_Stone::Clone(void* pArg)
{
	CDynamic_Stone* pInstance = new CDynamic_Stone(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CDynamic_Stone");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CDynamic_Stone::Free() //���� free �Լ� ȣ�⺸�� 
{
	if(m_isCloned)
	{
		for (auto& pObj : m_pDynamicObj_List)
		{
			if (pObj != nullptr)
			{
				dynamic_cast<CDynamic_StoneCube*>(pObj)->Actor_Clear();
				Safe_Release(pObj);
			}
		}
		m_pDynamicObj_List.clear();
	}
	__super::Free();
}
