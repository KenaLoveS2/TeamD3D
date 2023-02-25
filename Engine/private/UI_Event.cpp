#include "stdafx.h"
#include "..\public\UI_Event.h"
#include "Utile.h"


CUI_Event::CUI_Event()
	: m_fTime(0.1f)
	, m_fTimeAcc(0.f)
{
}

void CUI_Event::Set_UIName(const _tchar * cloneTag)
{
}

HRESULT CUI_Event::SetUp_ShaderResources(CShader * pShader)
{
	return S_OK;
}

void CUI_Event::Imgui_RenderProperty()
{
}



void CUI_Event::Free()
{
}
