#pragma once
#include"Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CShader;
class CModel;
class CCollider;
END

BEGIN(Client)

class CKena_Parts abstract : public CGameObject
{
public:
	typedef enum KenaPartsType {
		KENAPARTS_OUTFIT, KENAPARTS_WEAPON, KENAPARTS_ROT, KENAPARTS_END
	} KENAPARTS_TYPE;

	typedef struct tagKenaParts : public CGameObject::GAMEOBJECTDESC
	{
		class CKena*			pPlayer = nullptr;
		KENAPARTS_TYPE	eType = KENAPARTS_END;
	} KENAPARTS_DESC;

protected:
	CKena_Parts(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CKena_Parts(const CKena_Parts& rhs);
	virtual ~CKena_Parts() = default;

public:
	virtual HRESULT		Initialize_Prototype() override;
	virtual HRESULT		Initialize(void* pArg) override;
	virtual void			Tick(_float fTimeDelta) override;
	virtual void			Late_Tick(_float fTimeDelta) override;
	virtual HRESULT		Render() override;
	virtual void			Imgui_RenderProperty() override;

protected:
	CRenderer*			m_pRendererCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CModel*				m_pModelCom = nullptr;

protected:
	class CKena*			m_pPlayer = nullptr;
	KENAPARTS_DESC	m_tPartsInfo;

protected:
	virtual HRESULT		Ready_Parts() { return S_OK; }
	virtual HRESULT		SetUp_Components() PURE;
	virtual HRESULT		SetUp_ShaderResource() PURE;

public:
	virtual CGameObject*	Clone(void* pArg = nullptr) PURE;
	virtual void				Free() override;
};

END