#pragma once
#include "Effect_Mesh.h"
#include "Client_Defines.h"

BEGIN(Client)

class CE_Wind final : public CEffect_Mesh
{
private:
	CE_Wind(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CE_Wind(const CE_Wind& rhs);
	virtual ~CE_Wind() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pFilePath = nullptr);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void    Tick(_float fTimeDelta) override;
	virtual void    Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();

public:
	static  CE_Wind* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pFilePath = nullptr);
	virtual CGameObject*  Clone(void* pArg = nullptr) override;
	virtual void          Free() override;

};

END