#pragma once

#define IMGUI_LEFT_LABEL(func, label, ...) (ImGui::TextUnformatted(label), ImGui::SameLine(), func("##" label, __VA_ARGS__))
#define IMGUI_DEFINE_MATH_OPERATORS
#include "ImGui\imgui.h"
#include "ImGui\imgui_impl_win32.h"
#include "ImGui\imgui_impl_dx11.h"
#include "ImGui\ImGuizmo.h"
#include "ImGui\ImGuiFileDialog.h"
#include "ImGui\Imgui_stdlib.h"

#pragma warning (disable : 4005)
#pragma warning (disable : 4251)

//#define USE_QUADTREE

#include <d3d11.h>
#include <Effects11/d3dx11effect.h>
#include <DirectXTK/DDSTextureLoader.h>
#include <DirectXTK/WICTextureLoader.h>
#include <DirectXTK/ScreenGrab.h>

#include <DirectXTK/PrimitiveBatch.h>
#include <DirectXTK/VertexTypes.h>
#include <DirectXTK/Effects.h>

#include <DirectXTK/SpriteBatch.h>
#include <DirectXTK/SpriteFont.h>

#include <d3dcompiler.h>
#include <typeinfo>
#include <DirectXCollision.h>

#include <mfidl.h>
#pragma comment(lib, "Mfplat.lib")
#pragma comment(lib, "Mfuuid.lib")

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <DirectXMath.h>
using namespace DirectX;

#include <DirectXTK\SimpleMath.h>

using namespace SimpleMath;

#include <vector>
#include <list>
#include <map>
#include <unordered_map>
#include <algorithm>
#include <array>
using namespace std;

#include <Json/json_fwd.hpp>
using Json = nlohmann::json;
using namespace nlohmann;

#define PX_PHYSX_CHARACTER_STATIC_LIB
#include "PhysX/PxPhysics.h"
#include "PhysX/PxPhysicsAPI.h"


using namespace physx;

#include "Engine_Macro.h"
#include "Engine_Typedef.h"
#include "Engine_Function.h"
#include "Engine_Struct.h"
#include "Engine_Enum.h"

#ifdef _DEBUG

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifndef DBG_NEW 

#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ ) 
#define new DBG_NEW 

#endif

#endif // _DEBUG

using namespace Engine;
