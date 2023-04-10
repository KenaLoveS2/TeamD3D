#pragma once
#include "Effect_Trail.h"

BEGIN(Client)

class CE_LazerTrail final : public CEffect_Trail
{
private:
	CE_LazerTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CE_LazerTrail(const CE_LazerTrail& rhs);
	virtual ~CE_LazerTrail() = default;

public:
	virtual HRESULT			Initialize_Prototype(const _tchar* pFilePath = nullptr);
	virtual HRESULT			Initialize(void* pArg) override;
	virtual void			Tick(_float fTimeDelta) override;
	virtual void			Late_Tick(_float fTimeDelta) override;
	virtual HRESULT			Render() override;
	virtual void			Imgui_RenderProperty() override;

public:
	void					Add_BezierCurve(const _float4& vStartPos, const _float4& vSplinePos, _float fTimeDelta);
	void					Copy_Path(list<_float4>& PathList);
	void					Reset();
	void					ToolTrail(const char* ToolTag);

private:
	class CTexture*			m_pTrailflowTexture = nullptr;
	class CTexture*			m_pTrailTypeTexture = nullptr;

private:
	_uint					m_iTrailFlowTexture = 0;
	_uint					m_iTrailTypeTexture = 0;
	_float					m_fDurationTime = 0.0f;

private:
	_float4					m_vStartPos;
	_float4					m_vSplinePos;
	vector<_float4>			m_vecPositions;

private:
	HRESULT					SetUp_ShaderResources();

public:
	static  CE_LazerTrail*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pFilePath = nullptr);
	virtual CGameObject*	Clone(void* pArg = nullptr) override;
	virtual void			Free() override;

};

END