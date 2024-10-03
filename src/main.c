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
		.position = { .x = 0, .y = 0, .z = 3},
		.rotation = { 0 },
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
		float elapsed = (float)(end-start)/CLOCKS_PER_SEC;
		transform.rotation.y += elapsed*50;

		RenderModel(model, 36, transform, RENDER_MODE_MESH);

		transform.position.x = 1;
		transform.position.y = 0.5;
		RenderModel(model, 36, transform, RENDER_MODE_MESH); // We can use the same mesh and just change the transform(similar to OpenGL)

		/* Of course don't forget to reset the transform */
		transform.position.x = 0;
		transform.position.y = 0;

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
