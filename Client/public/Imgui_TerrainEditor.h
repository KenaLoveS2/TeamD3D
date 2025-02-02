#pragma once
#include "Client_Defines.h"
#include "ImguiObject.h"
#include "EnviromentObj.h"

BEGIN(Engine)
class CGameInstance;
class CGameObject;
class CTransform;
class CVIBuffer_Terrain;
class CTexture;
END

BEGIN(Client)
class CTerrain;

// 일단 3개의 필터를 가질수 있게 설계함
class CImgui_TerrainEditor  final : public CImguiObject
{
private:
	enum FILTER_NUM {
		FILTER_FIRST, FILTER_SECOND, FILTER_THIRD, FLTER_END
	};


private:
	CImgui_TerrainEditor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

public:
	static void LoadFilterData(string jsonFileName);			// 나중에 그냥 불러올때


	CTerrain* Get_SelectedTerrin() { return m_pPickingTerrain; }
public:
	virtual HRESULT Initialize(void* pArg = nullptr);
	virtual void Imgui_FreeRender();

private:
	void		Ready_FilterBuffer();

private:
	void		Terrain_Selecte();
	void		Imgui_Save_Load();
	void		Imgui_FilterPixel_Save();
	void		Imgui_FilterPixel_Load();
	void		Imgui_FilterControl();
	
	void		Draw_FilterTexture();
	void		UnDraw_FilterTexture();

	void		Imgui_Change_HeightBmp();

	void		Create_Terrain();

	void		Clear_Filter_Pixel();

	void		Save_Terrain();
	void		Load_Terrain();
private:
	_ulong*									m_pPixel[FLTER_END] = { nullptr };
	std::set<_ulong>					m_FilterIndexSet[FLTER_END];			//중복 방지를 위한 자료구조 set
	_int										m_iFilterCaseNum = 0;

private:			/*Pointer*/
	ID3D11Texture2D*					pTexture2D = nullptr;
	D3D11_TEXTURE2D_DESC		TextureDesc;

private:
	class CGameInstance*			m_pGameInstance = nullptr;
	class	CTerrain*						m_pSelectedTerrain = nullptr;
	class			CTerrain*				m_pPickingTerrain = nullptr;


	class CVIBuffer_Terrain*		m_pSelected_Buffer = nullptr;
	class CTransform*					m_pSelected_Tranform = nullptr;


	class CTexture*					m_pHeightTexture = nullptr;

private: /*For.ToolStart*/
	_bool									m_bFilterStart = false;
	_bool									m_bFilterErase = false;
	_bool									m_bSaveWrite = false;
	string									m_strFileName = "";

	_bool									m_bControlHeight = false;

	_int										m_CurFilterIndexSize = 0;
	_int										m_OldFilterIndexSize = -1;
public:
	static	CImgui_TerrainEditor*	Create(ID3D11Device* pDevice, ID3D11DeviceContext*	pContext, void* pArg = nullptr);
	virtual void				Free() override;

};
END



