#pragma once
#include "Effect.h"
#include "PhysX_Defines.h"

BEGIN(Client)

class CE_ShamanLazer final : public CEffect
{
public:
	enum CHILD
	{
		CHILD_LINE,
		CHILD_SP,
		CHILD_P,
		CHILD_END
	};

private:
	CE_ShamanLazer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CE_ShamanLazer(const CE_ShamanLazer& rhs);
	virtual ~CE_ShamanLazer() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pFilePath = nullptr);
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Late_Initialize(void* pArg = nullptr)override;
	virtual void    Tick(_float fTimeDelta) override;
	virtual void    Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	void			Reset();
	
	virtual _int	Execute_Collision(CGameObject* pTarget, _float3 vCollisionPos, _int iColliderIndex) override;
	virtual void	ImGui_PhysXValueProperty()override;

private:
	HRESULT			SetUp_ShaderResources();
	HRESULT			SetUp_Components();
	HRESULT			SetUp_Effects();

	void			Imgui_RenderProperty() override;

public:
	void			TurnOnLazer( _float fTimeDelta);
	void			Set_SpawnPos(_float4 vPos);
	void			Calculate_Path(_float fTimeDelta);

	_bool			Get_FinalState() { return m_bFinalState; }

private:
	_vector			m_SpawnPos;
	_float			m_fRange = 2.0f;
	_bool			m_bFinalState = false;

private:
	_float3			m_vAimPos;
	list<_float4>	m_PathList;

	_smatrix		m_matDummy;

private:
	class CE_LazerTrail* m_pPathTrail = nullptr;
	class CShamanTrapHex* m_pShamanTrapHex = nullptr;

	_tchar m_szCopySoundKey_Charge[64] = { 0, };
	_tchar m_szCopySoundKey_Fire1[64] = { 0, };
	_tchar m_szCopySoundKey_Fire2[64] = { 0, };
	
	_bool m_bLaserCharge = false;
	_bool m_bLaserFire1 = false;	
	_bool m_bLaserFire2 = false;

public:
	static  CE_ShamanLazer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pFilePath = nullptr);
	virtual CGameObject*	   Clone(void* pArg = nullptr) override;
	virtual void               Free() override;

	void Play_LaserSound(_bool* pCheckFlag, _tchar* pSoundKey, _float fVolume);
};

END