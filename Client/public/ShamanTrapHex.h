#pragma once

#include "Effect_Mesh.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CBone;
END

BEGIN(Client)
class CShamanTrapHex final : public CEffect_Mesh
{
	enum PARTS { /*GEO, PLANE,*/ SHAMAN_0, SHAMAN_1, SHAMAN_2, SHAMAN_3, SHAMAN_4, PARTS_END };
	enum STATE { START_TRAP, TRAP, BREAK_TRAP, END_TRAP, STATE_END };
	enum ANIMATION { CONTRACT, EXPAND, ANIMATION_END };

private:
	CShamanTrapHex(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CShamanTrapHex(const CShamanTrapHex& rhs);
	virtual ~CShamanTrapHex() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* pArg) override;
	virtual void			Tick(_float fTimeDelta) override;
	virtual void			Late_Tick(_float fTimeDelta) override;
	virtual HRESULT			Render() override;

	_bool	IsActiveState();

public:
	virtual void			ImGui_AnimationProperty() override;
	virtual void			Imgui_RenderProperty() override;
	
	_float4					Get_JointBonePos();

private:
	HRESULT				SetUp_Components();
	HRESULT				SetUp_ShaderResources();
	HRESULT				SetUp_Effects();
	void				SetColor(_float4& vColor, const char* pTag);

	class CGameObject*	m_pPart[PARTS_END] = { nullptr, };
	CBone*				m_pPartBone = nullptr;

private:
	_uint				m_iNumMeshes = 0;
	_float4				m_vEdgeColor;
	_float4				m_vBaseColor;

	STATE m_eState = STATE_END;
	_float4 m_vInvisiblePos = { -100, -100, -100, 1.f };

	_float m_fTrapTime = 0.f;
	
	_bool m_bTrapSuccess = false;

public:
	static CShamanTrapHex*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	    Clone(void* pArg = nullptr)  override;
	virtual void					    Free() override;

	void Trap_Proc(_float fTimeDelta);
	void Execute_Trap(_float4 vPos);
	void Execute_Break();
	void Execute_End();

	_bool IsTrapSuccess() { return m_bTrapSuccess; }
};

END