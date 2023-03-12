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

/* 아마 이펙트 다수를 가질 거 같음*/
class CCrystal final : public CEnviromentObj
{
private:
	CCrystal(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCrystal(const CCrystal& rhs);
	virtual ~CCrystal() = default;

public:
	virtual HRESULT		Initialize_Prototype() override;
	virtual HRESULT		Initialize(void* pArg) override;
	virtual HRESULT		Late_Initialize(void* pArg = nullptr)override;
	
	virtual void				Tick(_float fTimeDelta) override;
	virtual void				Late_Tick(_float fTimeDelta) override;
	virtual HRESULT		Render() override;

public:
	void			Create_Pulse(_bool bActive);


private:	
	CModel*									m_pModelCom = nullptr;
	class CInteraction_Com*			m_pInteractionCom = nullptr;
	class CControlMove*				m_pControlMoveCom = nullptr;
	
private:
	list<class CEffect_Base*>			m_pCrystal_EffectList;

public:
	virtual HRESULT		Add_AdditionalComponent(_uint iLevelIndex, const _tchar* pComTag, COMPONENTS_OPTION eComponentOption)override;



private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();

public:
	static  CCrystal*	   Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};
END

