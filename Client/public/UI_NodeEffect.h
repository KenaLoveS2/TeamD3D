#pragma once
#include "Client_Defines.h"
#include "UI_Node.h"

#define MAX_PARTICLES 50

BEGIN(Client)
class CEffect_Particle_Base;
class CUI_NodeEffect final : public CUI_Node
{
public:
	enum	TYPE { TYPE_NONEANIM, TYPE_ANIM, TYPE_SEPERATOR, TYPE_RING, TYPE_ALPHA, TYPE_END };
private:
	CUI_NodeEffect(ID3D11Device*	pDevice, ID3D11DeviceContext* pContext);
	CUI_NodeEffect(const CUI_NodeEffect& rhs);
	virtual ~CUI_NodeEffect() = default;

public:
	void	Start_Effect(CUI* pTarget, _float fX, _float fY);
	void	Change_Scale(_float fData);
	void	BackToOriginalScale();
	void	Set_Alpha(_float fAlpha) { m_fAlpha = fAlpha; }
	void	Set_Effect(CUI* pTarget, _float fX, _float fY);

public:
	virtual HRESULT			Initialize_Prototype()			override;
	virtual HRESULT			Initialize(void* pArg)			override;
	virtual void			Tick(_float fTimeDelta)			override;
	virtual void			Late_Tick(_float fTimeDelta)	override;
	virtual HRESULT			Render()						override;

public:
	virtual void			Imgui_RenderProperty();
	virtual HRESULT			Save_Data();
	virtual HRESULT			Load_Data(wstring fileName);

private:
	HRESULT					SetUp_Components();
	HRESULT					SetUp_ShaderResources();

private:
	//_bool					m_bAnimation;
	TYPE					m_eType;
	CUI*					m_pTarget;

	/* For. Simple Action  */
	_float					m_fTime;
	_float					m_fAlpha;
	_float4					m_vColor;

	/* For. Particle */
	vector<CEffect_Particle_Base*>	m_vecParticles;
	_uint							m_iParticleIndex;
	_float							m_fParticleTimeAcc;

public:
	static	CUI_NodeEffect*		Create(ID3D11Device* pDevice, ID3D11DeviceContext*	pContext);
	virtual CGameObject*		Clone(void* pArg = nullptr);
	virtual void				Free() override;
};
END
