#include "stdafx.h"
#include "..\public\UI_Manager.h"
#include "GameInstance.h"

IMPLEMENT_SINGLETON(CUI_Manager)

CUI_Manager::CUI_Manager()
{
}

HRESULT CUI_Manager::Initialize(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	m_pDevice = pDevice;
	m_pContext = pContext;

	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);

	return S_OK;
}

HRESULT CUI_Manager::Ready_Proto_TextureComponent()
{

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	/* For. LifeBar */
	//FAILED_CHECK_RETURN(pGameInstance->Add_Proto_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Proto_Component_Texture_LifeBarFrame"),
	//	CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/LifeBar/T_Frame.png"))), E_FAIL, "Failed To Create Proto Texture for UI_LifeBar : UI_Manager");


	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CUI_Manager::Ready_Proto_GameObject()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	/* For. LifeBar */
	//FAILED_CHECK_RETURN(
	//	pGameInstance->Add_Proto(TEXT("Proto_GameObject_LifeBarFrame"),
	//		CUI_LifeBarFrame::Create(m_pDevice, m_pContext, (LEVELID)CGameInstance::Get_StaticLevelIndex())),
	//	E_FAIL, "Failed To Add LifeBarFrame Proto : CUI_Manager");

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CUI_Manager::Clone_GameObject()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	/* For. LifeBar */
	//FAILED_CHECK_RETURN(pGameInstance->Clone_GameObject(
	//	CGameInstance::Get_StaticLevelIndex(), TEXT("Layer_UI_LifeFrame"),
	//	TEXT("Proto_GameObject_LifeBarFrame")), E_FAIL,
	//	"Failed To Clone LifeBarFrame");

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

void CUI_Manager::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}