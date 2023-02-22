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
		_uint			iRoomIndex = 0;
	} ENVIROMENT_DESC;		/*wstring 이  있는 애들은 zeromemory를 쓰지마라*/

protected:
	ENVIROMENT_DESC m_EnviromentDesc;
	class CEnviroment_Manager* m_pEnviroment_Manager = nullptr;
	
	/*
	CModel* m_pModelCom = nullptr;
	CTexture* m_pTextureCom = nullptr;
	CShader* m_pShaderCom = nullptr;
	CRenderer* m_pRendererCom = nullptr;
	*/
	_bool m_bRenderActive = false;

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

	_uint Get_RoomIndex() { return m_EnviromentDesc.iRoomIndex; }
	void Set_RoomIndex(_uint iIndex) { m_EnviromentDesc.iRoomIndex = iIndex; }

	void Set_RenderActive(_bool bFlag) { m_bRenderActive = bFlag; }
	_bool Get_RenderActive() { return m_bRenderActive; }
};
END