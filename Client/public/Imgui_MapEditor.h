#pragma once
#include "Client_Defines.h"
#include "ImguiObject.h"
#include "EnviromentObj.h"

BEGIN(Engine)
class CEnviromentObj;
END

#define		MAX_CHATERNUM			2




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



private: /*For_Tool*/
	string		m_strChapter[MAX_CHATERNUM];


private:	/*Use_Data*/
	_int	m_iChapterOption = static_cast<_uint>(CEnviromentObj::CHAPTER_END);





public:
	static	CImgui_MapEditor*	Create(ID3D11Device* pDevice, ID3D11DeviceContext*	pContext, void* pArg = nullptr);
	virtual void				Free() override;
};
END