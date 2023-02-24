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
	static				void					Load_MapObjects(_uint iLevel);
	static				void					Load_ComTagToCreate(class CGameInstance *pGameInstace, class CGameObject* pGameObject, vector<string> vecStr);
	static				void					Insert_TextureFilePath(class CGameInstance *pGameInstace, CEnviromentObj::tagEnviromnetObjectDesc& EnviromentDesc, vector<string> vecStr);
private:
	void			Imgui_SelectOption();						// 컴포넌트 기능 선택
	void			Imgui_CreateEnviromentObj();
	void			Imgui_Save_Load_Json();
	void			Imgui_SelectObject_Add_TexturePath();


private:		/*Logic*/
	void					Imgui_Save_Func();
	HRESULT			Imgui_Load_Func();
	void					Imgui_AddComponentOption_CreateCamFront(class CGameInstance *pGameInstace ,class CGameObject* pGameObject);
	void					Imgui_Create_Option_Reset();
	



private: /*For_Tool*/
	string		m_strChapter[MAX_CHATERNUM];
	wstring		m_wstrProtoName  = TEXT("");
	wstring		m_wstrModelName = TEXT("");
	wstring		m_wstrTexturelName = TEXT("");
	char			m_strCloneTag[CLONE_TAG_BUFF_SIZE] = "";

	_bool		m_bWireFrame = false;
	string      m_strFileName = "";
	_bool		m_bSaveWrite = false;

	//char		szSaveFileName[MAX_PATH] = "";
	//char		szSaveFileName[MAX_PATH]

private:	/*Use_Data*/
	_int			m_iChapterOption = static_cast<_uint>(CEnviromentObj::CHAPTER_END);
	_int			m_iCreateObjRoom_Option = 0;
	array<_bool, CEnviromentObj::COMPONENTS_END>	m_bComOptions;

	



public:
	static	CImgui_MapEditor*	Create(ID3D11Device* pDevice, ID3D11DeviceContext*	pContext, void* pArg = nullptr);
	virtual void				Free() override;
};
END