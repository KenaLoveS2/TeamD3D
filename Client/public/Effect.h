#pragma once
#include "Client_Defines.h"
#include "Effect_Base.h"

BEGIN(Client)

class CEffect final : public CEffect_Base
{
private:
	CEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffect(const CEffect& rhs);
	virtual ~CEffect() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void    Tick(_float fTimeDelta) override;
	virtual void    Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual HRESULT	Set_Child(EFFECTDESC eEffectDesc, _int iCreateCnt, char* ProtoTag);
	virtual HRESULT Edit_Child(const _tchar * ProtoTag);

public:
	virtual HRESULT				 Set_Trail(class CEffect_Base* pEffect, const _tchar* pProtoTag);
	virtual class CEffect_Trail* Get_Trail();
	virtual void				 Delete_Trail(const _tchar* pProtoTag);

public:
	void			 Set_FreePos();
	_bool			 Play_FreePos(_float4& vPos);
	vector<_float4>* Get_FreePos();
	void			Set_Lerp(_bool bLerp) { m_bLerp = bLerp; }

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();

private:
	_float m_fTimeDelta = 0.0f;
	_float m_fFreePosTimeDelta = 0.0f;
	vector<_float4>  m_vecFreePos;

	_vector m_vPrePos;
	_vector m_vCurPos;

	_float m_fLerp = 0;
	_bool  m_bLerp = false;

public:
	static  CEffect*     Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void         Free() override;
};

END