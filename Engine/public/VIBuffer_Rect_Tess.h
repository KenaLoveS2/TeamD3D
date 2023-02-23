#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Rect_Tess final : public CVIBuffer
{
private:
	CVIBuffer_Rect_Tess(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Rect_Tess(const CVIBuffer_Rect_Tess& rhs);
	virtual ~CVIBuffer_Rect_Tess() = default;

public:
	virtual HRESULT Initialize_Prototype(D3D11_PRIMITIVE_TOPOLOGY ePrimitiveTopology);
	virtual HRESULT Initialize(void* pArg, class CGameObject* pOwner);


public:
	virtual HRESULT Render();

public:
	static CVIBuffer_Rect_Tess* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, D3D11_PRIMITIVE_TOPOLOGY ePrimitiveTopology);
	virtual CComponent* Clone(void* pArg = nullptr, class CGameObject* pOwner = nullptr);
	virtual void Free();
};

END