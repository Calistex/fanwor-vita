/* *** Prototypes for loadimg.c *** */

#include <vdi.h>

long LoadImg(char *Filename, MFDB *raster);
int getximgpal(char *filename, short pal[][3]);
int transform_truecolor( MFDB *image, long size, int planes, int img_handle);
