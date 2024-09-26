#ifndef CR_IMAGING_H_
#define CR_IMAGING_H_

typedef struct
{
#if defined(__WIN32)
	HBITMAP hBmp;
#elif defined(__unix__)
	XImage *xImg;
#endif
	unsigned int width, height;
	unsigned char *data;
}CRImage;

CRImage CreateRenderImage(unsigned int width, unsigned int height);
void DrawRenderImage(CRImage image, unsigned int x, unsigned int y);
void freeRenderImage(CRImage image);

#endif
