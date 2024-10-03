#ifndef CR_H_
#define CR_H_

#if (__STDC_VERSION__ >= 199901L && __STDC_VERSION__ < 202000L)
	#include <stdbool.h>
#elif __STDC_VERSION__ < 199901L
	typedef unsigned int _Bool;
	#define false ((unsigned int)0)
	#define true ((unsigned int)1)
	#define bool _Bool
#endif

#include <stddef.h>

typedef enum
{
	RENDER_MODE_MESH,
	RENDER_MODE_TRIANGLE_STRIP,
}CR_RenderMode;

typedef struct
{
	float x;
	float y;
	float z;
	float w;
}vec3;

typedef struct
{
	vec3 position;
}vertex;

typedef struct
{
	vec3 position;
	vec3 rotation;
	vec3 scale;
}crTransform;

bool InitCR(unsigned int width, unsigned int height, float fov, float nearPlane, float farPlane);
void SetTimescale(float newScale);
void SetRenderDestination(unsigned char *dest);
unsigned char* RenderModel(vertex *model, size_t vertexCount, crTransform transform, CR_RenderMode renderMode);
void CRClearDepthBuffer(void);

#endif
