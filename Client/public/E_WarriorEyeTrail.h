#pragma once
#include "Effect_Trail.h"

BEGIN(Engine)
class CBone;
class CModel;
END

BEGIN(Client)

class CE_WarriorEyeTrail final : public CEffect_Trail
{
public:
	enum TYPE
	{
		TYPE_L,
		TYPE_R,
		TYPE_END
	};
private:
	CE_WarriorEyeTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CE_WarriorEyeTrail(const CE_WarriorEyeTrail& rhs);
	virtual ~CE_WarriorEyeTrail() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pFilePath = nullptr);
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Late_Initialize(void* pArg) override;
	virtual void    Tick(_float fTimeDelta) override;
	virtual void    Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	void			Reset();

private:
	HRESULT			SetUp_ShaderResources();
	HRESULT			SetUp_Components();

public:
	void			Update_Trail();
	void			ToolTrail(const char* ToolTag);

	void			Set_Boneprt(_uint iType, class CBone* pBone) {
		m_eType = (TYPE)iType;
		m_pWarriorBone = pBone;
	}

private:
	class CTexture*	m_pTrailflowTexture = nullptr;
	class CTexture*	m_pTrailTypeTexture = nullptr;
	_uint			m_iTrailFlowTexture = 4;
	_uint			m_iTrailTypeTexture = 2;
	_float			m_fDurationTime = 0.0f;

private:
	class CBone* m_pWarriorBone = nullptr;
	class CModel* m_pWarriorModel = nullptr;
	_float		 m_fRange = 0.5f;
	TYPE		 m_eType = TYPE_L;

public:
	static  CE_WarriorEyeTrail* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pFilePath = nullptr);
	virtual CGameObject*	 Clone(void* pArg = nullptr) override;
	virtual void			 Free() override;

};

END