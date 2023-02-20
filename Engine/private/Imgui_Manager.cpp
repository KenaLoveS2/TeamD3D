#include "stdafx.h"
#include "../public/Imgui_Manager.h"
#include "Graphic_Device.h"
#include "ImGui/ImGuiFileDialog.h"
#include "ImguiObject.h"
#include "ImGui/ImGuizmo.h"

IMPLEMENT_SINGLETON(CImgui_Manager)

CImgui_Manager::CImgui_Manager()
{
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
	static bool opt_fullscreen = true;
	static bool opt_padding = false;
	static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;

	if (opt_fullscreen)
	{
		const ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->WorkPos);
		ImGui::SetNextWindowSize(viewport->WorkSize);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::SetNextWindowBgAlpha(0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		window_flags |= ImGuiDockNodeFlags_PassthruCentralNode;
	}
	else
	{
		dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
	}

	if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
		window_flags |= ImGuiWindowFlags_NoBackground;

	if (!opt_padding)
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

	bool IsShow = true;

	ImGui::Begin("DockSpace Demo", &IsShow, window_flags);
	ImGui::GetStyle().Colors[ImGuiCol_WindowBg].w = 0.3f; // À©µµ¿ì ¾ËÆÄ °ª

	if (!opt_padding)
		ImGui::PopStyleVar();

	if (opt_fullscreen)
		ImGui::PopStyleVar(2);

	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
	{
		ImGuiID dockspace_id = ImGui::GetID("MyDockspace");
		ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
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

void CImgui_Manager::Add_ImguiObject(CImguiObject* pImguiObject)
{
	if (nullptr == pImguiObject)
		return;

	if (strcmp(pImguiObject->GetTabName(), "##")) // Tab 
		m_vecTab.push_back(pImguiObject);
	else if (strcmp(pImguiObject->GetWindowName(), "##")) // Window
		m_vecWin.push_back(pImguiObject);
	else
		m_vecWin.push_back(pImguiObject); // Free Render¿ë
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
