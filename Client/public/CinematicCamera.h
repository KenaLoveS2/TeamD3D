#pragma once

#include "Client_Defines.h"
#include "Camera.h"
#include "Delegator.h"
#include "UI_ClientManager.h"

BEGIN(Engine)
class CRenderer;
class CShader;
class CModel;
END

BEGIN(Client)

class CCinematicCamera : public CCamera
{
public:
	struct CAMERAKEYFRAME
	{
		_float3 vPos;
		_float3 vLookAt;
		_float	fTime;
	};

private:
	CCinematicCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCinematicCamera(const CCinematicCamera& rhs);
	virtual ~CCinematicCamera() = default;

public:
	virtual HRESULT		Initialize_Prototype() override;
	virtual HRESULT		Initialize(void* pArg)override;
	virtual void		Tick(_float TimeDelta) override;
	virtual void		Late_Tick(_float TimeDelta) override;
	virtual HRESULT		Render() override;
	virtual void		Imgui_RenderProperty() override;
	void				Play();

public:
	Delegator<CUI_ClientManager::UI_PRESENT, _bool, _float, wstring>	m_CinemaDelegator;
	void		Load_ChatData(string str);
	void		CinemaUIOff();

private:
	void		AddKeyFrame(CAMERAKEYFRAME keyFrame);
	void		Interpolate(float time, _float3& position, _float3& lookAt);
	XMFLOAT3	CatmullRomInterpolation(_float3 p0, _float3 p1, _float3 p2, _float3 p3, float t);
	HRESULT		SetUp_Components();
	HRESULT		SetUp_ShaderResources();
	void		Calc_Ratio();

private:
	void		Save_Data();
	void		Load_Data();

public:
	static void Clone_Load_Data(string JsonFileName, vector<CAMERAKEYFRAME>& v, string& chatFileName);
	_bool		CameraFinishedChecker(_float fRatio = 0.95f);

private:
	vector<CAMERAKEYFRAME>		m_keyframes;
	_bool						m_bPlay			= false;
	_float						m_fDeltaTime	= 0.f;
	_float						m_fInputTime	= 0.5f;

private:
	CRenderer*					m_pRendererCom	= nullptr;
	CShader*					m_pShaderCom	= nullptr;
	CModel*						m_pModelCom		= nullptr;

private:
	char								m_szChatFileName[MAX_PATH] = "";
	vector<wstring>						m_vecChat;
	_int								m_iChatIndex;
	_bool								m_bDebugRender = false;
#pragma region Render Variable
#ifdef _DEBUG
private:
	PrimitiveBatch<VertexPositionColor>*		m_pBatch		= nullptr;
	BasicEffect*								m_pEffect		= nullptr;
	ID3D11InputLayout*							m_pInputLayout	= nullptr;
	_float4										m_vColor;
#endif // _DEBUG

	_bool										m_bInitSet		= false;
	class CCamera*								m_pPlayerCam	= nullptr;
	_bool										m_bPausePlay	= false;
	_uint										m_iNumKeyFrames = 0;
	_bool										m_bSaveWrite	= false;
	string										m_strFileName;
	_float										m_fChatRatio = 0.f;
	_float										m_fDurationRatio = 0.f;
	_float										m_fNumChat = 0.f;
	_bool										m_bFinished = false;
	_bool										m_bFinishSet = false;

public:
	static CCinematicCamera*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg = nullptr) override;
	virtual void				Free() override;
};

END