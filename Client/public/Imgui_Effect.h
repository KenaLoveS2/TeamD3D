#pragma once
#include "ImguiObject.h"
#include "Effect_Base.h"

class CImgui_Effect final : public CImguiObject
{
private:
	CImgui_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void    Imgui_RenderWindow() override;

private:
// 	CEffect_Base* m_pEffect = nullptr;
// 	CEffect_Base::EFFECTDESC m_eEffectDesc;

public:
	static CImgui_Effect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg = nullptr);
	virtual void          Free() override;
};

