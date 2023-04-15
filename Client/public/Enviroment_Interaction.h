#pragma once
#include "Client_Defines.h"
#include "EnviromentObj.h"
#include "GameInstance.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
class CTexture;
END

BEGIN(Client)
class CEnviroment_Interaction : public CEnviromentObj
{
protected:
	CRenderer* m_pRendererCom = nullptr;
	CShader* m_pShaderCom = nullptr;
	CModel* m_pModelCom = nullptr;

	class CInteraction_Com*			m_pInteractionCom = nullptr;
	class CControlMove*				m_pControlMoveCom = nullptr;

	_uint m_iNumMeshes = 0;
	class CGameInstance* m_pGameInstance = nullptr;

	CFSMComponent* m_pFSM = nullptr;

protected:
	CEnviroment_Interaction(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEnviroment_Interaction(const CEnviroment_Interaction& rhs);
	virtual ~CEnviroment_Interaction() = default;

protected:
	virtual HRESULT SetUp_Components();
	virtual HRESULT SetUp_ShaderResources();
	virtual HRESULT SetUp_ShadowShaderResources() override;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Late_Initialize(void * pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT RenderShadow() override;

	virtual HRESULT Add_AdditionalComponent(_uint iLevelIndex, const _tchar* pComTag, COMPONENTS_OPTION eComponentOption)override;

public:
	static  CEnviroment_Interaction* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
	
	void ImGui_ShaderValueProperty();
	virtual	HRESULT SetUp_State() { return S_OK; };
};
END
