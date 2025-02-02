#pragma once
#include "Component.h"
#include "Json/json.hpp"
#include <fstream>


BEGIN(Engine)
class ENGINE_DLL CStatus abstract : public CComponent
{
protected:
	string m_strJsonFilePath;

	_int m_iMaxHP = 0;
	_int m_iHP = 0;
	_int m_iAttack = 0;
	_int m_iHealAmount = 0;

protected:
	CStatus(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CStatus(const CStatus& rhs);
	virtual ~CStatus() = default;
	
public:
	virtual CComponent* Clone(void* pArg = nullptr, class CGameObject* pOwner = nullptr) = 0;	
	virtual void Free() override;
		
	virtual HRESULT Initialize_Prototype() override;	
	virtual HRESULT Initialize(void* pArg, class CGameObject* pOwner);
	virtual void Tick(_float fTimeDelta);
	virtual void Imgui_RenderProperty() override;

	
	virtual HRESULT Save(Json& rJson);
	virtual HRESULT Load(Json& rJson);

	virtual _bool IsDead();
	virtual _bool UnderAttack(CStatus* pEnemyStatus);
	virtual _bool UnderAttack(_int fDamage);

	virtual void Revive() { m_iHP = m_iMaxHP; }

public:
	inline _int Get_MaxHP() { return m_iMaxHP; }
	inline _int Get_HP() { return m_iHP; }
	inline _int Get_Attack() { return m_iAttack; }
	
	_float Get_PercentHP();
	void Add_CurrentHP(_int iValue);
	void Add_HealAmount(_int iValue) { m_iHealAmount += iValue; }

public:
	inline void Set_MaxHP(_int iValue) { m_iMaxHP = iValue; }
	inline void Set_HP(_int iValue) { m_iHP = iValue; }
	inline void Set_Attack(_int iValue) { m_iAttack = iValue; }
};

END