#pragma once
#include "Base.h"

BEGIN(Engine)
class CLight_Manager final : public CBase
{
public:
	enum eColor { COLOR_DIFFUSE, COLOR_AMBIENT, COLOR_SPECULAR, COLOR_END };

	DECLARE_SINGLETON(CLight_Manager)
private:
	CLight_Manager();
	virtual ~CLight_Manager() = default;

public:
	const LIGHTDESC* Get_LightDesc(_uint iIndex);
	class CLight* Get_Light(_uint iIndex);

public:
	HRESULT Add_Light(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const LIGHTDESC& LightDesc, class CLight** ppOut = nullptr);
	void Render_Light(class CVIBuffer_Rect* pVIBuffer, class CShader* pShader);
	void Clear();

	void	Imgui_Render();
	void Set_ColorValue(eColor eType, char* pLightTag, OUT _float4& vColor);

private:
	vector<class CLight*>					m_Lights;
	typedef vector<class CLight*>		LIGHTS;

public:
	virtual void Free() override;
};
END