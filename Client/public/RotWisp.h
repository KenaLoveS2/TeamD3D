#pragma once

#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CModel;
class CTexture;
class CShader;
class CRenderer;
class CFSMComponent;
END

BEGIN(Client)

class CRotWisp final : public CGameObject
{
public:
	enum ANIMATION
	{
		ROTWISP_APPEAR13_AFTERCUTSCENE,
		ROTWISP_APPEAR13_CUTSCENE,
		ROTWISP_APPEAR2,
		ROTWISP_APPEAR3,
		ROTWISP_APPEAR4,
		ROTWISP_APPEAR5,
		ROTWISP_APPEAR6,
		ROTWISP_APPEAR7,
		ROTWISP_APPEAR8,
		ROTWISP_APPEAR9,
		ROTWISP_COLLECT,
		ROTWISP_FLOATINGLOOP1,
		ROTWISP_FLOWERPULSE,
		ROTWISP_FLOWERPULSEEXIT,
		ROTWISP_HIDE,
		ROTWISP_INTERACT,
		ROTWISP_INTERACTBIG,
		ROTWISP_INTERACTBIG2,
		ROTWISP_INTERACTBIG3,
		ROTWISP_INTOPUDDLE,
		ROTWISP_POPUP,
		ROTWISP_ROTCOLLECT_FLOATING,
		ROTWISP_ROTINPOT_IDLE,
		ROTWISP_RUN,
		ROTWISP_SPLAT,
		ROTWISP_SPLATLOOP,
		ANIMATION_END
	};

private:
	CRotWisp(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRotWisp(const CRotWisp& rhs);
	virtual ~CRotWisp() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* pArg) override;
	virtual void					Tick(_float fTimeDelta) override;
	virtual void					Late_Tick(_float fTimeDelta) override;
	virtual HRESULT			Render() override;
	virtual void					Calc_RootBoneDisplacement(_fvector vDisplacement) override;

private:
	HRESULT						SetUp_Components();
	HRESULT						SetUp_ShaderResources();
	HRESULT						SetUp_State();
	_bool							AnimFinishChecker(_uint eAnim, _double FinishRate = 0.95);
	_bool							AnimIntervalChecker(_uint eAnim, _double StartRate, _double FinishRate);

public:
	_bool							Get_Collect() { return m_bCollect; }

private:
	CShader*						m_pShaderCom = nullptr;
	CModel*						m_pModelCom = nullptr;
	CTexture*						m_pTextureCom = nullptr;
	CRenderer*					m_pRendererCom = nullptr;
	CFSMComponent*		m_pFSM = nullptr;

	CTexture*						m_pWPOTextureCom = nullptr;
	CTexture*						m_pDissolveAlphaTextureCom = nullptr;

private:
	_bool							m_bPulse = false;
	_bool							m_bCollect = false;
	
public:
	static CRotWisp*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg = nullptr)  override;
	virtual void					Free() override;
};

END