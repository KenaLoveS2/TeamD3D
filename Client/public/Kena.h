#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CRenderer;
class CShader;
class CModel;
class CCollider;
class CRenderer;
class CNavigation;
class CFSMComponent;
END

BEGIN(Client)

class CKena final : public CGameObject
{
private:
	CKena(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CKena(const CKena& rhs);
	virtual ~CKena() = default;

public:
	virtual HRESULT		Initialize_Prototype() override;
	virtual HRESULT		Initialize(void* pArg) override;
	virtual void			Tick(_float fTimeDelta) override;
	virtual void			Late_Tick(_float fTimeDelta) override;
	virtual HRESULT		Render() override;
	virtual void			Imgui_RenderProperty() override;

private:
	CRenderer*			m_pRendererCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CModel*				m_pModelCom = nullptr;
	CCollider*				m_pRangeCol = nullptr;
	CNavigation*			m_pNavigationCom = nullptr;
	CFSMComponent*	m_pStateMachine = nullptr;

private:
	vector<class CKena_Parts*>	m_vecPart;

private:
	_int					m_iAnimationIndex = 0;

private:
	HRESULT				Ready_Parts();
	HRESULT				SetUp_Components();
	HRESULT				SetUp_ShaderResources();
	HRESULT				SetUp_State();

public:
	static CKena*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg = nullptr) override;
	virtual void			Free() override;
};

END