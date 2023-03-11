#pragma once
#include "Client_Defines.h"
#include "ImguiObject.h"
#include "EnviromentObj.h"

BEGIN(Engine)
class CGameInstance;
class CEnviromentObj;
class CTransform;
END

#define		MAX_CHATERNUM			2
#define		CLONE_TAG_BUFF_SIZE			64

BEGIN(Client)
class CImgui_MapEditor  final : public CImguiObject
{

private:
	CImgui_MapEditor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	
public:
	virtual HRESULT Initialize(void* pArg = nullptr);
	virtual void Imgui_FreeRender();

public:
	static				void									Load_MapObjects(_uint iLevel, string JsonFileName);
	static				void									Load_ComTagToCreate(class CGameInstance *pGameInstace, class CGameObject* pGameObject, vector<string> vecStr);
	static				void									Imgui_Instacing_PosLoad(CGameObject* pSelectEnvioObj, vector<_float4x4>	vecMatrixVec, CEnviromentObj::CHAPTER eChapter);

private:
	void														Imgui_SelectOption();						// 컴포넌트 기능 선택
	void														Imgui_Save_Load_Json();
	void														Imgui_CreateEnviromentObj();
	void														Imgui_ViewMeshOption(class CGameObject* pSelecteObj);
	void														Imgui_SelectObject_InstancingControl();
	void														Imgui_Control_ViewerCamTransform();
	void														Imgui_TexturePathNaming();
	void														Imgui_Instancing_control(CGameObject*	pSelectEnviObj);
	void														imgui_ObjectList_Clear();
	void														imgui_Gimmic_Class_Viewr();


private:		/*Logic*/
	void														Imgui_Save_Func();
	HRESULT														Imgui_Load_Func();
	void														Imgui_AddComponentOption_CreateCamFront(class CGameInstance *pGameInstace, class CGameObject* pGameObject);
	void														Imgui_Create_Option_Reset();

	void														Imgui_Maptool_Terrain_Selecte();


	void										Imgui_Crystal_Create_Pulse();

private: /*For_Tool*/
	string														m_strChapter[MAX_CHATERNUM];
	wstring														m_wstrProtoName = TEXT("");
	wstring														m_wstrModelName = TEXT("");
	wstring														m_wstrTexturelName = TEXT("");
	char														m_strCloneTag[CLONE_TAG_BUFF_SIZE] = "";

	_bool														m_bWireFrame = false;
	string														m_strFileName = "";
	_bool														m_bSaveWrite = false;
	class CModelViewerObject*									m_pViewerObject = nullptr;
	_bool														m_bOnceSearch = false;
	_bool														m_bModelChange = false;
	_int														m_iTexturePathNum = false;

	_int														m_iSelectMeshIndex = -1;


private: /*Use.Terrain*/
	class					CTerrain*							m_pSelectedTerrain = nullptr;
	_bool														m_bUseTerrainPicking = false;
	_bool														m_bIstancingObjPicking = false;


private:	/*Use_Data*/
	_int														m_iChapterOption = static_cast<_uint>(CEnviromentObj::Gimmick_TYPE_NONE);
	_int														m_iCreateObjRoom_Option = 0;
	array<_bool, CEnviromentObj::COMPONENTS_END>				m_bComOptions;

public:
	static	CImgui_MapEditor*									Create(ID3D11Device* pDevice, ID3D11DeviceContext*	pContext, void* pArg = nullptr);
	virtual void												Free() override;

};
END