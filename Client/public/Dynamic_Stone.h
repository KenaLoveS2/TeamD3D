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
class CDynamic_Stone  final : public CEnviromentObj
{
private:
	CDynamic_Stone(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDynamic_Stone(const CDynamic_Stone& rhs);
	virtual ~CDynamic_Stone() = default;


public:
	virtual HRESULT		Initialize_Prototype() override;
	virtual HRESULT		Initialize(void* pArg) override;
	virtual HRESULT		Late_Initialize(void* pArg = nullptr) override;
	virtual void				Tick(_float fTimeDelta) override;
	virtual void				Late_Tick(_float fTimeDelta) override;
	virtual HRESULT		Render() override;
	virtual HRESULT		RenderShadow() override;

	virtual HRESULT		RenderCine() override;
	virtual _int Execute_Collision(CGameObject* pTarget, _float3 vCollisionPos, _int iColliderIndex);
	//virtual void					ImGui_PhysXValueProperty() override;
private:

	_bool				m_bOnceColl = false;

	vector<CGameObject*>	m_pDynamicObj_List;

public:
	virtual HRESULT		Add_AdditionalComponent(_uint iLevelIndex, const _tchar* pComTag, COMPONENTS_OPTION eComponentOption)override;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();
	HRESULT SetUp_ShadowShaderResources();

public:
	static  CDynamic_Stone* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};
END
