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


public:
	virtual HRESULT		Initialize_Prototype() override;
	virtual HRESULT		Initialize(void* pArg) override;
	virtual HRESULT		Late_Initialize(void* pArg = nullptr) override;

	virtual void				Tick(_float fTimeDelta) override;
	virtual void				Late_Tick(_float fTimeDelta) override;
	virtual HRESULT		Render() override;
	virtual HRESULT		RenderShadow() override;

#ifdef _DEBUG
	_float4				Set_ColorValue();
#endif
public:
	void	Change_Model(_int iDissolveTimer);

private:
	CModel*				m_pModelCom = nullptr;
	CModel*				m_pModelChangeCom = nullptr;
	CTexture*			m_pDissolveTextureCom = nullptr;

private:
	_bool				m_bOnlyTest = false;	// Test

	_int				m_iDeadZoneModelID = -1;	// Model Change
	_bool				m_bDeadZoneRender = true;
	_bool				m_bChangeModelRender = false;
	_bool				m_bChaning_ModelRender = false;


	_float				m_fTimeDeltaTest = 0.f;
	_int				m_iSign = 1;


	_float				m_fDissolveTime = 1.f;
	_bool				m_bDissolve = false;


	_float4 m_vColor = _float4(130.f / 255.f, 144.f / 255.f, 196.f / 255.f, 1.f);
	

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ChangeModel();
	HRESULT SetUp_ShaderResources();
	HRESULT SetUp_ShadowShaderResources();

public:
	static  CDeadZoneObj*	   Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};
END
