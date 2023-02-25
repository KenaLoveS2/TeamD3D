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
	HRESULT				 Set_ShapePosition(CVIBuffer_Point_Instancing::POINTDESC& ePointDesc);

public:
	void	Set_VIBufferProtoTag(_tchar* pProtoTag) {
		m_szVIBufferProtoTag = pProtoTag;
	}

	CVIBuffer_Point_Instancing::POINTDESC::SHAPETYPE		Get_ShapeType();
	void													Set_ShapeType(CVIBuffer_Point_Instancing::POINTDESC::SHAPETYPE eType);

	CVIBuffer_Point_Instancing::POINTDESC::MOVEDIR			Get_MoveDir();
	void													Set_MoveDir(CVIBuffer_Point_Instancing::POINTDESC::MOVEDIR eType);

	CVIBuffer_Point_Instancing::POINTDESC				    Get_PointInstanceDesc();
	void													Set_PointInstanceDesc(CVIBuffer_Point_Instancing::POINTDESC eEffectDesc);
	void	Set_PointSetRange(_float fRange){}

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