#pragma once
#include "Effect_Mesh.h"
#include "Client_Defines.h"

BEGIN(Client)

class CE_EnemyWispBackground final : public CEffect_Mesh
{
private:
	CE_EnemyWispBackground(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CE_EnemyWispBackground(const CE_EnemyWispBackground& rhs);
	virtual ~CE_EnemyWispBackground() = default;

public:
	void       Set_InitMatrixScaled(_float3 vScale);
	_float3    Get_InitMatrixScaled();

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pFilePath = nullptr);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void    Tick(_float fTimeDelta) override;
	virtual void    Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT			SetUp_ShaderResources();

private:
	_float4x4       m_SaveInitWorldMatrix;
	_float			m_fLife = 0.0f;

public:
	static  CE_EnemyWispBackground* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pFilePath = nullptr);
	virtual CGameObject*  Clone(void* pArg = nullptr) override;
	virtual void          Free() override;

};

END