#pragma once
#include "GameObject.h"

BEGIN(Engine)
class ENGINE_DLL CEnviromentObj : public CGameObject
{
public:
	typedef struct tagEnviromnetObjectDesc
	{	
		CGameObject::GAMEOBJECTDESC ObjectDesc;

		// Ã¤¿ö!!


	} DESC;

protected:
	DESC m_Desc;

	/*
	CModel* m_pModelCom = nullptr;
	CTexture* m_pTextureCom = nullptr;
	CShader* m_pShaderCom = nullptr;
	CRenderer* m_pRendererCom = nullptr;
	*/

protected:
	CEnviromentObj(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEnviromentObj(const CEnviromentObj& rhs);
	virtual ~CEnviromentObj() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

public:		
	virtual CEnviromentObj* Clone(void* pArg) { return nullptr; };
	virtual void Free() override;
};
END