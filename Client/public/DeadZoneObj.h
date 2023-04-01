#pragma once
#include "Client_Defines.h"
#include "EnviromentObj.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
class CTexture;
END

/*나무 뿌리 와 관련된 클래스입니다. */
BEGIN(Client)
class CControlRoom;
class CDeadZoneObj  final : public CEnviromentObj
{
private:
	CDeadZoneObj(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDeadZoneObj(const CDeadZoneObj& rhs);
	virtual ~CDeadZoneObj() = default;

public:
	void	Set_DeadZoneModel_ChangeID(_int iChangeModel_ID){ m_iDeadZoneModelID  = iChangeModel_ID;}
	const _int Get_DZModel_ID() const {
		return m_iDeadZoneModelID;
	}

public:
	virtual HRESULT		Initialize_Prototype() override;
	virtual HRESULT		Initialize(void* pArg) override;
	virtual HRESULT		Late_Initialize(void* pArg = nullptr) override;

	virtual void				Tick(_float fTimeDelta) override;
	virtual void				Late_Tick(_float fTimeDelta) override;
	virtual HRESULT		Render() override;
	virtual HRESULT		RenderShadow() override;


public:
	void	Change_Model(_int iDissolveTimer);

private:
	CModel*				m_pModelCom = nullptr;
	class CInteraction_Com*			m_pInteractionCom = nullptr;
	class CControlMove*				m_pControlMoveCom = nullptr;

	_bool				m_bOnlyTest = false;
	_int				m_iDeadZoneModelID = -1;
public:
	virtual HRESULT		Add_AdditionalComponent(_uint iLevelIndex, const _tchar* pComTag, COMPONENTS_OPTION eComponentOption)override;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();
	HRESULT SetUp_ShadowShaderResources();

public:
	static  CDeadZoneObj*	   Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};
END
