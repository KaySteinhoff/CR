#include <cr.h>
/* Although it is prefixed with 'cr' it's not neccesary to include these two headers. They are just to manage Windows/Linux Window and Image handleing respectively */
#include <crWindowing.h>
#include <crImaging.h>
#include <time.h>
#include <string.h>
#include <stdio.h>

static const int screenWidth = 800;
static const int screenHeight = 600;

vertex model[] = {
	/* FRONT FACE */
	{ .position = { .x = -0.5, .y = -0.5, .z = -0.5, .w = 1}, .uv = { .x = 0, .y = 0, .w = 0 } },
	{ .position = { .x =  0.5, .y =  0.5, .z = -0.5, .w = 1}, .uv = { .x = 1, .y = 1, .w = 0 } },
	{ .position = { .x =  0.5, .y = -0.5, .z = -0.5, .w = 1}, .uv = { .x = 1, .y = 0, .w = 0 } },

	{ .position = { .x = -0.5, .y = -0.5, .z = -0.5, .w = 1}, .uv = { .x = 0, .y = 0, .w = 0 } },
	{ .position = { .x = -0.5, .y =  0.5, .z = -0.5, .w = 1}, .uv = { .x = 0, .y = 1, .w = 0 } },
	{ .position = { .x =  0.5, .y =  0.5, .z = -0.5, .w = 1}, .uv = { .x = 1, .y = 1, .w = 0 } },
	/* LEFT FACE */
	{ .position = { .x = -0.5, .y = -0.5, .z = -0.5, .w = 1}, .uv = { .x = 1, .y = 0, .w = 0 } },
	{ .position = { .x = -0.5, .y = -0.5, .z =  0.5, .w = 1}, .uv = { .x = 0, .y = 0, .w = 0 } },
	{ .position = { .x = -0.5, .y =  0.5, .z =  0.5, .w = 1}, .uv = { .x = 0, .y = 1, .w = 0 } },

	{ .position = { .x = -0.5, .y = -0.5, .z = -0.5, .w = 1}, .uv = { .x = 1, .y = 0, .w = 0 } },
	{ .position = { .x = -0.5, .y =  0.5, .z =  0.5, .w = 1}, .uv = { .x = 0, .y = 1, .w = 0 } },
	{ .position = { .x = -0.5, .y =  0.5, .z = -0.5, .w = 1}, .uv = { .x = 1, .y = 1, .w = 0 } },
	/* RIGHT FACE */
	{ .position = { .x = 0.5, .y = -0.5, .z = -0.5, .w = 1}, .uv = { .x = 0, .y = 0, .w = 0 } },
	{ .position = { .x = 0.5, .y =  0.5, .z =  0.5, .w = 1}, .uv = { .x = 1, .y = 1, .w = 0 } },
	{ .position = { .x = 0.5, .y = -0.5, .z =  0.5, .w = 1}, .uv = { .x = 1, .y = 0, .w = 0 } },

	{ .position = { .x = 0.5, .y = -0.5, .z = -0.5, .w = 1}, .uv = { .x = 0, .y = 0, .w = 0 } },
	{ .position = { .x = 0.5, .y =  0.5, .z = -0.5, .w = 1}, .uv = { .x = 0, .y = 1, .w = 0 } },
	{ .position = { .x = 0.5, .y =  0.5, .z =  0.5, .w = 1}, .uv = { .x = 1, .y = 1, .w = 0 } },
	/* BOTTOM FACE */
	{ .position = { .x = -0.5, .y = -0.5, .z = -0.5, .w = 1}, .uv = { .x = 0, .y = 1, .w = 0 } },
	{ .position = { .x =  0.5, .y = -0.5, .z = -0.5, .w = 1}, .uv = { .x = 1, .y = 1, .w = 0 } },
	{ .position = { .x =  0.5, .y = -0.5, .z =  0.5, .w = 1}, .uv = { .x = 1, .y = 0, .w = 0 } },

	{ .position = { .x = -0.5, .y = -0.5, .z = -0.5, .w = 1}, .uv = { .x = 0, .y = 1, .w = 0 } },
	{ .position = { .x =  0.5, .y = -0.5, .z =  0.5, .w = 1}, .uv = { .x = 1, .y = 0, .w = 0 } },
	{ .position = { .x = -0.5, .y = -0.5, .z =  0.5, .w = 1}, .uv = { .x = 0, .y = 0, .w = 0 } },
	/* TOP FACE */
	{ .position = { .x = -0.5, .y = 0.5, .z = -0.5, .w = 1}, .uv = { .x = 0, .y = 0, .w = 0 } },
	{ .position = { .x =  0.5, .y = 0.5, .z =  0.5, .w = 1}, .uv = { .x = 0, .y = 0, .w = 0 } },
	{ .position = { .x =  0.5, .y = 0.5, .z = -0.5, .w = 1}, .uv = { .x = 0, .y = 0, .w = 0 } },

	{ .position = { .x = -0.5, .y = 0.5, .z = -0.5, .w = 1}, .uv = { .x = 0, .y = 0, .w = 0 } },
	{ .position = { .x = -0.5, .y = 0.5, .z =  0.5, .w = 1}, .uv = { .x = 0, .y = 0, .w = 0 } },
	{ .position = { .x =  0.5, .y = 0.5, .z =  0.5, .w = 1}, .uv = { .x = 0, .y = 0, .w = 0 } },
	/* BACK FACE */
	{ .position = { .x = -0.5, .y = -0.5, .z = 0.5, .w = 1}, .uv = { .x = 1, .y = 0, .w = 0 } },
	{ .position = { .x =  0.5, .y = -0.5, .z = 0.5, .w = 1}, .uv = { .x = 0, .y = 0, .w = 0 } },
	{ .position = { .x =  0.5, .y =  0.5, .z = 0.5, .w = 1}, .uv = { .x = 0, .y = 1, .w = 0 } },

	{ .position = { .x = -0.5, .y = -0.5, .z = 0.5, .w = 1}, .uv = { .x = 1, .y = 0, .w = 0 } },
	{ .position = { .x =  0.5, .y =  0.5, .z = 0.5, .w = 1}, .uv = { .x = 0, .y = 1, .w = 0 } },
	{ .position = { .x = -0.5, .y =  0.5, .z = 0.5, .w = 1}, .uv = { .x = 1, .y = 1, .w = 0 } },
};

/* Triangle strips don't work at the moment but I'm working on it ;) */
vertex cubeTriStrip[] = {
	{ .position = { .x = 0, .y = 0, .z = 0} },
	{ .position = { .x = 1, .y = 0, .z = 0} },
	{ .position = { .x = 0, .y = 0, .z = 1} },
	{ .position = { .x = 1, .y = 0, .z = 1} },
	{ .position = { .x = 0, .y = 1, .z = 1} },
	{ .position = { .x = 1, .y = 1, .z = 1} },
	{ .position = { .x = 0, .y = 1, .z = 0} },
	{ .position = { .x = 1, .y = 1, .z = 0} },
	{ .position = { .x = 0, .y = 0, .z = 0} },
	{ .position = { .x = 1, .y = 0, .z = 0} }
};

size_t simpleTex[8*8] = {
	0xffff0000, 0xff00ff00, 0xff0000ff, 0xffffff00, 0xff00ffff, 0xffff00ff, 0xff00ffff, 0xffffffff,
	0xff00ff00, 0xff0000ff, 0xffffff00, 0xff00ffff, 0xffff00ff, 0xff00ffff, 0xffffffff, 0xffff0000,
	0xff0000ff, 0xffffff00, 0xff00ffff, 0xffff00ff, 0xff00ffff, 0xffffffff, 0xffff0000, 0xff00ff00,
	0xffffff00, 0xff00ffff, 0xffff00ff, 0xff00ffff, 0xffffffff, 0xffff0000, 0xff00ff00, 0xff0000ff,
	0xff00ffff, 0xffff00ff, 0xff00ffff, 0xffffffff, 0xffff0000, 0xff00ff00, 0xff0000ff, 0xffffff00,
	0xffff00ff, 0xff00ffff, 0xffffffff, 0xffff0000, 0xff00ff00, 0xff0000ff, 0xffffff00, 0xff00ffff,
	0xff00ffff, 0xffffffff, 0xffff0000, 0xff00ff00, 0xff0000ff, 0xffffff00, 0xff00ffff, 0xffff00ff,
	0xffffffff, 0xffff0000, 0xff00ff00, 0xff0000ff, 0xffffff00, 0xff00ffff, 0xffff00ff, 0xff00ffff,
};

size_t fragment(int x, int y, float u, float v, float w)
{
//	int tx = 8*u;
//	int ty = 7*v;
//	size_t bc = simpleTex[tx+(ty<<3)];
/*	if(w < 0)
		return 0xffff0000;
	else if(w > 1)
		return 0xff00ff00;
	size_t bc = 0xffffffff;
	unsigned char *rgba = (unsigned char*)&bc;
	unsigned char brightness = (1.0-w)*255;
	rgba[0] *= brightness;
	rgba[1] *= brightness;
	rgba[2] *= brightness;*/
//	return bc;
//	if(u < 0.05 || u > 0.95 || v < 0.05 || v > 0.95)
//		return 0xffffffff;
//	return 0xff000000;
	return 0xffff0000;
}

int main(int argc, char **argv)
{
	clock_t start, end;

	if(!InitCRWindow())
		return 0;

	if(!InitCR(screenWidth, screenHeight, CR_RGBA, 90, 0.1, 1000))
	{
#if defined(__WIN32)
		MessageBox(NULL, "Failed to initialize cr lib!", "InitCR() error", MB_OK | MB_ICONEXCLAMATION);
#elif defined(__unix__)
		printf("Failed to initialize cr lib!\n");
#endif
		return 1;
	}

	CRImage image = CreateRenderImage(screenWidth, screenHeight);
	if(!image.data) // Hacky but ok for initial implementations
	{
		printf("Failed to create render image!\n");
		return 2;
	}
	SetRenderDestination(image.data);
	start = clock();
	crTransform transform = {
		.position = { .x = 0, .y = 0, .z = 1.5},
		.rotation = { .x = 0, .y = 45, .z =0 },
		.scale = { .x = 1, .y = 1, .z = 1 }
	};

	while(!ShouldCRWindowClose())
	{
		/* Clear rendered image */
		memset(image.data, 0, (screenWidth * screenHeight)<<2);

		/* Check if 1/30th of a second passed */
		end = clock();
		if(end-start < CLOCKS_PER_SEC * (1.0/30.0)) continue;

		// Update transform and render image
//		float elapsed = (float)(end-start)/CLOCKS_PER_SEC;
//		transform.rotation.y += elapsed*50;
//		transform.position.y -= elapsed*0.5;

		RenderModel(model, 36, transform, RENDER_MODE_MESH, fragment);

//		transform.position.x = 1;
//		transform.position.y = 0.5;
//		RenderModel(model, 36, transform, RENDER_MODE_MESH, fragment); // We can use the same mesh and just change the transform(similar to OpenGL)

		/* Of course don't forget to reset the transform */
//		transform.position.x = 0;
//		transform.position.y = 0;

		start = end;
		DrawRenderImage(image, 0, 0);
		/* You need to manually clear the depth buffer as cr doesn't know when the render step finished(unlike in OpenGL where the buffer swap function would indicate when to clear it) */
		CRClearDepthBuffer();
	}

	/* Technically isn't needed but I like to train myself because I know my dumbass would otherwise constantly cause memory leaks(which I probably still do anyways) */
	freeRenderImage(image);
	CleanupCR();
	CloseCRWindow();

	return 0;
}
