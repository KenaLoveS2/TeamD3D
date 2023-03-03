#pragma once
#include "Client_Defines.h"
#include "Effect_Base.h"

BEGIN(Client)

class CEffect_Mesh_T : public CEffect_Base
{
private:
	CEffect_Mesh_T(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffect_Mesh_T(const CEffect_Mesh_T& rhs);
	virtual ~CEffect_Mesh_T() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pFilePath = nullptr) ;
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
	void			 Set_Lerp(_bool bLerp) { m_bLerp = bLerp; }
	HRESULT			 Set_ModelCom(EFFECTDESC::MESHTYPE eMeshType);

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();

private:
	_float           m_fTimeDelta = 0.0f;
	_float           m_fDistotionTime = 0.0f;

public:
	static  CEffect_Mesh_T* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pFilePath = nullptr);
	virtual CGameObject*  Clone(void* pArg = nullptr) override;
	virtual void          Free() override;
};

END