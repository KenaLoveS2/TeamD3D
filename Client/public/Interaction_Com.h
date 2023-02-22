#pragma once
#include "Client_Defines.h"
#include "Component.h"

BEGIN(Client)
class CInteraction_Com final  : public CComponent
{
public:
	enum INTERATIONCTION_TYPE {
		INTERATIONCTION_TYPE_TEST1, INTERATIONCTION_TYPE_TEST2, INTERATIONCTION_TYPE_TEST3,	INTERATIONCTION_TYPE_END
	};
	typedef struct tag_Interaction_Desc
	{
		INTERATIONCTION_TYPE		eType = INTERATIONCTION_TYPE_END;
		_bool									bPulse_Interaction = false;				// Player Pulse Active
	}INTERACTION_DESC;

private:
	CInteraction_Com(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CInteraction_Com(const CInteraction_Com& rhs);
	virtual ~CInteraction_Com() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg, class CGameObject* pOwner) override;
	virtual void		  Imgui_RenderProperty() override;

private: /*Data*/
	INTERACTION_DESC		m_InteractionDesc;

private: /*For.Imgui*/
	_int									m_iMgui_InterOption = 0;

public:
	static CInteraction_Com* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg = nullptr, class CGameObject* pOwner = nullptr) override;
	virtual void Free() override;
};

END