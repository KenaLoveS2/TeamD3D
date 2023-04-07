#pragma once

#include "Client_Defines.h"
#include "Effect_Base.h"
#include "VIBuffer_Point_Instancing.h"

BEGIN(Client)

class CEffect_Point_Instancing_T final : public CEffect_Base
{
private:
	CEffect_Point_Instancing_T(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffect_Point_Instancing_T(const CEffect_Point_Instancing_T& rhs);
	virtual ~CEffect_Point_Instancing_T() = default;

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
	const _tchar*	Get_VIBufferProtoTag() { return m_szVIBufferProtoTag; }

	CVIBuffer_Point_Instancing::POINTDESC*				    Get_PointInstanceDesc();
	void													Set_PointInstanceDesc(CVIBuffer_Point_Instancing::POINTDESC* eEffectDesc);

	CVIBuffer_Point_Instancing::INSTANCEDATA*				Get_InstanceData();
	CVIBuffer_Point_Instancing::INSTANCEDATA				Get_InstanceData_Idx(_int iIndex);
	void													Set_InstanceData(CVIBuffer_Point_Instancing::INSTANCEDATA* eInstanceData);

	void			Set_PointSetRange(_float fRange) {}

	void			Set_RandomSpeeds(_float fmin, _float fmax);
	HRESULT			Set_Pos(_float3 fMin, _float3 fMax);
	void			Set_PSize(_float2 PSize);
	void			Set_RandomPSize(_float2 PSizeMinMax);

	vector<class CEffect_Trail*> Get_TrailEffect() { return m_vecTrailEffect; }
	void Set_TrailEffect(vector<class CEffect_Trail*> vecTrail) {
		memcpy(&m_vecTrailEffect, &vecTrail, sizeof(vecTrail.size()));
	}

public:
	virtual HRESULT Initialize_Prototype(_tchar* pProtoTag, const _tchar* pFilePath = nullptr);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void    Tick(_float fTimeDelta) override;
	virtual void    Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual void	Imgui_RenderProperty()override;

private:
	HRESULT         SetUp_Components();
	HRESULT         SetUp_ShaderResources();

private:
	const _tchar*                m_szVIBufferProtoTag = L"";
	_float		                 m_fTimeDelta = 0.0f;
	vector<class CEffect_Trail*> m_vecTrailEffect;
	_float					     m_fLife = 0.0f;

public:
	static CEffect_Point_Instancing_T* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _tchar* pProtoTag);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END