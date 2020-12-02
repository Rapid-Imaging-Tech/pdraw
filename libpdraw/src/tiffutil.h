// tiffutil.h

#define TIFF_RGBA 0
#define TIFF_MAPPED 1

struct TIFF_IMAGE_BUF {
	int nx;
	int ny;
	unsigned char *image;
	int *r;
	int *g;
	int *b;
	int type;
};

typedef struct TIFF_IMAGE_BUF tiff_image_buf;

#ifndef ISCSOURCE
extern "C" {
	int  loadTiffImage(TIFF *tif, tiff_image_buf *buffer);
	void freeTiffBuffer(tiff_image_buf *buffer);
}
#endif
