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
class CRotEater_Small final : public CEnviromentObj
{
private:
	enum RotEater_Small_ShaderPass
	{
		DEFAULT,
		AO_R_M,
		AO_R_M_E,
		AO_R_M_G,
		AO_R_M_O,
		AO_R_M_EEM,
		SEPARATE_AO_R_M_E,
		MASK,
		SAPLING_BOMBUP,
		ALPHA_AO_R_M,
		SHADOW,
		BOSS_AO_R_M,
		BOSS_AO_R_M_E,
		PASS_END
	};

	
private:
	CRotEater_Small(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRotEater_Small(const CRotEater_Small& rhs);
	virtual ~CRotEater_Small() = default;
public:
	virtual HRESULT		Initialize_Prototype() override;
	virtual HRESULT		Initialize(void* pArg) override;
	virtual HRESULT		Late_Initialize(void* pArg=nullptr) override;
	virtual void				Tick(_float fTimeDelta) override;
	virtual void				Late_Tick(_float fTimeDelta) override;
	virtual HRESULT		Render() override;

private: 
	void				State_RotEaterSmall(_float fTimedelta);

public:
	virtual void ImGui_AnimationProperty()override;

private:
	CShader* m_pShaderCom = nullptr;
	CRenderer* m_pRendererCom = nullptr;
	CModel* m_pModelCom = nullptr;
	class CInteraction_Com* m_pInteractionCom = nullptr;
	class CControlMove* m_pControlMoveCom = nullptr;

	_bool		m_bOnceTest = false;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();

public:
	static  CRotEater_Small* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};
END
