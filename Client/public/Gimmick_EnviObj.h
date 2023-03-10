#pragma once
#include "Client_Defines.h"
#include "EnviromentObj.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
class CTexture;
END

BEGIN(Client)
class CGimmick_EnviObj  final : public CEnviromentObj
{
public:
	enum Gimmick_TYPE	{
		Gimmick_TYPE_GO_UP,Gimmick_TYPE_DISSOLVE , Gimmick_TYPE_DISSOLVE_AND_MODEL_CHANGE,
		Gimmick_TYPE_END};

	enum VECTOR_POS {
		VECTOR_POS_X, VECTOR_POS_Y, VECTOR_POS_Z, VECTOR_POS_END};

private:
	CGimmick_EnviObj(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGimmick_EnviObj(const CGimmick_EnviObj& rhs);
	virtual ~CGimmick_EnviObj() = default;

public:
	virtual HRESULT		Initialize_Prototype() override;
	virtual HRESULT		Initialize(void* pArg) override;
	virtual HRESULT		Late_Initialize(void* pArg=nullptr)override;
	virtual void		Tick(_float fTimeDelta) override;
	virtual void		Late_Tick(_float fTimeDelta) override;
	virtual HRESULT		Render() override;

public:
	void				Gimmik_Start(_int iRoomNumber, _float fTimeDelta);

private:
	void				Gimmick_Go_up(_float fTimeDelta);

private:
	CShader*						m_pShaderCom = nullptr;
	CRenderer*						m_pRendererCom = nullptr;
	CModel*							m_pModelCom = nullptr;
	class CInteraction_Com*			m_pInteractionCom = nullptr;
	class CControlMove*				m_pControlMoveCom = nullptr;

private:
	Gimmick_TYPE					m_GimmickType = Gimmick_TYPE_GO_UP;
	_float4							m_vOriginPos;
	_bool							m_bGimmick_Active = false;

public:
	virtual HRESULT		Add_AdditionalComponent(_uint iLevelIndex, const _tchar* pComTag, COMPONENTS_OPTION eComponentOption)override;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();



public:
	static  CGimmick_EnviObj*	   Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};
END
