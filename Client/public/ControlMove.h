#pragma once
#include "Client_Defines.h"
#include "Component.h"

BEGIN(Client)
class CControlMove final: public CComponent
{
public:
	enum CONTROL_MOVE_TYPE {
		MOVE_TYPE_WALL, MOVE_TYPE_LAND, MOVE_TYPE_PARKOUR, MOVE_TYPE_END
	};
	
	typedef struct tag_ControlMoveDesc
	{	
		CONTROL_MOVE_TYPE		eType = MOVE_TYPE_END;
		
	}CONTROLMOVE_DESC;

private:
	CControlMove(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CControlMove(const CControlMove& rhs);
	virtual ~CControlMove() = default;

public:
	inline void					Set_MoveOption(CControlMove::CONTROL_MOVE_TYPE eOption) {
		m_CtrlMove_Desc.eType = eOption; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg, class CGameObject* pOwner) override;
	virtual void		  Imgui_RenderProperty() override;


private:
	CONTROLMOVE_DESC			m_CtrlMove_Desc;

private: /*For.Imgui*/
	_int									m_iMgui_Option = 0;


public:
	static CControlMove* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg = nullptr, class CGameObject* pOwner = nullptr) override;
	virtual void Free() override;
};

END