#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CNavigation;
class CVIBuffer_Terrain;
END

BEGIN(Client)
class CTerrain final : public CGameObject
{
public:
	enum TEXTURE { TYPE_DIFFUSE, TYPE_BRUSH, TYPE_FILTER, TYPE_END };

	typedef struct tag_TerrainDesc
	{
		wstring			wstrDiffuseTag;
		wstring			wstrNormalTag;
		wstring			wstrFilterTag;
		_int			iBaseDiffuse = 0;
		_int			iFillterOne_TextureNum = 0;
		_int			iFillterTwo_TextureNum = 0;
		_int			iFillterThree_TextureNum = 0;
		_int			iHeightBmpNum = 0;
		tag_TerrainDesc()
		{
			wstrDiffuseTag = L"";	wstrFilterTag = L""; wstrNormalTag = L"";
		}

	}TERRAIN_DESC;


private:
	CTerrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTerrain(const CTerrain& rhs);
	virtual ~CTerrain() = default;

public:
	TERRAIN_DESC*		Get_TerrainDesc() { return &m_TerrainDesc; }
	

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Late_Initialize(void* pArg = nullptr)override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	
	virtual HRESULT Render() override;
	virtual void Imgui_RenderProperty() override;

private:
	CShader*						m_pShaderCom = nullptr;
	CRenderer*					m_pRendererCom = nullptr;
	CTexture*						m_pTextureCom[TYPE_END] = { nullptr };
	CVIBuffer_Terrain*		m_pVIBufferCom = nullptr;
	//CNavigation*				m_pNavigationCom = nullptr;

public: /*For.IMgui*/
	void					Imgui_Tool_Add_Component(_uint iLevel, const _tchar* ProtoTag, const _tchar* ComTag);
	void					Erase_FilterCom();
	void					Change_HeightMap(const _tchar* pHeightMapFilePath);

	_bool				CreateEnvrObj_PickingPos(_float4& vPos);

public:	
	

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();

private:
	TERRAIN_DESC						m_TerrainDesc;
	_bool								m_bLoadData = false;

private: /*For.Brush*/
	_float								m_fBrushRange = 5.f;
	_float4								m_vBrushPos;
public:
	static CTerrain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END