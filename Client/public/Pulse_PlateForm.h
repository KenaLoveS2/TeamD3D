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
class CPulse_PlateForm final : public CEnviromentObj
{
private:
	CPulse_PlateForm(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPulse_PlateForm(const CPulse_PlateForm& rhs);
	virtual ~CPulse_PlateForm() = default;
public:
	virtual HRESULT		Initialize_Prototype() override;
	virtual HRESULT		Initialize(void* pArg) override;
	virtual void				Tick(_float fTimeDelta) override;
	virtual void				Late_Tick(_float fTimeDelta) override;
	virtual HRESULT		Render() override;

private:
	CShader*									m_pShaderCom = nullptr;
	CRenderer*								m_pRendererCom = nullptr;
	CModel*									m_pModelCom = nullptr;
	class CInteraction_Com*			m_pInteractionCom = nullptr;
	class CControlMove*				m_pControlMoveCom = nullptr;

public:
	virtual HRESULT		Add_AdditionalComponent(_uint iLevelIndex, const _tchar* pComTag, COMPONENTS_OPTION eComponentOption)override;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();
public:
	static  CPulse_PlateForm*	   Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};
END



