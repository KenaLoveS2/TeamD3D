#pragma once
#include "GameObject.h"

#define MAX_COMPONENT_NUM 5

BEGIN(Engine)
class ENGINE_DLL CEnviromentObj : public CGameObject
{
public:
	enum SHADER_PASS
	{
		ONLY_MRAO = 6,
		PASS_END
	};

	enum COMPONENTS_OPTION {
		COMPONENTS_CONTROL_MOVE, COMPONENTS_INTERACTION, COMPONENTS_END
	};

	enum  CHAPTER {
		Gimmick_TYPE_NONE, Gimmick_TYPE_GO_UP, Gimmick_TYPE_DISSOLVE, Gimmick_TYPE_DISSOLVE_AND_MODEL_CHANGE,
		Gimmick_TYPE_END};

	typedef struct tagEnviromnetObjectDesc
	{	
		CGameObject::GAMEOBJECTDESC ObjectDesc;
		wstring			szProtoObjTag  =  TEXT("");
		wstring			szModelTag =  TEXT("");
		_uint			iRoomIndex = 0;
		CHAPTER			eChapterType = Gimmick_TYPE_END;
		_int			iCurLevel = 0;				//이건 툴에서만 일단.
		
		tagEnviromnetObjectDesc()
		{
			ObjectDesc.TransformDesc.fRotationPerSec = 0;
			ObjectDesc.TransformDesc.fSpeedPerSec = 0;

		}

	} ENVIROMENT_DESC;		/*wstring 이  있는 애들은 zeromemory를 쓰지마라*/


protected:
	CEnviromentObj(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEnviromentObj(const CEnviromentObj& rhs);
	virtual ~CEnviromentObj() = default;

public:
	ENVIROMENT_DESC	&	Get_EnviromentDesc() { return m_EnviromentDesc; }
	_uint Get_RoomIndex() { return m_EnviromentDesc.iRoomIndex; }
	void Set_RoomIndex(_uint iIndex) { m_EnviromentDesc.iRoomIndex = iIndex; }
	void Set_RenderActive(_bool bFlag) { m_bRenderActive = bFlag; }
	_bool Get_RenderActive() { return m_bRenderActive; }

	vector<const _tchar*>*	Get_CurObjectComponentTag() { return &m_CurComponenteTag; }

	
public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();


public:
	virtual HRESULT		Add_AdditionalComponent(_uint iLevelIndex, const _tchar* pComTag, COMPONENTS_OPTION eComponentOption);
	virtual  void				Imgui_RenderComponentProperties()override;

protected:
	HRESULT					Set_UpTexture_FilePathToMaterial(class CModel* pMode, const _tchar * TexturePath, aiTextureType Type);

protected:
	class CEnviroment_Manager* m_pEnviroment_Manager = nullptr;

protected:
	ENVIROMENT_DESC					m_EnviromentDesc;	
	vector<const _tchar*>				m_CurComponenteTag;

	_bool										m_bRenderActive = false;
	_uint											m_iShaderOption = 0;			// Defualt =0 , Wire_Frame= 3 

private:/*For.ImguiTool*/
	string										m_str_Imgui_ComTag = "";
	_int											m_iImgui_ComponentOption = 0;
	_bool										m_bWireFrame_Rendering = false;
	//vector<const _tchar*>				m_vecStr_textureFilePath;				//Texture 경로들
	

public:		
	virtual CGameObject* Clone(void* pArg) { return nullptr; };
	virtual void Free() override;

};
END