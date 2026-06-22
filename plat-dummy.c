#include "smush.h"

#ifdef _SMUSH_PLAT_
#error Only one platform can be set, it is ## _SMUSH_PLAT_
#else
#define _SMUSH_PLAT_ _SMUSH_PLAT_DUMMY_
#endif


/****************** VIDEO **************************/

void vid_video_init(void)
{
	printf("vid_video_init\n");
}

void vid_video_terminate(void)
{
	printf("vid_video_terminate\n");
}

void vid_palette_set(uint8_t *pal)
{
	printf("vid_palette_set: 0x%p\n", pal);
}

void vid_palette_clear(void)
{
	printf("vid_palette_clear\n");
}

void VID_PLATFORM_PAL_SET_ENTRY(uint16_t coloridx, uint8_t r, uint8_t g, uint8_t b)
{
	printf("VID_PLATFORM_PAL_SET_ENTRY: %d -> %d/%d/%d\n", coloridx, r, g, b);
}

void VID_PLATFORM_BLIT_DELTA(uint8_t *srcbuf, uint16_t sxoff, uint16_t syoff,
			    uint16_t dxoff, uint16_t dyoff, uint16_t width, uint16_t height,
			    uint16_t srcstride, uint8_t *refbuf)
{
	printf("VID_PLATFORM_DELTABLIT: %p/%p  (%d/%d)->(%d/%d), w/h %d/%d  s %d\n",
	       srcbuf, refbuf, sxoff, syoff, dxoff, dyoff, width, height, srcstride);
}

void VID_PLATFORM_BLIT_RAW(uint8_t *srcbuf, uint16_t sxoff, uint16_t syoff,
			   uint16_t dxoff, uint16_t dyoff, uint16_t width, uint16_t height,
			   uint16_t srcstride, uint8_t *refbuf)
{
	printf("VID_PLATFORM_BLIT_RAW: %p/%p  (%d/%d)->(%d/%d), w/h %d/%d  s %d\n",
	       srcbuf, refbuf, sxoff, syoff, dxoff, dyoff, width, height, srcstride);
}

void VID_PLATFORM_CLEAR_LINES(uint16_t linecnt, uint16_t startline)
{
	printf("VID_PLATFORM_CLEAR_LINES: %d->%d (%d)\n", startline, startline + linecnt, linecnt);
}

void vid_process_fade(uint8_t *srcimg, uint8_t *maskdata, uint16_t size)
{
	printf("vid_process_fade: 0x%p, 0x%p, %d\n", srcimg, maskdata, size);
}

void sys_timer_init(int cfg_timerrate)
{

}

void sys_timer_terminate(void)
{
	sys_timer_continue_generic();
}

void sys_timer_continue(void)
{
	sys_timer_continue_generic();
}
