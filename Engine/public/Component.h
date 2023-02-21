#pragma once

#include "Base.h"

/* �츮�� ������ ����� ������Ʈ(�ؽ���, Ʈ������, ����,�ε�������, ��) ���� �θ� �Ǵ� Ŭ�����̴�. */
/* Ŭ���� �������� �̿��Ͽ� �ϳ��� ���ø������̳ʾȿ� ��Ƶε��� �������Ѵ�. */

BEGIN(Engine)

class ENGINE_DLL CComponent abstract : public CBase
{
protected:
	CComponent(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CComponent(const CComponent& rhs);
	virtual ~CComponent() = default;

public:
	class CGameObject*		Get_Owner() const { return m_pOwner; }
	const wstring&	Get_FilePath() const { return m_wstrFilePath; }

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg, class CGameObject* pOwner);

#ifdef _DEBUG
public:
	virtual HRESULT Render() { return S_OK;  };
#endif // 

protected:
	ID3D11Device*				m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pContext = nullptr;

	class CGameObject*		m_pOwner = nullptr;

protected:
	_bool						m_isCloned = false;
	wstring					m_wstrFilePath = L"";

public:
	virtual CComponent* Clone(void* pArg = nullptr, class CGameObject* pOwner = nullptr) = 0;
	virtual void Free() override;
	
	// �� ������Ʈ���� ������ �����͸� imgui�� �ۼ��Ѵ�.
	virtual void Imgui_RenderProperty() {}
};

END