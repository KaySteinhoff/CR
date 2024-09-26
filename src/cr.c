#include <cr.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define USE_MATH_DEFINES

typedef struct
{
	float m[4][4];
}mat4x4;

mat4x4 matProj = { 0 };
unsigned char *screen = NULL;
unsigned int screenWidth = 0;
unsigned int screenHeight = 0;
unsigned int pixelCount = 0;
float totalTime = 0, timescale = 1;

vec3 mat4x4_vec3_mul(vec3 v, mat4x4 m)
{
	vec3 o = { 0 };
	o.x = v.x * m.m[0][0] + v.y * m.m[1][0] + v.z * m.m[2][0] + m.m[3][0];
	o.y = v.x * m.m[0][1] + v.y * m.m[1][1] + v.z * m.m[2][1] + m.m[3][1];
	o.z = v.x * m.m[0][2] + v.y * m.m[1][2] + v.z * m.m[2][2] + m.m[3][2];

	float w = v.x * m.m[0][3] + v.y * m.m[1][3] + v.z * m.m[2][3] + m.m[3][3];

	if (w != 0.0f)
	{
		o.x /= w;
		o.y /= w;
		o.z /= w;
	}

	return o;
}

bool InitCR(unsigned int width, unsigned int height, float fov, float nearPlane, float farPlane)
{
	pixelCount = width*height;
	screenWidth = width;
	screenHeight = height;

	float aspectRatio = height/(float)width;
	float fovRad = 1.0f / tanf(fov * 0.5f / 180.0f * M_PI);

	matProj.m[0][0] = aspectRatio * fovRad;
	matProj.m[1][1] = fovRad;
	matProj.m[2][2] = farPlane / (farPlane - nearPlane);
	matProj.m[3][2] = (-farPlane * nearPlane)/(farPlane-nearPlane);
	matProj.m[2][3] = 1.0f;
	matProj.m[3][3] = 0.0f;

	return true;
}

void SetTimescale(float newScale)
{
	timescale = newScale;
}

void SetRenderDestination(unsigned char *dest)
{
	screen = dest;
}

void PutPixel(unsigned int x, unsigned int y, unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
	screen[((x+y*screenWidth)<<2) + 0] = r;
	screen[((x+y*screenWidth)<<2) + 1] = g;
	screen[((x+y*screenWidth)<<2) + 2] = b;
	screen[((x+y*screenWidth)<<2) + 3] = a;
}

void DrawLineH(int x0, int y0, int x1, int y1)
{
	if (x0 > x1)
	{
		int tmp = x0;
		x0 = x1;
		x1 = tmp;

		tmp = y0;
		y0 = y1;
		y1 = tmp;
	}

	int dx = x1-x0;
	if(dx == 0)
		return;
	int dy = y1-y0;

	int dir = dy < 0 ? -1 : 1;
	dy *= dir;

	int y = y0;
	int p = (dy<<1)-dx;

	for(int x = x0; x < x0+dx; ++x)
	{
		PutPixel(x, y, 255, 255, 255, 255);
		if( p >= 0)
		{
			y += dir;
			p = p - (dx<<1);
		}
		p = p + (dy<<1);
	}
}

void DrawLineV(int x0, int y0, int x1, int y1)
{
	if (y0 > y1)
	{
		int tmp = x0;
		x0 = x1;
		x1 = tmp;

		tmp = y0;
		y0 = y1;
		y1 = tmp;
	}

	int dy = y1-y0;
	if(dy == 0)
		return;
	int dx = x1-x0;

	int dir = dx < 0 ? -1 : 1;
	dx *= dir;

	int x = x0;
	int p = (dx<<1)-dy;

	for(int y = y0; y < y0+dy; ++y)
	{
		PutPixel(x, y, 255, 255, 255, 255);
		if( p >= 0)
		{
			x += dir;
			p = p - (dy<<1);
		}
		p = p + (dx<<1);
	}
}

void DrawLine(int x0, int y0, int x1, int y1)
{
	if(abs(x1-x0)>abs(y1-y0))
		DrawLineH(x0, y0, x1, y1);
	else
		DrawLineV(x0, y0, x1, y1);
}

vec3 ProjectVertex(vertex v, crTransform transform)
{
	mat4x4 matRotZ = { 0 }, matRotX = { 0 };

	transform.rotation.x = transform.rotation.z * M_PI / 180.0;
	transform.rotation.y = transform.rotation.z * M_PI / 180.0;
	transform.rotation.z = transform.rotation.z * M_PI / 180.0;
	// Rotation Z
	matRotZ.m[0][0] = cosf(transform.rotation.z);
	matRotZ.m[0][1] = sinf(transform.rotation.z);
	matRotZ.m[1][0] = -matRotZ.m[0][1];
	matRotZ.m[1][1] = matRotZ.m[0][0];
	matRotZ.m[2][2] = 1;
	matRotZ.m[3][3] = 1;

	// Rotation X
	matRotX.m[0][0] = 1;
	matRotX.m[1][1] = cosf(transform.rotation.x);
	matRotX.m[1][2] = sinf(transform.rotation.x);
	matRotX.m[2][1] = -matRotX.m[1][2];
	matRotX.m[2][2] = matRotX.m[1][1];
	matRotX.m[3][3] = 1;

	vec3 vecRotZ = mat4x4_vec3_mul(v.position, matRotZ);
	vec3 vecRotZX = mat4x4_vec3_mul(vecRotZ, matRotX);

	vecRotZX.x += transform.position.x;
	vecRotZX.y += transform.position.y;
	vecRotZX.z += transform.position.z;

	vec3 vecProj = mat4x4_vec3_mul(vecRotZX, matProj);

	vecProj.x = (vecProj.x+1)*0.5*screenWidth;
	vecProj.y = (vecProj.y+1)*0.5*screenHeight;

	return vecProj;
}

unsigned char* RenderModel(vertex *model, size_t vertexCount, crTransform transform)
{
	for(size_t i = 0; i < vertexCount; i += 3)
	{
		vec3 p1 = ProjectVertex(model[i + 0], transform);
		vec3 p2 = ProjectVertex(model[i + 1], transform);
		vec3 p3 = ProjectVertex(model[i + 2], transform);

		DrawLine((int)p1.x, (int)p1.y, (int)p2.x, (int)p2.y);
		DrawLine((int)p1.x, (int)p1.y, (int)p3.x, (int)p3.y);
		DrawLine((int)p3.x, (int)p3.y, (int)p2.x, (int)p2.y);
	}

	return screen;
}

unsigned char* RenderTriangleStrip(vertex *model, size_t vertexCount, crTransform transform)
{
	for(size_t i = 0; i < vertexCount-2; ++i)
	{
		vec3 p1 = ProjectVertex(model[i + 0], transform);
		vec3 p2 = ProjectVertex(model[i + 1], transform);
		vec3 p3 = ProjectVertex(model[i + 2], transform);

		DrawLine((int)p1.x, (int)p1.y, (int)p2.x, (int)p2.y);
		DrawLine((int)p1.x, (int)p1.y, (int)p3.x, (int)p3.y);
		DrawLine((int)p3.x, (int)p3.y, (int)p2.x, (int)p2.y);
	}

	return screen;
}
