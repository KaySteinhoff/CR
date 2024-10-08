# CR

A CPU based 3D renderer. Can be compiled on both Linux with X11(and Wayland I think?) and Windows(see compile instructions).

# How to use

Although this repo contains a crWindowing.* and crImaging.* file only the cr.h and cr.c files are needed.

First the CR lib has to be initialized:
```C
#include <stdio.h>
#include <cr.h>

int main(void)
{
	if(!InitCR(800, 600, CR_RGBA, 90, 0.1, 1000.0))
	{
		printf("Failed to initialize CR lib!\n"); // It's recommended to use MessageBox on Windows(idk about Mac)
		return -1;
	}

	printf("Initialized CR lib!\n");
	CleanupCR();

	return 0;
}
```

After initialization the render destination must be set. This destination has to have either 24 bit depth(CR_RGB) or 32 bit depth(CR_RGBA) depending on what was specified during initialization.<br>
Depending on your window/imaging library it is possible you have to retreive the pointer from the image creation function(Win32) or you have to pass a allocated one in(X11). For both cases you can have a look at the crImaging.c file.
```C
#include <stdio.h>
#include <cr.h>

int main(void)
{
	unsigned char renderTarget[32*32*4] = { 0 }; // In this example the render target is just a raw unsigned char pointer

	if(!InitCR(800, 600, CR_RGBA, 90, 0.1, 1000.0))
	{
		printf("Failed to initialize CR lib!\n"); // It's recommended to use MessageBox on Windows(idk about Mac)
		return -1;
	}

	printf("Initialized CR lib!\n");

	SetRenderDestination(renderTarget);

	CleanupCR();

	return 0;
}
```

After successfully initializing and setting the destination a pointer/array to some vertex struct has to be created.<br>
Currently a vertex contains a position(vec3) and the uv coordinates(vec2) but in the future it might hold more data.<br>

This mesh data is then passed to the RenderModel function along with the number of elements, a crTransform struct containing information of the position, rotation and scalation of the mesh, the render mode and a function pointer to the fragment shading function.<br>
Currently the only available render mode is RENDER_MODE_MESH as RENDER_MODE_TRIANGLE_STRIP was broken with the introduction of calculated normals(working on getting it running again).<br>
```C
#include <stdio.h>
#include <cr.h>

size_t texture[9] = {
	0xffff0000, 0xff00ff00, 0xff0000ff, 
	0xff00ff00, 0xff0000ff, 0xffff0000, 
	0xff0000ff, 0xffff0000, 0xff00ff00, 
};

size_t fragmentShader(int x, int y, float u, float v, float w)
{
	int tx = 3*u;
	int ty = 2*v;
	return texture[tx+ty*3];
}

int main(void)
{
	unsigned char renderTarget[32*32*4] = { 0 }; // In this example the render target is just a raw unsigned char pointer
	vertex triangle[3] = {
		{ .position={ .x = -0.5, .y = -0.5, .z = 0, .w = 1}, .uv = { .x = 0.0, .y = 0.0 } }, // the positions w always has to be set to 1 initialy
		{ .position={ .x =  0.0, .y =  0.5, .z = 0, .w = 1}, .uv = { .x = 0.5, .y = 1.0 } },
		{ .position={ .x =  0.5, .y = -0.5, .z = 0, .w = 1}, .uv = { .x = 1.0, .y = 0.0 } },
	};
	crTransform triTransform = {
		.position = { .x = 0, .y = 0, .z = 3},
		.rotation = { 0 },
		.scale = { .x = 1, .y = 1, .z = 1},
	};

	if(!InitCR(800, 600, CR_RGBA, 90, 0.1, 1000.0))
	{
		printf("Failed to initialize CR lib!\n"); // It's recommended to use MessageBox on Windows(idk about Mac)
		return -1;
	}

	printf("Initialized CR lib!\n");

	/* Set up a window, create the image... */

	SetRenderDestination(renderTarget);

	while(windowOpen)
	{
		/* render loop */

		triTransform.rotation.y += elapsedTime;
		RenderModel(triangle, 3, triTransform, RENDER_MODE_MESH, fragmentShader);

		/* Display image... */
		CRClearDepthBuffer(); // This doesn't have to be called after displaying the image but should be called before the next render loop, or at the start
	}

	CleanupCR();

	return 0;
}
```

And that's it! Have fun with my barely functioning render lib!

__**INFO**__

Triangle strip are now supported again but need to have back face culling turned of. To see how to do this please have a look at the documentation <a href="https://github.com/KaySteinhoff/CR/wiki/Documentation">here</a>.

# Compile instructions(for gcc)

For Linux(in this folder setup):
```bash
gcc src/*.c -I inc -lX11 -lm -Wall -Werror -o cr.out
```

For Windows(in this folder setup):
```bash
gcc src/*.c -I inc -mwindows -Wall -Werror -o cr.exe
```

If you want to enable debug view you can define a DEBUG flag:
```bash
gcc src/*.c -DDEBUG -I inc -lX11 -lm -Wall -Werror -o cr.out
```
