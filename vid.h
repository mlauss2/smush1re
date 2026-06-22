/*
 * SMUSHv1 RE
 *
 * vid - video output related.
 */

#ifndef _SMUSH_VID_H_
#define _SMUSH_VID_H_

#include "smush.h"

/*
 * PLATFORM-SPECIFIC THINGS
 */

/* Platform: init a 320x200x8bpp video mode.
 * for DOS: set VGA mode 0x13.
 */
void vid_video_init(void);

/* Platform: shut down video mode.
 * for DOS: set 80x25 text mode.
 */
void vid_video_terminate(void);


/* Platform: set the palette in video hardware.
 * for DOS: write 768 bytes to VGA DAC Registers.
 */
void vid_palette_set(uint8_t *pal);

/* Platform: blank the palette to show black screen.
 * for DOS: write all zeroes to all 768 VGA DAC registers.
 */
void vid_palette_clear(void);

/* Platform: set a single palette entry.
 * for DOS: write coloridx to 0x3c8, and r/g/b to 0x3c9.
 */
void vid_palette_setcolor(uint16_t coloridx, uint8_t r, uint8_t g, uint8_t b);

/* Platform: do a deltablit to video memory.
 * for DOS: find the pixels that have changed in srcbuf compared to refbuf, and
 * only write those to VGA memry at 0x000a0000.  Probably intended for cards where
 * writing was slow?
 * All faster platforms should probably just write the whole srcbuf and not bother
 *  with the delta.
 */
void vid_hw_blt_to_screen_delta(uint8_t *srcbuf, uint16_t sxoff, uint16_t syoff,
				uint16_t dxoff, uint16_t dyoff, uint16_t width, uint16_t height,
				uint16_t srcstride, uint8_t *refbuf);					/* 0029048c */

/* Draw the @srcbuf to the bottom 20 lines of the 320x200 screen memory */
void vid_hw_blt_to_statusbar(uint8_t *srcbuf, uint32_t val);						/* 00290574 */


/* Platform: copy from @srcimg to screen directly using @maskdata.
 * implemented in vid_process_fade_core(), @pixelbuf is a pointer to the
 * memory area where the screen image is placed. 0x000A0000 (VGAMEM) for DOS,
 * or another linear surface.
 */
void vid_process_fade(uint8_t *srcimg, uint8_t *maskdata, uint16_t size);
void vid_process_fade_core(uint8_t *srcimg, uint8_t *maskdata, uint16_t size, uint8_t *pixelbuf);	/* 002905bb */

/* Platform: write a full buffer directly to screen.
 * for DOS: just blt the whole 64kb srcbuf to VGA memory at 0x000a0000.
 */
void vid_hw_blt_to_screen(uint8_t *srcbuf, uint16_t sxoff, uint16_t syoff,
			  uint16_t dxoff, uint16_t dyoff, uint16_t width, uint16_t height,
			  uint16_t srcstride, uint8_t *refbuf);						/* 002906a4 */


/* Platform: clear a number of screen lines directly.
 * for DOS: write zero to membase+(startline*320).
 */
void vid_hw_clear_lines(uint16_t linecnt, uint16_t startline);						/* 002a1580 */




/*
 * INTERNAL / PLAT-INDEPENDENT THINGS
 */

void	vid_palette_set_raw(uint8_t *pal);								/* 002a0d57 */
void	vid_palette_set_intensity(uint8_t *unused_pal, int16_t intensity);				/* 002a0e0f */
void	vid_palette_clear(void);									/* 002a0efc */
void	vid_palette_write_lookup_idx(uint16_t coloridx, uint8_t r, uint8_t g, uint8_t b);		/* 002a0f25 */
void	vid_palette_set_brightness(int16_t delta);							/* 002a0f96 */
void	vid_video_disable_deltablt(void);								/* 002a1083 */
int16_t vid_bitmap_alloc(uint8_t **dstout);								/* 002a11ca */
void	vid_bitmap_free(uint8_t **base);								/* 002a1259 */
void	vid_bitmap_clear(uint8_t *);									/* 002a1281 */
void	vid_blt_to_screen(uint8_t *srcbuf, uint16_t sxoff, uint16_t syoff,
			  uint16_t dxoff, uint16_t dyoff, uint16_t width, uint16_t height,
			  uint16_t srcstride);								/* 002a1291 */
void	vid_buffer_flip(uint8_t **);									/* 002a133f */

int16_t	vid_statusbar_alloc(void **statusbar);								/* 002a136d */
void	vid_statusbar_free(void **statusbar);								/* 002a1382 */
/* clear statusbar on screen directly */
void	vid_statusbar_clear_buf(uint8_t *statusbar);							/* 002a1392 */
void	vid_statusbar_blt(uint8_t * statusbar);								/* 002a13a2 */
void	vid_statusbar_set_changed(void);								/* 002a13b4 */
void	vid_statusbar_clear_vidbufarea(void);								/* 002a13bc */
void	vid_clear_lines(uint16_t linecount,uint16_t startline);						/* 002a1580 */
void	vid_blt_to_screen_raw(uint8_t *srcbuf, uint16_t sxoff, uint16_t syoff,
			      uint16_t dxoff, uint16_t dyoff, uint16_t width,
			      uint16_t height, uint16_t srcstride);					/* 002a159b */


#endif
