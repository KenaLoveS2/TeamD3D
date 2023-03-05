#include "stdafx.h"
#include "../public/Imgui_Manager.h"
#include "Graphic_Device.h"
#include "ImGui/ImGuiFileDialog.h"
#include "ImguiObject.h"
#include "ImGui/ImGuizmo.h"
#include "Imgui_PropertyEditor.h"
IMPLEMENT_SINGLETON(CImgui_Manager)

CImgui_Manager::CImgui_Manager()
{
}

CGameObject * CImgui_Manager::Get_SelectObjectPtr()
{
	if (dynamic_cast<CImgui_PropertyEditor*>(m_pSelectViewer_ImguiObj) != nullptr)
		return static_cast<CImgui_PropertyEditor*>(m_pSelectViewer_ImguiObj)->Get_SelectedObject();

	return nullptr;
}

void CImgui_Manager::Ready_Imgui(HWND hWnd, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContextOut)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	
	(void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // Enable Multi-Viewport / Platform Windows
	//io.ConfigViewportsNoAutoMerge = true;
	io.ConfigViewportsNoTaskBarIcon = true;

	ImGui::StyleColorsDark();

	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 0.2f;
	}

	m_pDevice = pDevice;
	m_pDeviceContext = pDeviceContextOut;
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);

	ImGui_ImplWin32_Init(hWnd);
	ImGui_ImplDX11_Init(m_pDevice, m_pDeviceContext);
}

void CImgui_Manager::Tick_Imgui()
{
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.BackendFlags |= ImGuiBackendFlags_PlatformHasViewports;

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void CImgui_Manager::Render_Imgui()
{
	Render_Window();
	Render_Tab();

	ImGui::Render();
}

void CImgui_Manager::Render_Window()
{
	ImGui_DockSpace();

	for (const auto& ImWinObj : m_vecWin)
	{
		if (strcmp(ImWinObj->GetWindowName(), "##") == 0)			
		{   // FreeRender
			ImGui::Begin(ImWinObj->GetFreeRenderName());
			ImWinObj->Imgui_FreeRender();
		}
		else
		{
			ImGui::Begin(ImWinObj->GetWindowName());
			ImWinObj->Imgui_RenderWindow();
			ImGui::End();
		}
	}
	ImGui::End();
}

void CImgui_Manager::Render_Tab()
{
	if (m_vecTab.empty())
		return;

	// ÅÇ ·»´õ
	ImGui::Begin("Tab");
	if (ImGui::BeginTabBar("Manager_Tab", m_iTabBarFlags))
	{
		for (const auto& imObj : m_vecTab)
		{
			if (ImGui::BeginTabItem(imObj->GetTabName()))
			{
				imObj->Imgui_RenderTab();
				ImGui::EndTabItem();
			}
		}
		ImGui::EndTabBar();
	}
	ImGui::End();
}

void CImgui_Manager::ImGui_DockSpace()
{
	ImGuiWindowFlags			WindowFlag = ImGuiWindowFlags_NoDocking;

	const ImGuiViewport*	Viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(Viewport->WorkPos);
	ImGui::SetNextWindowSize(Viewport->WorkSize);
	ImGui::SetNextWindowViewport(Viewport->ID);
	ImGui::SetNextWindowBgAlpha(0.f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.f, 0.f));

	WindowFlag |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
	WindowFlag |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	WindowFlag |= ImGuiDockNodeFlags_PassthruCentralNode;
	WindowFlag |= ImGuiWindowFlags_NoBackground;

	_bool	bIsShow = true;

	ImGui::Begin("DockSpace", &bIsShow, WindowFlag);
	ImGui::PopStyleVar(1);
	ImGui::PopStyleVar(2);

	ImGuiIO&	io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
	{
		ImGuiID	DockSpaceID = ImGui::GetID("DockSpace");
		ImGuiDockNodeFlags Flag = ImGuiDockNodeFlags_PassthruCentralNode;
		ImGui::DockSpace(DockSpaceID, ImVec2(0.f, 0.f), Flag);
	}
}

void CImgui_Manager::Render_Update_ImGui()
{
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}
}

void CImgui_Manager::Add_ImguiObject(CImguiObject* pImguiObject, bool bIsSelectViewer )
{
	if (nullptr == pImguiObject)
		return;

	if (strcmp(pImguiObject->GetTabName(), "##")) // Tab 
		m_vecTab.push_back(pImguiObject);
	else if (strcmp(pImguiObject->GetWindowName(), "##")) // Window
		m_vecWin.push_back(pImguiObject);
	else
		m_vecWin.push_back(pImguiObject); // Free Render¿ë

	if (bIsSelectViewer == true)
		m_pSelectViewer_ImguiObj = pImguiObject;
}

CImguiObject * CImgui_Manager::Get_ImguiObject(const char * pName)
{
	auto Iter = find_if(m_vecWin.begin(), m_vecWin.end(), [&](CImguiObject* pObj)->bool
	{
		const char* pClassTag = typeid( *pObj).name();

		return !strcmp(pName, pClassTag);
	});

	if (Iter != m_vecWin.end())
		return *Iter;


	return nullptr;
}

void CImgui_Manager::Clear_ImguiObjects()
{
	for (auto& ImguiTab : m_vecTab)
		Safe_Release(ImguiTab);
	m_vecTab.clear();

	for (auto& ImguiWin : m_vecWin)
		Safe_Release(ImguiWin);
	m_vecWin.clear();
}

void CImgui_Manager::Free()
{
	Clear_ImguiObjects();

	Safe_Release(m_pDevice);
	Safe_Release(m_pDeviceContext);
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}
