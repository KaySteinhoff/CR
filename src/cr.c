#include <cr.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdio.h>

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
	o.x = v.x * m.m[0][0] + v.y * m.m[1][0] + v.z * m.m[2][0] + v.w * m.m[3][0];
	o.y = v.x * m.m[0][1] + v.y * m.m[1][1] + v.z * m.m[2][1] + v.w * m.m[3][1];
	o.z = v.x * m.m[0][2] + v.y * m.m[1][2] + v.z * m.m[2][2] + v.w * m.m[3][2];
	o.w = v.x * m.m[0][3] + v.y * m.m[1][3] + v.z * m.m[2][3] + v.w * m.m[3][3];

	return o;
}

mat4x4 mat4x4_mat4x4_mul(mat4x4 m, mat4x4 n)
{
	mat4x4 res = { 0 };

	for(int c = 0; c < 4; ++c)
		for(int r = 0; r < 4; ++r)
			res.m[r][c] = m.m[r][0]*n.m[0][c] + m.m[r][1]*n.m[1][c] + m.m[r][2]*n.m[2][c] + m.m[r][3]*n.m[3][c];

	return res;
}

mat4x4 CreateXRotationMatrix(float theta)
{
	mat4x4 matRotX = { 0 };
	matRotX.m[0][0] = 1;
	matRotX.m[1][1] = cosf(theta);
	matRotX.m[1][2] = sinf(theta);
	matRotX.m[2][1] = -matRotX.m[1][2];
	matRotX.m[2][2] = matRotX.m[1][1];
	matRotX.m[3][3] = 1;

	return matRotX;
}

mat4x4 CreateYRotationMatrix(float theta)
{
	mat4x4 matRotY = { 0 };
	matRotY.m[0][0] = cosf(theta);
	matRotY.m[0][2] = sinf(theta);
	matRotY.m[1][1] = 1;
	matRotY.m[2][0] = -matRotY.m[0][2];
	matRotY.m[2][2] = matRotY.m[0][0];
	matRotY.m[3][3] = 1;

	return matRotY;
}

mat4x4 CreateZRotationMatrix(float theta)
{
	mat4x4 matRotZ = { 0 };
	matRotZ.m[0][0] = cosf(theta);
	matRotZ.m[0][1] = sinf(theta);
	matRotZ.m[1][0] = -matRotZ.m[0][1];
	matRotZ.m[1][1] = matRotZ.m[0][0];
	matRotZ.m[2][2] = 1;
	matRotZ.m[3][3] = 1;

	return matRotZ;
}

mat4x4 CreateTranslationMatrix(vec3 position)
{
	mat4x4 matTrans = { 0 };
	matTrans.m[0][0] = 1;
	matTrans.m[1][1] = 1;
	matTrans.m[2][2] = 1;
	matTrans.m[3][3] = 1;
	matTrans.m[3][0] = position.x;
	matTrans.m[3][1] = position.y;
	matTrans.m[3][2] = position.z;

	return matTrans;
}

mat4x4 CreateProjectionMatrix(float fov, float aspectRatio, float nearPlane, float farPlane)
{
	float fovRad = 1.0/tanf(fov * 0.5/180 * M_PI);
	mat4x4 matProj = { 0 };
	matProj.m[0][0] = aspectRatio * fovRad;
	matProj.m[1][1] = fovRad;
	matProj.m[2][2] = farPlane / (farPlane - nearPlane);
	matProj.m[3][2] = (-farPlane * nearPlane) / (farPlane - nearPlane);
	matProj.m[2][3] = 1.0;
	matProj.m[3][3] = 0;

	return matProj;
}

mat4x4 CreateLookAtMatrix(vec3 position, vec3 target, vec3 up)
{
	
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
	if(x < 0)
		x =0;
	else if(x > screenWidth)
		x = screenWidth-1;

	if(y < 0)
		y =0;
	else if(y > screenHeight)
		y = screenHeight-1;

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

vec3 ProjectVertex(vertex v, crTransform transform, mat4x4 matRot)
{
	v.position.w = 1;

	mat4x4 matTransl = CreateTranslationMatrix(transform.position);
	mat4x4 matModel = mat4x4_mat4x4_mul(matTransl, matRot);

	vec3 vecTransf = mat4x4_vec3_mul(v.position, matModel);

	printf("Transformed vector: %f, %f, %f\n", vecTransf.x, vecTransf.y, vecTransf.z);

	vec3 vecProj = mat4x4_vec3_mul(vecTransf, matProj);

	vecProj.x = (vecProj.x+1)*0.5*screenWidth;
	vecProj.y = (vecProj.y+1)*0.5*screenHeight;

	return vecProj;
}

unsigned char* RenderModel(vertex *model, size_t vertexCount, crTransform transform)
{
	transform.rotation.x = transform.rotation.z * M_PI / 180.0;
	transform.rotation.y = transform.rotation.z * M_PI / 180.0;
	transform.rotation.z = transform.rotation.z * M_PI / 180.0;

	mat4x4 matRot = CreateYRotationMatrix(transform.rotation.y), matRotTMP = CreateXRotationMatrix(transform.rotation.x);
	matRot = mat4x4_mat4x4_mul(matRot, matRotTMP);
	matRotTMP = CreateZRotationMatrix(transform.rotation.z);
	matRot = mat4x4_mat4x4_mul(matRot, matRotTMP);

	for(size_t i = 0; i < vertexCount; i += 3)
	{
		vec3 p1 = ProjectVertex(model[i + 0], transform, matRot);
		vec3 p2 = ProjectVertex(model[i + 1], transform, matRot);
		vec3 p3 = ProjectVertex(model[i + 2], transform, matRot);

		DrawLine((int)p1.x, (int)p1.y, (int)p2.x, (int)p2.y);
		DrawLine((int)p1.x, (int)p1.y, (int)p3.x, (int)p3.y);
		DrawLine((int)p3.x, (int)p3.y, (int)p2.x, (int)p2.y);
	}

	return screen;
}

unsigned char* RenderTriangleStrip(vertex *model, size_t vertexCount, crTransform transform)
{
	transform.rotation.x = transform.rotation.z * M_PI / 180.0;
	transform.rotation.y = transform.rotation.z * M_PI / 180.0;
	transform.rotation.z = transform.rotation.z * M_PI / 180.0;

	mat4x4 matRot = CreateYRotationMatrix(transform.rotation.y), matRotTMP = CreateXRotationMatrix(transform.rotation.x);
	matRot = mat4x4_mat4x4_mul(matRot, matRotTMP);
	matRotTMP = CreateZRotationMatrix(transform.rotation.z);
	matRot = mat4x4_mat4x4_mul(matRot, matRotTMP);

	for(size_t i = 0; i < vertexCount-2; ++i)
	{
		vec3 p1 = ProjectVertex(model[i + 0], transform, matRot);
		vec3 p2 = ProjectVertex(model[i + 1], transform, matRot);
		vec3 p3 = ProjectVertex(model[i + 2], transform, matRot);

		DrawLine((int)p1.x, (int)p1.y, (int)p2.x, (int)p2.y);
		DrawLine((int)p1.x, (int)p1.y, (int)p3.x, (int)p3.y);
		DrawLine((int)p3.x, (int)p3.y, (int)p2.x, (int)p2.y);
	}

	return screen;
}
