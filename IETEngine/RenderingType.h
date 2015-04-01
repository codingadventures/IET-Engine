#ifndef RenderingType_h__
#define RenderingType_h__

enum RenderingType
{
	NONE,
	SKYBOX,
	SKYBOX_REFR,
	AMBIENT,
	DIFFUSE,
	COOK_TORRANCE,
	SPECULAR,
	PHONG,
	TOON,
	FRESNEL,
	HATCHING

};

enum HatchingType
{
	WEIGHTED_HATCHING,
	NORMAL_HATCHING,
	GEOMETRY_HATCHING,
	GEOMETRY_BLENDING_HATCHING
};

#endif // RenderingType_h__
