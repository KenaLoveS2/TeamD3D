#pragma once
#include "Base.h"
#include "Input_Device.h"
#include "Component_Manager.h"
#include "PipeLine.h"
#include "Sound_Manager.h"
#include "Utile.h"
#include "Function_Manager.h"
#include "UI_Manager.h"
#include "Enviroment_Manager.h"
#include "PhysX_Manager.h"
#include "FSMComponent.h"
#include "Graphic_Device.h"
#include "Animation.h"
#include "Bone.h"

#define CONTEXT_LOCK CContext_LockGuard _CtxLock_(CGameInstance::GetInstance()->GetContextMtx());
#define TIMEDELTA CGameInstance::GetInstance()->GetTimeDelta()

BEGIN(Engine)
class ENGINE_DLL CGameInstance final : public CBase
{
	DECLARE_SINGLETON(CGameInstance);
private:
	CGameInstance();
	virtual ~CGameInstance() = default;

public:
	static _uint Get_StaticLevelIndex()
	{	
		return m_iStaticLevelIndex;
	}

	_uint Get_CurLevelIndex();

	HWND GetHWND() { return m_hClientWnd; }
	_float GetTimeDelta() { return m_fTimeDelta; }

public: /* For.GameInstance */
	static const _tchar* m_pPrototypeTransformTag;

public: /* For.GameInstance */
	HRESULT Initialize_Engine(HINSTANCE hInst, _uint iNumLevels, const GRAPHIC_DESC& GraphicDesc,
							  ID3D11Device** ppDeviceOut, ID3D11DeviceContext** ppContextOut);
	void Tick_Engine(_bool bWinActive, _float fTimeDelta);
	void Clear_Level(_uint iLevelIndex, _bool bCamreaClearFlag = false);

public: /* For.Graphic_Device */
	mutex& GetContextMtx() const;
	HRESULT Clear_Graphic_Device(const _float4* pColor);
	HRESULT Present();
	HRESULT Update_SwapChain(HWND hWnd, _uint iWinCX, _uint iWinCY, _bool bIsFullScreen, _bool bNeedUpdate);

public: /* For.Input_Device */
	_byte Get_DIKeyState(_ubyte byKeyID);
	_byte Get_DIMouseState(MOUSEKEYSTATE byMouseID);
	_long Get_DIMouseMove(MOUSEMOVESTATE eMoveState);
	_float		Get_KeyChargeTime(_ubyte byKeyID);
	_bool		Mouse_Down(MOUSEKEYSTATE MouseButton);
	_bool		Mouse_Up(MOUSEKEYSTATE MouseButton);
	_bool		Mouse_DoubleClick(MOUSEKEYSTATE MouseButton);
	_bool		Mouse_Pressing(MOUSEKEYSTATE MouseButton);
	_bool		Key_Pressing(_ubyte byKeyID);
	_bool		Key_Down(_ubyte byKeyID);
	_bool		Key_DoubleDown(_ubyte byKeyID);
	_bool		Key_Up(_ubyte byKeyID);
	_bool		Key_Charge(_ubyte byKeyID, _float fTime);
	void		Reset_EveryKey(_float fTimeDelta);

public: /* For.Level_Manager */
	HRESULT Open_Level(_uint iLevelIndex, class CLevel* pNewLevel);
	HRESULT Render_Level();

public: /* For.Object_Manager */
	class CComponent* Get_ComponentPtr(_uint iLevelIndex, const _tchar* pLayerTag, const _tchar* pCloneObjectTag, const _tchar* pComponentTag);
	class CGameObject* Get_GameObjectPtr(_uint iLevelIndex, const _tchar* pLayerTag, const _tchar* pCloneObjectTag);
	map<const _tchar*, class CGameObject*>*	Get_AnimObjects(_uint iLevelIndex);
	map<const _tchar*, class CGameObject*>*	Get_ShaderValueObjects(_uint iLevelIndex);
	map <const _tchar*, class CGameObject*>* Get_GameObjects(_uint iLevelIndex, const _tchar* pLayerTag);


	HRESULT Add_Prototype(const _tchar* pPrototypeTag, class CGameObject* pPrototype);
	CGameObject* Clone_GameObject(const _tchar* pPrototypeTag, const _tchar* pCloneObjectTag = nullptr, void* pArg = nullptr);
	HRESULT Clone_GameObject(_uint iLevelIndex, const _tchar* pLayerTag, const _tchar* pPrototypeTag, const _tchar * pCloneObjectTag,
							void* pArg = nullptr, CGameObject** ppObj = nullptr);
	HRESULT		Clone_AnimObject(_uint iLevelIndex, const _tchar* pLayerTag, const _tchar* pPrototypeTag,
		const _tchar* pCloneObjectTag,
		void* pArg = nullptr, CGameObject** ppOut = nullptr);
	HRESULT		Add_AnimObject(_uint iLevelIndex, class CGameObject* pGameObject);
	HRESULT		Add_ShaderValueObject(_uint iLevelIndex, CGameObject * pGameObject);
	HRESULT Add_ClonedGameObject(_uint iLevelIndex, const _tchar* pLayerTag, const _tchar* pCloneObjectTag, CGameObject* pGameObject);
	class CLayer* Find_Layer(_uint iLevelIndex, const _tchar* pLayerTag);
	HRESULT	Late_Initialize(_uint iLevelIndex);

	void Imgui_ProtoViewer(_uint iLevel, const _tchar*& szSelectedProto);
	void Imgui_ObjectViewer(_uint iLevel, CGameObject*& pSelectedObject);
	void Imgui_DeleteComponentOrObject(OUT class  CGameObject*& pSelectedObject);
	void Imgui_Push_Group(class CGameObject* pSelectedObject);	// 다중 컴포넌트 상속, 삭제

	map<const _tchar*, class CGameObject*>& Get_ProtoTypeObjects();

	vector<map<const _tchar*, class CGameObject*>>& Get_CopyPrototypes();
	_uint Get_NumCopyPrototypes();


	HRESULT Delete_Object(_uint iLevelIndex, const _tchar * pLayerTag, const _tchar * pCloneObjectTag);
	void		  RoomIndex_Object_Clear(_int iCurLevel, const _tchar* LayerTag, _int iRoomIndex);

	void		Set_SingleLayer(_uint iCurLevel, const _tchar* pLayerTag);
	void		Get_Back();
	


public: /* For.Component_Manager */
	HRESULT Add_Prototype(_uint iLevelIndex, const _tchar* pPrototypeTag, class CComponent* pPrototype);
	class CComponent* Clone_Component(_uint iLevelIndex, const _tchar* pPrototypeTag, void* pArg = nullptr, class CGameObject* pOwner = nullptr);
	map<const _tchar*, class CComponent*>*		Get_ComponentProtoType();
	void		Delete_ProtoComponent(_uint iLevelIndex, const _tchar* pProtoTypeTag);

public: /* For.PipeLine */
		_matrix Get_TransformMatrix(CPipeLine::TRANSFORMSTATE eState);
		_float4x4 Get_TransformFloat4x4(CPipeLine::TRANSFORMSTATE eState);
		_matrix Get_TransformMatrix_Inverse(CPipeLine::TRANSFORMSTATE eState);
		_float4x4 Get_TransformFloat4x4_Inverse(CPipeLine::TRANSFORMSTATE eState);
		void Set_Transform(CPipeLine::TRANSFORMSTATE eState, _fmatrix TransformMatrix);
	
		_float4 Get_CamPosition();
		_float4 Get_LightCamPosition();
		_float4 Get_LightCamLook();

		_float4		Get_CamRight_Float4();
		_float4		Get_CamUp_Float4();
		_float4		Get_CamLook_Float4();
		_float3		Get_CamRight_Float3();
		_float3		Get_CamUp_Float3();
		_float3		Get_CamLook_Float3();
		_float3		Get_CamPosition_Float3();

	public: /* For.Timer_Manager */
		_float Get_TimeDelta(const _tchar* pTimerTag);
		HRESULT Ready_Timer(const _tchar* pTimerTag);
		void Update_Timer(const _tchar* pTimerTag);
		const _float	Get_TimeRate(const _tchar* pTimerTag);
		void			Set_TimeRate(const _tchar* pTimerTag, _float Time);
		void			Set_TimeSleep(const _tchar* pTimerTag, _float fTimeSleep);

	public: /* For.Light_Manager */
		const LIGHTDESC* Get_LightDesc(_uint iIndex);
		class CLight* Get_Light(_uint iIndex);
		HRESULT Add_Light(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const LIGHTDESC& LightDesc, class CLight** ppOut = nullptr);
		void Imgui_LightManagerRender();
		void Clear();

	public: /* For.Font_Manager */
		HRESULT Add_Font(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pFontTag,
						 const _tchar* pFontFilePath);
		HRESULT Render_Font(const _tchar* pFontTag, const _tchar* pText, const _float2& vPos, _float fRadian,
							_float2 vScale, _fvector vColor = XMVectorSet(1.f, 1.f, 1.f, 1.f));

	public: /* For.Frustum */
		_bool isInFrustum_WorldSpace(_fvector vWorldPos, _float fRange = 0.f);
		_bool isInFrustum_LocalSpace(_fvector vLocalPos, _float fRange = 0.f);
		_float isInFrustum_WorldSpace(_int iPlaneIndex, _fvector vWorldPos);
	public: /* For.Target_Manager */
		ID3D11ShaderResourceView* Get_DepthTargetSRV();

	public: /* For.Sound_Manager */
		_int Play_Sound(const _tchar* pSoundKey, _float fVolume, _bool bIsBGM = false, _int iManualChannelIndex = -1);
		_int Play_ManualSound(const _tchar* pSoundKey, _float fVolume, _bool bIsBGM = false, _int iManualChannelIndex = -1);
		void Stop_Sound(_uint iManualChannelIndex);
		void Set_Volume(_uint iManualChannelIndex, _float fVolume);
		void Set_MasterVolume(_float fVolume);
		void Set_SoundDesc(const _tchar* pSoundKey, CSound::SOUND_DESC& SoundDesc);
		_bool Is_StopSound(_uint iManualChannelIndex);
		HRESULT Copy_Sound(_tchar* pOriginSoundKey, _tchar* pCopySoundKeyOut);
		void Stop_All();

	public: // for imgui manager
		void Render_ImGui();
		void Render_Update_ImGui();
		void Add_ImguiObject(class CImguiObject* pImguiObject, bool bIsSelectViewer = false);
		void Clear_ImguiObjects();
		class CGameObject* Get_SelectObjectPtr();
		class CImguiObject* Get_ImguiObject(const char* pName);


	public: // for String manager
		HRESULT Add_String(_uint iLevelIndex, _tchar* pStr);
		HRESULT Add_String(_tchar* pStr);
		_tchar* Find_String(_uint iLevelIndex, _tchar* pStr);
		/* Only For UI */
		void				Add_UIString(_uint iKey, const string& str);
		vector<string>*		Get_UIString(_uint iKey);
		void				Add_UIWString(_uint iKey, const wstring& str);
		vector<wstring>*	Get_UIWString(_uint iKey);


	public: // for Camera manager
		HRESULT Add_Camera(const _tchar* pCameraTag, class CCamera* pCamrea, _bool bWorkFlag = false);
		HRESULT Work_Camera(const _tchar* pCameraTag);
		class CCamera* Find_Camera(const _tchar* pCameraTag);
		class CCamera* Get_WorkCameraPtr();

		HRESULT Add_LightCamera(const _tchar* pCameraTag, class CCamera* pCamrea, _bool bWorkFlag = false);
		HRESULT Work_LightCamera(const _tchar* pCameraTag);
		class CCamera* Find_LightCamera(const _tchar* pCameraTag);

		_float*		Get_CameraFar();
		_float*		Get_CameraNear();
		_float*		Get_CameraFov();
		map<const _tchar*, class CCamera*>*	Get_CameraContainer();
		_bool IsWorkCamera(const _tchar * pCameraTag);
		HRESULT Camera_SceneClear();
	public:		/* For Function Manager */
		template<typename T>
		HRESULT		Add_Function(T* pObj, const _tchar* pFuncName, void (T::*memFunc)(_bool, _float)) {
			NULL_CHECK_RETURN(m_pFunction_Manager, E_FAIL);
			return m_pFunction_Manager->Add_Function(pObj, pFuncName, memFunc);
		};
		HRESULT		Call_Function(CBase* pObj, const _tchar* pFuncName, _float fTimeDelta);

// 	public:		/* For Group Manager */
// 		HRESULT					Add_Member(const wstring& wstrGroupName, class CGameObject* pObject);
// 
// 		template<typename T>
// 		_bool					Check_MemberState_byFunction(const wstring& wstrGroupName, _bool (T::* memFunc)());
// 
// 		template<typename T>
// 		void					SetState_byFunction(const wstring& wstrGroupName, void (T::* memFunc)(_bool), _bool bValue);


	public: // for m_pEnviroment_Manager
		void	Set_PlayerPtr(class CGameObject* pPlayer);
		_bool	Is_RenderIndex(_uint iObjRoomIndex);
		_bool	Is_Render_TerrainIndex(_uint iTerrainRoomIndex);
		_uint Get_CurrentPlayerRoomIndex();
		void	Scene_EnviMgr_Change();
public:// For.Texture_Manager
	HRESULT	   Texture_Manager_SceneChange();
	HRESULT		Texture_Erase(const _tchar* pTextureFilePath);

private:
		static _uint m_iStaticLevelIndex;
		HWND m_hClientWnd = NULL;
		_float m_fTimeDelta = 0.f;

	private:
		class CGraphic_Device* m_pGraphic_Device = nullptr;
		class CInput_Device* m_pInput_Device = nullptr;
		class CLevel_Manager* m_pLevel_Manager = nullptr;
		class CObject_Manager* m_pObject_Manager = nullptr;
		class CComponent_Manager* m_pComponent_Manager = nullptr;
		class CPipeLine* m_pPipeLine = nullptr;
		class CTimer_Manager* m_pTimer_Manager = nullptr;
		class CLight_Manager* m_pLight_Manager = nullptr;
		class CFont_Manager* m_pFont_Manager = nullptr;
		class CFrustum* m_pFrustum = nullptr;
		class CTarget_Manager* m_pTarget_Manager = nullptr;
		class CSound_Manager* m_pSound_Manager = nullptr;
		class CImgui_Manager* m_pImgui_Manager = nullptr;
		class CString_Manager* m_pString_Manager = nullptr;
		class CCamera_Manager* m_pCamera_Manager = nullptr;
		class CEnviroment_Manager* m_pEnviroment_Manager = nullptr;
		class CPostFX* m_pPostFX = nullptr;
		class CFunction_Manager*	m_pFunction_Manager = nullptr;
		//class CGroup_Manager* m_pGroup_Manager = nullptr;
		class CPhysX_Manager* m_pPhysX_Manager = nullptr;
		class CTexture_Manager* m_pTexture_Manager = nullptr;
	public:
		static void Release_Engine();

	public:
		virtual void Free() override;
};

END