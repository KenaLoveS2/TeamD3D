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

class CE_P_Sakura final : public CEnviromentObj
{
private:
	CE_P_Sakura(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CE_P_Sakura(const CE_P_Sakura& rhs);
	virtual ~CE_P_Sakura() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pFilePath = nullptr);
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Late_Initialize(void* pArg=nullptr) override;
	virtual void    Tick(_float fTimeDelta) override;
	virtual void    Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CModel* m_pModelCom = nullptr;

	_bool	m_bStart_InstMove = false;
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();


public:
	static  CE_P_Sakura*  Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pFilePath = nullptr);
	virtual CGameObject*  Clone(void* pArg = nullptr) override;
	virtual void          Free() override;
};

END