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
class CControlRoom;
class CDeadZoneBossTree  final : public CEnviromentObj
{
private:
	CDeadZoneBossTree(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDeadZoneBossTree(const CDeadZoneBossTree& rhs);
	virtual ~CDeadZoneBossTree() = default;


public:
	virtual HRESULT		Initialize_Prototype() override;
	virtual HRESULT		Initialize(void* pArg) override;
	virtual HRESULT		Late_Initialize(void* pArg = nullptr) override;

	virtual void				Tick(_float fTimeDelta) override;
	virtual void				Late_Tick(_float fTimeDelta) override;
	virtual HRESULT		Render() override;
	virtual HRESULT		RenderShadow() override;

	virtual void		ImGui_PhysXValueProperty() override;

	void	Change_Model(_int iDissolveTimer);

	virtual _int		Execute_Collision(CGameObject* pTarget, _float3 vCollisionPos, _int iColliderIndex);
	virtual _int				Execute_TriggerTouchFound(CGameObject* pTarget, _uint iTriggerIndex, _int iColliderIndex) override;
	_float4				Set_ColorValue();
	_float4				Set_ColorValue_1();

private:
	CModel* m_pModelCom = nullptr;
	CTexture* m_pDissolveTextureCom = nullptr;
	
private:
	_bool			m_bTestOnce = false;


	_float				m_fTimeDeltaTest = 0.f;
	_float4				m_vColor = _float4(1.0f, 0.05f, 0.46f, 1.f);
	_float				m_fDissolveTime = 0.f;
	_bool				m_bDissolve = false;
	_bool				m_bBossAttackOn = false;
	_bool				m_bCollOnce = false;
	_bool				m_bOriginRender = true;

	_float4 m_vBaseColor1 = _float4(1.0f, 0.05f, 0.46f, 1.f);
	_float4 m_vBaseColor2 = _float4(1.0f, 0.05f, 0.46f, 1.f);
	_float	fFirstRatio = 1.f;
	_float	fSecondRatio = 1.f;
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();
	HRESULT SetUp_ShadowShaderResources() override;

public:
	static  CDeadZoneBossTree* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};
END
