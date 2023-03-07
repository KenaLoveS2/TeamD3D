#pragma once

#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CRenderer;
class CShader;
class CModel;
class CCollider;
class CBone;
END

BEGIN(Client)

class CMonsterWeapon : public CGameObject
{
public:
	typedef struct tagMonsterWeaponDesc
	{
		_float4x4		PivotMatrix;
		CBone*			pSocket;
		CTransform* pTargetTransform;
	}MONSTERWEAPONDESC;

protected:
	CMonsterWeapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonsterWeapon(const CMonsterWeapon& rhs);
	virtual ~CMonsterWeapon() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* pArg) override;
	virtual HRESULT			Late_Initialize(void* pArg) override;
	virtual void					Tick(_float fTimeDelta) override;
	virtual void					Late_Tick(_float fTimeDelta) override;
	virtual HRESULT			Render() override;
	virtual HRESULT			RenderShadow() override;
	virtual void					Imgui_RenderProperty() override;
	virtual void					ImGui_ShaderValueProperty() override;
	virtual void					ImGui_PhysXValueProperty() override;

protected:
	CRenderer*					m_pRendererCom = nullptr;
	CShader*						m_pShaderCom = nullptr;
	CModel*						m_pModelCom = nullptr;
	CCollider*						m_pRangeCol = nullptr;

protected:
	MONSTERWEAPONDESC			m_WeaponDesc;
	_float4x4									m_SocketMatrix;

	_float4										m_vPivotPos;
	_float4										m_vPivotRot;

private:
	virtual  void		 	Update_Collider(_float fTimeDelta) PURE;
	virtual  HRESULT  SetUp_Components() PURE;
	virtual  HRESULT  SetUp_ShaderResources() PURE;
	virtual	HRESULT	SetUp_ShadowShaderResources() PURE;

public:
	virtual CGameObject* Clone(void* pArg = nullptr) PURE;
	virtual void Free() override;
};

END