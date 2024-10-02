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

vec3 vec3_vec3_add(vec3 a, vec3 b)
{
	return (vec3) { .x = a.x+b.x, .y = a.y+b.y, .z = a.z+b.z, .w = 1.0f };
}

vec3 vec3_vec3_sub(vec3 a, vec3 b)
{
	return (vec3) { .x = a.x-b.x, .y = a.y-b.y, .z = a.z-b.z, .w = 1.0f };
}

vec3 vec3_vec3_mul(vec3 a, vec3 b)
{
	return (vec3) { .x = a.x*b.x, .y = a.y*b.y, .z = a.z*b.z, .w = 1.0f };
}

vec3 vec3_vec3_div(vec3 a, vec3 b)
{
	return (vec3) { .x = a.x/b.x, .y = a.y/b.y, .z = a.z/b.z, .w = 1.0f };
}

vec3 vec3_scale(vec3 a, float k)
{
	return (vec3) { .x = a.x*k, .y = a.y*k, .z = a.z*k, .w = 1.0f };
}

float vec3_dot(vec3 a, vec3 b)
{
	return a.x*b.x+a.y*b.y+a.z*b.z;
}

float vec3_length(vec3 v)
{
	return sqrtf(vec3_dot(v, v));
}

vec3 vec3_normalize(vec3 v)
{
	float l = vec3_length(v);
	return (vec3) { .x = v.x/l, .y = v.y/l, .z = v.z/l, .w = 1.0f };
}

vec3 vec3_cross(vec3 a, vec3 b)
{
	return (vec3) {
		.x = a.y*b.z-a.z*b.y,
		.y = a.z*b.x-a.x*b.z,
		.z = a.x*b.y-a.y*b.x,
		.w = 1.0
	};
}

vec3 mat4x4_vec3_mul(vec3 v, mat4x4 m)
{
	vec3 o = { 0 };
	o.x = (v.x * m.m[0][0] + v.y * m.m[1][0] + v.z * m.m[2][0]) + v.w * m.m[3][0];
	o.y = (v.x * m.m[0][1] + v.y * m.m[1][1] + v.z * m.m[2][1]) + v.w * m.m[3][1];
	o.z = (v.x * m.m[0][2] + v.y * m.m[1][2] + v.z * m.m[2][2]) + v.w * m.m[3][2];
	o.w = (v.x * m.m[0][3] + v.y * m.m[1][3] + v.z * m.m[2][3]) + v.w * m.m[3][3];

	return o;
}

mat4x4 mat4x4_mat4x4_mul(mat4x4 n, mat4x4 m)
{
	mat4x4 o = { 0 };
	for(int r = 0; r < 4; ++r)
		for(int c = 0; c < 4; ++c)
			o.m[r][c] = n.m[r][0]*m.m[0][c] + n.m[r][1]*m.m[1][c] + n.m[r][2]*m.m[2][c] + n.m[r][3]*m.m[3][c];

	return o;
}

mat4x4 CreateXRotationMatrix(float rad)
{
	mat4x4 matrix = { 0 };
	matrix.m[0][0] = 1.0f;
	matrix.m[1][1] = cosf(rad);
	matrix.m[1][2] = sinf(rad);
	matrix.m[2][1] = -matrix.m[1][2];
	matrix.m[2][2] = matrix.m[1][1];
	matrix.m[3][3] = 1.0f;

	return matrix;
}

mat4x4 CreateYRotationMatrix(float rad)
{
	mat4x4 matrix = { 0 };
	matrix.m[0][0] = cosf(rad);
	matrix.m[0][2] = sinf(rad);
	matrix.m[2][0] = -matrix.m[0][2];
	matrix.m[1][1] = 1.0f;
	matrix.m[2][2] = matrix.m[0][0];
	matrix.m[3][3] = 1.0f;

	return matrix;
}

mat4x4 CreateZRotationMatrix(float rad)
{
	mat4x4 matrix = { 0 };
	matrix.m[0][0] = cosf(rad);
	matrix.m[0][1] = sinf(rad);
	matrix.m[1][0] = -matrix.m[0][1];
	matrix.m[1][1] = matrix.m[0][0];
	matrix.m[2][2] = 1.0f;
	matrix.m[3][3] = 1.0f;

	return matrix;
}

mat4x4 CreateTranslationMatrix(vec3 position)
{
	mat4x4 matrix = { 0 };
	matrix.m[0][0] = 1.0f;
	matrix.m[1][1] = 1.0f;
	matrix.m[2][2] = 1.0f;
	matrix.m[3][3] = 1.0f;
	matrix.m[3][0] = position.x;
	matrix.m[3][1] = position.y;
	matrix.m[3][2] = position.z;

	return matrix;
}

mat4x4 CreateScalationMatrix(vec3 scale)
{
	mat4x4 matrix = { 0 };
	matrix.m[0][0] = scale.x;
	matrix.m[1][1] = scale.y;
	matrix.m[2][2] = scale.z;
	matrix.m[3][3] = 1;

	return matrix;
}

mat4x4 CreatePointAtMatrix(vec3 position, vec3 target, vec3 up)
{
	vec3 forward = vec3_normalize(vec3_vec3_sub(target, position));

	//Calculate new up
	vec3 a = vec3_scale(forward, vec3_dot(up, forward));
	vec3 newUp = vec3_normalize(vec3_vec3_sub(up, a));

	vec3 right = vec3_cross(newUp, forward);

	mat4x4 matrix = { 0 };
	matrix.m[0][0] = right.x;
	matrix.m[0][1] = right.y;
	matrix.m[0][2] = right.z;

	matrix.m[1][0] = up.x;
	matrix.m[1][1] = up.y;
	matrix.m[1][2] = up.z;

	matrix.m[2][0] = forward.x;
	matrix.m[2][1] = forward.y;
	matrix.m[2][2] = forward.z;

	matrix.m[3][0] = position.x;
	matrix.m[3][1] = position.y;
	matrix.m[3][2] = position.z;

	return matrix;
}

mat4x4 MatrixInverse(mat4x4 m)
{
	mat4x4 matrix = { 0 };
	matrix.m[0][0] = m.m[0][0];
	matrix.m[0][1] = m.m[1][0];
	matrix.m[0][2] = m.m[2][0];

	matrix.m[1][0] = m.m[0][1];
	matrix.m[1][1] = m.m[1][1];
	matrix.m[1][2] = m.m[2][1];

	matrix.m[2][0] = m.m[0][2];
	matrix.m[2][1] = m.m[1][2];
	matrix.m[2][2] = m.m[2][2];

	matrix.m[3][0] = -(m.m[3][0] * matrix.m[0][0] + m.m[3][1] * matrix.m[1][0] + m.m[3][2] * matrix.m[2][0]);
	matrix.m[3][1] = -(m.m[3][0] * matrix.m[0][1] + m.m[3][1] * matrix.m[1][1] + m.m[3][2] * matrix.m[2][1]);
	matrix.m[3][2] = -(m.m[3][0] * matrix.m[0][2] + m.m[3][1] * matrix.m[1][2] + m.m[3][2] * matrix.m[2][2]);
	matrix.m[3][3] = 1.0f;

	return matrix;
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
	if(x < 0 || x > screenWidth-1 || y < 0 || y > screenHeight-1)
		return;

	int offset = (x+y*screenWidth)<<2;
	screen[offset] = r;
	screen[offset + 1] = g;
	screen[offset + 2] = b;
	screen[offset + 3] = a;
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

vec3 ProjectVertex(vec3 vecView)
{
	vec3 vecProj = mat4x4_vec3_mul(vecView, matProj);
	vec3 vecScreen = vec3_scale(vecProj, 1.0/vecProj.w);

	vecScreen.x = (vecScreen.x+1)*0.5*screenWidth;
	vecScreen.y = (vecScreen.y+1)*0.5*screenHeight;

	return vecScreen;
}

vec3 TransformVertex(vertex v, crTransform transform, mat4x4 matModel, mat4x4 matView)
{
	vec3 vecTransf = mat4x4_vec3_mul(v.position, matModel);
	vecTransf.x += transform.position.x;
	vecTransf.y += transform.position.y;
	vecTransf.z += transform.position.z;

	return mat4x4_vec3_mul(vecTransf, matView);
}

unsigned char* RenderMesh(vertex *model, size_t vertexCount, crTransform transform, mat4x4 matModel, mat4x4 matView)
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
		vec3 t1 = TransformVertex(model[i], transform, matModel, matView);
		vec3 t2 = TransformVertex(model[i + 1], transform, matModel, matView);
		vec3 t3 = TransformVertex(model[i + 2], transform, matModel, matView);

		vec3 line1 = vec3_vec3_sub(t2, t1);
		vec3 line2 = vec3_vec3_sub(t3, t1);

		vec3 normal = vec3_normalize(vec3_cross(line1, line2));

		vec3 camRay = vec3_vec3_sub(t1, (vec3){ 0 });
		if(vec3_dot(normal, camRay) >= 0)
			continue;

		vec3 p1 = ProjectVertex(t1);
		vec3 p2 = ProjectVertex(t2);
		vec3 p3 = ProjectVertex(t3);

		DrawLine((int)p1.x, (int)p1.y, (int)p2.x, (int)p2.y);
		DrawLine((int)p1.x, (int)p1.y, (int)p3.x, (int)p3.y);
		DrawLine((int)p3.x, (int)p3.y, (int)p2.x, (int)p2.y);
	}

	return screen;
}

unsigned char* RenderTriangleStrip(vertex *model, size_t vertexCount, crTransform transform, mat4x4 matModel, mat4x4 matView)
{
	for(size_t i = 2; i < vertexCount-2; ++i)
	{
		vec3 t1 = TransformVertex(model[i], transform, matModel, matView);
		vec3 t2 = TransformVertex(model[i + 1], transform, matModel, matView);
		vec3 t3 = TransformVertex(model[i + 2], transform, matModel, matView);

		vec3 line1 = vec3_vec3_sub(t2, t1);
		vec3 line2 = vec3_vec3_sub(t3, t1);

		vec3 normal = vec3_normalize(vec3_cross(line1, line2));

		vec3 camRay = vec3_vec3_sub(t1, (vec3){ 0 });
		if(vec3_dot(normal, camRay) >= 0)
			continue;

		vec3 p1 = ProjectVertex(t1);
		vec3 p2 = ProjectVertex(t2);
		vec3 p3 = ProjectVertex(t3);

		DrawLine((int)p1.x, (int)p1.y, (int)p2.x, (int)p2.y);
		DrawLine((int)p1.x, (int)p1.y, (int)p3.x, (int)p3.y);
		DrawLine((int)p3.x, (int)p3.y, (int)p2.x, (int)p2.y);
	}

	return screen;
}

unsigned char* RenderModel(vertex *model, size_t vertexCount, crTransform transform, CR_RenderMode renderMode)
{
	transform.rotation.x = transform.rotation.x * M_PI / 180.0;
	transform.rotation.y = transform.rotation.y * M_PI / 180.0;
	transform.rotation.z = transform.rotation.z * M_PI / 180.0;

	mat4x4 matRot = CreateXRotationMatrix(transform.rotation.x);
	mat4x4 matRotTMP = CreateZRotationMatrix(transform.rotation.y);
	matRot = mat4x4_mat4x4_mul(matRot, matRotTMP);
	matRotTMP = CreateZRotationMatrix(transform.rotation.z);
	matRot = mat4x4_mat4x4_mul(matRot, matRotTMP);

	mat4x4 matView = CreatePointAtMatrix((vec3){ 0 }, (vec3) { .x = 0, .y = 0, .z = 1 }, (vec3){ .x = 0, .y = 1, .z = 0 });
	matView = MatrixInverse(matView);

	switch(renderMode)
	{
		case RENDER_MODE_MESH:
			return RenderMesh(model, vertexCount, transform, matRot, matView);
			break;
		case RENDER_MODE_TRIANGLE_STRIP:
			return RenderTriangleStrip(model, vertexCount, transform, matRot, matView);
			break;
		default:
#if defined(__unix__)
			printf("Render mode %d doesn't exist or isn't suported!\n", renderMode);
#elif defined(__WIN32)
			MessageBox(NULL, "Unknown render mode!", "Render mode error", MB_OK | MB_ICONEXCLAMATION);
#endif
			break;
	}

	return NULL;
}
