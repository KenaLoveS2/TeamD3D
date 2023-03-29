#pragma once
#include "ImguiObject.h"
#include "Effect_Base.h"

BEGIN(Engine)
class CComponent;
class CGameObject;
class CLayer;
END

BEGIN(Client)

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
	void				LayerChild_ListBox(OUT char**& pObjectTag, OUT _uint& iHaveChildSize, OUT char*& pSelectObjectTag, const char* pLabelTag, _int& iSelectObject, CEffect_Base* pEffect, vector<CEffect_Base*>* pChild, TAGTYPE eTag);

	// Color // 
	_float4	Set_ColorValue();

	// MovePosition_Rect // 
	void	Set_OptionWindow_Rect(class CEffect_Base* pEffect);
	void	Set_OptionWindow_Particle(_int& iCreateCnt, class CEffect_Base* pEffect);

	// Child Setting //
	void	Set_Child(class CEffect_Base* pEffect);

	// Trail Setting //
	void	Set_Trail(class CEffect_Base* pEffect);

	// FreeMoving Setting //
	void	Set_FreePos(class CEffect_Base* pEffect);
	void   	Set_MeshType(OUT CEffect_Base::EFFECTDESC& effectType, _int & iSelectMeshType);

	void	TransformView(_int iSelectObject, class CEffect_Base* pEffect);
	void	TransformView_child(_int iSelectObject, class CEffect_Base* pEffect);

public:
	void	CreateEffect_Plane(_int& iCurSelect, _int& iSelectObject);
	void	CreateEffect_Particle(_int& iCreateCnt, _int& iCurSelect, _int& iSelectObject);
	void	CreateEffect_Mesh(_int& iSelectObject);

public:
	void	Show_ChildWindow(class CEffect_Base* pEffect);
	void	Show_TrailWindow(class CEffect_Base* pEffect);

public:
	HRESULT Save_EffectData();
	HRESULT Load_EffectData();

private:
	CEffect_Base::EFFECTDESC m_eEffectDesc;

	_bool m_bIsRectLayer = false;
	_bool m_bIsParticleLayer = false;
	_bool m_bIsMeshLayer = false;

public:
	static CImgui_Effect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg = nullptr);
	virtual void          Free() override;
};

END