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
		_float3 vSize;
		_float fLinearDamping;
		_float fMass;

		tag_Dynamic_StoneCube()
			: fLinearDamping{ 0.f },
			fMass{ 0.f }
			, vSize{ _float3(1.f, 1.f, 1.f) }
		{
			
		}

	}Dynamic_StoneCube_DESC;


private:
	CDynamic_StoneCube(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDynamic_StoneCube(const CDynamic_StoneCube& rhs);
	virtual ~CDynamic_StoneCube() = default;

public:
	void	Set_StoneCubeDesc(Dynamic_StoneCube_DESC& StoneCubeDesc);


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

private:
	class CShader* m_pShaderCom = nullptr;
	class CRenderer* m_pRendererCom = nullptr;
	class CModel* m_pModelCom = nullptr;

private:
	CEnviromentObj::ENVIROMENT_DESC							m_EnviromentDesc;
	Dynamic_StoneCube_DESC									m_StoneCubeDesc;
	_bool				m_bTestOnce = false;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();
	HRESULT SetUp_ShadowShaderResources();

public:
	static  CDynamic_StoneCube* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr)override;
	virtual void Free() override;
};
END
