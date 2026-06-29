/*
 * SMUSHv1 RE
 *
 * vid - video output related.
 *  platform-independent stuff.
 */

#include "smush.h"

void vid_palette_set(uint8_t *palnew)
{
	uint8_t *p = AG(vid_palette);
	uint8_t temppal[768];

	memcpy(p, palnew, 768);
	for (int i = 0; i < 768; i++) {
		temppal[i] = AG(vid_palette_lookup[palnew[i]]) / 4;
	}
	vid_palette_set_raw(temppal);
}

void vid_palette_set_intensity(uint8_t *unused_pal, int16_t intensity)
{
	uint8_t *p = AG(vid_palette);
	uint8_t temppal[768];

	for (int i = 0; i < 768; i++) {
		temppal[i] = (intensity * AG(vid_palette_lookup[i])) / 16;
	}
	vid_palette_set_raw(temppal);
}

void vid_palette_write_lookup_idx(uint16_t coloridx, uint8_t r, uint8_t g, uint8_t b)
{
	/* r/g/b are indices into the 256-entry vid_palette_lookup table, write
	 * these entries to the VGA DAC.
	 * Unused in the original, but present in the code.
	 */
	r = AG(vid_palette_lookup[r]) / 4;
	g = AG(vid_palette_lookup[g]) / 4;
	b = AG(vid_palette_lookup[b]) / 4;
	vid_palette_setcolor(coloridx, r, g, b);
}

void vid_palette_set_brightness(int16_t delta)
{
	int b = AG(vid_pal_brightness);
	uint8_t *p = AG(vid_palette);
	uint8_t temppal[768];

	if ((-1 < b + delta) && (b + delta < 0x11))
		b += delta;

	for (int i = 0; i < 0x100; i++)
		AG(vid_palette_lookup[i]) = ((((i - ((i*i)/255)) * b) / 16) + i) & 0xff;

	if (delta) {
		for (int i = 0; i < 768; i++) {
			temppal[i] = AG(vid_palette_lookup[p[i]]) / 4;
		}
		/* the original assembly now writes the 768 values to the VGA DAC,
		 * need to abstract a bit here.
		 */
		vid_palette_set_raw(temppal);
	}
}

void vid_video_disable_deltablt(void)
{
	AG(vid_no_deltablit_to_screen) = 1;
}

int16_t vid_bitmap_alloc(uint8_t **dstout)
{
	int ret;

	AG(vid_vidbuf_curridx) = 0;
	ret = mem_blk_alloc((void **)&(AG(vid_vidbuf_base)[0]), 320*200);
	if (ret)
		return -1;
	ret = mem_blk_alloc((void **)&(AG(vid_vidbuf_base)[1]), 320*200);
	if (ret) {
		mem_blk_free((void **)&(AG(vid_vidbuf_base)[0]));
		return -1;
	}
	msc_memset(AG(vid_vidbuf_base[0]), 0, 320*200);
	msc_memset(AG(vid_vidbuf_base[1]), 0, 320*200);
	*dstout = AG(vid_vidbuf_base)[AG(vid_vidbuf_curridx)];
	return 0;
}

void vid_bitmap_free(uint8_t **base)
{
	mem_blk_free((void **)&(AG(vid_vidbuf_base)[0]));
	mem_blk_free((void **)&(AG(vid_vidbuf_base)[1]));
	*base = NULL;
}

void vid_bitmap_clear(uint8_t *dst)
{
	/* the original EXE has this unrolled 5 times, with explicit 32bit stores */
	for (int i = 0; i < 5; i++) {
		if (AG(sys_abort_flag) != 0) {
			sys_timer_continue();
		}
		memset(dst, 0, 64000 / 5);
		dst += 64000 / 5;
	}
}

void vid_blt_to_screen(uint8_t *srcbuf, uint16_t sxoff, uint16_t syoff,
		       uint16_t dxoff, uint16_t dyoff, uint16_t width, uint16_t height,
		       uint16_t srcstride)
{
	vid_statusbar_clear_vidbufarea();
	if ((AG(vid_vga_blt_last_copyheight) < height)
		|| (AG(vid_no_deltablit_to_screen) != 0)
		|| (AG(vid_vidbuf_base[1 - AG(vid_vidbuf_curridx)]) == NULL)) {
		vid_blt_to_screen_raw(srcbuf, sxoff, syoff, dxoff, dyoff, width, height, srcstride);
		AG(vid_no_deltablit_to_screen) = 0;
	} else {
		vid_hw_blt_to_screen_delta(srcbuf, sxoff, syoff, dxoff, dyoff, width,
					  height, srcstride,
					  AG(vid_vidbuf_base[1 - AG(vid_vidbuf_curridx)]));
	}
	AG(vid_vga_blt_last_copyheight) = height;
}

void vid_buffer_flip(uint8_t **dstout)
{
	AG(vid_vidbuf_curridx) ^= 1;
	if (AG(vid_vidbuf_base)[AG(vid_vidbuf_curridx)] != NULL)
		*dstout = AG(vid_vidbuf_base)[AG(vid_vidbuf_curridx)];
}

int16_t vid_statusbar_alloc(void **statusbar)
{
	return mem_blk_alloc(statusbar, 6400);
}

void vid_statusbar_free(void **statusbar)
{
	mem_blk_free(statusbar);
}

void vid_blt_to_screen_raw(uint8_t *srcbuf, uint16_t sxoff, uint16_t syoff,
			   uint16_t dxoff, uint16_t dyoff, uint16_t width,
			   uint16_t height, uint16_t srcstride)
{
	vid_hw_blt_to_screen(srcbuf, sxoff, syoff, dxoff, dyoff, width, height,
			      srcstride, AG(vid_vidbuf_base[1 - AG(vid_vidbuf_curridx)]));
}

void vid_statusbar_clear_buf(uint8_t *statusbar)
{
	msc_memset(statusbar, 0, 6400);
}

void vid_statusbar_blt(uint8_t *bmp)
{
	vid_hw_blt_to_statusbar(bmp, 20);	/* "20" is unused */
}

void vid_statusbar_set_changed(void)
{
	AG(vid_statusbar_changed_flag_now) = 1;
}

void vid_statusbar_clear_vidbufarea(void)
{
	game_set_statusbar_changed(AG(vid_statusbar_changed_flag_now));
	if ((AG(vid_statusbar_changed_flag_previous) != 0) && (AG(vid_statusbar_changed_flag_now) == 0)) {
		vid_clear_lines(0x14, 0xb2);
		if (AG(vid_vidbuf_base[1 - AG(vid_vidbuf_curridx)]) != NULL) {
			msc_memset(AG(vid_vidbuf_base[1 - AG(vid_vidbuf_curridx)]) + 0xde80, 0, 0x1900);
		}
	}
	AG(vid_statusbar_changed_flag_previous) = AG(vid_statusbar_changed_flag_now);
	AG(vid_statusbar_changed_flag_now) = 0;
}

void vid_process_fade_core(uint8_t *src, uint8_t *mask, uint16_t masksize, uint8_t *dst)	/* 002905bb */
{
	uint8_t c;
	int s;

	while (masksize > 0) {
		if (AG(sys_abort_flag) == 0) {
			sys_timer_continue();
		}

		c = *mask++;
		masksize -= 1;
		if ((c & 0x80) != 0) {
			s = c & 0x7F;
			if (s == 0) {
				s = *(uint16_t *)mask;
				mask += 2;
				masksize -= 2;
			}
			dst += s;
			src += s;

		} else {
			if (c == 0x00) {
				s = *(uint16_t *)mask;
				mask += 2;
				memcpy(dst, src, s);
				dst += s;
				src += s;
				masksize -= 2;
			} else {
				memcpy(dst, src, c);
				dst += c;
				src += c;
			}
		}
	}
}

void vid_clear_lines(uint16_t linecount,uint16_t startline)
{
	vid_hw_clear_lines(linecount, startline);
}
