#pragma once

#include "Client_Defines.h"
#include "Effect_Base.h"
#include "VIBuffer_Point_Instancing.h"

BEGIN(Client)

class CEffect_Point_Instancing final : public CEffect_Base
{
private:
	CEffect_Point_Instancing(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffect_Point_Instancing(const CEffect_Point_Instancing& rhs);
	virtual ~CEffect_Point_Instancing() = default;

public:
	HRESULT				 Set_ShapePosition();

public:
	virtual HRESULT				 Set_Trail(class CEffect_Base* pEffect, const _tchar* pProtoTag);
	virtual class CEffect_Trail* Get_Trail();
	virtual void				 Delete_Trail(const _tchar* pProtoTag);

public:
	void	Set_VIBufferProtoTag(_tchar* pProtoTag) {
		m_szVIBufferProtoTag = pProtoTag;
	}

	CVIBuffer_Point_Instancing::POINTDESC*				    Get_PointInstanceDesc();
	void													Set_PointInstanceDesc(CVIBuffer_Point_Instancing::POINTDESC eEffectDesc);

	CVIBuffer_Point_Instancing::INSTANCEDATA*				Get_InstanceData();
	CVIBuffer_Point_Instancing::INSTANCEDATA				Get_InstanceData_Idx(_int iIndex);
	void													Set_InstanceData(CVIBuffer_Point_Instancing::INSTANCEDATA eInstanceData);

	void			Set_PointSetRange(_float fRange){}

	void			Set_RandomSpeeds(_double fMin, _double fMax);
	HRESULT			Set_Pos(_float3 fMin, _float3 fMax);
	void			Set_PSize(_float2 PSize);
	void			Set_RandomPSize(_float2 PSizeMinMax);

public:
	virtual HRESULT Initialize_Prototype(_tchar* pProtoTag);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void    Tick(_float fTimeDelta) override;
	virtual void    Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT         SetUp_Components();
	HRESULT         SetUp_ShaderResources();

private:
	const _tchar*                m_szVIBufferProtoTag = L"";
	_float		                 m_fTimeDelta = 0.0f;
	_float		                 m_fVIBufferTime = 0.0f;
	vector<class CEffect_Trail*> m_vecTrailEffect;

public:
	static CEffect_Point_Instancing* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _tchar* pProtoTag);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END