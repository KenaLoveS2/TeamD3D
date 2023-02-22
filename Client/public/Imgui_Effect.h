#pragma once
#include "ImguiObject.h"
#include "Effect_Base.h"

class CImgui_Effect final : public CImguiObject
{
public:
	enum CREATETYPE { EFFECT_PLANE = 1, EFFECT_PARTICLE, EFFECT_MESH, EFFECT_END };

private:
	CImgui_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void    Imgui_RenderWindow() override;

public:
	void	LayerEffects_ListBox(_bool& bIsCreate, _int& iSelectObject, _int& iSelectEffectType);
	void	Set_ColorValue(OUT _float4& vColor);
	void	Set_OptionWindow_Rect(class CEffect_Base* pEffect);

public:
	void	CreateEffect_Plane(_int& iSelectObject);
	void	CreateEffect_Particle(_int iSelectObject);
	void	CreateEffect_Mesh(_int iSelectObject);

private:
	CEffect_Base::EFFECTDESC m_eEffectDesc;

	_bool m_bIsRectLayer = false;

public:
	static CImgui_Effect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg = nullptr);
	virtual void          Free() override;
};

