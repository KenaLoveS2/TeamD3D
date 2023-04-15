#pragma once
#include "Effect_Mesh.h"

BEGIN(Engine)
class CBone;
class CModel;
END

BEGIN(Client)

class CE_ShamanWeaponBall final : public CEffect_Mesh
{
private:
	CE_ShamanWeaponBall(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CE_ShamanWeaponBall(const CE_ShamanWeaponBall& rhs);
	virtual ~CE_ShamanWeaponBall() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar * pFilePath = nullptr);
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Late_Initialize(void* pArg = nullptr)override;
	virtual void    Tick(_float fTimeDelta) override;
	virtual void    Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual void	Imgui_RenderProperty() override;

public:
	void			Update_Trail();
	void			Reset();
	void			Set_Boneprt(class CBone* pBone) {
		m_pShamanBone = pBone;
	}

private:
	HRESULT SetUp_ShaderResources();
	HRESULT SetUp_Components();

private:
	class CBone* m_pShamanBone = nullptr;
	class CModel* m_pShamanModel = nullptr;

public:
	static  CE_ShamanWeaponBall* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar * pFilePath = nullptr);
	virtual CGameObject*	  Clone(void* pArg = nullptr) override;
	virtual void			  Free() override;

};

END