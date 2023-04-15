#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CModel;
class CShader;
class CBone;
END

BEGIN(Client)
class CFakeShaman final : public CGameObject
{
public:
	typedef struct tagShamanTrapDesc
	{
		_float4x4		PivotMatrix;
		CBone*			pSocket;
		CTransform* pTargetTransform;
		_float4			vPivotRot;
	}ShamanTrapDESC;

private:
	CFakeShaman(ID3D11Device* pDevice, ID3D11DeviceContext* p_context);
	CFakeShaman(const CFakeShaman& rhs);
	virtual ~CFakeShaman() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Late_Initialize(void * pArg);
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT RenderShadow() override;
	virtual void Imgui_RenderProperty() override;
	virtual void ImGui_ShaderValueProperty() override;
	virtual void ImGui_PhysXValueProperty() override;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();
	HRESULT SetUp_ShadowShaderResources();

private:
	ShamanTrapDESC					m_Desc;
	CRenderer*								m_pRendererCom = nullptr;
	CShader*									m_pShaderCom = nullptr;
	CModel*									m_pModelCom = nullptr;
	CTexture* m_pDissolveTextureCom = nullptr;
	_uint											m_iNumMeshes = 0;
	_float4										m_vPivotPos;
	_float4										m_vPivotRot;
	_float4x4									m_SocketMatrix;

	_bool m_bDisolve = false;
	_float m_fDissolveTime = false;
	_float4 m_vTrapPosition = { 0.f,0.f, 0.f, 1.f, };

	_tchar m_szCopySoundKey_Ding[64] = { 0, };
	_bool m_bSoundFlag = false;
	
	_bool m_bInvisivleFlag = false;

public:
	static CFakeShaman* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

	_int Execute_Collision(CGameObject * pTarget, _float3 vCollisionPos, _int iColliderIndex);
	void Clear();

	_float4 Get_TrapPosition() { return m_vTrapPosition; }
	void Go_InvisiblePos();
};
END
