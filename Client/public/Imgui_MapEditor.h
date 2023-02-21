#pragma once
#include "Client_Defines.h"
#include "ImguiObject.h"
#include "EnviromentObj.h"

BEGIN(Engine)
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

private:
	HRESULT	Ready_For_MapString();

	void			Imgui_SelectOption();
	void			Imgui_CreateEnviromentObj();
	void			Imgui_Save_Load_Json();
	void			Imgui_ModelProtoFinder();
	
	void			Imgui_Save_Func();
	HRESULT			Imgui_Load_Func();


private: /*For_Tool*/
	string		m_strChapter[MAX_CHATERNUM];
	wstring		m_wstrProtoName  = TEXT("");
	wstring		m_wstrModelName = TEXT("");
	wstring		m_wstrTexturelName = TEXT("");
	char			m_strCloneTag[CLONE_TAG_BUFF_SIZE] = "";



private:	/*Use_Data*/
	_int	m_iChapterOption = static_cast<_uint>(CEnviromentObj::CHAPTER_END);





public:
	static	CImgui_MapEditor*	Create(ID3D11Device* pDevice, ID3D11DeviceContext*	pContext, void* pArg = nullptr);
	virtual void				Free() override;
};
END