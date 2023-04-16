#pragma once

#include "EnviromentObj.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CRenderer;
class CShader;
class CModel;
END

BEGIN(Client)

class CWaterPlane : public CEnviromentObj
{
	enum Type
	{
		NORMAL,
		MASK,
		NOISE,
		TYPE_END
	};


private:
	CWaterPlane(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWaterPlane(const CWaterPlane& rhs);
	virtual ~CWaterPlane() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* pArg) override;
	virtual HRESULT			Late_Initialize(void* pArg) override;
	virtual void					Tick(_float fTimeDelta) override;
	virtual void					Late_Tick(_float fTimeDelta) override;
	virtual HRESULT			Render() override;
	virtual void					Imgui_RenderProperty() override;
	virtual void					ImGui_ShaderValueProperty() override;
	virtual void					ImGui_PhysXValueProperty() override;

private:
	CModel*							m_pModelCom = nullptr;
	CTexture*							m_pTextureCom[TYPE_END] = { nullptr, };
	float									m_fTimeDelta = 0.f;
	int									m_iTexNum = 2;
	int									m_iMaskTexNum = 1;
	class CKena*					m_pKena = nullptr;
	_bool								m_bSoundCheck[2] = { false, };
	_tchar								m_szCopySoundKey_Water_1[64] = { 0, };
	_tchar								m_szCopySoundKey_Water_2[64] = { 0, };
private:
	HRESULT		SetUp_Components();
	HRESULT		SetUp_ShaderResources();
	void				Setting_Sound();


public:
	static  CWaterPlane*	   Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	   Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END