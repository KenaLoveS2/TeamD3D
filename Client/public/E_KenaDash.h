#pragma once
#include "Effect.h"

BEGIN(Client)

class CE_KenaDash final : public CEffect
{
public:
	enum CHILD
	{
		CHILD_RING_0,
		CHILD_RING_1,
		CHILD_RING_2,
		CHILD_CONE,
		CHILD_P,
		CHILD_END
	};

private:
	CE_KenaDash(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CE_KenaDash(const CE_KenaDash& rhs);
	virtual ~CE_KenaDash() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pFilePath = nullptr);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void    Tick(_float fTimeDelta) override;
	virtual void    Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	void	Reset();

public:
	void	Imgui_RenderProperty();
	HRESULT	SetUp_Child();

public:
	void	Tick_State(_float4 vPos);
	void	Tick_RingState(_float4 vPos);
	
private:
	_float fTime = 0.0f;

public:
	static  CE_KenaDash* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pFilePath = nullptr);
	virtual CGameObject*  Clone(void* pArg = nullptr) override;
	virtual void          Free() override;

};

END