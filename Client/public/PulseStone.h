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
class CPulseStone  final : public CEnviromentObj
{
private:
	CPulseStone(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPulseStone(const CPulseStone& rhs);
	virtual ~CPulseStone() = default;

public:
	void		Gimmick_Active(_bool bGimmickStart) {
		m_bGimmickActive = bGimmickStart;
	}

public:
	virtual HRESULT		Initialize_Prototype() override;
	virtual HRESULT		Initialize(void* pArg) override;
	virtual HRESULT		Late_Initialize(void* pArg=nullptr) override;
	virtual void				Tick(_float fTimeDelta) override;
	virtual void				Late_Tick(_float fTimeDelta) override;
	virtual HRESULT		Render() override;
	virtual HRESULT		RenderShadow() override;

private:
	CModel*									m_pModelCom = nullptr;
	
	class CPortalPlane*		m_pRenderFalsePortal = nullptr;

private:
	vector<class CEffect_Base*>			m_VecCrystal_Effect;
	_bool								m_bGimmickActive = false;
public:
	
	virtual _int Execute_TriggerTouchFound(CGameObject* pTarget, _uint iTriggerIndex, _int iColliderIndex) override;
	virtual _int Execute_TriggerTouchLost(CGameObject* pTarget, _uint iTriggerIndex, _int iColliderIndex) override;



private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();
	HRESULT SetUp_ShadowShaderResources();

public:
	static  CPulseStone*	   Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};
END


