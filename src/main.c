#include <cr.h>
#include <crWindowing.h>
#include <crImaging.h>
#include <time.h>
#include <string.h>
#include <stdio.h>

static const int screenWidth = 800;
static const int screenHeight = 600;

vertex model[] = {
	/* FRONT FACE */
	{ .position = { .x = -0.5, .y = -0.5, .z = -0.5, .w = 1} },
	{ .position = { .x =  0.5, .y =  0.5, .z = -0.5, .w = 1} },
	{ .position = { .x =  0.5, .y = -0.5, .z = -0.5, .w = 1} },

	{ .position = { .x = -0.5, .y = -0.5, .z = -0.5, .w = 1} },
	{ .position = { .x = -0.5, .y =  0.5, .z = -0.5, .w = 1} },
	{ .position = { .x =  0.5, .y =  0.5, .z = -0.5, .w = 1} },
	/* LEFT FACE */
	{ .position = { .x = -0.5, .y = -0.5, .z = -0.5, .w = 1} },
	{ .position = { .x = -0.5, .y = -0.5, .z =  0.5, .w = 1} },
	{ .position = { .x = -0.5, .y =  0.5, .z =  0.5, .w = 1} },

	{ .position = { .x = -0.5, .y = -0.5, .z = -0.5, .w = 1} },
	{ .position = { .x = -0.5, .y =  0.5, .z =  0.5, .w = 1} },
	{ .position = { .x = -0.5, .y =  0.5, .z = -0.5, .w = 1} },
	/* RIGHT FACE */
	{ .position = { .x = 0.5, .y = -0.5, .z = -0.5, .w = 1} },
	{ .position = { .x = 0.5, .y =  0.5, .z =  0.5, .w = 1} },
	{ .position = { .x = 0.5, .y = -0.5, .z =  0.5, .w = 1} },

	{ .position = { .x = 0.5, .y = -0.5, .z = -0.5, .w = 1} },
	{ .position = { .x = 0.5, .y =  0.5, .z = -0.5, .w = 1} },
	{ .position = { .x = 0.5, .y =  0.5, .z =  0.5, .w = 1} },
	/* BOTTOM FACE */
	{ .position = { .x = -0.5, .y = -0.5, .z = -0.5, .w = 1} },
	{ .position = { .x =  0.5, .y = -0.5, .z = -0.5, .w = 1} },
	{ .position = { .x =  0.5, .y = -0.5, .z =  0.5, .w = 1} },

	{ .position = { .x = -0.5, .y = -0.5, .z = -0.5, .w = 1} },
	{ .position = { .x =  0.5, .y = -0.5, .z =  0.5, .w = 1} },
	{ .position = { .x = -0.5, .y = -0.5, .z =  0.5, .w = 1} },
	/* TOP FACE */
	{ .position = { .x = -0.5, .y = 0.5, .z = -0.5, .w = 1} },
	{ .position = { .x =  0.5, .y = 0.5, .z =  0.5, .w = 1} },
	{ .position = { .x =  0.5, .y = 0.5, .z = -0.5, .w = 1} },

	{ .position = { .x = -0.5, .y = 0.5, .z = -0.5, .w = 1} },
	{ .position = { .x = -0.5, .y = 0.5, .z =  0.5, .w = 1} },
	{ .position = { .x =  0.5, .y = 0.5, .z =  0.5, .w = 1} },
	/* BACK FACE */
	{ .position = { .x = -0.5, .y = -0.5, .z = 0.5, .w = 1} },
	{ .position = { .x =  0.5, .y = -0.5, .z = 0.5, .w = 1} },
	{ .position = { .x =  0.5, .y =  0.5, .z = 0.5, .w = 1} },

	{ .position = { .x = -0.5, .y = -0.5, .z = 0.5, .w = 1} },
	{ .position = { .x =  0.5, .y =  0.5, .z = 0.5, .w = 1} },
	{ .position = { .x = -0.5, .y =  0.5, .z = 0.5, .w = 1} },
};

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

int main(int argc, char **argv)
{
	clock_t start, end;

	if(!InitCRWindow())
		return 0;

	if(!InitCR(screenWidth, screenHeight, 90, 0.1, 1000))
	{
#if defined(__WIN32)
		MessageBox(NULL, "Failed to initialize cRender lib!", "InitCR() error", MB_OK | MB_ICONEXCLAMATION);
#elif defined(__unix__)
		printf("Failed to initialize cRender lib!\n");
#endif
		return 1;
	}

	CRImage image = CreateRenderImage(screenWidth, screenHeight);
	if(!image.data)
	{
		printf("Failed to create render image!\n");
		return 2;
	}
	SetRenderDestination(image.data);
	start = clock();
	crTransform transform = {
		.position = { .x = 0, .y = 0, .z = 9},
		.rotation = { 0 },
		.scale = { .x = 1, .y = 1, .z = 1 }
	};

	while(!ShouldCRWindowClose())
	{
		memset(image.data, 0, (screenWidth * screenHeight)<<2);
		end = clock();
		if(end-start < CLOCKS_PER_SEC * (1.0/30.0)) continue;
		float elapsed = (float)(end-start)/CLOCKS_PER_SEC;
		transform.rotation.x += elapsed*50;
		transform.rotation.y += elapsed*50;
		transform.rotation.z += elapsed*50;
		RenderModel(model, 36, transform, RENDER_MODE_MESH);
		start = end;
		DrawRenderImage(image, 0, 0);
	}

	freeRenderImage(image);
	CloseCRWindow();

	return 0;
}
