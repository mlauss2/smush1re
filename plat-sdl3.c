/*
 * SMUSH v1 RE
 *
 * platform-specific stuff reimplemented in SDL3.
 *
 */

#include "smush.h"
#include <ctype.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_audio.h>


SDL_Renderer *sren = NULL;
SDL_Window *swin = NULL;
SDL_Surface *ssurf = NULL;
SDL_Texture *stex = NULL;
SDL_AudioStream *saudio = NULL;
SDL_Palette *spal = NULL;
SDL_TimerID stimer = 0;

uint64_t sdl_timer_cntr;

int ra1_plat_win_scale = 4;	/* i want 1280x800 window at least */
int ra1_latest_key = 0;

#define APPNAME "SMUSHv1 RE"


static void vid_present()
{
	SDL_UpdateTexture(stex, NULL, ssurf->pixels, ssurf->pitch);
	SDL_SetSurfacePalette(ssurf, spal);
	SDL_SetTexturePalette(stex, spal);
	SDL_RenderClear(sren);
	SDL_RenderTexture(sren, stex, NULL, NULL);
	SDL_RenderPresent(sren);
}


void vid_video_init(void)
{
	int width = 320 * ra1_plat_win_scale;
	int height = 200 * ra1_plat_win_scale;

	SDL_SetHint(SDL_HINT_APP_NAME, APPNAME);
	if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK | SDL_INIT_EVENTS))
		return;

	swin = SDL_CreateWindow(APPNAME, width, height, 0);
	if (!swin)
		goto out;

	sren = SDL_CreateRenderer(swin, NULL);
	if (!sren)
		goto out;

	ssurf = SDL_CreateSurface(320, 200, SDL_PIXELFORMAT_INDEX8);
	if (!ssurf)
		goto out;
	spal = SDL_CreatePalette(256);
	if (!spal)
		goto out;
	stex = SDL_CreateTexture(sren, SDL_PIXELFORMAT_INDEX8, SDL_TEXTUREACCESS_STREAMING, 320, 200);
	if (!stex)
		goto out;

	AG(vid_vga_pal_is_fast) = 1;

	vid_palette_set_brightness(0);
	vid_present();
	return;
out:
	vid_video_terminate();
}

void vid_video_terminate(void)
{
	if (spal) {
		SDL_DestroyPalette(spal);
		spal = NULL;
	}
	if (stex) {
		SDL_DestroyTexture(stex);
		stex = NULL;
	}
	if (ssurf) {
		SDL_DestroySurface(ssurf);
		ssurf = NULL;
	}
	if (sren) {
		SDL_DestroyRenderer(sren);
		sren = NULL;
	}
	if (swin) {
		SDL_DestroyWindow(swin);
		swin = NULL;
	}
	SDL_Quit();
}

void vid_palette_set_raw(uint8_t *pal)
{
	for (int i = 0; i < 256; i++) {
		spal->colors[i].a = 0xff;
		spal->colors[i].b = pal[i * 3 + 0];
		spal->colors[i].g = pal[i * 3 + 1];
		spal->colors[i].r = pal[i * 3 + 2];
	}
	SDL_SetSurfacePalette(ssurf, spal);
	SDL_SetTexturePalette(stex, spal);
	vid_present();
}

void vid_palette_setcolor(uint16_t coloridx, uint8_t r, uint8_t g, uint8_t b)
{
	spal->colors[coloridx].r = r;
	spal->colors[coloridx].g = g;
	spal->colors[coloridx].b = b;
	SDL_SetSurfacePalette(ssurf, spal);
	SDL_SetTexturePalette(stex, spal);
	vid_present();
}

void vid_palette_clear(void)
{
	for (int i = 0; i < 256; i++) {
		spal->colors[i].a = 0xff;
		spal->colors[i].b = 0;
		spal->colors[i].g = 0;
		spal->colors[i].r = 0;
	}
	SDL_SetSurfacePalette(ssurf, spal);
	SDL_SetTexturePalette(stex, spal);
	vid_present();
}

void vid_hw_clear_lines(uint16_t linecnt, uint16_t startline)
{
	int lc = linecnt;
	uint8_t *sa;

	printf("SDL3_clear_lines: %d->%d (%d)\n", startline, startline + linecnt, linecnt);
	if (linecnt < 1 || startline > 319)
		return;
	if (startline + linecnt > 320)
		lc = 320 - startline;
	sa = (uint8_t *)ssurf->pixels + (startline * ssurf->pitch);
	memset(sa, 0, lc * ssurf->pitch);
	vid_present();
}

void vid_hw_blt_to_screen_delta(uint8_t *srcbuf, uint16_t sxoff, uint16_t syoff,
			       uint16_t dxoff, uint16_t dyoff, uint16_t width, uint16_t height,
			       uint16_t srcstride, uint8_t *refbuf)
{
	/* we don't do delta since our VGA access is blazing fast */
	vid_hw_blt_to_screen(srcbuf, sxoff, syoff, dxoff, dyoff, width, height,
			     srcstride, refbuf);
}


/* Platform: write a full buffer directly to screen.
 * for DOS: just blt the whole 64kb srcbuf to VGA memory at 0x000a0000.
 */
void vid_hw_blt_to_screen(uint8_t *srcbuf, uint16_t sxoff, uint16_t syoff,
			  uint16_t dxoff, uint16_t dyoff, uint16_t width, uint16_t height,
			  uint16_t srcstride, uint8_t *refbuf)
{
	uint8_t *dst = (uint8_t *)ssurf->pixels + (dyoff * ssurf->pitch) + dxoff;
	uint32_t srcp = srcstride - width;
	uint32_t dstp = 320 - width;

	srcbuf += (syoff * srcstride) + sxoff;
	while (height--) {
		memcpy(dst, srcbuf, width);
		srcbuf += srcstride;
		dst += ssurf->pitch;

		if (AG(sys_abort_flag) == 0) {
			sys_timer_continue();
		}
	}
}

/* FADE: copy parts of srcimg based on a pattern in maskdata directly to the
 * screen buffer.
 */
void vid_process_fade(uint8_t *srcimg, uint8_t *maskdata, uint16_t size)
{
	vid_process_fade_core(srcimg, maskdata, size, ssurf->pixels);
	vid_present();
}


/* directly blt 20 lines at line offset 178 */
void vid_hw_blt_to_statusbar(uint8_t *srcbuf, uint32_t val)
{
	uint8_t *dst = ssurf->pixels + (178 * 320);
	memcpy(dst, srcbuf, 6400);
	vid_present();
}


int16_t sou_sdl3_cb1(void *dmabuf, uint32_t dmasize, uint16_t x86io, uint16_t x86dma,
		  uint16_t x86irq, void *lut1, void *lut2)
{
	int16_t *l1 = (int16_t *)lut1;
	int16_t *l2 = (int16_t *)lut2;

	for (int x = 0; x < 17; x++) {
		for (int y = 0; y < 256; y++) {
			int z = (y - 128) * x * 4;
			*l1++ = z;
			*l2++ = z;
		}
	}

	SDL_ClearAudioStream(saudio);
	SDL_PutAudioStreamData(saudio, dmabuf, dmasize);
	SDL_ResumeAudioStreamDevice(saudio);

	return 0;
}

void sou_sdl3_cb2(void)
{
	SDL_ResumeAudioStreamDevice(saudio);
	SDL_FlushAudioStream(saudio);
}

/* queue data (double buffered) */
int8_t sou_sdl3_cb3(void *snddata, uint32_t datasize)
{
	SDL_PutAudioStreamData(saudio, snddata, datasize);
	return 0;
}

/* convert to hw format and apply volume (0..127) */
void sou_sdl3_cb4(void *dst, uint8_t *src, uint16_t srccnt, uint16_t volume)
{
	volume &= 0x7f;
	int16_t *dstx = (int16_t *)dst;
	while (srccnt--) {
		*dstx++ = AG(sou_soudrv_LUT1)[volume * 0x100 + *src++];
	}
}

/* convert to hw format, apply volume (0..127) and accumulate in dst */
void sou_sdl3_cb5(void *dst, uint8_t *src, uint16_t srccnt, uint16_t volume)
{
	int16_t *dstx = (int16_t *)dst;
	volume &= 0x7f;
	while (srccnt--) {
		*dstx++ += AG(sou_soudrv_LUT2)[volume * 0x100 + *src++];
	}
}

/* fill the buffer with hw-specific silence */
void sou_sdl3_cb6(void *dst, uint32_t datasize)
{
	uint8_t *d = (uint8_t *)dst;
	memset(d, 0x7f, datasize);
}

void sou_sdl3_cb7(uint16_t volume)
{
	SDL_SetAudioStreamGain(saudio, volume / 127.0f);
}

uint16_t sou_sdl3_cb8(void)
{
	return (uint16_t)(SDL_GetAudioStreamGain(saudio) * 127);
}

uint16_t sou_sdl3_set_callbacks(sou_soudrv_cb1_fn_t *initfn,   sou_soudrv_cb2_fn_t *termfn,
				sou_soudrv_cb3_fn_t *ackfn,    sou_soudrv_cb4_fn_t *copyfn,
				sou_soudrv_cb5_fn_t *accumfn,  sou_soudrv_cb6_fn_t *silencefn,
				sou_soudrv_cb7_fn_t *setvolfn, sou_soudrv_cb8_fn_t *getvolfn)

{
	*initfn = sou_sdl3_cb1;
	*termfn = sou_sdl3_cb2;
	*ackfn = sou_sdl3_cb3;
	*copyfn = sou_sdl3_cb4;
	*accumfn = sou_sdl3_cb5;
	*silencefn = sou_sdl3_cb6;
	*setvolfn = sou_sdl3_cb7;
	*getvolfn = sou_sdl3_cb8;
	return 1;		/* 16 bit destination format */
}

int16_t sou_init_platform(void)
{
	SDL_AudioSpec spec;

	spec.freq = 11025;
	spec.format = SDL_AUDIO_S16LE;
	spec.channels = 1;
	saudio = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK,
					   &spec, NULL, NULL);
	if (!saudio) {
		printf("cannot create 11kHz/8/1 audio stream\n");
		return 1;
	}

	/* 16bit: 2 4kB buffers for audio doublebuffering */
	AG(sou_dmabuf_arr[0]) = malloc(4096);
	AG(sou_dmabuf_arr[1]) = malloc(4096);
	memset(AG(sou_dmabuf_arr[0]), 127, 4096);
	memset(AG(sou_dmabuf_arr[1]), 127, 4096);

	AG(sou_soudrv_sizeshift) = sou_sdl3_set_callbacks(&AG(sou_soudrv_cb1_init_fn), &AG(sou_soudrv_cb2_deinit_fn),
			       &AG(sou_soudrv_cb3_ackint_fn), &AG(sou_soudrv_cb4_fill_fn),
			       &AG(sou_soudrv_cb5_accum_fn), &AG(sou_soudrv_cb6_silence_fn),
			       &AG(sou_soudrv_cb7_setvol_fn), &AG(sou_soudrv_cb8_getvol_fn));

	AG(sou_soudrv_type) = 1;		/* any sound card is good */
	AG(sou_soudrv_pref_bufsize) = 1500000;	/* what SB16 DOS uses */
	AG(sou_dmabuf_idx) = 0;
	AG(sou_soudrv_iobase) = 0x220;
	AG(sou_soudrv_irq) = 5;
	AG(sou_soudrv_dma) = 1;
	snprintf(AG(sou_soudrv_namestr), 100, "SDL3: %s\n",
		 SDL_GetAudioDeviceName(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK));

	return 0;
}

void sou_terminate_platform(void)
{
	AG(sou_soudrv_type) = 0;
	AG(sou_soudrv_pref_bufsize) = 0;
	if (saudio) {
		SDL_DestroyAudioStream(saudio);
		saudio = NULL;
	}
	if (AG(sou_dmabuf_arr[0])) {
		free(AG(sou_dmabuf_arr[0]));
		AG(sou_dmabuf_arr[0]) = NULL;
	}
	if (AG(sou_dmabuf_arr[1])) {
		free(AG(sou_dmabuf_arr[1]));
		AG(sou_dmabuf_arr[1]) = NULL;
	}
}

/* sound format is 11,025kHz, 8bit mono, prepared in 1024-byte chunks.
 * the sou engine was designed to prepare data whenever the sound card triggered
 * an irq on dma transfers finished.
 * -> 10,766 ms call frequency.
 */
void sou_engine_fill(void)
{
	/* nothing special to do here, just call the main handler */
	sou_engine_fill_core();
}

int game_init_platform(int argc, char **argv)
{
	int i = 1;
	int32_t sndio, snddma, sndirq, sndid, sndbufsize, cddrvusage;
	int16_t timerrate, maxfps;

	timerrate = 300;
	maxfps = 15;
	cddrvusage = 0;
	game_cfg_set_perf_params(maxfps, timerrate, cddrvusage);

	while (1) {
		if (argc <= i)
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

	/* DOS exe does a hard exit(1) here */
	return 1;
}

void sys_timer_continue(void)
{
	sys_timer_continue_generic();
}

void sys_delay(int ms)
{
	SDL_Delay(ms);
}

/* Timer "emulation" of single-thread DOS application.  In SDL the timer
 * runs in a separate thread, which does not work with the assumptions and
 * setjmp()/longjmp() architecture in the DOS engine.  So we pump an event
 * at the timer rate and use the "ctl_mou_center()" call which is called in a
 * loop in game_run() until termination is requested, to call the engines
 * timer handler. It also runs in the context of the rest of the engine, which
 * makes things simpler (no stack setup, ...)
 */
#define SMUSH_TICK_EVENT 37
uint32_t SDL_timer_event_pusher(void *ctx, SDL_TimerID self, uint32_t interval)
{
	SDL_Event event;

	SDL_zero(event);
	event.type = SDL_EVENT_USER;
	event.user.code = SMUSH_TICK_EVENT;
	SDL_PushEvent(&event);

	return interval;
}

void sys_timer_init(int cfg_timerrate)
{
	int ms = 1000 / cfg_timerrate;

	AG(sys_timer_abort_toggle) = 0;
	AG(sys_abort_flag) = 0;
	AG(sys_timer_yielding) = 0;
	AG(sys_animation_running_flag) = 0;
	AG(sys_timer_handler_disable_cntr) = 0;
	AG(sys_timer_idle_cntr) = 0;
	AG(sys_saved_flag) = 0;
	SDL_timer_event_pusher(NULL, -1, -1);
	stimer = SDL_AddTimer(ms, SDL_timer_event_pusher, NULL);
}

void sys_timer_terminate(void)
{
	if (stimer) {
		SDL_RemoveTimer(stimer);
		stimer = 0;
	}
}

void ctl_kbd_query(uint16_t *key_out)
{
	*key_out = ra1_latest_key;
}

void ctl_mou_center(void)
{
	SDL_Event e;
	while (SDL_PollEvent(&e)) {

		if (e.type == SDL_EVENT_USER && e.user.code == SMUSH_TICK_EVENT) {
			sys_timer_handler_generic();
			continue;
		}

		if (e.type == SDL_EVENT_QUIT) {
			anm_cmd_quit();
		} else if (e.type == SDL_EVENT_KEY_DOWN) {
			SDL_KeyboardEvent *ke = (SDL_KeyboardEvent *)&e;
			uint16_t mapped_key = 0;
			switch (ke->scancode) {
			case SDL_SCANCODE_UP:    mapped_key = 0x81; break;
			case SDL_SCANCODE_DOWN:  mapped_key = 0x82; break;
			case SDL_SCANCODE_LEFT:  mapped_key = 0x83; break;
			case SDL_SCANCODE_RIGHT: mapped_key = 0x84; break;
			case SDL_SCANCODE_PERIOD:
				vid_palette_set_brightness(1);
				ra1_latest_key = 0;
				break;
			case SDL_SCANCODE_COMMA:
				vid_palette_set_brightness(-1);
				ra1_latest_key = 0;
				break;
			default:
				if (ke->key >= 32 && ke->key <= 126)
					mapped_key = (uint16_t)ke->key;
			break;
			}
			if (mapped_key != 0)
				ra1_latest_key = mapped_key;
		} else if (e.type = SDL_EVENT_MOUSE_MOTION) {
			SDL_MouseMotionEvent *me = (SDL_MouseMotionEvent *)&e;
			//SDL_ConvertEventToRenderCoordinates(sren, &e);
			AG(ctl_mousedrv_xpos) = 1;
			AG(ctl_mousedrv_ypos) = 1;
			printf("mouse: %d %d\n", (int)me->x, (int)me->y);
		}
	}
}

void ctl_init_platform(int mou_en, int joy_en)
{
	AG(ctl_mousedrv_xpos) = 0;
	AG(ctl_mousedrv_ypos) = 0;
	AG(ctl_mousedrv_button1) = 0;
	AG(ctl_mousedrv_button2) = 0;
}

void ctl_terminate_platform(void)
{
}

void ctl_joy_plat_update_axes(uint16_t *a0, uint16_t *a1, uint16_t *a2, uint16_t *a3,
			      uint16_t *a4, uint16_t *a5, uint16_t *a6, uint16_t *a7)
{
	*a0 = 32767;
	*a1 = 32767;
	*a2 = 0;
	*a3 = 0;
}
