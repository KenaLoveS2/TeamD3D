#pragma once
#include "Effect.h"

BEGIN(Client)

class CE_Chest final : public CEffect
{
public:
	enum CHILD
	{
		CHILD_0,
		CHILD_1,
		CHILD_END
	};
private:
	CE_Chest(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CE_Chest(const CE_Chest& rhs);
	virtual ~CE_Chest() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pFilePath = nullptr);
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Late_Initialize(void* pArg = nullptr);
	virtual void    Tick(_float fTimeDelta) override;
	virtual void    Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual void	Imgui_RenderProperty()override;

public:
	HRESULT			SetUp_Components();
	HRESULT			SetUp_ShaderResources();
	void			Reset();

	void			Update_childPosition();
	void			Set_ChestLight(_float4 vPos, _fvector vDir, _bool bActive) {
		m_pTransformCom->Set_Position(vPos);
		vParentDir = vDir;
		m_eEFfectDesc.bActive = bActive;
	}

private:
	_vector			vParentDir = { 0.0f,0.0f ,0.0f ,0.0f };

public:
	static  CE_Chest*     Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pFilePath = nullptr);
	virtual CGameObject*  Clone(void* pArg = nullptr) override;
	virtual void          Free() override;

};

END