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

#define CR_BACKFACE_CULLING 0x00000001

#include <stddef.h>
#include <stdint.h>

typedef uint32_t (*CRFRAGMENTPROC)(int x, int y, float u, float v, float depth);

typedef enum
{
	RENDER_MODE_MESH,
	RENDER_MODE_TRIANGLE_STRIP,
}CR_RenderMode;

typedef enum
{
	CR_RGB,
	CR_RGBA,
}CR_RENDER_BUFFER_TYPE;

typedef struct
{
	float x;
	float y;
	float z;
	float w;
}vec3;

typedef struct
{
	float x;
	float y;
	float w;
}vec2;

typedef struct
{
	vec3 position;
	vec2 uv;
}vertex;

typedef struct
{
	vec3 position;
	vec3 rotation;
	vec3 scale;
}crTransform;

bool InitCR(unsigned int width, unsigned int height, CR_RENDER_BUFFER_TYPE buffType, float fov, float nearPlane, float farPlane);
void SetRenderDestination(unsigned char *dest);
void CREnable(uint32_t settingBit);

unsigned char* RenderModel(vertex *model, size_t vertexCount, crTransform transform, CR_RenderMode renderMode, CRFRAGMENTPROC fragmentProc);
void CRClearDepthBuffer(void);

void CleanupCR(void);

#endif
