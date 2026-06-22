/*
 * SMUSHv1 RE
 *
 * msc - misc.
 */

#include "smush.h"

int16_t mem_blk_alloc(void **mem_out, uint32_t size)
{
	void *m = malloc(size);
	*mem_out = m;
	if (!m) {
		sprintf(AG(anm_errstr), "ERROR: Cannot allocate buffer of %d\n", size);
		return -1;
	}
	return 0;
}

void mem_blk_free(void **mem)
{
	if (mem && *mem) {
		free(*mem);
		*mem = NULL;
	}
}

void msc_draw_rect(uint8_t *dst, int16_t xoff, int16_t yoff, uint16_t width,
		   uint16_t height, uint8_t color, uint16_t stride, uint16_t maxy)
{
	int skiptop, skipbot, skipleft, skipright;
	skiptop = skipleft = skipbot = skipright = 0;

	if (yoff < 0) {
		height += yoff;
		yoff = 0;
		skiptop = 1;
	}
	if (xoff < 0) {
		width += xoff;
		xoff = 0;
		skipleft = 1;
	}
	if (maxy <= (yoff + height)) {
		height = maxy -1 - yoff;
		skipbot = 1;
	}
	if (stride < (xoff + width)) {
		width = stride - 1 -xoff;
		skipright = 1;
	}
	if ((width < 1) || (height < 1))
		return;

	dst += xoff + (yoff * stride);
	if (skiptop == 0) {
		uint8_t *d = dst;
		for (int i = 0; i < width; i++)
			*d++ = color;
	}
	if (skipbot == 0) {
		uint8_t *d = dst + ((height - 1) * stride);
		for (int i = 0; i < width; i++)
			*d++ = color;
	}
	if (skipleft == 0) {
		uint8_t *d = dst;
		for (int i = 0; i < height; i++, d += stride)
			*d = color;
	}
	if (skipright == 0) {
		uint8_t *d = dst + width;
		for (int i = 0; i < height; i++, d += stride)
			*d = color;
	}
}

void msc_fill_rect(uint8_t *dst, int16_t xoff, int16_t yoff, uint16_t width,
		   uint16_t height, uint8_t color, uint16_t stride)
{
	dst += xoff + (yoff * stride);
	while (height--) {
		msc_memset(dst, color, width);
		dst += stride;
	}
}

uint16_t msc_random(uint16_t p)
{
	anm_globals.msc_random_runval = (anm_globals.msc_random_runval * 9) + 0x35;
	return (anm_globals.msc_random_runval * p) >> 8;
}

void msc_find_contrast_alt(uint16_t *brightest_index,uint16_t *darkest_index,uint8_t *pal)
{
	int i, j, k, l;

	i = 0;
	j = 1000;
	for (k = 0; k < 256; k++) {
		l = pal[0] + pal[1] + pal[2];
		pal += 3;
		if (k && (i < l)) {
			*darkest_index = k;
			i = l;
		}
		if (l < j) {
			*brightest_index = k;
			j = l;
		}
	}
}

int32_t msc_color_distance(int16_t color1,int16_t color2,uint8_t *pal)
{
	int i, j, k;
	i = pal[color1 * 3 + 0] + pal[color2 * 3 + 0];
	j = pal[color1 * 3 + 1] + pal[color2 * 3 + 1];
	k = pal[color1 * 3 + 2] + pal[color2 * 3 + 2];
	if (i < 0)
		i = -i;
	if (j < 0)
		j = -j;
	if (k < 0)
		k= -k;
	return (i*i) + (j*j) + (k*k);
}

void msc_memcpy(void *dst, void *src, uint32_t cnt)
{
	uint8_t *d = dst, *s = src;
	while (cnt--)
		*d++ = *s++;
}

void msc_memset(void *dst, uint8_t val, uint32_t cnt)
{
	uint8_t *d = dst;
	while (cnt--)
		*d++ = val;
}
