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
class CModelViewerObject final : public CEnviromentObj
{
private:
	CModelViewerObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CModelViewerObject(const CModelViewerObject& rhs);
	virtual ~CModelViewerObject() = default;

public:
	const _float					Get_ViewerCamYPos() const { return m_fViewerCamYPos; }
	const _float					Get_ViewerCamXAngle() const { return m_fViewerCame_XAngle; }
	
	void						Set_ViewerCamMoveRatio(_float fAngle, _float fYPos) {
		m_fViewerCamYPos_Num     = fYPos;
		m_fViewerCamXAngle_Num = fAngle;
	}

	void						Set_ViewerCamZPos(_float fZPos);


public:
	virtual HRESULT		Initialize_Prototype() override;
	virtual HRESULT		Initialize(void* pArg) override;
	virtual void				Tick(_float fTimeDelta) override;
	virtual void				Late_Tick(_float fTimeDelta) override;
	virtual HRESULT		Render() override;

public:
	HRESULT							Change_Model(_uint iCurLevel, const _tchar* pModelTag);


private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CModel*				m_pModelCom = nullptr;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();
private:
	_float4x4					m_ViewerCamView;
	_float4x4					m_ViewerCamProv;

	float							 m_fViewerCamYPos = 25.f;
	float							 m_fViewerCame_XAngle = 50.f;

	_float						m_fViewerCamYPos_Num =0.1f;
	_float						m_fViewerCamXAngle_Num = 0.1f;
	_float						m_fViewerCamZPos = -5.f;

public:
	static  CModelViewerObject*	   Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};
END

