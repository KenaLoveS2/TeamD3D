#pragma once
#include "Effect_Trail.h"

BEGIN(Client)

class CE_BombTrail final : public CEffect_Trail
{
private:
	CE_BombTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CE_BombTrail(const CE_BombTrail& rhs);
	virtual ~CE_BombTrail() = default;

public:
	virtual HRESULT			Initialize_Prototype(const _tchar* pFilePath = nullptr);
	virtual HRESULT			Initialize(void* pArg) override;
	virtual void				Tick(_float fTimeDelta) override;
	virtual void				Late_Tick(_float fTimeDelta) override;
	virtual HRESULT			Render() override;
	virtual void				Imgui_RenderProperty() override;

public:
	void						Add_BezierCurve(const _float4& vStartPos, const _float4& vSplinePos, _float fTimeDelta);
	void						Copy_Path(list<_float4>& PathList);
	void						Reset();

private:
	class CTexture*			m_pTrailflowTexture = nullptr;
	class CTexture*			m_pTrailTypeTexture = nullptr;

private:
	_uint						m_iTrailFlowTexture = 0;
	_uint						m_iTrailTypeTexture = 0;

	_float4					m_vStartPos;
	_float4					m_vSplinePos;
	vector<_float4>			m_vecPositions;

private:
	HRESULT					SetUp_ShaderResources();

public:
	static  CE_BombTrail*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pFilePath = nullptr);
	virtual CGameObject*	Clone(void* pArg = nullptr) override;
	virtual void				Free() override;

};

END