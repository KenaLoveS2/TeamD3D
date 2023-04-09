#pragma once

//#include "Effect_Mesh.h"
#include "Effect.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CBone;
END

BEGIN(Client)

class CShamanTrapGeo final : public CEffect //public CEffect_Mesh
{
public:
	typedef struct tagShamanTrapDesc
	{
		_float4x4		PivotMatrix;
		CBone*			pSocket;
		CTransform*			pTargetTransform;
	}ShamanTrapDESC;

private:
	CShamanTrapGeo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CShamanTrapGeo(const CShamanTrapGeo& rhs);
	virtual ~CShamanTrapGeo() = default;

public:
	virtual HRESULT			Initialize_Prototype(const _tchar* pFilePath) override;
	virtual HRESULT			Initialize(void* pArg) override;
	virtual void			Tick(_float fTimeDelta) override;
	virtual void			Late_Tick(_float fTimeDelta) override;
	virtual HRESULT			Render() override;

	_bool					IsActiveState();

public:
	virtual void			ImGui_AnimationProperty() override;
	virtual void			Imgui_RenderProperty() override;

private:
	HRESULT					SetUp_Components();
	HRESULT					SetUp_ShaderResources();

private:
	ShamanTrapDESC			m_Desc;
	_float4x4				m_SocketMatrix;
	_float4					m_vPivotPos;
	_float4					m_vPivotRot;
	_uint					m_iNumMeshes = 0;

public:
	static CShamanTrapGeo*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pFilePath);
	virtual CGameObject*	Clone(void* pArg = nullptr)  override;
	virtual void			Free() override;
};

END