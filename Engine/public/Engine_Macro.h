#pragma once

#define EPSILON 0.0001
#define FLOAT_EQ(x,v) (((v - EPSILON) < x) && (x <( v + EPSILON)))

#define D3DCOLOR_ARGB(a,r,g,b) \
    ((D3DCOLOR)((((a)&0xff)<<24)|(((r)&0xff)<<16)|(((g)&0xff)<<8)|((b)&0xff)))

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
	/** Dummy value.
	*
	*  No texture, but the value to be used as 'texture semantic'
	*  (#aiMaterialProperty::mSemantic) for all material properties
	*  *not* related to textures.
	*/
	aiTextureType_NONE = 0,

	/** LEGACY API MATERIALS
	* Legacy refers to materials which
	* Were originally implemented in the specifications around 2000.
	* These must never be removed, as most engines support them.
	*/

	/** The texture is combined with the result of the diffuse
	*  lighting equation.
	*/
	aiTextureType_DIFFUSE = 1,

	/** The texture is combined with the result of the specular
	*  lighting equation.
	*/
	aiTextureType_SPECULAR = 2,

	/** The texture is combined with the result of the ambient
	*  lighting equation.
	*/
	aiTextureType_AMBIENT = 3,

	/** The texture is added to the result of the lighting
	*  calculation. It isn't influenced by incoming light.
	*/
	aiTextureType_EMISSIVE = 4,

	/** The texture is a height map.
	*
	*  By convention, higher gray-scale values stand for
	*  higher elevations from the base height.
	*/
	aiTextureType_HEIGHT = 5,

	/** The texture is a (tangent space) normal-map.
	*
	*  Again, there are several conventions for tangent-space
	*  normal maps. Assimp does (intentionally) not
	*  distinguish here.
	*/
	aiTextureType_NORMALS = 6,

	/** The texture defines the glossiness of the material.
	*
	*  The glossiness is in fact the exponent of the specular
	*  (phong) lighting equation. Usually there is a conversion
	*  function defined to map the linear color values in the
	*  texture to a suitable exponent. Have fun.
	*/
	aiTextureType_SHININESS = 7,

	/** The texture defines per-pixel opacity.
	*
	*  Usually 'white' means opaque and 'black' means
	*  'transparency'. Or quite the opposite. Have fun.
	*/
	aiTextureType_OPACITY = 8,

	/** Displacement texture
	*
	*  The exact purpose and format is application-dependent.
	*  Higher color values stand for higher vertex displacements.
	*/
	aiTextureType_DISPLACEMENT = 9,

	/** Lightmap texture (aka Ambient Occlusion)
	*
	*  Both 'Lightmaps' and dedicated 'ambient occlusion maps' are
	*  covered by this material property. The texture contains a
	*  scaling value for the final color value of a pixel. Its
	*  intensity is not affected by incoming light.
	*/
	aiTextureType_LIGHTMAP = 10,

	/** Reflection texture
	*
	* Contains the color of a perfect mirror reflection.
	* Rarely used, almost never for real-time applications.
	*/
	aiTextureType_REFLECTION = 11,

	/** PBR Materials
	* PBR definitions from maya and other modelling packages now use this standard.
	* This was originally introduced around 2012.
	* Support for this is in game engines like Godot, Unreal or Unity3D.
	* Modelling packages which use this are very common now.
	*/

	aiTextureType_BASE_COLOR = 12,
	aiTextureType_NORMAL_CAMERA = 13,
	aiTextureType_EMISSION_COLOR = 14,
	aiTextureType_METALNESS = 15,
	aiTextureType_DIFFUSE_ROUGHNESS = 16,
	aiTextureType_AMBIENT_OCCLUSION = 17,

	/** Unknown texture
	*
	*  A texture reference that does not match any of the definitions
	*  above is considered to be 'unknown'. It is still imported,
	*  but is excluded from any further post-processing.
	*/
	aiTextureType_UNKNOWN = 18,
};

#define AI_TEXTURE_TYPE_MAX  aiTextureType_UNKNOWN