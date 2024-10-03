#include <stdio.h>
#include <string.h>
#include <time.h>
#include <cr.h>
#include <crWindowing.h>
#include <crImaging.h>

int main(void)
{
	vertex triangle[3] = {
		{ .position={ .x = -0.5, .y = -0.5, .z = 0, .w = 1} }, // w always has to be set to 1 initialy
		{ .position={ .x =  0.0, .y =  0.5, .z = 0, .w = 1} },
		{ .position={ .x =  0.5, .y = -0.5, .z = 0, .w = 1} },
	};
	crTransform triTransform = {
		.position = { .x = 0, .y = 0, .z = 3},
		.rotation = { 0 },
		.scale = { .x = 1, .y = 1, .z = 1},
	};

	clock_t start = clock(), end;

	if(!InitCR(800, 600, CR_RGBA, 90, 0.1, 1000.0))
	{
		printf("Failed to initialize CR lib!\n"); // It's recommended to use MessageBox on Windows(idk about Mac)
		return -1;
	}

	printf("Initialized CR lib!\n");

	/* Set up a window, create the image... */
	InitCRWindow();
	CRImage image = CreateRenderImage(800, 600);

	SetRenderDestination(image.data);

	while(!ShouldCRWindowClose())
	{
		end = clock();
		if(end-start < CLOCKS_PER_SEC*(1.0/30.0)) continue;
		memset(image.data, 0, 800*600*4);
		/* render loop */

		triTransform.position.z -= (double)(end-start)/CLOCKS_PER_SEC;
		RenderModel(triangle, 3, triTransform, RENDER_MODE_MESH);

		start = clock();
		/* Display image... */
		DrawRenderImage(image, 0, 0);
		CRClearDepthBuffer(); // This doesn't have to be called after displaying the image but should be called before the next render loop, or at the start
	}

	CleanupCR();

	return 0;
}
