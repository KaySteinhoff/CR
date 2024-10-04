#include <cr.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define USE_MATH_DEFINES

typedef struct
{
	float m[4][4];
}mat4x4;

/* Currently global variables. Will be reworked to work be handled in some struct */
mat4x4 matProj = { 0 };
unsigned char *screen = NULL;
float *zbuffer = NULL;
unsigned int screenWidth = 0;
unsigned int screenHeight = 0;
CR_RENDER_BUFFER_TYPE bufferType;

/* Custom linear functions to stay independent */
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
	o.x = v.x * m.m[0][0] + v.y * m.m[1][0] + v.z * m.m[2][0] + v.w * m.m[3][0];
	o.y = v.x * m.m[0][1] + v.y * m.m[1][1] + v.z * m.m[2][1] + v.w * m.m[3][1];
	o.z = v.x * m.m[0][2] + v.y * m.m[1][2] + v.z * m.m[2][2] + v.w * m.m[3][2];
	o.w = v.x * m.m[0][3] + v.y * m.m[1][3] + v.z * m.m[2][3] + v.w * m.m[3][3];

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

bool InitCR(unsigned int width, unsigned int height, CR_RENDER_BUFFER_TYPE buffType, float fov, float nearPlane, float farPlane)
{
	zbuffer = malloc(width*height*sizeof(float));
	if(!zbuffer)
		return false;
	bufferType = buffType; // Prime example why global variables suck ass, but works for now and is no big deal to rework
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

void SetRenderDestination(unsigned char *dest)
{
	screen = dest;
}

void PutPixel(unsigned int x, unsigned int y, unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
	if(x < 0 || x > screenWidth-1 || y < 0 || y > screenHeight-1)
		return;

	int offset = (x+y*screenWidth)*(bufferType == CR_RGBA ? 4 : 3);
	screen[offset] = r;
	screen[offset + 1] = g;
	screen[offset + 2] = b;
	if(bufferType == CR_RGBA)
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

/* The renderer uses his own draw function to stay independent from what ever library you used to create the image */
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
	float w = vecProj.w;
	vec3 vecScreen = vec3_scale(vecProj, 1.0/vecProj.w);

	vecScreen.x = (int)((vecScreen.x+1)*screenWidth)>>1;
	vecScreen.y = (int)((vecScreen.y+1)*screenHeight)>>1;
	vecScreen.w = w;

	return vecScreen;
}

/* For some reason applying the translation using the matrix offsets the verticies from the mesh origin instead of the world origin. Applying them this way however works fine */
vec3 TransformVertex(vertex v, crTransform transform, mat4x4 matModel, mat4x4 matView)
{
	vec3 vecTransf = mat4x4_vec3_mul(v.position, matModel);
	vecTransf.x += transform.position.x;
	vecTransf.y += transform.position.y;
	vecTransf.z += transform.position.z;

	return mat4x4_vec3_mul(vecTransf, matView);
}

void SwapVec3(vec3 *a, vec3 *b)
{
	float tmp = a->x;
	a->x = b->x;
	b->x = tmp;

	tmp = a->y;
	a->y = b->y;
	b->y = tmp;

	tmp = a->z;
	a->z = b->z;
	b->z = tmp;

	tmp = a->w;
	a->w = b->w;
	b->w = tmp;
}

void SwapVec2(vec2 *a, vec2 *b)
{
	float tmp = a->x;
	a->x = b->x;
	b->x = tmp;

	tmp = a->y;
	a->y = b->y;
	b->y = tmp;

	tmp = a->w;
	a->w = b->w;
	b->w = tmp;
}

/* Blatantly stolen from OneLoneCoders olcConsoleGameEngine. Needs to be reworked(soon!) */
void RasterizeTriangle(vertex p1, vertex p2, vertex p3, CRFRAGMENTPROC fragmentProc)
{
	//Sort from "top" to "bottom"
	if(p2.position.y < p1.position.y)
	{
		SwapVec2(&p1.uv, &p2.uv);
		SwapVec3(&p1.position, &p2.position);
	}
	if(p3.position.y < p1.position.y)
	{
		SwapVec2(&p1.uv, &p3.uv);
		SwapVec3(&p1.position, &p3.position);
	}
	if(p3.position.y < p2.position.y)
	{
		SwapVec2(&p2.uv, &p3.uv);
		SwapVec3(&p2.position, &p3.position);
	}

	int dx1 = p2.position.x-p1.position.x;
	int dy1 = p2.position.y-p1.position.y;
	float du1 = p2.uv.x - p1.uv.x;
	float dv1 = p2.uv.y - p1.uv.y;

	int dx2 = p3.position.x-p1.position.x;
	int dy2 = p3.position.y-p1.position.y;
	float du2 = p3.uv.x - p1.uv.x;
	float dv2 = p3.uv.y - p1.uv.y;

	float dw1 = p2.position.w-p1.position.w;
	float dw2 = p3.position.w-p1.position.w;

	float 	dax_step = 0, dbx_step = 0,
			dw1_step = 0, dw2_step = 0,
			du1_step = 0, dv1_step= 0,
			du2_step = 0, dv2_step = 0;
	float d_u, d_v, d_w;

	if(dy1)
	{
		float dy1Abs = (float)abs(dy1);
		dax_step = dx1/dy1Abs;
		dw1_step = dw1/dy1Abs;
		du1_step = du1/dy1Abs;
		dv1_step = dv1/dy1Abs;
	}
	if(dy2)
	{
		float dy2Abs = (float)abs(dy2);
		dbx_step = dx2/dy2Abs;
		dw2_step = dw2/dy2Abs;
		du2_step = du2/dy2Abs;
		dv2_step = dv2/dy2Abs;
	}

	if(dy1)
	{
		for(int i = p1.position.y; i <= p2.position.y; ++i)
		{
			float ydiff = (float)(i - p1.position.y);
			int ax = p1.position.x + ydiff * dax_step;
			int bx = p1.position.x + ydiff * dbx_step;

			vec2 s = {
				.x = p1.uv.x + ydiff * du1_step,
				.y = p1.uv.y + ydiff * dv1_step,
				.w = p1.position.w + ydiff * dw1_step
			};

			vec2 e = {
				.x = p1.uv.x + ydiff * du2_step,
				.y = p1.uv.y + ydiff * dv2_step,
				.w = p1.position.w + ydiff * dw2_step
			};

			if(ax > bx)
			{
				float tmp = ax;
				ax = bx;
				bx = tmp;

				SwapVec2(&s, &e);
			}

			d_u = s.x;
			d_v = s.y;
			d_w = s.w;
			float tstep = 1.0/(float)(bx-ax);
			float t = 0;

			for(int j = ax; j < bx; ++j)
			{
				d_u = (1.0 - t) * s.x + t * e.x;
				d_v = (1.0 - t) * s.y + t * e.y;
				d_w = (1.0 - t) * s.w + t * e.w;
				int offset = i*screenWidth+j;
				if(d_w > zbuffer[offset])
				{
					zbuffer[offset] = d_w;
					if(fragmentProc)
					{
						size_t cv = fragmentProc(j, i, d_u/d_w, d_v/d_w, d_w);
						unsigned char *rgba = (unsigned char*)&cv;
						PutPixel(j, i, rgba[0], rgba[1], rgba[2], rgba[3]);
					}
					else
						PutPixel(j, i, 0, 0, 0, 255);
				}
				t += tstep;
			}
		}
	}

	dy1 = p3.position.y-p2.position.y;
	dx1 = p3.position.x-p2.position.x;
	du1 = p3.uv.x-p2.uv.x;
	dv1 = p3.uv.y-p2.uv.y;
	dw1 = p3.position.w-p2.position.w;

	if(dy1)
	{
		float dy1Abs = (float)abs(dy1);
		dax_step = dx1/dy1Abs;
		dw1_step = dw1/dy1Abs;
		du1_step = du1/dy1Abs;
		dv1_step = dv1/dy1Abs;
	}
	if(dy2) dbx_step = dx2/(float)abs(dy2);

	if(dy1)
	{
		for(int i = p2.position.y; i < p3.position.y; ++i)
		{
			float y1diff = (float)(i - p1.position.y);
			float y2diff = (float)(i - p2.position.y);
			int ax = p2.position.x + y2diff * dax_step;
			int bx = p1.position.x + y1diff * dbx_step;

			vec2 s = {
				.x = p2.uv.x + y2diff * du1_step,
				.y = p2.uv.y + y2diff * dv1_step,
				.w = p2.position.w + y2diff * dw1_step
			};

			vec2 e = {
				.x = p1.uv.x + y1diff * du2_step,
				.y = p1.uv.y + y1diff * dv2_step,
				.w = p1.position.w + y1diff * dw2_step
			};

			if(ax > bx)
			{
				float tmp = ax;
				ax = bx;
				bx = tmp;

				SwapVec2(&s, &e);
			}

			d_u = s.x;
			d_v = s.y;
			d_w = s.w;
			float tstep = 1.0/(float)(bx-ax);
			float t = 0;
			for(int j = ax; j < bx; ++j)
			{
				d_u = (1.0 - t) * s.x + t * e.x;
				d_v = (1.0 - t) * s.y + t * e.y;
				d_w = (1.0 - t) * s.w + t * e.w;
				int offset = i*screenWidth+j;
				if(d_w > zbuffer[offset])
				{
					zbuffer[offset] = d_w;
					if(fragmentProc)
					{
						size_t cv = fragmentProc(j, i, d_u/d_w, d_v/d_w, d_w);
						unsigned char *rgba = (unsigned char*)&cv;
						PutPixel(j, i, rgba[0], rgba[1], rgba[2], rgba[3]);
					}
					else
						PutPixel(j, i, 0, 0, 0, 255);
				}
				t += tstep;
			}
		}
	}
}

unsigned char* RenderMesh(vertex *model, size_t vertexCount, crTransform transform, mat4x4 matModel, mat4x4 matView, CRFRAGMENTPROC fragmentProc)
{
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

		vertex p1 = {
			.position = ProjectVertex(t1),
			.uv = model[i].uv
		};
		p1.uv.x /= p1.position.w;
		p1.uv.y /= p1.position.w;
		p1.position.w = 1.0/p1.position.w;

		vertex p2 = {
			.position = ProjectVertex(t2),
			.uv = model[i + 1].uv
		};
		p2.uv.x /= p2.position.w;
		p2.uv.y /= p2.position.w;
		p2.position.w = 1.0/p2.position.w;

		vertex p3 = {
			.position = ProjectVertex(t3),
			.uv = model[i + 2].uv
		};
		p3.uv.x /= p3.position.w;
		p3.uv.y /= p3.position.w;
		p3.position.w = 1.0/p3.position.w;

		RasterizeTriangle(p1, p2, p3, fragmentProc);

		// Was used for debug purposes, not needed right now.
/*		DrawLine((int)p1.x, (int)p1.y, (int)p2.x, (int)p2.y);
		DrawLine((int)p1.x, (int)p1.y, (int)p3.x, (int)p3.y);
		DrawLine((int)p3.x, (int)p3.y, (int)p2.x, (int)p2.y);*/
	}

	return screen;
}

/* Out of order: Triangle strips don't work at the moment due to the fact that normals are calculated using the face which could be solved using passed down normals but I've got textures to work on for now */
unsigned char* RenderTriangleStrip(vertex *model, size_t vertexCount, crTransform transform, mat4x4 matModel, mat4x4 matView, CRFRAGMENTPROC fragmentProc)
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

		vertex p1 = {
			.position = ProjectVertex(t1),
			.uv = model[i].uv
		};
		vertex p2 = {
			.position = ProjectVertex(t2),
			.uv = model[i + 1].uv
		};
		vertex p3 = {
			.position = ProjectVertex(t3),
			.uv = model[i + 2].uv
		};

		RasterizeTriangle(p1, p2, p3, fragmentProc);

		// Was used for debug purposes, not needed right now.
/*		DrawLine((int)p1.x, (int)p1.y, (int)p2.x, (int)p2.y);
		DrawLine((int)p1.x, (int)p1.y, (int)p3.x, (int)p3.y);
		DrawLine((int)p3.x, (int)p3.y, (int)p2.x, (int)p2.y);*/
	}

	return screen;
}

unsigned char* RenderModel(vertex *model, size_t vertexCount, crTransform transform, CR_RenderMode renderMode, CRFRAGMENTPROC fragmentProc)
{
	transform.rotation.x = transform.rotation.x * M_PI / 180.0;
	transform.rotation.y = transform.rotation.y * M_PI / 180.0;
	transform.rotation.z = transform.rotation.z * M_PI / 180.0;

	mat4x4 matRot = CreateXRotationMatrix(transform.rotation.x);
	mat4x4 matRotTMP = CreateYRotationMatrix(transform.rotation.y);
	matRot = mat4x4_mat4x4_mul(matRot, matRotTMP);
	matRotTMP = CreateZRotationMatrix(transform.rotation.z);
	matRot = mat4x4_mat4x4_mul(matRot, matRotTMP);

	mat4x4 matView = CreatePointAtMatrix((vec3){ 0 }, (vec3) { .x = 0, .y = 0, .z = 1 }, (vec3){ .x = 0, .y = 1, .z = 0 });
	matView = MatrixInverse(matView);

	switch(renderMode)
	{
		case RENDER_MODE_MESH:
			return RenderMesh(model, vertexCount, transform, matRot, matView, fragmentProc);
			break;
		case RENDER_MODE_TRIANGLE_STRIP:
			/* Fall through. Reason: see RenderTriangleStrip comment */
//			return RenderTriangleStrip(model, vertexCount, transform, matRot, matView);
//			break;
		default:
			break;
	}

	return NULL;
}

void CRClearDepthBuffer(void)
{
	memset(zbuffer, 0, screenWidth*screenHeight*(bufferType == CR_RGBA ? 4 : 3));
}

void CleanupCR(void)
{
	free(zbuffer);
}
