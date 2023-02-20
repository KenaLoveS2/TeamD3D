#include "stdafx.h"
#include "..\public\Imgui_Effect.h"

CImgui_Effect::CImgui_Effect(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CImguiObject(pDevice, pContext)
{
}

HRESULT CImgui_Effect::Initialize(void * pArg)
{
	m_szWindowName = "EffectTool";
	return S_OK;
}

void CImgui_Effect::Imgui_RenderWindow()
{

}

CImgui_Effect* CImgui_Effect::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, void * pArg)
{
	CImgui_Effect * pInstance = new CImgui_Effect(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CImgui_Effect Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CImgui_Effect::Free()
{
}
