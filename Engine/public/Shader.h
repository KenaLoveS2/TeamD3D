#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CShader final : public CComponent
{
protected:
	CShader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CShader(const CShader& rhs);
	virtual ~CShader() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* pElements, const _uint iNumElements);
	virtual HRESULT Initialize(void* pArg, class CGameObject* pOwner) override;

public:
	HRESULT Begin(_uint iPassIndex);

	HRESULT Set_RawValue(const char* pConstantName, const void* pData, _uint iLength);
	HRESULT Set_Matrix(const char* pConstantName, const _float4x4* pMatrix);
	HRESULT Set_MatrixArray(const char* pConstantName, const _float4x4* pMatrix, _uint iNumMatrices);
	HRESULT Set_ShaderResourceViewArray(const char* pConstantName, ID3D11ShaderResourceView** ppSRV, _uint iNumTextures);
	HRESULT Set_ShaderResourceView(const char* pConstantName, ID3D11ShaderResourceView* pSRV);

	void Imgui_RenderProperty() override;
private:
	ID3DX11Effect*				m_pEffect = nullptr;
	vector<ID3D11InputLayout*>	m_InputLayouts;

private:
	_uint						m_iNumPasses = 0;
	const D3D11_INPUT_ELEMENT_DESC* m_pElements; 
	_uint m_iNumElements;

private:
	_uint		m_iCurrentPassIndex = 0;

public:
	static CShader* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* pElements, const _uint iNumElements);
	virtual CComponent* Clone(void* pArg = nullptr, class CGameObject* pOwner = nullptr) override;
	virtual void Free() override;
	HRESULT ReCompile();
};


END
