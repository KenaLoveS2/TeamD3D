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
class CDeadZoneBossTree  final : public CEnviromentObj
{
private:
	CDeadZoneBossTree(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDeadZoneBossTree(const CDeadZoneBossTree& rhs);
	virtual ~CDeadZoneBossTree() = default;


public:
	virtual HRESULT		Initialize_Prototype() override;
	virtual HRESULT		Initialize(void* pArg) override;
	virtual HRESULT		Late_Initialize(void* pArg = nullptr) override;

	virtual void				Tick(_float fTimeDelta) override;
	virtual void				Late_Tick(_float fTimeDelta) override;
	virtual HRESULT		Render() override;
	virtual HRESULT		RenderShadow() override;



private:
	CModel* m_pModelCom = nullptr;

	_bool				m_bOnlyTest = false;
	

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();
	HRESULT SetUp_ShadowShaderResources();

public:
	static  CDeadZoneBossTree* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};
END
