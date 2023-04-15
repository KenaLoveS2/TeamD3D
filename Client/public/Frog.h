#pragma once
#include "Client_Defines.h"
#include "EnviromentObj.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
class CTexture;
END

BEGIN(Client)
class CFrog final : public CEnviromentObj
{
private:
	enum Frog_ShaderPass
	{
		SHADOW = 11,

		DEFAULT = 14,
		AO_R_M,
		AO_R_M_E,
		AO_R_M_G,
		AO_R_M_O,
		AO_R_M_EEM,
		SEPARATE_AO_R_M_E,
		MASK,
		SAPLING_BOMBUP,
		ALPHA_AO_R_M,
		BOSS_AO_R_M,
		BOSS_AO_R_M_E,
		HUNTER_ARROW,
		PASS_END
	};

	enum Frog_Animation
	{
		CHEER,
		CRY,
		CRY_BIG,
		IDLE_LOOP,
		JUMP,
		LAND,
		LOOK_AROUND,
		LOOP,
		ROT_GET,
		SAD_LOOK,
		SAD_LOOP,
		Frog_Animation_END,
	};

private:
	CFrog(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CFrog(const CFrog& rhs);
	virtual ~CFrog() = default;
public:
	virtual HRESULT		Initialize_Prototype() override;
	virtual HRESULT		Initialize(void* pArg) override;
	virtual HRESULT		Late_Initialize(void* pArg = nullptr) override;
	virtual void				Tick(_float fTimeDelta) override;
	virtual void				Late_Tick(_float fTimeDelta) override;
	virtual HRESULT		Render() override;
	virtual HRESULT      RenderShadow() override;

public:
	virtual void ImGui_AnimationProperty()override;

private:
	CShader* m_pShaderCom = nullptr;
	CRenderer* m_pRendererCom = nullptr;
	CModel* m_pModelCom = nullptr;
	class CInteraction_Com* m_pInteractionCom = nullptr;
	class CControlMove* m_pControlMoveCom = nullptr;
	_bool		m_bAnimActive = false;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();
	HRESULT SetUp_ShadowShaderResources() override;

public:
	static  CFrog* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};
END
