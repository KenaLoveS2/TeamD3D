#pragma once
#include "Effect_Mesh.h"
#include "Client_Defines.h"

BEGIN(Client)

class CE_PulseObject final : public CEffect_Mesh
{
public:
	enum PULSE_TYPE {
		PULSE_OBJ_DELIVER, PULSE_OBJ_RECIVE, PULSE_OBJ_END,
	};

	typedef struct tag_PulseObject
	{
		
		PULSE_TYPE		eObjType;	
		_float		   fPulseMaxSize;
		_float			fIncreseRatio;
		_float3			vResetSize;
		_float4			vResetPos;
	
		tag_PulseObject()
		{
			eObjType = PULSE_OBJ_DELIVER;
			fPulseMaxSize =	3.f;
			fIncreseRatio = 1.f;
			vResetSize = _float3(1.f, 1.f, 1.f);
			vResetPos = _float4(0.f, 0.f, 0.f, 1.f);
		}
	}E_PulseObject_DESC;


private:
	CE_PulseObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CE_PulseObject(const CE_PulseObject& rhs);
	virtual ~CE_PulseObject() = default;

public:
	void    Set_InitMatrixScaled(_float3 vScale);
	_float3 Get_InitMatrixScaled();

	void	Set_PulseObject_DESC(E_PulseObject_DESC& PulseDesc) {
		ZeroMemory(&m_ePulseDesc, sizeof(m_ePulseDesc));
		memcpy(&m_ePulseDesc, &PulseDesc, sizeof(m_ePulseDesc));
	}

	 _bool Get_Finish()const { return m_bFinish; }
	void	Set_Finish(_bool bFinish) { m_bFinish = bFinish; }

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pFilePath = nullptr);
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT	Late_Initialize(void* pArg = nullptr)override;
	virtual void    Tick(_float fTimeDelta) override;
	virtual void    Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	virtual void	ImGui_PhysXValueProperty()override;

private:
	HRESULT SetUp_ShaderResources();


private:
	void	Type_Tick(_float TimeDelta);


private:
	_float	m_fActivePlusScale = 1.3f;
	_float4x4 m_SaveInitWorldMatrix;
	_float	m_fDissolveTime = 0.0f;

private:
	E_PulseObject_DESC			m_ePulseDesc;
	_bool						m_bFinish = false;

public:
	static  CE_PulseObject* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pFilePath = nullptr);
	virtual CGameObject*  Clone(void* pArg = nullptr) override;
	virtual void          Free() override;

};

END