#include "stdafx.h"
#include "..\public\CinematicCamera.h"
#include "GameInstance.h"
#include "PipeLine.h"
#include "DebugDraw.h"
#include "Utile.h"
#include <codecvt>
#include <locale>

_float fTemp = 0.f;
wstring wstrTemp = L"";

void XM_CALLCONV DrawLine(PrimitiveBatch<VertexPositionColor>* batch,
	FXMVECTOR pointA, FXMVECTOR pointB, FXMVECTOR color)
{
	VertexPositionColor verts[2];
	XMStoreFloat3(&verts[0].position, pointA);
	XMStoreFloat3(&verts[1].position, pointB);

	for (size_t j = 0; j < 2; ++j)
	{
		XMStoreFloat4(&verts[j].color, color);
	}

	batch->Draw(D3D_PRIMITIVE_TOPOLOGY_LINELIST, verts, 2);
}

void XM_CALLCONV DrawRing(PrimitiveBatch<VertexPositionColor>* batch,
	FXMVECTOR origin,
	FXMVECTOR majorAxis,
	FXMVECTOR minorAxis,
	GXMVECTOR color)
{
	static const size_t c_ringSegments = 32;

	VertexPositionColor verts[c_ringSegments + 1];

	FLOAT fAngleDelta = XM_2PI / float(c_ringSegments);
	// Instead of calling cos/sin for each segment we calculate
	// the sign of the angle delta and then incrementally calculate sin
	// and cosine from then on.
	XMVECTOR cosDelta = XMVectorReplicate(cosf(fAngleDelta));
	XMVECTOR sinDelta = XMVectorReplicate(sinf(fAngleDelta));
	XMVECTOR incrementalSin = XMVectorZero();
	static const XMVECTORF32 s_initialCos =
	{
		{ { 1.f, 1.f, 1.f, 1.f } }
	};
	XMVECTOR incrementalCos = s_initialCos.v;
	for (size_t i = 0; i < c_ringSegments; i++)
	{
		XMVECTOR pos = XMVectorMultiplyAdd(majorAxis, incrementalCos, origin);
		pos = XMVectorMultiplyAdd(minorAxis, incrementalSin, pos);
		XMStoreFloat3(&verts[i].position, pos);
		XMStoreFloat4(&verts[i].color, color);
		// Standard formula to rotate a vector.
		XMVECTOR newCos = incrementalCos * cosDelta - incrementalSin * sinDelta;
		XMVECTOR newSin = incrementalCos * sinDelta + incrementalSin * cosDelta;
		incrementalCos = newCos;
		incrementalSin = newSin;
	}
	verts[c_ringSegments] = verts[0];

	batch->Draw(D3D_PRIMITIVE_TOPOLOGY_LINESTRIP, verts, c_ringSegments + 1);
}

void XM_CALLCONV Draw(PrimitiveBatch<VertexPositionColor>* batch,
	const BoundingSphere& sphere,
	FXMVECTOR color)
{
	XMVECTOR origin = XMLoadFloat3(&sphere.Center);

	const float radius = sphere.Radius;

	XMVECTOR xaxis = g_XMIdentityR0 * radius;
	XMVECTOR yaxis = g_XMIdentityR1 * radius;
	XMVECTOR zaxis = g_XMIdentityR2 * radius;

	DrawRing(batch, origin, xaxis, zaxis, color);
	DrawRing(batch, origin, xaxis, yaxis, color);
	DrawRing(batch, origin, yaxis, zaxis, color);
}

CCinematicCamera::CCinematicCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CCamera(pDevice, pContext)
	, m_iChatIndex(0)
{
}

CCinematicCamera::CCinematicCamera(const CCinematicCamera& rhs)
	: CCamera(rhs)
	, m_iChatIndex(0)
{
	strcpy_s(m_szChatFileName, sizeof(MAX_PATH), rhs.m_szChatFileName);
}

HRESULT CCinematicCamera::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCinematicCamera::Initialize(void* pArg)
{
	CCamera::CAMERADESC			CameraDesc;
	ZeroMemory(&CameraDesc, sizeof CameraDesc);

	CameraDesc.vEye = _float4(0.f, 7.f, -5.f, 1.f);
	CameraDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	CameraDesc.vUp = _float4(0.f, 1.f, 0.f, 0.f);
	CameraDesc.fFovy = XMConvertToRadians(75.0f);
	CameraDesc.fAspect = g_iWinSizeX / _float(g_iWinSizeY);
	CameraDesc.fNear = 0.2f;
	CameraDesc.fFar = 500.f;
	CameraDesc.TransformDesc.fSpeedPerSec = 10.0f;
	CameraDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(CCamera::Initialize(&CameraDesc)))
		return E_FAIL;

	if (pArg != nullptr)
		m_keyframes = *(vector<CAMERAKEYFRAME>*)pArg;

	FAILED_CHECK_RETURN(SetUp_Components(), E_FAIL);

//#ifdef _DEBUG
	m_pBatch = new PrimitiveBatch<VertexPositionColor>(m_pContext);
	m_pEffect = new BasicEffect(m_pDevice);
	m_pEffect->SetVertexColorEnabled(true);
	const void*		pShaderByteCode;
	size_t			iShaderByteCodeSize;
	m_pEffect->GetVertexShaderBytecode(&pShaderByteCode, &iShaderByteCodeSize);
	m_pDevice->CreateInputLayout(VertexPositionColor::InputElements, VertexPositionColor::InputElementCount, pShaderByteCode, iShaderByteCodeSize, &m_pInputLayout);
//#endif

	return S_OK;
}

void CCinematicCamera::Tick(_float fTimeDelta)
{
#ifdef _DEBUG
	ImGui::Begin("CinematicCam");
	Imgui_RenderProperty();
	ImGui::End();
#endif

	m_iNumKeyFrames = (_uint)m_keyframes.size();

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance)

	if (pGameInstance->Key_Down(DIK_L))
	{
		m_pPlayerCam = pGameInstance->Find_Camera(L"PLAYER_CAM");
		if (m_pPlayerCam)
			m_pTransformCom->Set_WorldMatrix_float4x4(m_pPlayerCam->Get_TransformCom()->Get_WorldMatrixFloat4x4());
	}

	if (m_bPlay &&m_iNumKeyFrames >= 4)
	{
		if (m_bInitSet)
		{
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance)
			m_pPlayerCam = pGameInstance->Find_Camera(L"PLAYER_CAM");
			RELEASE_INSTANCE(CGameInstance)
			m_pTransformCom->Set_WorldMatrix_float4x4(m_pPlayerCam->Get_TransformCom()->Get_WorldMatrixFloat4x4());
			_float4 vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
			_float4 vLookAt = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
			m_keyframes.front().vPos = _float3(vPos.x, vPos.y, vPos.z);
			m_keyframes.front().vLookAt = _float3(vLookAt.x, vLookAt.y, vLookAt.z);

			/* Call CinemaUI */
			CUI_ClientManager::UI_PRESENT eLetterBox = CUI_ClientManager::BOT_LETTERBOX;
			_bool bOn = true;
			m_CinemaDelegator.broadcast(eLetterBox, bOn, fTemp, wstrTemp);
			/* ~Call CinemaUI */

			if (!m_vecChat.empty())
			{
				/* Call Chat */
				CUI_ClientManager::UI_PRESENT eChat = CUI_ClientManager::BOT_CHAT;
				_float fY = 450.f;
				m_CinemaDelegator.broadcast(eChat, bOn, fY, m_vecChat[m_iChatIndex]);
				m_iChatIndex++;
				/* ~Call Chat */
			}

			m_bInitSet = false;
		}

		if (m_fDeltaTime <= m_keyframes.back().fTime && !m_bPausePlay)
			m_fDeltaTime += fTimeDelta;
		else
		{
			m_bPlay = false;
			m_bFinished = true;
		}

		_float4 vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
		_float4 vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
		_float3 InterPolatePos = _float3(vPos.x, vPos.y, vPos.z);
		_float3 InterPolateLook = _float3(vLook.x, vLook.y, vLook.z);
		Interpolate(m_fDeltaTime, InterPolatePos, InterPolateLook);
		vPos = _float4(InterPolatePos.x, InterPolatePos.y, InterPolatePos.z, 1.f);
		vLook = _float4(InterPolateLook.x, InterPolateLook.y, InterPolateLook.z, 0.f);
		m_pTransformCom->Set_Position(vPos);
		m_pTransformCom->Set_Look(vLook);
		CCamera::Tick(fTimeDelta);
	}
	else if(m_bFinished)
	{
		if(!m_bFinishSet)
		{
			CGameInstance::GetInstance()->Work_Camera(TEXT("PLAYER_CAM"));
			/* Call CinemaUI */
			CUI_ClientManager::UI_PRESENT eLetterBox = CUI_ClientManager::BOT_LETTERBOX;
			_bool bOn = false;
			m_CinemaDelegator.broadcast(eLetterBox, bOn, fTemp, wstrTemp);
			/* ~Call CinemaUI */
			m_bFinishSet = true;
		}
	}
	
	pGameInstance->Set_Transform(CPipeLine::D3DTS_CINEVIEW, m_pTransformCom->Get_WorldMatrix_Inverse());
	RELEASE_INSTANCE(CGameInstance)
}

void CCinematicCamera::Late_Tick(_float TimeDelta)
{
	CCamera::Late_Tick(TimeDelta);

#ifdef _DEBUG
	if (m_pRendererCom && CGameInstance::GetInstance()->Get_WorkCameraPtr() != this)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);
#endif
}

HRESULT CCinematicCamera::Render()
{
//#ifdef _DEBUG
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_ShaderResources(), E_FAIL);
	_uint	 iNumMeshes = m_pModelCom->Get_NumMeshes();
	for (_uint i = 0; i < iNumMeshes; ++i)
		m_pModelCom->Render(m_pShaderCom, i, nullptr, 12);

	if (m_iNumKeyFrames < 4)
		return S_OK;

	m_pEffect->SetWorld(XMMatrixIdentity());
	CGameInstance* pInst = GET_INSTANCE(CGameInstance);
	m_pEffect->SetView(pInst->Get_TransformMatrix(CPipeLine::D3DTS_VIEW));
	m_pEffect->SetProjection(pInst->Get_TransformMatrix(CPipeLine::D3DTS_PROJ));
	RELEASE_INSTANCE(CGameInstance);

	m_pEffect->Apply(m_pContext);
	m_pContext->IASetInputLayout(m_pInputLayout);
	m_pBatch->Begin();

	BoundingSphere**	ppSphere = new BoundingSphere*[m_iNumKeyFrames];

	for (unsigned int i = 1; i < m_iNumKeyFrames; ++i)
	{
		DrawLine(m_pBatch, m_keyframes[i - 1].vPos, m_keyframes[i].vPos, _float4(1.f, 0.f, 1.f, 1.f));
	}

	for (unsigned int i = 0; i < m_iNumKeyFrames; ++i)
	{
		ppSphere[i] = new BoundingSphere(m_keyframes[i].vPos, 0.3f);
		Draw(m_pBatch, *ppSphere[i], _float4(0.f, 0.f, 1.f, 1.f));
	}

	m_pBatch->End();

	for (_uint i = 0; i < m_iNumKeyFrames; ++i)
		Safe_Delete(ppSphere[i]);
	Safe_Delete_Array(ppSphere);
//#endif // _DEBUG
	return CCamera::Render();
}

void CCinematicCamera::Imgui_RenderProperty()
{
	CCamera::Imgui_RenderProperty();

	ImGui::InputFloat("InputTime", &m_fInputTime);

	if (ImGui::Button("Add KeyFrame"))
	{
		CAMERAKEYFRAME keyFrame;
		ZeroMemory(&keyFrame, sizeof(CAMERAKEYFRAME));
		if (m_fInputTime != 0.f)
		{
			keyFrame.fTime = m_fInputTime + m_keyframes.size() *  0.5f;
			keyFrame.vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
			keyFrame.vLookAt = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
			m_keyframes.push_back(keyFrame);
		}
	}

	if (m_keyframes.size() >= 4)
	{
		static _int iSelectKeyFrame = -1;
		char**			ppKeyFrameNum = new char*[m_iNumKeyFrames];

		if (ImGui::SliderFloat("Duration", &m_fDeltaTime, 0.f, m_keyframes.back().fTime))
		{
			m_bPausePlay = true;
			m_bPlay = true;
			iSelectKeyFrame = -1;
		}
				
		// 시네마틱인지 디버그 플레이인지 구분할것.
		if (ImGui::Button("CinematicPlay"))
		{
			m_fDeltaTime = 0.f;
			m_bPlay = true;
			m_bPausePlay = false;
			CGameInstance::GetInstance()->Work_Camera(m_szCloneObjectTag);
			/* Call CinemaUI */
			CUI_ClientManager::UI_PRESENT eLetterBox = CUI_ClientManager::BOT_LETTERBOX;
			_bool bOn = true;
			m_CinemaDelegator.broadcast(eLetterBox, bOn, fTemp, wstrTemp);
			/* ~Call CinemaUI */
		}
		ImGui::SameLine();
		if(ImGui::Button("CinemaUIOff"))
		{
			/* Call CinemaUI */
			CUI_ClientManager::UI_PRESENT eLetterBox = CUI_ClientManager::BOT_LETTERBOX;
			_bool bOn = false;
			m_CinemaDelegator.broadcast(eLetterBox, bOn, fTemp, wstrTemp);
			/* ~Call CinemaUI */
		}

		if(ImGui::Button("DebugPlay"))
		{
			m_fDeltaTime = 0.f;
			m_bPlay = true;
			m_bPausePlay = false;
		}
		
		for (_uint i = 0; i < m_iNumKeyFrames; ++i)
		{
			ppKeyFrameNum[i] = new char[16];
			sprintf_s(ppKeyFrameNum[i], sizeof(char) * 16, "%u", i);
		}

		ImGui::ListBox("KeyFrameList", &iSelectKeyFrame, ppKeyFrameNum, (int)m_iNumKeyFrames);

		if (iSelectKeyFrame != -1)
		{
			float fPos[3] = { m_keyframes[iSelectKeyFrame].vPos.x , m_keyframes[iSelectKeyFrame].vPos.y,m_keyframes[iSelectKeyFrame].vPos.z };
			ImGui::DragFloat3("KFPos", fPos, 0.01f, -1000.f, 1000.f);
			m_keyframes[iSelectKeyFrame].vPos = _float3(fPos[0], fPos[1], fPos[2]);
			float fLookAT[3] = { m_keyframes[iSelectKeyFrame].vLookAt.x , m_keyframes[iSelectKeyFrame].vLookAt.y,m_keyframes[iSelectKeyFrame].vLookAt.z };
			ImGui::DragFloat3("KFLookAT", fLookAT, 0.01f, -1000.f, 1000.f);
			m_keyframes[iSelectKeyFrame].vLookAt = _float3(fLookAT[0], fLookAT[1], fLookAT[2]);
			ImGui::DragFloat("KFTime", &m_keyframes[iSelectKeyFrame].fTime);

			m_pTransformCom->Set_Position(m_keyframes[iSelectKeyFrame].vPos);
			m_pTransformCom->Set_Look(m_keyframes[iSelectKeyFrame].vLookAt);

			if (ImGui::Button("Erase"))
			{
				m_keyframes.erase(m_keyframes.begin() + iSelectKeyFrame);
				iSelectKeyFrame = -1;
			}
		}

		for (_uint i = 0; i < m_iNumKeyFrames; ++i)
			Safe_Delete_Array(ppKeyFrameNum[i]);
		Safe_Delete_Array(ppKeyFrameNum);
	}

	if (ImGui::Button("Test Chat"))
	{
		CUI_ClientManager::UI_PRESENT eChat = CUI_ClientManager::BOT_CHAT;
		_bool bVal = true;

		if (m_iChatIndex >= (int)m_vecChat.size())
		{
			//m_iChatIndex++;
			bVal = false;
			_float fY = 450.f;
			m_CinemaDelegator.broadcast(eChat, bVal, fY, m_vecChat[0]);
			return;
		}
		else
		{
			_float fY = 450.f;
			m_CinemaDelegator.broadcast(eChat, bVal, fY, m_vecChat[m_iChatIndex]);
			m_iChatIndex++;
		}
	}

	if (!m_bPlay)
	{
		if (ImGui::Button("KeyFrameClear"))
			m_keyframes.clear();
	}

	/* Input ChatFile Name */
	static	char szSaveFileName[MAX_PATH] = "";
	ImGui::SetNextItemWidth(200);
	ImGui::InputTextWithHint("ChatFileName", "File Name", szSaveFileName, MAX_PATH);
	ImGui::SameLine();
	if (ImGui::Button("Reset ChatFile"))
		strcpy_s(szSaveFileName, sizeof(MAX_PATH), m_szChatFileName);
	ImGui::SameLine();
	if (ImGui::Button("Confirm ChatFile"))
		strcpy_s(m_szChatFileName, sizeof(MAX_PATH), szSaveFileName);
	/* ~Input ChatFile Name */


	if (ImGui::CollapsingHeader("Save&Load"))
	{
		ImGui::Checkbox("CineCamWriteFileOn", &m_bSaveWrite);

		if (m_bSaveWrite)
			ImGui::InputText("CineCamSave_Name : ", &m_strFileName);

		if (ImGui::Button("Confirm_CineCamSave"))
			ImGuiFileDialog::Instance()->OpenDialog("CineCam Save Folder", "Select CineCam Save Folder", ".json", "../Bin/Data", ".", 0, nullptr, ImGuiFileDialogFlags_Modal);

		if (ImGuiFileDialog::Instance()->Display("CineCam Save Folder"))
		{
			if (ImGuiFileDialog::Instance()->IsOk())        // OK 눌렀을 때
			{
				Save_Data();
				ImGuiFileDialog::Instance()->Close();
			}

			if (!ImGuiFileDialog::Instance()->IsOk())       // Cancel 눌렀을 때
				ImGuiFileDialog::Instance()->Close();
		}

		if (ImGui::Button("Confirm_CineCamLoad"))
			ImGuiFileDialog::Instance()->OpenDialog("CineCam Load Folder", "Select CineCam Load Folder", ".json", "../Bin/Data", ".", 0, nullptr, ImGuiFileDialogFlags_Modal);

		if (ImGuiFileDialog::Instance()->Display("CineCam Load Folder"))
		{
			if (ImGuiFileDialog::Instance()->IsOk())        // OK 눌렀을 때
			{
				Load_Data();
				ImGuiFileDialog::Instance()->Close();
			}
			if (!ImGuiFileDialog::Instance()->IsOk())       // Cancel 눌렀을 때
				ImGuiFileDialog::Instance()->Close();
		}
	}

	m_pTransformCom->Imgui_RenderProperty();
}

void CCinematicCamera::AddKeyFrame(CAMERAKEYFRAME keyFrame)
{
	m_keyframes.push_back(keyFrame);
}

void CCinematicCamera::Interpolate(float time, _float3& position, _float3& lookAt)
{
	if (m_iNumKeyFrames < 4) {
		// Not enough keyframes to interpolate, just return the first keyframe
		position = m_keyframes[0].vPos;
		lookAt = m_keyframes[0].vLookAt;
		return;
	}

	// Find the two keyframes that surround the given time
	int i = 1;
	while (i < (int)m_iNumKeyFrames - 1 && m_keyframes[i].fTime < time) {
		i++;
	}

	if (i < (int)m_iNumKeyFrames - 1)
	{
		// Calculate the parameter t for the interpolation
		float t = 0.f;
		if (m_keyframes[i].fTime - m_keyframes[i - 1].fTime != 0)
			t = (time - m_keyframes[i - 1].fTime) / (m_keyframes[i].fTime - m_keyframes[i - 1].fTime);
		else
			t = time - m_keyframes[i - 1].fTime;

		// Calculate the interpolated position and look-at direction
		position = CatmullRomInterpolation(m_keyframes[i - 1].vPos, m_keyframes[i].vPos,
			m_keyframes[i + 1].vPos, m_keyframes[i + 1].vPos, t);
		lookAt = CatmullRomInterpolation(m_keyframes[i - 1].vLookAt, m_keyframes[i].vLookAt,
			m_keyframes[i + 1].vLookAt, m_keyframes[i + 1].vLookAt, t);
	}
}

XMFLOAT3 CCinematicCamera::CatmullRomInterpolation(_float3 p0, _float3 p1, _float3 p2, _float3 p3, float t)
{
	XMFLOAT3 v;
	v.x = 0.5f * ((2.0f * p1.x) + (-p0.x + p2.x) * t + (2.0f * p0.x - 5.0f * p1.x + 4.0f * p2.x - p3.x) * t * t + (-p0.x + 3.0f * p1.x - 3.0f * p2.x + p3.x) * t * t * t);
	v.y = 0.5f * ((2.0f * p1.y) + (-p0.y + p2.y) * t + (2.0f * p0.y - 5.0f * p1.y + 4.0f * p2.y - p3.y) * t * t + (-p0.y + 3.0f * p1.y - 3.0f * p2.y + p3.y) * t * t * t);
	v.z = 0.5f * ((2.0f * p1.z) + (-p0.z + p2.z) * t + (2.0f * p0.z - 5.0f * p1.z + 4.0f * p2.z - p3.z) * t * t + (-p0.z + 3.0f * p1.z - 3.0f * p2.z + p3.z) * t * t * t);
	return v;
}

HRESULT CCinematicCamera::SetUp_Components()
{
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Shader_VtxModel", L"Com_Shader", (CComponent**)&m_pShaderCom), E_FAIL);
	FAILED_CHECK_RETURN(__super::Add_Component(g_LEVEL, L"Prototype_Component_Model_CineCam", L"Com_Model", (CComponent**)&m_pModelCom, nullptr, this), E_FAIL);
	return S_OK;
}

HRESULT CCinematicCamera::SetUp_ShaderResources()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);
	FAILED_CHECK_RETURN(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ViewMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_Matrix("g_ProjMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)), E_FAIL);
	FAILED_CHECK_RETURN(m_pShaderCom->Set_RawValue("g_vCamPosition", &CGameInstance::GetInstance()->Get_CamPosition(), sizeof(_float4)), E_FAIL);
	return S_OK;
}

void CCinematicCamera::Play()
{
	m_bPlay = true;
	m_bInitSet = true;
}

void CCinematicCamera::Save_Data()
{
	string      strSaveDirectory = ImGuiFileDialog::Instance()->GetCurrentPath();

	if (m_bSaveWrite == true)
	{
		char   szDash[128] = "\\";
		strcat_s(szDash, m_strFileName.c_str());
		strSaveDirectory += string(szDash);
		strSaveDirectory += ".json";
	}
	else
	{
		string	   strSaveFileName = ImGuiFileDialog::Instance()->GetCurrentFileName();
		char   szDash[128] = "\\";
		strcat_s(szDash, strSaveFileName.c_str());
		strSaveDirectory += string(szDash);
	}

	ofstream      file(strSaveDirectory.c_str());
	Json	jCineCamKeyFrameList;

	jCineCamKeyFrameList["0_KeyFrameSize"] = m_iNumKeyFrames;

	_float		fElement = 0.f;

	for (_uint i = 0; i < m_iNumKeyFrames; ++i)
	{
		Json jChild;
		_float3 vPos = m_keyframes[i].vPos;
		_float3 vLookAt = m_keyframes[i].vLookAt;
		for (int i = 0; i < 3; ++i)
		{
			fElement = 0.f;
			memcpy(&fElement, (float*)&vPos + i, sizeof(float));
			jChild["0_Pos"].push_back(fElement);
		}

		for (int i = 0; i < 3; ++i)
		{
			fElement = 0.f;
			memcpy(&fElement, (float*)&vLookAt + i, sizeof(float));
			jChild["1_LookAt"].push_back(fElement);
		}

		jChild["2_Time"] = m_keyframes[i].fTime;

		jCineCamKeyFrameList["1_Data"].push_back(jChild);
	}

	string str = m_szChatFileName;
	jCineCamKeyFrameList["2_ChatFileName"] = str;

	file << jCineCamKeyFrameList;
	file.close();
	MSG_BOX("Save_jCineCamKeyFrameList");
}

void CCinematicCamera::Load_Data()
{
	list<CGameObject*> gameobjectList;

	string      strLoadDirectory = ImGuiFileDialog::Instance()->GetCurrentPath();   // GetCurrentPath F12로 들가면 비슷한 다른 함수 더 있음.
	string	   strLoadFileName = ImGuiFileDialog::Instance()->GetCurrentFileName();
	char   szDash[128] = "\\";
	strcat_s(szDash, strLoadFileName.c_str());
	strLoadDirectory += string(szDash);

	ifstream      file(strLoadDirectory.c_str());
	Json	jLoadCineCamKeyFrameList;
	file >> jLoadCineCamKeyFrameList;
	file.close();

	m_keyframes.clear();

	jLoadCineCamKeyFrameList["0_KeyFrameSize"].get_to<_uint>(m_iNumKeyFrames);
	_float3 vPos;
	_float3 vLookAt;
	for (auto jLoadChild : jLoadCineCamKeyFrameList["1_Data"])
	{
		CAMERAKEYFRAME Desc;
		ZeroMemory(&Desc, sizeof(CAMERAKEYFRAME));
		int k = 0;
		for (float fElement : jLoadChild["0_Pos"])
			memcpy(((float*)&vPos) + (k++), &fElement, sizeof(float));
		Desc.vPos = vPos;
		int j = 0;
		for (float fElement : jLoadChild["1_LookAt"])
			memcpy(((float*)&vLookAt) + (j++), &fElement, sizeof(float));
		Desc.vLookAt = vLookAt;
		Desc.fTime = jLoadChild["2_Time"];

		m_keyframes.push_back(Desc);
	}

	//string str;
	//jLoadCineCamKeyFrameList["2_ChatFileName"].get_to<string>(str);

	///* Chat File Load */
	//Load_ChatData(str);

}

void CCinematicCamera::Load_ChatData(string str)
{
	Json jLoad;

	string filePath = "../Bin/Data/Chat/";
	filePath += str;
	filePath += ".json";
	ifstream file(filePath);
	if (file.fail())
		return;
	file >> jLoad;
	file.close();

	//using convert_type = codecvt_utf8<wchar_t>;
	//wstring_convert<convert_type> utf8_conv;

	//_int iNumChat[10];
	//_uint i = 0;
	for (auto jSub : jLoad["Chat"])
	{
		string line;
		jSub.get_to<string>(line);
		wstring wstr = /*utf8_conv.from_bytes(line);*/ CUtile::utf8_to_wstring(line);
		m_vecChat.push_back(wstr);
		//iNumChat[i] = iNum;

		//string strSub = "Chat" + to_string(i);
		//for (size_t j = 1; j <= iNumChat[i]; ++j)
		//{
		//	for (auto jSub : jLoad[strSub])
		//	{
		//		string str = "line" + to_string(j);
		//		string line;
		//		jSub[str].get_to<string>(line);

		//		wstring wstr = utf8_conv.from_bytes(line);
		//		m_vecChat[i].push_back(wstr);
		//	}
		//}

		//++i;
	}

	/* ~Chat File Load */
}

void CCinematicCamera::Clone_Load_Data(string JsonFileName, vector<CAMERAKEYFRAME>& v, string& chatFileName)
{
	string      strLoadDirectory = "../Bin/Data/CineCam/";
	strLoadDirectory += JsonFileName;

	ifstream      file(strLoadDirectory.c_str());
	Json	jLoadCineCamKeyFrameList;
	file >> jLoadCineCamKeyFrameList;
	file.close();

	_float3 vPos;
	_float3 vLookAt;
	for (auto jLoadChild : jLoadCineCamKeyFrameList["1_Data"])
	{
		CAMERAKEYFRAME Desc;
		ZeroMemory(&Desc, sizeof(CAMERAKEYFRAME));
		int k = 0;
		for (float fElement : jLoadChild["0_Pos"])
			memcpy(((float*)&vPos) + (k++), &fElement, sizeof(float));
		Desc.vPos = vPos;
		int j = 0;
		for (float fElement : jLoadChild["1_LookAt"])
			memcpy(((float*)&vLookAt) + (j++), &fElement, sizeof(float));
		Desc.vLookAt = vLookAt;
		Desc.fTime = jLoadChild["2_Time"];

		v.push_back(Desc);
	}

	jLoadCineCamKeyFrameList["2_ChatFileName"].get_to<string>(chatFileName);

}

CCinematicCamera* CCinematicCamera::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCinematicCamera*		pInstance = new CCinematicCamera(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCinematicCamera");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CCinematicCamera::Clone(void* pArg)
{
	CCinematicCamera*		pInstance = new CCinematicCamera(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCinematicCamera");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCinematicCamera::Free()
{
	CCamera::Free();
//#ifdef _DEBUG
	Safe_Release(m_pInputLayout);
	Safe_Delete(m_pBatch);
	Safe_Delete(m_pEffect);
//#endif // _DEBUG

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);

	m_vecChat.clear();
}