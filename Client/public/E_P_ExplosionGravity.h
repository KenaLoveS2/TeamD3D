#pragma once
#include "Effect_Point_Instancing.h"

BEGIN(Engine)
class CBone;
class CModel;
END

BEGIN(Client)

class CE_P_ExplosionGravity final : public CEffect_Point_Instancing
{
public:
	enum TYPE
	{ 
		TYPE_DEFAULT,
		/* Boss + Monster */
		TYPE_DEAD_MONSTER, 
		TYPE_BOSS_WEAPON,
		TYPE_BOSS_ATTACK,
		TYPE_BOSS_PARRY,
		TYPE_BOSS_GATHER,
		TYPE_BOSS_HAND,
		TYPE_ROCK_GOLEM,
		/* Kena */
		TYPE_DAMAGE_PULSE,
		TYPE_KENA_ATTACK,
		TYPE_KENA_ATTACK2,
		/* Obejct */
		TYPE_HEALTHFLOWER,
		TYPE_BOWTARGET,

		TYPE_END
	};

private:
	CE_P_ExplosionGravity(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CE_P_ExplosionGravity(const CE_P_ExplosionGravity& rhs);
	virtual ~CE_P_ExplosionGravity() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pFilePath = nullptr);
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Late_Initialize(void* pArg) override;
	virtual void    Tick(_float fTimeDelta) override;
	virtual void    Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	HRESULT SetUp_ShaderResources();
	HRESULT SetUp_Components();

	HRESULT SetUp_ChangeBuffer();
	HRESULT Save_Desc(const char* pFileTag);
	HRESULT Load_Desc(const char* pFilePath);

public:
	TYPE	Get_Type() { return m_eType; }
	void	Set_Option(TYPE eType, _vector vSetDir = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f));
	void	Set_Dir(_vector vSetDir) { m_pVIInstancingBufferCom->Set_Dir(vSetDir); }
	void	Reset();

public:
	void	ParticleOption_Parabola(CVIBuffer_Point_Instancing::POINTDESC* ePointDesc, _float fDiffuseIndx, _fvector vColor,
		_float pSpeed, _float fTerm, _float2 pSize, _bool bSetDir, _fvector vDir = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f));

public:
	void	UpdateParticle(_float4 vPos, _vector vDir = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f));
	void	UpdateColor(_fvector vColor); /* => Kena State Color Change */
	void	Set_ParentBone(class CBone* pBone) { 
		Set_Option(CE_P_ExplosionGravity::TYPE_BOSS_HAND);
		m_pParentBone = pBone; 
	}

	HRESULT Reset_Type();

	/* Option Test Tool */
public:
	void	Set_OptionTool();

private:
	TYPE	m_eType = TYPE_DEFAULT;
	_float	m_fDurationTime = 0.0f;
	_float4	m_fRandomColor = { 1.f,1.f,1.f,1.f };

	_vector m_vFixPos = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

	class CBone* m_pParentBone = nullptr;
	class CModel* m_pParentModel = nullptr;

public:
	static  CE_P_ExplosionGravity* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pFilePath = nullptr);
	virtual CGameObject*  Clone(void* pArg = nullptr) override;
	virtual void          Free() override;
};

END