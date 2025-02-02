#pragma once
#include "Base.h"
#include <typeinfo>  //typeid를 사용을 위해

BEGIN(Engine)
class CImguiObject;

class CImgui_Manager final : public CBase
{
	DECLARE_SINGLETON(CImgui_Manager)

private:
	CImgui_Manager();
	virtual ~CImgui_Manager() = default;

public:
	class CGameObject* Get_SelectObjectPtr();

public:
	void Ready_Imgui(HWND hWnd, ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut);
	void Tick_Imgui();
	void Render_Imgui();
	void Render_Update_ImGui();


public:
	void Render_Tab();
	void Render_Window();
	void ImGui_DockSpace();

	// imgui object를 새로운 window로 추가한다.
	void Add_ImguiObject(CImguiObject* pImguiObject,bool bIsSelectViewer=false);			// true 일때 Viewr에 있는 포인터 얻어올려고

	class CImguiObject* Get_ImguiObject(const char* pName);

public:
	// 현재 사용중인 imgui object를 모두 삭제한다.
	void Clear_ImguiObjects();

private:
	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pDeviceContext = nullptr;

	vector<CImguiObject*> m_vecTab;
	vector<CImguiObject*> m_vecWin;

	ImGuiWindowFlags m_iWindowFlags = ImGuiWindowFlags_MenuBar;
	ImGuiTabBarFlags m_iTabBarFlags = ImGuiTabBarFlags_None;

	class		CImguiObject* m_pSelectViewer_ImguiObj = nullptr;

public:
	virtual void Free() override;
};

END


