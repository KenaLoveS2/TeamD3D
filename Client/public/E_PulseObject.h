#pragma once
#include "Effect_Mesh.h"
#include "Client_Defines.h"

BEGIN(Client)

class CE_PulseObject final : public CEffect_Mesh
{
private:
	CE_PulseObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CE_PulseObject(const CE_PulseObject& rhs);
	virtual ~CE_PulseObject() = default;

public:
	void    Set_InitMatrixScaled(_float3 vScale);
	_float3 Get_InitMatrixScaled();

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pFilePath = nullptr);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void    Tick(_float fTimeDelta) override;
	virtual void    Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT SetUp_ShaderResources();

private:
	_float	m_fActivePlusScale = 1.3f;
	_float4x4 m_SaveInitWorldMatrix;

public:
	static  CE_PulseObject* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pFilePath = nullptr);
	virtual CGameObject*  Clone(void* pArg = nullptr) override;
	virtual void          Free() override;

};

END