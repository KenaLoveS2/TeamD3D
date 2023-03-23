#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)
class CEffect_Base_S2 abstract : public CGameObject
{
protected:
	CEffect_Base_S2(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffect_Base_S2(const CEffect_Base_S2& rhs);
	virtual ~CEffect_Base_S2() = default;

public:
	inline	void					Set_Target(CGameObject* pTarget) { m_pTarget = pTarget; }

public:
	virtual HRESULT					Initialize_Prototype();
	virtual HRESULT					Initialize(void* pArg) override;
	virtual HRESULT					Late_Initialize(void* pArg = nullptr) override;
	virtual void					Tick(_float fTimeDelta) override;
	virtual void					Late_Tick(_float fTimeDelta) override;
	virtual HRESULT					Render() override;

public:
	virtual void					Imgui_RenderProperty() = 0;
	virtual HRESULT					Save_Data() = 0;
	virtual	HRESULT					Load_Data(_tchar* fileName) = 0;

protected:	/* Tool Function */
	_float4		ColorCode();

protected:
	CGameObject*					m_pTarget;
	_tchar*							m_pfileName;
	_bool							m_bFire;

protected: /* Property */
	_int							m_iRenderPass;
	_int							m_iTextureIndex;
	_float4							m_vColor;

public:
	virtual void					Free()			override;

};
END