/*
 * SMUSHv1 RE
 *
 * anm - animator.
 */

#include "smush.h"

int16_t anm_overlay_alloc(void **ppOut)
{
	return mem_blk_alloc(ppOut, 65536);
}

void anm_overlay_set(uint8_t *ovlbuf, uint8_t *src, uint8_t *c4tilestore,
		     struct anm_rect *viewport)
{
	uint32_t csz = __be32(src + 4);
	if ((csz + 8) < 0xffff) {
		if (src[8] == 3) {
			msc_memcpy(ovlbuf, src, 22);
			fob_decode_render(ovlbuf + 22, src + 22, c4tilestore,
					  csz - 14, ovlbuf + 8, 0, 0, viewport, 8);
			ovlbuf[8] = 20;		/* codec20 */
			if (__le16(ovlbuf + 14) >= 0x140) {
				ovlbuf[14] = 0x40;
				ovlbuf[15] = 0x01;
			}
			if (__le16(ovlbuf + 16) >= 200) {
				ovlbuf[16] = 200;
				ovlbuf[17] = 0;
			}
		} else {
			msc_memcpy(ovlbuf, src, csz + 8);
		}
	}
}

void anm_overlay_show(uint8_t *dst, uint8_t *ovlbuf, uint8_t *c4tilestore,
		      int16_t xoff, int16_t yoff, struct anm_rect *viewport,
		      uint16_t anm_flags)
{
	fob_decode_render(dst, ovlbuf + 20, c4tilestore,
			  __be32(ovlbuf + 4) - 14, ovlbuf + 8,
			  xoff, yoff, viewport, anm_flags | 0x0800);
}

void anm_process_SEGA(uint32_t s1, uint32_t s2, uint32_t s3, uint32_t s4)
{
	/* empty in DOS ASSAULT.EXE 1.7 */
}

void anm_process_fade(uint8_t *dst, uint8_t *src, uint16_t size)
{
	AG(vid_no_deltablit_to_screen) = 1;
	vid_statusbar_clear_vidbufarea();	/* clear away the statusbar in the buffers */
	vid_process_fade(dst, src, size);
}

int16_t anm_animator_render(int16_t xoff, int16_t yoff, uint16_t anm_flags)
{
	uint8_t b;
	int16_t stat;

	while (1) {

		while (1) {
			if (AG(sys_abort_flag) != 0) {
				sys_timer_continue();
			}
			if (0x40 < AG(anm_animator_state))
				break;
			if (AG(anm_animator_state) != 0)
				goto L002979ad;
			AG(anm_anim_frme_dataptr) = fle_streamer_dispense(8);
			if (AG(anm_anim_frme_dataptr) == (uint8_t *)(-1))
				return -1;
			if (AG(anm_anim_frme_dataptr) != NULL) {
				b = AG(anm_anim_frme_dataptr)[0];
				AG(anm_anim_frme_size) = __be32ua(AG(anm_anim_frme_dataptr) + 4);
				AG(anm_anim_frme_curr) = 0;
				AG(anm_animator_state) = b;
				if (b == 'F') {
					AG(anm_anim_started) = 0;
					AG(anm_newanim_posted) = 0;
				}
			}
		}

		if (AG(anm_animator_state) < 'B') {		/* AHDR */
			AG(anm_anim_frme_dataptr) = fle_streamer_dispense(AG(anm_anim_frme_size) + 8);
			if (AG(anm_anim_frme_dataptr) == (uint8_t *)(-1))
				return -1;
			if (AG(anm_anim_frme_dataptr) != NULL) {
				anm_anim_reset();
				b = *(uint8_t *)(AG(anm_anim_frme_dataptr) + AG(anm_anim_frme_size));
				AG(anm_anim_frme_size) = __be32ua(AG(anm_anim_frme_dataptr) + AG(anm_anim_frme_size) + 4);
				AG(anm_anim_frme_curr) = 0;
				AG(anm_animator_state) = b;
				if (b == 'F') {
					AG(anm_anim_started) = 0;
					AG(anm_newanim_posted) = 0;
				}
			} else {
				AG(anm_animator_state) = 0;
				continue;
			}
		}

		if (AG(anm_animator_state) < 'F') {
L002979ad:
			snprintf(AG(anm_errstr), 104, "Bad Frame Header id found\n");
			anm_cmd_quit();
			return -4;
		}

		if (AG(anm_animator_state) < 'G') {		/* FRME */
			if (0x10007 < AG(anm_anim_frme_size)) {
				snprintf(AG(anm_errstr), 104, "Bad IFF Header size found\n");
				anm_cmd_quit();
				return -2;
			}
			AG(anm_anim_frme_dataptr) = fle_streamer_dispense(AG(anm_anim_frme_size) + 8);
			if (AG(anm_anim_frme_dataptr) == (uint8_t *)(-1))
				return -1;
			if (AG(anm_anim_frme_dataptr) == NULL) {
				AG(anm_animator_state) = 0;
				continue;
			}
		} else if (AG(anm_animator_state) != 'x') {
			goto L002979ad;
		}

		stat = anm_process_frme(xoff, yoff, anm_flags);

		if ((AG(anm_pause_status) == 1) && (stat == 0)) {
			AG(anm_animator_state) = 'x';
			return 0;
		}
		if ((AG(anm_pause_status) == 1) && (stat != 0)) {
			AG(anm_pause_status) = 3;
		} else if (AG(anm_pause_status) == 3) {
			AG(anm_pause_status) = 1;
		}
		AG(anm_anim_frme_curr) += 1;
		if (stat == 1) {
			AG(anm_animator_state) = 0;
		} else if (AG(anm_anim_frme_curr) < (AG(anm_anim_frme_cnt) - 1)) {
			uint32_t fs = AG(anm_anim_frme_size);
			AG(anm_animator_state) = AG(anm_anim_frme_dataptr)[fs];
			AG(anm_anim_frme_size) = __be32ua(AG(anm_anim_frme_dataptr) + fs + 4);
		} else {
			AG(anm_animator_state) = 'x';
			if ((AG(anm_anim_started) != 0) || (AG(anm_newanim_posted) != 0)) {
				if (fle_streamer_switch() == 0) {
					AG(anm_animator_state) = 0;
				}
			}
		}
		if (stat != 2) {
			return 0;
		}
	}
}

int16_t anm_process_frme(int16_t xoff,int16_t yoff,uint16_t anm_flags)
{
	int nc;

	nc = 0;
	if (AG(anm_do_splice) != 0) {
		if ((AG(anm_anim_started) == 0) && (AG(anm_newanim_posted) == 0)) {
			if (AG(anm_anim_frme_curr) != AG(anm_splice1)) {
				return 2;
			}
			AG(anm_do_splice) = 0;
			AG(anm_anim_started) = 0;
			AG(anm_newanim_posted) = 0;
		} else {
			nc = 1;
			if (0 < AG(anm_splice2)) {
				if ((AG(anm_anim_frme_curr) < AG(anm_splice2)) || (AG(anm_splice2) < 1)) {
					nc = 0;
				} else {
					AG(anm_splice1) += AG(anm_anim_frme_curr) - AG(anm_splice2);
					AG(anm_splice2) = -1;
				}
			}
		}
	}

	if (((anm_flags | AG(anm_anmflags_global)) & 0x0020) != 0) {
		vid_bitmap_clear(AG(anm_default_dst));
	}

	anm_frame_render(AG(anm_default_dst), AG(GAME_statusbar_6400buf),
			 AG(anm_overlay_buf),  AG(anm_anim_frme_dataptr),
			 AG(anm_anim_frme_size), AG(anm_codec4_tilestore), xoff, yoff,
			 &(AG(anm_default_viewport)), AG(anm_frme_gamecb),
			 AG(anm_hide_table), anm_flags | AG(anm_anmflags_global));

	sou_engine_kill_dead_channels();

	if (AG(anm_frme_postcb) != NULL) {
		(AG(anm_frme_postcb))(AG(anm_default_dst), AG(GAME_statusbar_6400buf),
				       AG(anm_anim_frme_curr), &(AG(anm_default_viewport)),
				       AG(anm_anim_frme_cnt), AG(anm_anim_frme_size));
	}
	if (AG(smush_debug_enabled)) {
		int i = fle_streamer_get_available_data();
		msc_memset(AG(anm_default_dst) + 0x258a, 0xe3, i >> 9);
		AG(anm_default_dst)[(AG(fle_bufsize)) >> 9] = 0xe3;
	}
	anm_frame_display(AG(anm_default_dst));
	if (nc) {
		fle_streamer_switch();
	}
	return nc;
}

void anm_anim_reset(void)
{
	int i;

	msc_memcpy(&(AG(anm_anim_header_current)), AG(anm_anim_frme_dataptr), 0x306);
	AG(anm_anim_header_current).animpal[0] = 0;
	AG(anm_anim_header_current).animpal[1] = 0;
	AG(anm_anim_header_current).animpal[2] = 0;
	if (AG(anm_anim_started) != 0) {
		if ((AG(anm_anmflags_restart) & 0x0400) != 0) {
			anm_cmd_palette_changed();
		}
		AG(anm_anim_started) = 0;
		AG(anm_anmflags_global) = AG(anm_anmflags_restart);
	}
	if (AG(anm_newanim_posted) != 0) {
		if ((AG(anm_anmflags_new) & 0x0400) != 0) {
			vid_palette_clear();
			anm_cmd_palette_changed();
		}
		if ((AG(anm_hide_table) != NULL) && ((AG(anm_anmflags_new) & 0x0040) == 0)) {
			for (int i = 0; i < 80; i++)
				AG(anm_hide_table)[i] = 0;
			for (; i < 150; i++)
				AG(anm_hide_table)[i] = 0xff;
			for (; i < 300; i++)
				AG(anm_hide_table)[i] = 0;
		}
		AG(anm_newanim_posted) = 0;
		AG(anm_anmflags_global) = AG(anm_anmflags_new);
	}
	AG(anm_anim_frme_curr) = 0;
	AG(anm_anim_frme_cnt) = AG(anm_anim_header_current).frmecnt;
	if ((AG(anm_anmflags_global) & 0x0040) == 0) {
		anm_video_reset();
	}
}

static int anm_togglet_test(uint8_t *hidetbl, int bitindex, int ofs)
{
	int byi = (bitindex - 1) / 8;
	int bii = (bitindex - 1) & 7;
	return  (((hidetbl[byi + ofs] & 0x80) >> bii) != 0);
}

static void anm_togglet_clr(uint8_t *hidetbl, int bitindex)
{
	int byi = (bitindex - 1) / 8;
	int bii = (bitindex - 1) & 7;
	hidetbl[byi] &= ~(0x80 >> bii);
}

static void anm_togglet_set(uint8_t *hidetbl, int bitindex)
{
	int byi = (bitindex - 1) / 8;
	int bii = (bitindex - 1) & 7;
	hidetbl[byi] |= (0x80 >> bii);
}

void anm_frame_render(uint8_t *dst, uint8_t *statusbar, uint8_t *ovlbuf,
		      uint8_t *frmedata, uint32_t frmesize, uint8_t *c4tilestore,
		      int16_t xoff, int16_t yoff, struct anm_rect *viewport,
		      anm_frme_gamecb_t gamecb, uint8_t *anm_hide_table,
		      uint16_t anm_flags)
{
	uint32_t cid, csz;
	int can_run_gamecb, fobj_rendered, do_stor, ii;
	uint16_t skip;
	uint8_t *last_fobj;

	do_stor = 0;
	ii = 0;
	can_run_gamecb = 1;
	/* original checks for just >1 */
	while (frmesize > 7) {
		if (((uintptr_t)frmedata & 1) && (frmedata[0] == 0)) {
			--frmesize;
			++frmedata;
			if (frmesize < 8)
				break;
		}
		fobj_rendered = 0;
		if (do_stor && ovlbuf) {
			anm_overlay_set(ovlbuf, frmedata, c4tilestore, viewport);
			anm_overlay_show(dst, ovlbuf, c4tilestore, xoff, yoff, viewport, anm_flags);
			do_stor = 0;
			fobj_rendered = 1;
		}
		cid = __be32ua(frmedata + 0);
		csz = __be32ua(frmedata + 4);
		if (cid == 0x46414445) {					/* FADE */
			if (AG(anm_pause_status) != 0) {
				anm_process_fade(dst, frmedata + 32, csz - 24);
				AG(anm_anmflags_global) |= 0x8000;
			}
		} else if (cid == 0x464f424a) {					/* FOBJ */
			last_fobj = frmedata + 8;
			if (fobj_rendered == 0) {
				skip = __le16ua(frmedata + 18);
				if (anm_hide_table) {
					if ((skip < 1) || (skip > 639) || (gamecb == NULL)) {
						if ((skip < 640) || (ii == 0)) {
							; /* goto 002980ff */
						} else {
							anm_togglet_clr(anm_hide_table, skip);
						}
					} else {
						if (!anm_hide_table
						    || anm_togglet_test(anm_hide_table, skip, 0)) {
							ii = 1;
						} else {
							ii = 0;
						}
					}
				}
/* 002980ff */			if (skip < 0)
					skip = -skip;
				if ((skip == 0) || (anm_hide_table == NULL)
				    || (0 == anm_togglet_test(anm_hide_table, skip, 0))) {
					can_run_gamecb = 1;
					if (skip
					    && anm_hide_table
					    && anm_togglet_test(anm_hide_table, skip, 0x96)
					    && (AG(anm_pause_status) != 1)
					    && (frmedata[8] == 23)) {		/* codec 23 */
						    frmedata[9] -= 16;		/* more color delta for c23 */
					}
					fob_decode_render(dst, frmedata + 22, c4tilestore,
							  csz - 14, frmedata + 8, xoff, yoff,
							  viewport, anm_flags & 0xf7ff);

					if (skip
					    && anm_hide_table
					    && anm_togglet_test(anm_hide_table, skip, 0x96)
					    && (AG(anm_pause_status) != 1)
					    && (frmedata[8] == 23)) {
							frmedata[9] += 16;	/* undo the above */
					}
				} else {
					can_run_gamecb = 0;
				}
			}
		} else if (cid == 0x46544348) {					/* FTCH */
			if (ovlbuf) {
				int xo, yo;
				if (csz < 12) {
					xo = yo = 0;
				} else {
					xo = __be16ua(frmedata + 14);
					yo = __be16ua(frmedata + 18);
				}
				anm_overlay_show(dst, ovlbuf, c4tilestore, xoff + xo,
						 yoff + yo, viewport, anm_flags);
			}
		} else if ((cid == 0x47414d32) || (cid == 0x47414d45)) {	/* GAM2, GAME */
			if (gamecb && can_run_gamecb) {
				gamecb(dst, statusbar, viewport, anm_hide_table,
				       anm_flags, __be32ua(frmedata + 8),
				       __be32ua(frmedata + 12),
				       __be32ua(frmedata + 16),
				       __be32ua(frmedata + 20),
				       __be32ua(frmedata + 24),
				       __be32ua(frmedata + 28),
				       __be32ua(frmedata + 32));
			}
		} else if (cid == 0x474f5354) {					/* GOST */
			uint32_t c = __be32(frmedata + 8);
			uint16_t af = (fobj_rendered) ? anm_flags : anm_flags & 0xf7ff;
			int xo, yo;
			if (c == 28) {
				af |= 0x2000;
			} else if (c == 29) {
				af |= 0x4000;
			} else if (c == 30) {
				af |= 0x6000;
			}
			xo = __be16(frmedata + 14);
			yo = __be16(frmedata + 18);
			fob_decode_render(dst, last_fobj + 14, c4tilestore, csz - 14,
					  last_fobj, xoff + xo, yoff + yo, viewport, af);
		} else if (cid == 0x4e50414c) {					/* NPAL */
			if (AG(anm_pause_status) != 1) {
				msc_memcpy(AG(anm_anim_header_current).animpal, frmedata + 8, 768);
				AG(anm_anim_header_current).animpal[0] = 0;
				AG(anm_anim_header_current).animpal[1] = 0;
				AG(anm_anim_header_current).animpal[2] = 0;
				anm_cmd_palette_changed();
			}
		} else if ((cid == 0x50534144) || (cid == 0x53424c20)		/* PSAD/SBL /SBL2/PVOC/PSD2 */
			   || (cid ==  0x53424c32) || (cid == 0x50564f43)
			   || (cid == 0x50534433)) {
			if ((cid == 0x50564f43) || (can_run_gamecb)) {	/* PVOC */
				sou_engine_start(frmedata, 0);
			}
		} else if (cid == 0x53454741) {					/* SEGA */
			anm_process_SEGA(__be32(frmedata +  8), __be32(frmedata + 12),
					 __be32(frmedata + 16), __be32(frmedata + 20));
		} else if (cid == 0x53544f52) {					/* STOR */
			do_stor = 1;
		} else if (cid == 0x54455854) {					/* TEXT */
			char *t = frmedata + 16;
			int xo, yo;
			if ((frmedata[16] == '.') || AG(anm_text_force_enable)) {
				if (frmedata[16] == '.') {
					++t;
				}
			}
			xo = __be16(frmedata + 10);
			yo = __be16(frmedata + 14);
			txt_font_print_len(dst, viewport, xo, yo, 0x200, 500,
					   (csz - 8) - (frmedata[16] == '.') ? 1 : 0, t);
		} else if (cid ==  0x5850414c) {				/* XPAL */
			uint32_t cmd = __be32(frmedata + 8);
			if ((cmd == 0) || (cmd == 2)) {
				if (cmd == 2) {
					msc_memcpy(AG(anm_anim_header_current).animpal,
						   frmedata + 0x60c, 768);
				}
				for (int i = 0; i < 768; i++) {
					AG(anm_shift_pal)[i] = AG(anm_anim_header_current).animpal[i] << 7;
					AG(anm_delta_pal)[i] = __le16(frmedata + 8 + i*2);
				}
			} else if (AG(anm_pause_status) != 1) {
				for (int i = 0; i < 768; i++) {
					AG(anm_shift_pal)[i] += AG(anm_delta_pal)[i];
					AG(anm_anim_header_current).animpal[i] = AG(anm_shift_pal)[i] >> 7;
				}
				vid_palette_set(AG(anm_anim_header_current).animpal);
			}
		}
		frmedata = frmedata + 8 + csz;
		frmesize = frmesize - 8 - csz;
		if (AG(sys_abort_flag) != 0) {
			sys_timer_continue();
		}
	}
}

uint8_t anm_anim_check(void)
{
	return AG(anm_newanim_posted);
}

void anm_cmd_quit(void)
{
	AG(anm_quit_flag) = 1;
}

int16_t anm_cmd_new(char *anmfile,uint16_t anm_flags,int16_t splice1,int16_t splice2)
{
	char anim[16];
	int ret;

	AG(anm_anmflags_new) = anm_flags;
	if (AG(anm_streamer_handle) != 0) {
		fle_close(AG(anm_streamer_handle));
		AG(anm_streamer_handle) = 0;
	}
	AG(anm_streamer_handle) = fle_open(anmfile, "rb");
	if (AG(anm_streamer_handle) == 0) {
		snprintf(AG(anm_errstr), 104, "Cannot open file %s\n", anmfile);
		return -4;
	}

	fle_cd_read(AG(anm_streamer_handle), anim, 16);
	if ((anim[0] == 'A') && (anim[1] == 'N') && (anim[2] = 'I') && (anim[3] == 'M')) {
		fle_streamer_seek(AG(anm_streamer_handle), 8);
		AG(anm_newanim_posted) = 1;
		anm_cmd_splice(splice2, splice1);
		ret = 0;
	} else {
		snprintf(AG(anm_errstr), 104, "Bad ANIM file %c%c%c%c\n", anim[0], anim[1], anim[2], anim[3]);
		fle_close(AG(anm_streamer_handle));
		ret = -2;
	}
	return ret;
}

void anm_cmd_restart(uint16_t anm_flags,uint16_t splice1,uint16_t splice2)
{
	fle_streamer_seek(AG(anm_streamer_handle), 8);
	AG(anm_anmflags_restart) = anm_flags;
	AG(anm_anim_started) = 1;
	anm_cmd_splice(splice2, splice1);
}

void anm_cmd_splice(uint16_t frme1,uint16_t frme2)
{
	AG(anm_do_splice) = 1;
	AG(anm_splice1) = frme1;
	AG(anm_splice2) = frme2;
}

void anm_cmd_clearscreen(void)
{
	AG(anm_clear_video_after_present) = 1;
}

uint16_t anm_cmd_pause(uint16_t pcmd)
{
	if (pcmd == 2) {
		if (AG(anm_pause_status) == 0)
			AG(anm_pause_status) = 1;
		else
			AG(anm_pause_status) = 0;
	} else if (pcmd == 4) {
		if (AG(anm_pause_status) != 0)
			AG(anm_pause_status) = 3;
	} else {
		AG(anm_pause_status) = pcmd;
	}
	return pcmd;
}

void anm_cmd_palette_changed(void)
{
	AG(anm_palette_changed) = 1;
}

void anm_video_reset(void)
{
	struct anm_rect *r = &(AG(anm_default_viewport));
	r->x = 4;
	r->y = 4;
	r->w = 312;
	r->h = 192;
	fob_camera_projection_curve_init(0, 1);
	fob_camera_projection_set_params(0, 0, 50);
	AG(anm_clear_video_after_present) = 0;
}

void anm_frame_display(uint8_t *buffer)
{
	if (AG(anm_palette_changed))
		vid_palette_clear();

	if (0 == (AG(anm_anmflags_global) & ANM_FLAG_NO_SCREENBLIT)) {
		struct anm_rect *r = &AG(anm_default_viewport);
		vid_blt_to_screen(buffer, r->x, r->y, r->x, r->y, r->w, r->h, 320);
	} else {
		AG(anm_anmflags_global) &= ~ANM_FLAG_NO_SCREENBLIT;
	}

	if (AG(anm_palette_changed)) {
		AG(anm_anim_header_current.animpal)[0] = 0;
		AG(anm_anim_header_current.animpal)[1] = 0;
		AG(anm_anim_header_current.animpal)[2] = 0;
		vid_palette_set(AG(anm_anim_header_current.animpal));
		AG(anm_palette_changed) = 0;
	}
	vid_buffer_flip(&AG(anm_default_dst));
	if (AG(anm_clear_video_after_present))
		anm_video_reset();
}

int16_t anm_resource_load(struct anm_res *res, char *filename, uint16_t numobjs)
{
	int16_t ret;

	if (filename == NULL)
		return 2;
	res->rescnt = 0;
	ret = res_resource_load(filename, &res->membase);
	if (ret == 0) {
		res->rescnt = anm_find_flobject_blocks(res->membase, &(res->ofsarr[0]), numobjs);
		ret = 0;
	} else {
		snprintf(AG(anm_errstr), 104, "Cannot find %s.", filename);
		ret = 1;
	}
	return ret;
}

int16_t anm_resource_free(struct anm_res *res)
{
	if (res->membase == NULL)
		return -1;
	else if (res->rescnt == 0)
		return -2;

	res->rescnt = 0;
	res_resource_free(&res->membase);
	return 0;
}

int16_t anm_resource_render(uint8_t *dst, struct anm_res *anmres, int16_t residx,
			    int16_t xoff, int16_t yoff, struct anm_rect *viewport,
			    uint16_t anm_flags)
{
	uint8_t *dp;
	uint32_t fs;

	if (viewport == NULL)
		viewport = &AG(anm_default_viewport);
	if (dst == NULL)
		dst = AG(anm_default_dst);
	if ((residx < 0) || (anmres->rescnt <= residx) || (anmres->membase == NULL))
		return 1;

	dp = anmres->membase + anmres->ofsarr[residx];
	fs = (__be32ua(dp + 12) & 0x00ffffff) - 14;
	fob_decode_render(dst, dp + 0x1e, NULL, fs, dp + 4, xoff, yoff, viewport, anm_flags);
	return 0;
}

uint16_t anm_find_flobject_blocks(uint8_t* data, ptrdiff_t *ofs, uint16_t maxcnt)
{
	uint8_t *s;
	uint32_t v1, v2;
	uint16_t v3;

	v3 = 0;
	v2 = __be32(data + 4);
	s = data + 8;
	while ((0 < v2) && (v3 < maxcnt)) {
		v1 = __be32ua(s + 4);
		if (v1 < 0)
			return v3;
		if (__be32ua(s + 0) == 0x46524d45) {	/* FRME */
			ofs[v3] = (ptrdiff_t)(s - data);
			v3++;
		}
		s += v1 + 8;
		v2 -= v1 + 8;
	}
	return v3;
}


int16_t anm_init(uint8_t *dst, anm_frme_postcb_t postcb, anm_frme_gamecb_t gamecb,
		 uint8_t *hidetbl, uint32_t flebufsize)
{
	int ret;
	ret = fle_streamer_init(flebufsize);
	if (ret != 0)
		return ret;

	AG(fle_bufsize) = flebufsize;
	AG(anm_frme_postcb) = postcb;
	AG(anm_frme_gamecb) = gamecb;
	AG(anm_hide_table) = hidetbl;
	if (dst == NULL) {
		AG(anm_use_existing_dstbuf) = -0;
		ret = vid_bitmap_alloc(&(AG(anm_default_dst)));
		if (ret != 0)
			goto out1;
	} else {
		AG(anm_use_existing_dstbuf) = -1;
		AG(anm_default_dst) = dst;
	}

	ret = mem_blk_alloc((void **)&(AG(anm_codec4_tilestore)), 0x4000);
	if (ret != 0)
		goto out2;
	ret = vid_statusbar_alloc((void **)&(AG(GAME_statusbar_6400buf)));
	if (ret != 0)
		goto out3;
	ret = anm_overlay_alloc((void **)&(AG(anm_overlay_buf)));
	if (ret != 0)
		goto out4;
	ret = sou_init();
	if (ret != 0)
		goto out5;

	fob_codec4_tilestore_reset();
	AG(anm_anmflags_global) = 0;
	AG(fle_streamer_fhandle) = 0;
	AG(anm_anim_frme_curr) = 0;
	AG(anm_animator_state) = 0;
	AG(anm_anim_frme_cnt) = 1;
	AG(anm_do_splice) = 0;
	AG(anm_newanim_posted) = 0;
	AG(anm_palette_changed) = 0;
	AG(anm_anim_started) = 0;
	AG(anm_clear_video_after_present) = 0;
	AG(anm_quit_flag) = 0;
	AG(anm_pause_status) = 0;
	anm_video_reset();
	return 0;

out5:
	mem_blk_free((void **)&(AG(anm_overlay_buf)));
out4:
	vid_statusbar_free((void **)&(AG(GAME_statusbar_6400buf)));
out3:
	mem_blk_free((void **)&(AG(anm_codec4_tilestore)));
out2:
	if (AG(anm_use_existing_dstbuf) == 0) {
		vid_bitmap_free(&(AG(anm_default_dst)));
	}
out1:
	fle_streamer_terminate();
	return ret;
}

void anm_terminate(void)
{
	sou_terminate();
	if (AG(fle_streamer_fhandle) != 0) {
		fle_close(AG(anm_streamer_handle));
		AG(anm_streamer_handle) = 0;
	}
	if (AG(anm_use_existing_dstbuf) == 0) {
		vid_bitmap_free(&(AG(anm_default_dst)));
	} else {
		AG(anm_default_dst) = 0;
	}
	mem_blk_free((void **)&(AG(anm_codec4_tilestore)));
	mem_blk_free((void **)&(AG(anm_overlay_buf)));
	fle_streamer_terminate();
}
