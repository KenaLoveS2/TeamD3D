#pragma once
#include "ImguiObject.h"
#include "Effect_Base.h"

class CImgui_Effect final : public CImguiObject
{
public:
	enum CREATETYPE { EFFECT_PLANE = 1, EFFECT_PARTICLE, EFFECT_MESH, EFFECT_END };
	enum TAGTYPE { TAG_CLONE, TAG_PROTO, TAG_END };

private:
	CImgui_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void    Imgui_RenderWindow() override;

public:
	// "Layer_Effect" => ListBox
	class CGameObject*	LayerEffects_ListBox(OUT char**& pObjectTag, OUT _uint& iLayerSize, OUT char*& pSelectObjectTag,
								const char* pLabelTag, _int& iSelectObject, TAGTYPE eTag);
	void	LayerChild_ListBox(OUT char**& pObjectTag, OUT _uint& iHaveChildSize, OUT char*& pSelectObjectTag, _int& iSelectObject, class CEffect_Base* pEffect);

	// Color // 
	_float4	Set_ColorValue();

	// MovePosition_Rect // 
	void	Set_OptionWindow_Rect(class CEffect_Base* pEffect);
	void	Set_OptionWindow_Particle(class CEffect_Base* pEffect);

	// Child Setting //
	void	Set_Child(class CEffect_Base* pEffect);

	void	TransformView(_int iSelectObject, class CEffect_Base* pEffect);
	void	TransformView_child(_int iSelectObject, class CEffect_Base* pEffect);

public:
	void	CreateEffect_Plane(_int& iCurSelect, _int& iSelectObject);
	void	CreateEffect_Particle(_int& iCurSelect, _int& iSelectObject);
	void	CreateEffect_Mesh(_int iSelectObject);

private:
	CEffect_Base::EFFECTDESC m_eEffectDesc;

	_bool m_bIsRectLayer = false;
	_bool m_bIsParticleLayer = false;

public:
	static CImgui_Effect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg = nullptr);
	virtual void          Free() override;
};

