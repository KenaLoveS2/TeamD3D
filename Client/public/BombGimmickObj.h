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
class CBombGimmickObj final : public CEnviromentObj
{
private:
	CBombGimmickObj(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBombGimmickObj(const CBombGimmickObj& rhs);
	virtual ~CBombGimmickObj() = default;

public:
	virtual HRESULT		Initialize_Prototype() override;
	virtual HRESULT		Initialize(void* pArg) override;
	virtual HRESULT		Late_Initialize(void* pArg = nullptr);
	virtual void				Tick(_float fTimeDelta) override;
	virtual void				Late_Tick(_float fTimeDelta) override;
	virtual HRESULT		Render() override;
	virtual HRESULT		RenderShadow() override;

private:
	CModel* m_pModelCom = nullptr;
	class CInteraction_Com* m_pInteractionCom = nullptr;
	class CControlMove* m_pControlMoveCom = nullptr;

public:
	virtual HRESULT		Add_AdditionalComponent(_uint iLevelIndex, const _tchar* pComTag, COMPONENTS_OPTION eComponentOption)override;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();
	HRESULT SetUp_ShadowShaderResources();

public:
	static  CBombGimmickObj* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};
END

