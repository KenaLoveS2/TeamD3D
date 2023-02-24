#pragma once

#include "Client_Defines.h"
#include "Effect_Base.h"

BEGIN(Client)

class CEffect_Point_Instancing final : public CEffect_Base
{
private:
	CEffect_Point_Instancing(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffect_Point_Instancing(const CEffect_Point_Instancing& rhs);
	virtual ~CEffect_Point_Instancing() = default;

public:
	void	Set_VIBufferProtoTag(_tchar* pProtoTag) {
		m_szVIBufferProtoTag = pProtoTag;
	}

	_float2			Get_RandomSpeeds();
	void			Set_RandomSpeeds(_double fMin, _double fMax);

	HRESULT			Set_Pos(_float3 fMin, _float3 fMax);
	_float2			Get_PSize();
	void			Set_PSize(_float2 PSize);
	void			Set_RandomPSize(_float2 PSizeMinMax);

public:
	virtual HRESULT Initialize_Prototype(_tchar* pProtoTag);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();

private:
	const _tchar* m_szVIBufferProtoTag = L"";
	_float		  m_fTimeDelta = 0.0f;

public:
	static CEffect_Point_Instancing* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _tchar* pProtoTag);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END