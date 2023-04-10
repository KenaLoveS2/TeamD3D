#pragma once
#include "Client_Defines.h"
#include "UI_Node.h"

BEGIN(Client)
class CUI_NodeVideo final : public CUI_Node
{
private:
	CUI_NodeVideo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_NodeVideo(const CUI_NodeVideo& rhs);
	virtual ~CUI_NodeVideo() = default;

public:
	void	Play_Video(wstring wstrVideo, _bool bLoop = false,  _float fTime = 0.01f);

public:
	virtual HRESULT			Initialize_Prototype()			override;
	virtual HRESULT			Initialize(void* pArg)			override;
	virtual void			Tick(_float fTimeDelta)			override;
	virtual void			Late_Tick(_float fTimeDelta)	override;
	virtual HRESULT			Render()						override;

public:
	virtual void			Imgui_RenderProperty();
	//virtual HRESULT			Save_Data();
	//virtual HRESULT			Load_Data(wstring fileName);

private:
	HRESULT					SetUp_Components();
	HRESULT					SetUp_ShaderResources();

private:
	wstring					m_wstrCurVideo;
	_bool					m_bEnd;
	_bool					m_bLoop;
	
	_float					m_fTimeAcc;
	_float					m_fTime;

public:
	static	CUI_NodeVideo*		Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CGameObject*		Clone(void* pArg = nullptr);
	virtual void				Free() override;
};
END
