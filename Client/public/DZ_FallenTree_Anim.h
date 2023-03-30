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
class CDZ_FallenTree_Anim final : public CEnviromentObj
{
private:
	CDZ_FallenTree_Anim(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDZ_FallenTree_Anim(const CDZ_FallenTree_Anim& rhs);
	virtual ~CDZ_FallenTree_Anim() = default;

public:
	virtual HRESULT		Initialize_Prototype() override;
	virtual HRESULT		Initialize(void* pArg) override;
	virtual HRESULT		Late_Initialize(void* pArg = nullptr)override;
	virtual void		Tick(_float fTimeDelta) override;
	virtual void		Late_Tick(_float fTimeDelta) override;
	virtual HRESULT		Render() override;

	virtual void					ImGui_AnimationProperty()override;
	virtual void					ImGui_PhysXValueProperty() override;
private:
	CModel*							m_pModelCom = nullptr;
	class CInteraction_Com*			m_pInteractionCom = nullptr;
	class CControlMove*				m_pControlMoveCom = nullptr;
	_bool							m_bRenderCheck = false;
public:
	virtual HRESULT		Add_AdditionalComponent(_uint iLevelIndex, const _tchar* pComTag, COMPONENTS_OPTION eComponentOption)override;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();

public:
	static  CDZ_FallenTree_Anim*	   Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};
END

