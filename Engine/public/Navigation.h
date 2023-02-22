#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CNavigation final : public CComponent
{
public:
	typedef struct tagNavigationDesc
	{
		_int		iCurrentIndex = -1;
	}NAVIDESC;

private:
	CNavigation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNavigation(const CNavigation& rhs);
	virtual ~CNavigation() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pNavigationDataFilePath);
	virtual HRESULT Initialize(void* pArg, class CGameObject* pOwner) override;

public:
	_bool isMove_OnNavigation(_fvector TargetPos); 

private:	
	vector<class CCell*>	m_Cells;
	NAVIDESC				m_NaviDesc;

private:
	HRESULT	Ready_Neighbor();


#ifdef _DEBUG
private:
	class CShader*			m_pShader = nullptr;
public:
	virtual HRESULT Render();
#endif // _DEBUG


public:
	static CNavigation* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pNavigationDataFilePath);
	virtual CComponent* Clone(void* pArg, class CGameObject* pOwner = nullptr) override;
	virtual void Free() override;

	HRESULT Set_IndexByCurrentPos(_fvector vPos);
	_int Get_IndexByCurrentPos(_fvector vPos);
	void Set_OnNavigation(class CTransform* pTransform, _float fPlusValue = 0.f);
	_float HeightOnNavigation(_fvector vPos);
};

END