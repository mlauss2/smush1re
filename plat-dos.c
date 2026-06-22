/*
 * SMUSH v1 RE
 *
 * the DOS parts from the original EXE.
 *
 * XXX: use Watcom-C for this please.
 */

#ifndef __WATCOMC__
#error This source file requires the Open Watcom C/C++ compiler.
#else

#include <stdio.h>
#include <conio.h>
#include <dos.h>
#include <hwio.h>
#include <i86.h>
#include "smush.h"

uint16_t RA1DOS_app_DS;		/* DS segment of the application */


void __interrupt __far RA1DOS_int24h_handler_func(void);			/* 00263001 */
#pragma aux RA1DOS_int24h_handler_func =	\
	"inc byte ptr [di]"			\
	"add byte ptr [eax], dh"		\
	"add byte ptr es:[ebp+3], dh"		\
	"mov al, 0"				\
	"iret"					\
	__parm __caller []			\
	__modify [eax edi]

void __interrupt __far RA1DOS_PIC_cascade_handler_func(void);			/* 00263010 */
#pragma aux RA1DOS_PIC_cascade_handler_func =	\
	"int 0x1b"				\
	"iret"					\
	__parm __caller [] __modify[]


uint32_t sys_timer_tick;							/* 002b23c8 */
void (__interrupt __far)(RA1DOS_int23h_old_handler)(void);			/* 002b247e (6 bytes) */
void (__interrupt __far)(RA1DOS_int24h_old_handler)(void);			/* 002b2488 (8 bytes) */
uint16_t sys_x86_int8_tickthres;						/* 002b77ee */
void (__interrupt __far)(RA1DOS_int8_old_handler)(void);			/* 002b7858 (6 bytes) */


void __interrupt __far RA1DOS_int23_handler_func(void)				/* 002a06a5 */
{
	/* does nothing, really! */
}

void RA1DOS_set_int24h_handler(void __far __interrupt handlerfn)
{
	RA1DOS_int24h_old_handler = _dos_getvect(0x24);
	_dos_setvect(0x24, RA1DOS_int24h_handler_func);
{

void __interrupt __far sys_timer_handler(union REGS regs)
{
	/* restore DS to access global variables from IRQ context */
	_asm { mov ds, RA1DOS_app_DS }
	outp(0x20, 0x20);
	if (++sys_timer_tick >= sys_x86_int8_tickthres) {
		RA1DOS_int8_old_handler();
		sys_timer_tick = 0;
	}
	sys_timer_handler_generic();
}

void sys_timer_continue(void)
{
	_asm { push gs }
	sys_timer_continue_generic();
	_asm { pop gs }
}

void sys_timer_init(int cfg_timerrate)
{
	uint16_t pitdiv;

	sys_x86_int8_tickthres = cfg_timerrate / 12;
	RA1DOS_int8_old_handler = _dos_getvect(8);
	_dos_setvect(8, sys_timer_handler);
	pitdiv = 0x1234dc / cfg_timerrate;
	outp(0x20, 0xc0);
	_asm { cli }
	outp(0x43, 0x36);
	outp(0x40, pitdiv & 0xff);
	outp(0x40, (pitdiv >> 8) & 0xff);
	_asm { sti }
}

void sys_timer_terminate(void)
{
	_asm { cli }
	outp(0x43, 0x36);
	outp(0x40, 0);
	outp(0x40, 0);
	_asm { sti }
	outp(0x20, 0xc7);
	_dos_setvect(8, RA1DOS_int8_old_handler);
}

/* some sort of debug hook I assume */
int16_t _stub_return_0(void)							/* 002a0d01 */
{
	return 0;
}

int game_init_platform(int argc, char **argv)
{
	int i = 1;
	int32_t sndio, snddma, sndirq, sndid, sndbufsize, cddrvusage;
	int16_t timerrate, maxfps, ret;

	if (_stub_return_0() == 0) {
		RA1DOS_set_int24h_handler();
		while (1) {
			if (argc <= i) {
				RA1DOS_int23h_old_handler = _dos_getvect(0x23);
				_dos_setvect(0x23, RA1DOS_int23h_handler_func);
				return 0;

			if (argv[i][0] == '?' || argv[i][1] == '?')
				break;
			if (argv[i][0] == '/') {
				if (tolower(argv[i][1]) == 'm')
					ctl_mou_enable();
				else if (tolower(argv[i][1]) == 'j')
					ctl_joy_enable();
				else if (tolower(argv[i][1]) == 'x')
					sscanf(argv[i] + 2, "%"PRIi16, &(AG(game_param_gfxquality)));
				else if (tolower(argv[i][1]) == 'f')
					sscanf(argv[i] + 2, "%"PRIi16, &maxfps);
				else if (tolower(argv[i][1]) == 't')
					sscanf(argv[i] + 2, "%"PRIi16, &timerrate);
				else if (tolower(argv[i][1]) == 'u')
					sscanf(argv[i] + 2, "%"PRIi32, &cddrvusage);
				else if (tolower(argv[i][1]) == 'b')
					sscanf(argv[i] + 2, "%"PRIx32, &sndio);
				else if (tolower(argv[i][1]) == 'i')
					sscanf(argv[i] + 2, "%"PRIi32, &sndirq);
				else if (tolower(argv[i][1]) == 's')
					sscanf(argv[i] + 2, "%"PRIi32, &sndbufsize);
				else if (tolower(argv[i][1]) == 'd')
					sscanf(argv[i] + 2, "%"PRIi32, &snddma);
				else if (tolower(argv[i][1]) == 'c') {
					sscanf(argv[i] + 2, "%"PRIi32, &sndid);
					if (sndid == 0) {
						AG(anm_text_force_enable) = 1;
					}
				} else if (tolower(argv[i][1]) == 'z')
					game_cfg_read_rebltune_txt();
				else if (tolower(argv[i][1]) == 'y')
					game_cfg_write_rebltune_txt();



				game_cfg_set_sndcard_params(sndio, sndirq, sndbufsize,
							    sndid, snddma);
				game_cfg_set_perf_params(maxfps, timerrate, cddrvusage);

			}
			i++;
		}
		printf("Usage: %s <parameters>\n", argv[0]);
		printf("Where valid parameters are:\n");
		printf("...\n");

		fflush(stdout);
		while (kbhit())
			;
		while (kbhit() != 0)
			getch();

		printf("                            \n");
		printf("      /c<Card> - Set sound card type where:\n\n");
		printf("               0  = No sound\n");
		/* ... */
		fflush(stdout);
		while (kbhit())
			;
			while (kbhit() != 0)
				getch();

		printf("                            \n");
		printf("    Advanced tuning options:\n\n");
		printf("      /u<Pct > - Set cd driver cpu usage, where <Pct> is:\n\n");
		/* .. */
	} else {
		printf("%s\n", AG(anm_errstr));
	}

	exit(1);
}


static void vid_vga_test_palette_speed(void)				/* 002a10bc */
{
	// TODO
	AG(vid_vga_pal_is_fast) = 1;
}

void vid_video_init(void)
{
	union REGS regs;
	struct SREGS segs;

	printf("vid_video_init\n");
	regs.w.ax = 0x13;	/* 320x200x8 linear */
	segread(&segs);
	int386x(0x10, &regs, &regs, &segs);
	vid_palette_set_brightness(0);
	vid_vga_test_palette_speed(void);
}

void vid_video_terminate(void)
{
	union REGS regs;
	struct SREGS segs;

	printf("vid_video_terminate\n");
	regs.w.ax = 3;		/* text 80x25 */
	segread(&segs);
	int386x(0x10, &regs, &regs, &segs);
}

void vid_palette_set_raw(uint8_t *temppal)
{
	int i;

	printf("vid_palette_set_raw: 0x%p\n", temppal);
	i = 768;
	outp(0x3c8, 0);
	while (1) {
		if (0 != (inp(0x3da) & 8))
			break;
		if (AG(sys_abort_flag) != 0) {
			sys_timer_continue();
		}
	}

	while (i--)
		outp(0x3c9, *temppal++);
}

void vid_palette_setcolor(uint16_t coloridx, uint8_t r, uint8_t g, uint8_t b)
{
	outp(0x3c8, coloridx);
	outp(0x3c9, r);
	outp(0x3c9, g);
	outp(0x3c9, b);
}

void vid_palette_clear(void)
{
	int i;

	printf("vid_palette_clear\n");
	outp(0x3c8, 0);
	for (i = 0; i < 768; ++)
		outp(0x3c9, 0);
}

void vid_hw_clear_lines(uint16_t linecnt, uint16_t startline)						/* 0029066d */
{
	int lc = linecnt;
	uint8_t *sa;

	printf("vid_clear_screen_lines: %d->%d (%d)\n", startline, startline + linecnt, linecnt);
	if (linecnt < 1 || startline > 319)
		return;
	if (startline + linecnt > 320)
		lc = 320 - startline;
	sa = (uint8_t *)0x000a0000 + (startline * 320);
	memset(sa, 0, lc * 320);
}

/* deltablit: comparing ram was much faster than writing 64kB to VGA memory in 1992,
 * so this here compares the 2 internal frame buffers and only writes changed regions
 * back to VGA.
 */
void vid_hw_blt_to_screen_delta(uint8_t *srcbuf, uint16_t sxoff, uint16_t syoff,
			       uint16_t dxoff, uint16_t dyoff, uint16_t width, uint16_t height,
			       uint16_t srcstride, uint8_t *refbuf)
{
	srcbuf += (srcyoff * srcstride) + srcxoff;
	refbuf += (srcyoff * srcstride) + srcxoff;

	/* BUG in the assembly?  EAX is reused from the srcbuf/refbuf calculation,
	 * and applied to VGA as well, ignoring the dxoff/dyoff parameters!
	 */
	uint8_t *dst = (uint8_t *)0xA0000 + (srcyoff * srcstride) + srcxoff;
	uint32_t srcp = srcstride - copywidth;
	uint32_t dstp = 320 - copywidth;

	while (height--) {

		uint32_t *s32 = (uint32_t *)srcbuf;
		uint32_t *r32 = (uint32_t *)refbuf;
		uint32_t *v32 = (uint32_t *)dst;

		/* engine does a clever REPE CMPSD to find changed dwords. */
		for (int x = 0; x < (copywidth / 4); x++) {
			if (s32[x] != r32[x]) {
				v32[x] = s32[x];
			}
		}
		refbuf += srcstride;
		srcbuf += srcstride
		dst += 320;

		if (AG(sys_abort_flag) == 0) {
			sys_timer_continue();
		}
	}
}


/* Platform: write a full buffer directly to screen.
 * for DOS: just blt the whole 64kb srcbuf to VGA memory at 0x000a0000.
 */
void vid_hw_blt_to_screen(uint8_t *srcbuf, uint16_t sxoff, uint16_t syoff,
			  uint16_t dxoff, uint16_t dyoff, uint16_t width, uint16_t height,
			  uint16_t srcstride, uint8_t *refbuf)
{
	uint8_t *dst = (uint8_t *)0xA0000 + (dyoff * 320) + dxoff;
	uint32_t srcp = srcstride - width;
	uint32_t dstp = 320 - width;
	uint8_t row_dwords = width >> 2;

	src += (syoff * srcstride) + sxoff;
	while (height--) {
		for (uint8_t x = 0; x < row_dwords; x++)
			*(uint32_t *)dst++ = *(uint32_t *)src++;

		src += width + srcp;
		dst += width + dstp;

		if (AG(sys_abort_flag) == 0) {
			sys_timer_continue();
		}
	}
}

/* directly blt 20 lines at line offset 178 */
void vid_hw_blt_to_statusbar(uint8_t *srcbuf, uint32_t val)
{
	uint8_t *dst = (uint8_t *)0x000ade80;
	memcpy(dst, srcbuf, 6400);
}


/* FADE: copy parts of srcimg based on a pattern in maskdata directly to the
 * screen buffer.
 */
void vid_process_fade(uint8_t *srcimg, uint8_t *maskdata, uint16_t size)
{
	vid_process_fade_core(srcimg, maskdata, size, (uint8_t *)(0xA0000));
}

/* DOS: runs in IRQ context! */
void __interrupt __far sou_engine_fill(union REGS regs)
{
	/* restore DS to access global variables from IRQ context */
	_asm { mov ds, RA1DOS_app_DS }

	outp(0x20, 0xb);
	if (inp(0x20) && AG(sou_x86_drv_pic_mastermask)) {

		sou_engine_fill_core();

		/* DOS tail: signal EOI to the PICs */
		if (7 < AG(sou_soudrv_irq))
			outp(0xa0, 0x20);
		outp(0x20, 0x20);
	}
}





#endif		 /* __WATCOM_C__ */
