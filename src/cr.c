#include <cr.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define USE_MATH_DEFINES

typedef struct
{
	float m[4][4];
}mat4x4;

struct CREngine
{
	mat4x4 matProj;
	
	unsigned char *screen;
	float *zbuffer;
	
	unsigned int screenWidth;
	unsigned int screenHeight;
	CR_RENDER_BUFFER_TYPE bufferType;
	
	uint32_t settings;
} defaultCRInstance; // Replaced all global variables with a global struct instance. Yay!

/* Custom linear functions to stay independent */
vec3 vec3_vec3_add(vec3 a, vec3 b)
{
	return (vec3) { .x = a.x+b.x, .y = a.y+b.y, .z = a.z+b.z, .w = a.w+b.w };
}

vec3 vec3_vec3_sub(vec3 a, vec3 b)
{
	return (vec3) { .x = a.x-b.x, .y = a.y-b.y, .z = a.z-b.z, .w = a.w-b.w };
}

vec3 vec3_vec3_mul(vec3 a, vec3 b)
{
	return (vec3) { .x = a.x*b.x, .y = a.y*b.y, .z = a.z*b.z, .w = a.w*b.w };
}

vec3 vec3_vec3_div(vec3 a, vec3 b)
{
	return (vec3) { .x = a.x/b.x, .y = a.y/b.y, .z = a.z/b.z, .w = a.w/b.w };
}

vec3 vec3_scale(vec3 a, float k)
{
	return (vec3) { .x = a.x*k, .y = a.y*k, .z = a.z*k, .w = a.w*k };
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

bool InitCR(unsigned int width, unsigned int height, CR_RENDER_BUFFER_TYPE bufferType, float fov, float nearPlane, float farPlane)
{
	defaultCRInstance.zbuffer = malloc(width*height*sizeof(float));
	if(!defaultCRInstance.zbuffer)
		return false;
	defaultCRInstance.bufferType = bufferType;
	defaultCRInstance.screenWidth = width;
	defaultCRInstance.screenHeight = height;

	float aspectRatio = height/(float)width;
	float fovRad = 1.0f / tanf(fov * 0.5f / 180.0f * M_PI);

	defaultCRInstance.matProj.m[0][0] = aspectRatio * fovRad;
	defaultCRInstance.matProj.m[1][1] = fovRad;
	defaultCRInstance.matProj.m[2][2] = farPlane / (farPlane - nearPlane);
	defaultCRInstance.matProj.m[3][2] = (-farPlane * nearPlane)/(farPlane-nearPlane);
	defaultCRInstance.matProj.m[2][3] = 1.0f;
	defaultCRInstance.matProj.m[3][3] = 0.0f;
	
	defaultCRInstance.settings = CR_BACKFACE_CULLING;

	return true;
}

void SetRenderDestination(unsigned char *dest)
{
	defaultCRInstance.screen = dest;
}

void CREnable(uint32_t settingBit)
{
	defaultCRInstance.settings ^= settingBit;
}

void PutPixel(unsigned int x, unsigned int y, unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
	if(x < 0 || x > defaultCRInstance.screenWidth-1 || y < 0 || y > defaultCRInstance.screenHeight-1)
		return;

	int offset = (x+y*defaultCRInstance.screenWidth)*(defaultCRInstance.bufferType == CR_RGBA ? 4 : 3);
	defaultCRInstance.screen[offset] = r;
	defaultCRInstance.screen[offset + 1] = g;
	defaultCRInstance.screen[offset + 2] = b;
	if(defaultCRInstance.bufferType == CR_RGBA)
		defaultCRInstance.screen[offset + 3] = a;
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

vertex ProjectVertex(vertex v)
{
	v.position = mat4x4_vec3_mul(v.position, defaultCRInstance.matProj);
	v.uv.x /= v.position.w;
	v.uv.y /= v.position.w;
	float w = 1.0/v.position.w;
	v.position = vec3_scale(v.position, w);

	v.position.x = (int)((v.position.x+1)*defaultCRInstance.screenWidth)>>1;
	v.position.y = (int)((v.position.y+1)*defaultCRInstance.screenHeight)>>1;
	v.position.w = w;

	return v;
}

/* For some reason applying the translation using the matrix offsets the verticies from the mesh origin instead of the world origin. Applying them this way however works fine */
vec3 TransformVertex(vertex v, crTransform transform, mat4x4 matModel)
{
	vec3 vecTransf = mat4x4_vec3_mul(v.position, matModel);
	vecTransf.x += transform.position.x;
	vecTransf.y += transform.position.y;
	vecTransf.z += transform.position.z;

	return vecTransf;
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

void DrawRasterizedTriangleLine(vec2 s, vec2 e, int xStart, int xEnd, int y, CRFRAGMENTPROC fragmentProc)
{
	float d_u = s.x;
	float d_v = s.y;
	float d_w = s.w;
	float tstep = 1.0/(float)(xEnd-xStart);
	float t = 0;
	for(int x = xStart; x < xEnd; ++x)
	{
		d_u = (1.0 - t) * s.x + t * e.x;
		d_v = (1.0 - t) * s.y + t * e.y;
		d_w = (1.0 - t) * s.w + t * e.w;
		int offset = y*defaultCRInstance.screenWidth+x;
		if(d_w > defaultCRInstance.zbuffer[offset])
		{
			defaultCRInstance.zbuffer[offset] = d_w;
			if(fragmentProc)
			{
				uint32_t cv = fragmentProc(x, y, d_u/d_w, d_v/d_w, d_w);
				unsigned char *rgba = (unsigned char*)&cv;
				PutPixel(x, y, rgba[0], rgba[1], rgba[2], rgba[3]);
			}
			else
				PutPixel(x, y, 0, 0, 0, 255);
		}
		t += tstep;
	}
}

/* Blatantly stolen from OneLoneCoders olcConsoleGameEngine. So exactly how and why it works you can watch this video: https://www.youtube.com/watch?v=nBzCS-Y0FcY&t=2198s&ab_channel=javidx9 */
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
	//float d_u, d_v, d_w;

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

	// Resterize from top point(if a single top one is available) to the bottom
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

			DrawRasterizedTriangleLine(s, e, ax, bx, i, fragmentProc);
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

	// Resterize from top line(either because the algorithm subdivided it or it just has a flat top) to bottom point
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

			DrawRasterizedTriangleLine(s, e, ax, bx, i, fragmentProc);
		}
	}
}

vertex vertex_intersectPlane(vec3 plane, vec3 planeNormal, vertex start, vertex end)
{
	float planeDot = vec3_dot(planeNormal, plane);
	float startDot = vec3_dot(start.position, planeNormal);
	float endDot = vec3_dot(end.position, planeNormal);

	float t = (planeDot - startDot) / (endDot - startDot);
	vec3 dir = vec3_vec3_sub(end.position, start.position);
	return (vertex) {
		.position = vec3_vec3_add(start.position, vec3_scale(dir, t)),
		.uv = {
			.x = t * (end.uv.x - start.uv.x) + start.uv.x,
			.y = t * (end.uv.y - start.uv.y) + start.uv.y
		}
	};
}

float PointPlaneDist(vec3 plane, vec3 planeNormal, vec3 point)
{
	return planeNormal.x * point.x + planeNormal.y * point.y + planeNormal.z * point.z - vec3_dot(planeNormal, plane);
}

int ClipTriangle(vec3 plane, vec3 planeNormal, vertex v1, vertex v2, vertex v3, vertex *output)
{
	vertex insidePoints[3] = { 0 };
	vertex outsidePoints[3] = { 0 };

	int insideCount = 0;

	float dist1 = PointPlaneDist(plane, planeNormal, v1.position);
	float dist2 = PointPlaneDist(plane, planeNormal, v2.position);
	float dist3 = PointPlaneDist(plane, planeNormal, v3.position);

	if(dist1 >= 0)
	{
		insidePoints[insideCount] = v1;
		insideCount++;
	}
	else
		outsidePoints[0] = v1;

	if(dist2 >= 0)
	{
		insidePoints[insideCount] = v2;
		insideCount++;
	}
	else
		outsidePoints[1-insideCount] = v2;

	if(dist3 >= 0)
	{
		insidePoints[insideCount] = v3;
		insideCount++;
	}
	else
		outsidePoints[2-insideCount] = v3;

	if(insideCount == 0)
		return 0; // All points lie outside of the plane
	if(insideCount == 3)
	{
		output[0] = v1;
		output[1] = v2;
		output[2] = v3;
		return 1; // All points lie within the plane
	}

	// Now it gets interesting
	if(insideCount == 1) // we only need to check the insideCount as it is imperative that outsideCount must be 2 in this case
	{
		output[0] = insidePoints[0];
		output[1] = vertex_intersectPlane(plane, planeNormal, insidePoints[0], outsidePoints[0]);
		output[2] = vertex_intersectPlane(plane, planeNormal, insidePoints[0], outsidePoints[1]);
		return 1; // Two points are outside, thus "shrinking" the triangle is enough
	}

	if(insideCount == 2)
	{
		// The first triangle consists of two inside and one outside point
		output[0] = insidePoints[0];
		output[1] = insidePoints[1];
		output[2] = vertex_intersectPlane(plane, planeNormal, insidePoints[0], outsidePoints[0]);

		output[3] = insidePoints[1];
		output[4] = output[2];
		output[5] = vertex_intersectPlane(plane, planeNormal, insidePoints[1], outsidePoints[0]);

		return 2;
	}

	return 0; // Should never happen but if it does: skip
}

int ClipTriangleList(vec3 plane, vec3 planeNormal, vertex *list, int triangleCount, int maxListLength)
{
	int numReturnedTriangles = 0;

	// I'm using the modulo '%' instead of the binary '&' since I can't garuantee that maxListLength = 2^x-1 where x = ln(maxListLength+1)/ln(2)
	// or in human terms that maxListLength is a power of 2 minus one
	for(int i = 0; i < triangleCount; ++i)
	{
		numReturnedTriangles += ClipTriangle( plane, planeNormal,
							list[(i * 3) % maxListLength],
							list[(i * 3 + 1) % maxListLength],
							list[(i * 3 + 2) % maxListLength],
							&list[((triangleCount+ numReturnedTriangles) * 3) % maxListLength]);
	}
	
	return numReturnedTriangles;
}

void RenderTriangle(vertex vert1, vertex vert2, vertex vert3, crTransform transform, mat4x4 matModel, mat4x4 matView, CRFRAGMENTPROC fragmentProc)
{
	vec3 t1 = TransformVertex(vert1, transform, matModel);
	vec3 t2 = TransformVertex(vert2, transform, matModel);
	vec3 t3 = TransformVertex(vert3, transform, matModel);

	vec3 line1 = vec3_vec3_sub(t2, t1);
	vec3 line2 = vec3_vec3_sub(t3, t1);

	if(defaultCRInstance.settings & CR_BACKFACE_CULLING)
	{
		vec3 normal = vec3_normalize(vec3_cross(line1, line2));

		vec3 camRay = vec3_vec3_sub(t1, (vec3){ 0 });
		if(vec3_dot(normal, camRay) >= 0)
			return;
	}

	vertex v1 = {
		.position = mat4x4_vec3_mul(t1, matView),
		.uv = vert1.uv
	};
	vertex v2 = {
		.position = mat4x4_vec3_mul(t2, matView),
		.uv = vert2.uv
	};
	vertex v3 = {
		.position = mat4x4_vec3_mul(t3, matView),
		.uv = vert3.uv
	};

	// The clipping algorithm uses a "rolling queue".
	// This means that we use the efficency of an array and the pop and push features of a queue
	// by keeping track of the head and tail index and looping back to index 0 when we reached the end.
	// This of course limits us to some arbitrary max capacity but it's all we need and that good enough for me.
	//
	// Example: We have a capacity of 5 and two triangles at index 0 and 1.
	// We "pop" the first triangle from the "queue" and set the head index to 1. (Step 1 & 2)
	// When the triangle is split into two new ones we "push" them back onto the "queue" and set the tail index to 3 (Step 3)
	// When we now "pop" the second triangle and push the two split ones we "loop back", while trying to push the last element, to index 0 using modulo since we reached the maximum capacity(more representative of the last valid index)
	// and set the tail index to 0, meaning now the head is set to 2 and the tail to 0. (Step 4 & 5)
	// However because we use the modulo operation to access elements we can just add our element offset(or index) onto the head index, apply the modulo operation and get the correct result
	//
	// Steps:
	// 1. [ T1 ,  T2 , NULL, NULL, NULL] headIndex = 0, tailIndex = 1
	// 2. [NULL,  T2 , NULL, NULL, NULL] headIndex = 1, tailIndex = 1
	// 3. [NULL,  T2 ,  T3 ,  T4 , NULL] headIndex = 1, tailIndex = 3
	// 4. [NULL, NULL,  T3 ,  T4 , NULL] headIndex = 2, tailIndex = 3
	// 5. [ T6 , NULL,  T3 ,  T4 ,  T5 ] headIndex = 2, tailIndex = 0
	vertex clippedPoints[128] = { 0 }; // There should never be move than 96 verticies(aka 32 triangles) but I threw on 32 more to have some padding and be able to use the '&' operator for modulo
	// We only save the head index as the tail index is now obsolete with the convertion to a function. We just calculate it using the head and the length
	size_t headIndex = 0;
	int numClippedTriangles = ClipTriangle((vec3){ .x = 0, .y = 0, .z = 0.1 }, (vec3){ .x = 0, .y = 0, .z = 1.0 }, v1, v2, v3, clippedPoints);
	size_t queueLength = numClippedTriangles;

	for(int i = 0; i < numClippedTriangles; ++i)
	{
		clippedPoints[i * 3] = ProjectVertex(clippedPoints[i * 3]);
		clippedPoints[i * 3 + 1] = ProjectVertex(clippedPoints[i * 3 + 1]);
		clippedPoints[i * 3 + 2] = ProjectVertex(clippedPoints[i * 3 + 2]);
	}

	//Top
	numClippedTriangles = ClipTriangleList((vec3){ .x = 0, .y = 0								, .z = 0 }, (vec3){ .x =  0.0, .y =  1.0, .z = 0.0 }, clippedPoints			 	, numClippedTriangles, 128);
	headIndex += (queueLength * 3) & 127;
	queueLength = numClippedTriangles;

	//Bottom
	numClippedTriangles = ClipTriangleList((vec3){ .x = 0, .y = defaultCRInstance.screenHeight-1, .z = 0 }, (vec3){ .x =  0.0, .y = -1.0, .z = 0.0 }, &clippedPoints[headIndex] , numClippedTriangles, 128);
	headIndex += (queueLength * 3) & 127;
	queueLength = numClippedTriangles;
	
	//Left
	numClippedTriangles = ClipTriangleList((vec3){ .x = 0, .y = 0								, .z = 0 }, (vec3){ .x =  1.0, .y =  0.0, .z = 0.0 }, &clippedPoints[headIndex] , numClippedTriangles, 128);
	headIndex += (queueLength * 3) & 127;
	queueLength = numClippedTriangles;

	//Right
	numClippedTriangles = ClipTriangleList((vec3){ .x = defaultCRInstance.screenWidth-1, .y = 0	, .z = 0 }, (vec3){ .x = -1.0, .y =  0.0, .z = 0.0 }, &clippedPoints[headIndex] , numClippedTriangles, 128);
	headIndex += (queueLength * 3) & 127;
	queueLength = numClippedTriangles;

	for(int i = 0; i < queueLength; ++i)
	{
		vertex p1 = clippedPoints[(headIndex + i * 3) & 127];
		vertex p2 = clippedPoints[(headIndex + i * 3 + 1) & 127];
		vertex p3 = clippedPoints[(headIndex + i * 3 + 2) & 127];

		RasterizeTriangle(p1, p2, p3, fragmentProc);

		#ifdef DEBUG
			DrawLine(p1.position.x, p1.position.y, p2.position.x, p2.position.y);
			DrawLine(p1.position.x, p1.position.y, p3.position.x, p3.position.y);
			DrawLine(p3.position.x, p3.position.y, p2.position.x, p2.position.y);
		#endif
	}
}

unsigned char* RenderMesh(vertex *model, size_t vertexCount, crTransform transform, mat4x4 matModel, mat4x4 matView, CRFRAGMENTPROC fragmentProc)
{
	for(size_t i = 0; i < vertexCount; i += 3)
		RenderTriangle(model[i], model[i + 1], model[i + 2], transform, matModel, matView, fragmentProc);

	return defaultCRInstance.screen;
}

/* To render a triangle strip backface culling has to be tunred off. If it isn't every second triangle will (not) be visible due to the previous face's direction(depends on the initial faces direction) */
unsigned char* RenderTriangleStrip(vertex *model, size_t vertexCount, crTransform transform, mat4x4 matModel, mat4x4 matView, CRFRAGMENTPROC fragmentProc)
{
	for(size_t i = 2; i < vertexCount-2; ++i)
		RenderTriangle(model[i], model[i + 1], model[i + 2], transform, matModel, matView, fragmentProc);

	return defaultCRInstance.screen;
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

	mat4x4 matView = CreatePointAtMatrix((vec3){ 0 }, (vec3) { .x = 0, .y = 0, .z = 1.0 }, (vec3){ .x = 0, .y = 1, .z = 0 });
	matView = MatrixInverse(matView);

	switch(renderMode)
	{
		case RENDER_MODE_MESH:
			return RenderMesh(model, vertexCount, transform, matRot, matView, fragmentProc);
			break;
		case RENDER_MODE_TRIANGLE_STRIP:
			return RenderTriangleStrip(model, vertexCount, transform, matRot, matView, fragmentProc);
			break;
		default:
			break;
	}

	return NULL;
}

void CRClearDepthBuffer(void)
{
	memset(defaultCRInstance.zbuffer, 0, defaultCRInstance.screenWidth*defaultCRInstance.screenHeight*(defaultCRInstance.bufferType == CR_RGBA ? 4 : 3));
}

void CleanupCR(void)
{
	free(defaultCRInstance.zbuffer);
}
