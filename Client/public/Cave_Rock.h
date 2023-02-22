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
class CCave_Rock final : public CEnviromentObj
{
private:
	CCave_Rock(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCave_Rock(const CCave_Rock& rhs);
	virtual ~CCave_Rock() = default;

public:
	virtual HRESULT		Initialize_Prototype() override;
	virtual HRESULT		Initialize(void* pArg) override;
	virtual void				Tick(_float fTimeDelta) override;
	virtual void				Late_Tick(_float fTimeDelta) override;
	virtual HRESULT		Render() override;

private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CModel*				m_pModelCom = nullptr;

	class CInteraction_Com*			m_pInteractionCom = nullptr;	 
	class CControlMove*				m_pControlMoveCom = nullptr;

public:
	virtual HRESULT		Add_AdditionalComponent(_uint iLevelIndex,const _tchar* pComTag, COMPONENTS_OPTION eComponentOption)override;


private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();

private: /*For_Tool*/
	_uint	m_iShaderOption = 0;

public:
	static  CCave_Rock*	   Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};
END
