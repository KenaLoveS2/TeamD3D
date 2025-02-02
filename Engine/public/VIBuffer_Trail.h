#pragma once
#include "VIBuffer_Instancing.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Trail final : public CVIBuffer_Instancing
{
public:
	enum TYPE 
	{
		OBJ_KENA,
		OBJ_MONSTER,
		OBJ_BOSS,
		OBJ_ROTWISP,
		OBJ_B_SHAMAN,
		OBJ_W_SHAMAN,
		OBJ_BODY_SHAMAN,
		OBJ_B_HUNTER,
		OBJ_DEFAULT,
		OBJ_TRAIL,
		OBJ_BOWTARGET,
		OBJ_END
	};

private:
	CVIBuffer_Trail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Trail(const CVIBuffer_Trail& rhs);
	virtual ~CVIBuffer_Trail() = default;

public:
	vector<VTXMATRIX>*	Get_InstanceInfo() { return &m_vecInstanceInfo; }

public:
	virtual HRESULT Initialize_Prototype(_uint iNumInstance);
	virtual HRESULT Initialize(void* pArg, class CGameObject* pOwner) override;
	virtual HRESULT Tick(_float fTimeDelta) override;
	virtual HRESULT Render();

	HRESULT Trail_Tick(_int iObjectType, class CGameObject* pGameObject, _float fTimeDelta);

public:
	void			Add_Instance(_float4x4 InfoMatrix);
	HRESULT			Bind_ShaderResouce(class CShader* pShaderCom, const char* pConstanctName);
	HRESULT			Bind_RawValue(CShader * pShaderCom, const char * pConstanctName);

	void			ResetInfo() { m_vecInstanceInfo.clear(); }
	void			Refresh_InstanceCount();

private:
	_uint			  m_iInitNumInstance = 0;
	vector<VTXMATRIX> m_vecInstanceInfo;

private:
	_float*		  m_pSpeeds = nullptr;

public:
	static CVIBuffer_Trail* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iNumInstance = 1);
	virtual CComponent* Clone(void* pArg = nullptr, class CGameObject* pOwner = nullptr) override;
	virtual void Free() override;
};

END