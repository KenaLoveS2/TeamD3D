#pragma once
#include "GameObject.h"

BEGIN(Engine)
class ENGINE_DLL CEnviromentObj : public CGameObject
{
public:
	enum  CHAPTER {
		CHAPTER_ONE_CAVE, CHAPTER_TWO_FOREST,			CHAPTER_END};

	typedef struct tagEnviromnetObjectDesc
	{	
		CGameObject::GAMEOBJECTDESC ObjectDesc;
		_tchar			szProtoObjTag[MAX_PATH]  =  TEXT("");
		_tchar			szModelTag[MAX_PATH] =  TEXT("");
		_tchar			szTextureTag[MAX_PATH] =   TEXT("");
	} ENVIROMENT_DESC;		/*wstring 이  있는 애들은 zeromemory를 쓰지마라*/

protected:
	ENVIROMENT_DESC m_EnviromentDesc;

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
	ENVIROMENT_DESC		Get_EnviromentDesc()const { return m_EnviromentDesc; }

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

public:		
	virtual CGameObject* Clone(void* pArg) { return nullptr; };
	virtual void Free() override;
};
END