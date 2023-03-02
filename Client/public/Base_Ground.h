#pragma once
#include "Client_Defines.h"
#include "EnviromentObj.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
class CTexture;
END

/* 
	맵 찍을때 기준 매쉬를 잡는 클래스 입니다.
	일단은 _DEBUG 용 클래스입니다.
*/

BEGIN(Client)
class CBase_Ground final : public CEnviromentObj
{
private:
	CBase_Ground(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBase_Ground(const CBase_Ground& rhs);
	virtual ~CBase_Ground() = default;

public:
	virtual HRESULT		Initialize_Prototype() override;
	virtual HRESULT		Initialize(void* pArg) override;
	virtual void				Tick(_float fTimeDelta) override;
	virtual void				Late_Tick(_float fTimeDelta) override;
	virtual HRESULT		Render() override;

private:
	CShader*									m_pShaderCom = nullptr;
	CRenderer*								m_pRendererCom = nullptr;
	CModel*									m_pModelCom = nullptr;
	class CInteraction_Com*			m_pInteractionCom = nullptr;
	class CControlMove*				m_pControlMoveCom = nullptr;


	CTexture*									m_pMasterDiffuseBlendTexCom = nullptr;

public:
	virtual HRESULT		Add_AdditionalComponent(_uint iLevelIndex, const _tchar* pComTag, COMPONENTS_OPTION eComponentOption)override;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();

public:
	static  CBase_Ground*	   Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};
END


