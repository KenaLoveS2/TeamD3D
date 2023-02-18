#pragma once
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CVIBuffer_Rect;
class CUI abstract : public CGameObject
{
public:
	typedef struct tagUIDesc
	{
		_float				fPosX;
		_float				fPosY;
		_float				fSizeX;
		_float				fSizeY;
		_float4x4			ViewMatrix;
		_float4x4			ProjMatrix;
	} UIDESC;

protected:
	CUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI(const CUI& rhs);
	virtual ~CUI() = default;


public:
	void				Set_Parent(CUI* pUI) {
		m_pParent = pUI;
		Safe_AddRef(m_pParent);
	}


public:
	virtual HRESULT			Initialize_Prototype()			override;
	virtual HRESULT			Initialize(void* pArg)			override;
	virtual void			Tick(_float fTimeDelta)			override;
	virtual void			Late_Tick(_float fTimeDelta)	override;
	virtual HRESULT			Render()						override;



protected:
	CShader*			m_pShaderCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CVIBuffer_Rect*		m_pVIBufferCom = nullptr;

protected:
	UIDESC				m_tDesc;
	CUI*				m_pParent;

public:
	virtual CGameObject*	Clone(void* pArg = nullptr) = 0;
	virtual void			Free()	override;


};
END