#pragma once
#include "Effect_Mesh.h"
#include "Bone.h"
#include "Kena.h"

BEGIN(Engine)
class CBone;
END

BEGIN(Client)

class CE_ShamanIceDagger final : public CEffect_Mesh
{
public:
	enum CHILD
	{
		CHILD_TRAIL,
		CHILD_BREAK,
		CHILD_END
	};
private:
	CE_ShamanIceDagger(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CE_ShamanIceDagger(const CE_ShamanIceDagger& rhs);
	virtual ~CE_ShamanIceDagger() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar * pFilePath = nullptr);
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Late_Initialize(void* pArg = nullptr)override;
	virtual void    Tick(_float fTimeDelta) override;
	virtual void    Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual void	Imgui_RenderProperty() override;

public:
	void	Reset();
	void	Set_CenterBone(CBone* pBone, _float fAngle, _int iIndex) { 
		m_iIndex = iIndex;
		m_fAngle = fAngle;
		m_pCenterBone = pBone;
	}

	_bool 	Get_Finalposition() { return m_bFinalposition; }
	_bool 	Get_Chase() { return m_bChase; }
	_int	Get_Index() { return m_iIndex; }

	void	Set_Chase(_bool bChase) { m_bChase = bChase; }
	void	Set_IceDaggerMatrix();

public:
	void	Tick_IceDagger(_float fTimeDelta);
	void	Tick_Chase(_float fTimeDelta);

private:
	HRESULT SetUp_ShaderResources();
	HRESULT SetUp_Components();
	HRESULT SetUp_Effects();

private:
	CBone*		 m_pCenterBone = nullptr;
	CKena*		 m_pKena = nullptr;

	_vector		 m_pKenaPosition;
	_matrix		 m_pUpdateMatrix = XMMatrixIdentity();
	_float		 m_fAngle = 0.0f;
	_float		 m_fRange = 0.8f;
	_float		 m_fDurateionTime = 0.0f;
	_float		 m_fIdle2Chase = 0.0f;
	_float	    m_fDissolveTime = 0.0f;

	_int		 m_iIndex = 0;

	_bool		 m_bFinalposition = false;
	_bool		 m_bChase = false;
	_bool		 m_bDissolve = false;

public:
	static  CE_ShamanIceDagger* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar * pFilePath = nullptr);
	virtual CGameObject*	  Clone(void* pArg = nullptr) override;
	virtual void			  Free() override;

};

END