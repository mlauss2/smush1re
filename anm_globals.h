/*
 * SMUSHv1
 *
 * ANM globals
 */

#include "smush.h"

/* the anm global variables as used by the implementation in Rebel Assault 1.
 * While these are just globals in the EXE, I want them all in this struct so
 * they dont contaminate the global namespace.
 */
struct _anm_globals_v1 {

	uint16_t		ctl_joy_translated_axis_values[8];	/* 002ae07c */
	uint8_t			sou_music_enabled;		/* 002ae96d */
	uint8_t			sou_sfx_enabled;		/* 002ae96e */
	uint8_t			anm_text_force_enable;		/* 002ae96f */
	uint8_t			sou_voice_enabled;		/* 002ae970 */
	uint8_t			smush_debug_enabled;		/* 002ae971 */
	uint16_t		game_param_gfxquality;		/* 002ae975 */

	struct anm_rect		anm_default_viewport;		/* 002b08a0 */
	struct anm_headerv1	anm_anim_header_current;	/* 002b08a8 */

	uint32_t		fle_bufsize;			/* 002b0bae */

	uint16_t		anm_use_existing_dstbuf;	/* 002b0bb2 */
	uint16_t		txt_fontcnt;			/* 002b0bb8 */
	struct txt_font_v1	txt_fontstore[4];		/* 002b0bba - 002b1fd7 */

	int16_t			fob_cam_proj_x;			/* 002b2004 */
	int16_t			fob_cam_proj_y;			/* 002b2006 */
	int16_t			fob_cam_proj_extent;		/* 002b2008 */
	uint16_t		fobdec_c4_tilestore_param[2];	/* 002b200a */
	int16_t			fobdec_c4_tilestore_idx;	/* 002b200e */
	uint8_t			fob_cam_rand_en;		/* 002b2010 */

	char			sys_config_cdusage_string[100];	/* 002b20e0 */
	char			sou_soudrv_namestr[100];	/* 002b2144 */
	char			sou_soudrv_cfgstr1[100];	/* 002b21a8 */
	char			sou_soudrv_cfgstr2[100];	/* 002c220c */
	char			anm_errstr[104];		/* 002b2270 */

	uint8_t			ctl_mou_enabled;		/* 002b22d8 */
	uint8_t			ctl_joy_enabled;		/* 002b22d9 */
	int16_t			ctl_j_minx;			/* 002b22da */
	int16_t			ctl_j_miny;			/* 002b22dc */
	int16_t			ctl_j_maxx;			/* 002b22de */
	int16_t			ctl_j_maxy;			/* 002b22e0 */
	int16_t 		ctl_j_scalex1;			/* 002b22e2 */
	int16_t 		ctl_j_scalex2;			/* 002b22e4 */
	int16_t 		ctl_j_scaley1;			/* 002b22e6 */
	int16_t 		ctl_j_scaley2;			/* 002b22e8 */
	uint8_t			ctl_j_calibrating;		/* 002b22ea */
	uint8_t			ctl_mou_pos_changed;		/* 002b22eb */

	void *			fle_handles[10];		/* 002b22ec */

	void *			fle_cksum_file_buffer[10];	/* 002b2314 */
	uint32_t		fle_offsets[10];		/* 002b233c */
	uint32_t		fle_sizes[10];			/* 002b2364 */

	int16_t			fle_cksum_2b238c;		/* 002b238c */
	uint8_t			fle_buffer_ready;			/* 002b238e */
	uint16_t		game_cfg_timerrate;		/* 002b2390 */
	uint16_t		game_cfg_maxfps;		/* 002b2392 */
	uint16_t		game_cfg_maxfps_1315;		/* 002b2394 */
	uint16_t		sys_cfg_cddrvusage;		/* 002b2396 */
	uint8_t			game_current_statusbar_changed_flag_at_call;	/* 002b2398 */
	uint8_t			sys_cfg_cddrvusagecap_set;	/* 002b2399 */
	uint8_t			sys_timer_handler_disable_cntr;	/* 002b239a */
	uint8_t			sys_timer_cpu_usagecap_is_on;	/* 002b239b */
	uint16_t		sys_abort_flag;			/* 002b239c */

	uint8_t			sys_animation_running_flag;	/* 002b23c2 */
	uint16_t		sys_animation_tick_cntr;	/* 002b23c3 */
	uint8_t			sys_timer_yielding;		/* 002b23c5 */
	uint16_t		sys_timer_abort_toggle;		/* 002b23c6 */
//	uint32_t		sys_timer_int8_tick;		/* 002b23c8 */
	uint16_t		sys_timer_idle_cntr;		/* 002b23cc */
	uint8_t			sys_saved_flag;			/* 002b23ce */

	uint16_t		sou_soudrv_type;		/* 002b23d0 */
	uint16_t		sou_soudrv_irq;			/* 002b23d2 */
	uint16_t		sou_soudrv_iobase;		/* 002b23d4 */
	uint32_t		sou_soudrv_pref_bufsize;	/* 002b23d6 */
	uint16_t		sou_soudrv_dma;			/* 002b23da */
	uint32_t		sou_irq_cnt;			/* 002b23dc */
	uint8_t			sou_paused;			/* 002b23e0 */

	uint8_t *		sou_engine_fill_buf5120;	/* 002b23f1 */

	int8_t *		sou_soudrv_LUT1;		/* 002b23f9 */
	int8_t *		sou_soudrv_LUT2;		/* 002b23fd */
	uint8_t			sou_soudrv_initstatus;		/* 002b2401 */

	uint8_t			vid_vga_pal_is_fast;		/* 002b2490 */
	int16_t			vid_pal_brightness;		/* 002b2493 */
	uint8_t	*		vid_vidbuf_base[2];		/* 002b2495 */
	int16_t			vid_vidbuf_curridx;		/* 002b249d */
	int8_t			vid_no_deltablit_to_screen;	/* 002b249f */
	uint16_t		vid_vga_blt_last_copyheight;	/* 002b249f */

	int16_t			anm_shift_pal[768];		/* 002b62a0 */
	int16_t			anm_delta_pal[768];		/* 002b68a0 */
	anm_frme_postcb_t	anm_frme_postcb;		/* 002b6ea0 */
	anm_frme_gamecb_t	anm_frme_gamecb;		/* 002b6ea4 */
	uint8_t *		anm_default_dst;		/* 002b6ea8 */
	uint8_t *		anm_codec4_tilestore;		/* 002b6eac */
	uint8_t *		anm_anim_frme_dataptr;		/* 002b6eb0 */
	uint32_t		anm_streamer_handle;		/* 002b6eb4 */
	uint8_t *		anm_hide_table;			/* 002b6eb8 */
	uint16_t		anm_anim_frme_curr;		/* 002b6ebc */
	uint16_t		anm_anim_frme_cnt;		/* 002b6ebe */
	uint32_t		anm_anim_frme_size;		/* 002b6ec0 */
	uint16_t		anm_animator_state;		/* 002b6ec4 */
	uint8_t			anm_anim_started;		/* 002b6ec6 */
	uint16_t		anm_anmflags_restart;		/* 002b6ec7 */
	uint8_t			anm_newanim_posted;		/* 002b6ec9 */
	uint16_t		anm_anmflags_new;		/* 002b6eca */
	uint8_t			anm_do_splice;			/* 002b6ecc */
	uint16_t		anm_splice1;			/* 002b6ecd */
	uint16_t		anm_splice2;			/* 002b6ecf */
	uint8_t			anm_palette_changed;		/* 002b6ed1 */
	uint8_t			anm_quit_flag;			/* 002b6ed2 */
	uint16_t		anm_clear_video_after_present;	/* 002b6ed3 */
	uint16_t		anm_pause_status;		/* 002b6ed5 */
	uint8_t *		anm_overlay_buf;		/* 002b6ed7 */
	uint8_t *		GAME_statusbar_6400buf;		/* 002b6edb */
	uint16_t		anm_anmflags_global;		/* 002b6edf */

	uint8_t			msc_random_runval;		/* 002b6ee8 */

	int16_t			fob_cam_proj_curve[80];		/* 002b6eec */

	uint8_t			*fobdec_c4_last_tilestore;	/* 002b6f8c */

	int16_t			ctl_j_centy;			/* 002b7760 */
	int16_t			ctl_j_centx;			/* 002b7762 */
	int16_t			ctl_mousedrv_xpos;		/* 002b7764 */
	int16_t			ctl_mousedrv_ypos;		/* 002b7766 */

	int16_t			ctl_j_axis7;			/* 002b7768 */
	int16_t			ctl_j_axis6;			/* 002b776a */
	int16_t			ctl_j_axis3;			/* 002b776c */
	int16_t			ctl_j_axis2;			/* 002b776e */
	int16_t			ctl_mousedrv_button2;		/* 002b7770 */
	int16_t			ctl_mousedrv_button1;		/* 002b7772 */
	int16_t			ctl_mouse_xpos;			/* 002b7774 */
	int16_t			ctl_mouse_ypos;			/* 002b7776 */
	int16_t			ctl_j_axis5;			/* 002b7778 */
	int16_t			ctl_j_axis1;			/* 002b777a */
	int16_t			ctl_j_axis4;			/* 002b777c */
	int16_t			ctl_j_axis0;			/* 002b777e */
	uint8_t			ctl_mousedrv_in_handler;	/* 002b7780 */
	uint8_t			ctl_input_updates_locked;	/* 002b7781 */
	int16_t			ctl_input_mouse_ypos_last;	/* 002b7782 */
	int16_t			ctl_input_mouse_xpos_last;	/* 002b7784 */
	int16_t			ctl_input_mouse_xpos;		/* 002b7786 */
	int16_t			ctl_input_mouse_ypos;		/* 002b7788 */
	/* pad 2 bytes */
	uintptr_t		fle_buffer;			/* 002b778c */
	uintptr_t		fle_buffer_2kaligned_end;	/* 002b7790 */
	uint32_t		fle_bufsize_2kaligned;		/* 002b7794 */
	uint16_t		fle_streamer_dispense_inhibit;	/* 002b7798 */
	int8_t			fle_streamer_seeknew_flag;	/* 002b779a */
	uintptr_t		fle_streamer_readbuffer_newpos;	/* 002b779b */
	uint32_t		fle_streamer_inblk_offset;	/* 002b779f */
	uintptr_t		fle_streamer_readbuffer_wrptr;	/* 002b77a3 */
	uintptr_t		fle_streamer_readbuffer_rdptr;	/* 002b77a7 */
	uint32_t		fle_streamer_readbuffer_lastreadsize;	/* 002b77ab */
	uint32_t		fle_streamer_fhandle;		/* 002b77af */

	uint16_t		sys_cpu_usagecap_val;		/* 002b77e8 */
	uint16_t		game_anm_flags_initial;		/* 002b77ea */
	uint16_t		sys_timer_fps_tickthres;	/* 002b77ec */
//	uint16_t		sys_timer_int8_tickthres;	/* 002b77ee */

	jmp_buf			sys_jmpbuf_1;			/* 002b77f0 */
	jmp_buf			sys_jmpbuf_2;			/* 002b7824 */

	uint16_t		anm_animator_result_prev;	/* 002b785e */
	uint16_t		anm_animator_result;		/* 002b7860 */

//	uint32_t		sys_x86_stackptr1;		/* 002b9f6e */

	uint8_t *		sou_dmabuf_arr[2];		/* 002b9f74 */

	sou_soudrv_cb1_fn_t	sou_soudrv_cb1_init_fn;		/* 002b9f8c */
	sou_soudrv_cb2_fn_t	sou_soudrv_cb2_deinit_fn;	/* 002b9f90 */
	sou_soudrv_cb3_fn_t	sou_soudrv_cb3_ackint_fn;	/* 002b9f94 */
	sou_soudrv_cb4_fn_t	sou_soudrv_cb4_fill_fn;		/* 002b9f98 */
	sou_soudrv_cb5_fn_t	sou_soudrv_cb5_accum_fn;	/* 002b9f9c */
	sou_soudrv_cb6_fn_t	sou_soudrv_cb6_silence_fn;	/* 002b9fa0 */
	sou_soudrv_cb7_fn_t	sou_soudrv_cb7_setvol_fn;	/* 002b9fa4 */
	sou_soudrv_cb8_fn_t	sou_soudrv_cb8_getvol_fn;	/* 002b9fa8 */
	uint16_t		sou_pause_vol;			/* 002b9fb4 */

	uint16_t		sou_dmabuf_idx;			/* 002b9fbc */
	uint16_t		sou_soudrv_sizeshift;		/* 002b9fbe */
	uint8_t			sou_hooks[255];			/* 002b9fd0 */
	uint8_t			sou_global_volume;		/* 002ba0cf */
	uint8_t			vid_palette[768];		/* 002ba230 */
	uint8_t			vid_palette_lookup[768];	/* 002ba530 */
	uint8_t			vid_statusbar_changed_flag_now;	/* 002ba630 */
	uint8_t			vid_statusbar_changed_flag_previous;	/* 002ba631 */

	/* collected them into a struct, to mimic RA2+ engine style */
	struct _sou_channel	sou_channel[4];
	struct _sou_engine_fill	sou_engine_fill[4];
};


extern struct _anm_globals_v1 anm_globals;

#define AG(x) anm_globals.x
