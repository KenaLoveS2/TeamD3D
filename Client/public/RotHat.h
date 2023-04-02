#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CRenderer;
class CShader;
class CModel;
class CBone;
END

BEGIN(Client)
class CRotHat final : public CGameObject
{
public:
	enum HAT_TYPE {
		ACORN, CLOVER, EGG, MUSHROOM, BUTTERFLY, DINOSAUR, REDBOW, COWBOY, 
		HALO, DEER, PANCAKE, SUNFLOWER, CAT, BUNNY, CROWN, SAMURAI,
		HAT_TYPE_END
	};

	typedef struct tagRotHatDesc
	{
		HAT_TYPE eHatType;
		_float4x4 PivotMatrix;
		CBone* pSocket;
		CTransform* pTargetTransform;
		class CRot* pOwnerRot;		
	} HAT_DESC;

private:	
	CRenderer* m_pRendererCom = nullptr;
	CShader* m_pShaderCom = nullptr;
	CModel* m_pModelCom = nullptr;	
	class CRot* m_pOwnerRot = nullptr;
	
	_uint m_iNumMeshes = 0;
	_float4x4 m_SocketMatrix = g_IdentityFloat4x4;

	HAT_DESC m_HatDesc;

	_bool m_bShowFlag = false;

	// 사무라이
	/*_float3 m_vPivotTrans = { 0.f, -0.03f, -0.21f };
	_float3 m_vPivotRot = { -0.880f, -0.02f, -3.040f };*/

	static _tchar m_szModelTagTable[HAT_TYPE_END][64];
	static _tchar m_szAoTexturePathTable[HAT_TYPE_END][128];
	static _float3 m_vPivotTrans[HAT_TYPE_END];
	static _float3 m_vPivotRot[HAT_TYPE_END];

private:
	CRotHat(ID3D11Device* pDevice, ID3D11DeviceContext* p_context);
	CRotHat(const CRotHat& rhs);
	virtual ~CRotHat() = default;

	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();
	HRESULT SetUp_ShadowShaderResources();
		
public:
	static CRotHat* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Late_Initialize(void * pArg);
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT RenderShadow() override;
	
	virtual void Imgui_RenderProperty() override;	
	virtual void ImGui_ShaderValueProperty() override;
	
	HRESULT Create_HatModel(HAT_DESC& HatDesc, _bool bShowFlag = true);
	void Set_ShowFlag(_bool bFlag) { m_bShowFlag = bFlag; }
	
};

END