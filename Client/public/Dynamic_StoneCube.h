#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
#include "EnviromentObj.h"
BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
class CTexture;
END


BEGIN(Client)
class CDynamic_StoneCube final :public CGameObject
{
public:
	typedef struct  tag_Dynamic_StoneCube
	{
		_tchar pModelName[64] =L"";
		_float4 vParentPos;
		_float3 vSize;
		_float3 vPos;
		_float fLinearDamping;
		_float fMass;
	}Dynamic_StoneCube_DESC;


private:
	CDynamic_StoneCube(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDynamic_StoneCube(const CDynamic_StoneCube& rhs);
	virtual ~CDynamic_StoneCube() = default;

public:
	void				Set_CollActive();

public:
	virtual HRESULT		Initialize_Prototype() override;
	virtual HRESULT		Initialize(void* pArg=nullptr) override;
	virtual HRESULT		Late_Initialize(void* pArg = nullptr) override;
	virtual void				Tick(_float fTimeDelta) override;
	virtual void				Late_Tick(_float fTimeDelta) override;
	virtual HRESULT		Render() override;
	virtual HRESULT		RenderShadow() override;
	virtual HRESULT		RenderCine() override;
	//virtual void					ImGui_PhysXValueProperty() override;

public:
	void				Actor_Clear();
	void				Imgui_Move_Pos(_float4 vParentPos);
	

private:
	class CShader* m_pShaderCom = nullptr;
	class CRenderer* m_pRendererCom = nullptr;
	class CModel* m_pModelCom = nullptr;

private:
	Dynamic_StoneCube_DESC		m_StoneCubeDesc;
	_bool				m_bTestOnce = false;

				

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();
	HRESULT SetUp_ShadowShaderResources();

public:
	static  CDynamic_StoneCube* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr)override;
	virtual void Free() override;

	void Execute_SleepEnd();
};
END
