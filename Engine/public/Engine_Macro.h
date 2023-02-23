#pragma once

#define EPSILON 0.0001
#define FLOAT_EQ(x,v) (((v - EPSILON) < x) && (x <( v + EPSILON)))

// debug모드에서만 체크할 경우 사용
#define Assert(expression) assert(expression)
#define AssertMsg(expression, message) Assert((expression) && (message))

#define FAILED_CHECK(_hr) if(((HRESULT)(_hr)) < 0){__debugbreak();}

#define D3DCOLOR_ARGB(a,r,g,b) \
    ((D3DCOLOR)((((a)&0xff)<<24)|(((r)&0xff)<<16)|(((g)&0xff)<<8)|((b)&0xff)))

#define FLOAT4COLOR_RGBA(r, g, b, a) \
	_float4(r / 255.f, g / 255.f, b / 255.f, a / 255.f)

#define NULL_CHECK_RETURN( _ptr, _return)	\
	{if( _ptr == 0){__debugbreak();return _return;}}

#define FAILED_CHECK_RETURN(_hr, _return)	if( ((HRESULT)(_hr)) < 0 )		\
	{ MessageBoxW(nullptr, L"Failed", L"System Error",MB_OK); __debugbreak(); return _return;}

#define GET_INSTANCE(CLASSNAME)	[](){											\
	CLASSNAME*	pInstance = CLASSNAME::GetInstance();							\
	if(nullptr == pInstance) {													\
	char	szMessage[MAX_PATH] = "";											\
	strcpy_s(szMessage, typeid(CLASSNAME).name());								\
	strcat_s(szMessage, "is nullptr");											\
	MessageBoxA(0, szMessage, nullptr, MB_OK);}									\
	else {																		\
	pInstance->AddRef();}														\
	return pInstance;															\
	}();

#define RELEASE_INSTANCE(CLASSNAME)	[](){										\
	CLASSNAME*	pInstance = CLASSNAME::GetInstance();							\
	if(nullptr == pInstance) {													\
	char	szMessage[MAX_PATH] = "";											\
	strcpy_s(szMessage, typeid(CLASSNAME).name());								\
	strcat_s(szMessage, "is nullptr");											\
	MessageBoxA(0, szMessage, nullptr, MB_OK);}									\
	else {																		\
	pInstance->Release();}														\
	}();


#define BEGIN(NAMESPACE)  namespace NAMESPACE {
#define END				  }

#ifdef ENGINE_EXPORTS
#define ENGINE_DLL _declspec(dllexport) 
#else
#define ENGINE_DLL _declspec(dllimport) 
#endif

#define			MSG_BOX(_message)			MessageBox(NULL, TEXT(_message), L"System Message", MB_OK)

#define NO_COPY(CLASSNAME)										\
		private:												\
		CLASSNAME(const CLASSNAME&) = delete;					\
		CLASSNAME& operator = (const CLASSNAME&) = delete;		

#define DECLARE_SINGLETON(CLASSNAME)							\
		NO_COPY(CLASSNAME)										\
		private:												\
		static CLASSNAME*	m_pInstance;						\
		public:													\
		static CLASSNAME*	GetInstance( void );				\
		static unsigned long DestroyInstance( void );			

#define IMPLEMENT_SINGLETON(CLASSNAME)							\
		CLASSNAME*	CLASSNAME::m_pInstance = NULL;				\
		CLASSNAME*	CLASSNAME::GetInstance( void )	{			\
			if(NULL == m_pInstance) {							\
				m_pInstance = new CLASSNAME;					\
			}													\
			return m_pInstance;									\
		}														\
		unsigned long CLASSNAME::DestroyInstance( void ) {		\
			unsigned long	dwRefCnt = 0;						\
			if(nullptr != m_pInstance)	{						\
				dwRefCnt = m_pInstance->Release();				\
				if(0 == dwRefCnt)								\
					m_pInstance = nullptr;						\
			}													\
			return dwRefCnt;									\
		}


enum aiTextureType
{
	WJTextureType_NONE = 0,
	WJTextureType_DIFFUSE = 1,
	WJTextureType_SPECULAR = 2,
	WJTextureType_AMBIENT = 3,
	WJTextureType_EMISSIVE = 4,
	WJTextureType_EMISSIVEMASK = 5,
	WJTextureType_NORMALS = 6,
	WJTextureType_MASK = 7,
	WJTextureType_SSS_MASK = 8,
	WJTextureType_SPRINT_EMISSIVE = 9,
	WJTextureType_LIGHTMAP = 10,
	WJTextureType_REFLECTION = 11,
	WJTextureType_BASE_COLOR = 12,
	WJTextureType_NORMAL_CAMERA = 13,
	WJTextureType_EMISSION_COLOR = 14,
	WJTextureType_METALNESS = 15,
	WJTextureType_DIFFUSE_ROUGHNESS = 16,
	WJTextureType_AMBIENT_OCCLUSION = 17,

	/** Unknown texture
	*
	*  A texture reference that does not match any of the definitions
	*  above is considered to be 'unknown'. It is still imported,
	*  but is excluded from any further post-processing.
	*/
	WJTextureType_UNKNOWN = 18,
};

#define WJ_TEXTURE_TYPE_MAX  WJTextureType_UNKNOWN