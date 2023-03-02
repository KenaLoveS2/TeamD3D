#pragma once
#include "Client_Defines.h"
#include "UI_Node.h"

BEGIN(Client)
class CUI_NodeQuestMain final : public CUI_Node
{
private:
	CUI_NodeQuestMain(ID3D11Device*	pDevice, ID3D11DeviceContext* pContext);
	CUI_NodeQuestMain(const CUI_NodeQuestMain& rhs);
	virtual ~CUI_NodeQuestMain() = default;

public:
	virtual HRESULT			Initialize_Prototype()			override;
	virtual HRESULT			Initialize(void* pArg)			override;
	virtual void			Tick(_float fTimeDelta)			override;
	virtual void			Late_Tick(_float fTimeDelta)	override;
	virtual HRESULT			Render()						override;

private:
	virtual HRESULT			SetUp_Components() override;
	virtual HRESULT			SetUp_ShaderResources() override;

private: /* test */
	virtual HRESULT			Save_Data();
	virtual HRESULT			Load_Data(wstring fileName);

public:
	static	CUI_NodeQuestMain*		Create(ID3D11Device* pDevice, ID3D11DeviceContext*	pContext);
	virtual CGameObject*			Clone(void* pArg = nullptr);
	virtual void					Free() override;
};
END