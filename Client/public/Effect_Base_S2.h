#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)
class CEffect_Base_S2 abstract : public CGameObject
{
public:
	enum	OPTION { OPTION_UV, OPTION_SPRITE, OPTION_SPRITE_REPEAT, OPTION_END };
protected:
	CEffect_Base_S2(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffect_Base_S2(const CEffect_Base_S2& rhs);
	virtual ~CEffect_Base_S2() = default;

public:
	inline	void					Set_Target(CGameObject* pTarget) { m_pTarget = pTarget; }
	inline  void					Set_Active(_bool bActive) { m_bActive = bActive; }
	inline  void					Set_ActiveFlip() { m_bActive = !m_bActive; }
	inline	void					Set_EffectTag(string str) { m_strEffectTag = str; }

public:
	virtual HRESULT					Initialize_Prototype();
	virtual HRESULT					Initialize(void* pArg) override;
	virtual HRESULT					Late_Initialize(void* pArg = nullptr) override;
	virtual void					Tick(_float fTimeDelta) override;
	virtual void					Late_Tick(_float fTimeDelta) override;
	virtual HRESULT					Render() override;

public:
	virtual void					Imgui_RenderProperty() = 0;
	virtual HRESULT					Save_Data() = 0;
	virtual	HRESULT					Load_Data(_tchar* fileName) = 0;
	virtual void					Set_DissolveState() {};
	virtual	void					BackToNormal() {};

public: /* Activation Function */
	virtual	void					Activate(_float4 vPos) {};
	virtual	void					Activate(CGameObject* pTarget) {};
	virtual void					Activate_Scaling(CGameObject* pTarget, _float2 vScaleSpeed) {};
	virtual void					Activate_Scaling(_float4 vPos, _float2 vScaleSpeed) {};
	virtual void					Activate_Spread(_float4 vPos, _float2 vScaleSpeed) {};
	virtual void					DeActivate();



protected:	/* Tool Function */
	_float4		ColorCode();
	void		Options();

protected:
	CGameObject*					m_pTarget;
	_tchar*							m_pfileName;
	_bool							m_bActive;
	string							m_strEffectTag;/* From Tool Desc */

protected: /* Property */
	_int							m_iRenderPass;
	_int							m_iTextureIndex;
	_float4							m_vColor;
	_float							m_fHDRIntensity;
	_bool							m_bOptions[OPTION_END];
	//_bool							m_bSpriteAnim;
	//_bool							m_bUVAnim;

	/* For Dissolve */
	_float							m_fDissolveAlpha;
	_float							m_fDissolveSpeed;

	/* For Sprite Animation */
	_int							m_iFrames[2];
	_int							m_iFrameNow[2];
	_float							m_fFrameSpeed;
	_float							m_fFrameNow;

	/* For UV Animation */
	_float							m_fUVSpeeds[2];
	_float							m_fUVMove[2];

protected:
	_float4x4						m_WorldOriginal;
	_float4x4						m_LocalMatrix;
	_float4x4						m_LocalMatrixOriginal;
	_float4							m_ParentPosition;

protected: /* For. Activation */
	_float2							m_vScaleSpeed;

	_bool							m_bSelfStop;
	_float							m_fSelfStopTime;
	_float							m_fSelfStopTimeAcc;

public:
	virtual void					Free()			override;

};
END