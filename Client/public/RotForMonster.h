#pragma once

#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CRenderer;
class CShader;
class CModel;
class CFSMComponent;
END

BEGIN(Client)

class CRotForMonster final : public CGameObject
{
public:
	CRotForMonster();
	~CRotForMonster();
};

END