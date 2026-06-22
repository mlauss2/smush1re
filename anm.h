/*
 * SMUSHv1 RE
 *
 * anm - animator.
 */

#ifndef _SMUSH_AMN_H_
#define _SMUSH_AMN_H_

#include "smush.h"

#define ANM_FLAG_FOB_IGN_OFS		0x0001
#define ANM_FLAG_FOB_ORG_CENTER		0x0002
#define ANM_FLAG_FOB_C3FAST		0x0008
#define ANM_FLAG_FOB_OPAQUE		0x0100
#define ANM_FLAG_0x0080			0x0080
#define ANM_FLAG_FTCH_RESTORE		0x0800
#define ANM_FLAG_FOB_FLIPX		0x2000
#define ANM_FLAG_FOB_FLIPY		0x4000
#define ANM_FLAG_NO_SCREENBLIT		0x8000

/* a viewport rectangle: origin + extent */
struct anm_rect {
	int16_t x;
	int16_t y;
	int16_t w;
	int16_t h;
};

/* ANIM metadata (without the 8 byte ANIM/xxxx/AHDR/xxxx/ signatures */
struct anm_headerv1 {
	uint8_t		animversion;	/* zero or 1 */
	uint8_t		_pad1;
	uint16_t	frmecnt;	/* number of FRMEs, little-endian */
	uint16_t	_pad2;
	uint8_t		animpal[768];	/* palette */
};

/* anm resource: pointer to a memory base, number of resources, array of offsets
 * from membase that point to the individial resources.
 */
struct anm_res {
	void 		*membase;
	int16_t		rescnt;
	ptrdiff_t	ofsarr[];
};

/* int16_t postcb(uint8_t *dst, uint8_t *statusbar, uint16_t frmecurridx,
 *		struct anm_rect *viewport, uint16_t frmecntmax, uint32_t datasize);
 */
typedef int16_t(*anm_frme_postcb_t)(uint8_t*, uint8_t*, uint16_t,
				    struct anm_rect *, uint16_t, uint32_t);

/* void gamecb(uint8_t *dst, uint8_t *statusbar, struct anm_rect *viewport,
 *		uint8_t *anm_hide_table, uint16_t anm_flags, uint32_t gam1,
 *		uint32_t gam2, uint32_t gam3, uint32_t gam4, uint32_t gam5,
 * 		uint32_t gam6, uint32_t gam7);
 */
typedef void(*anm_frme_gamecb_t)(uint8_t*, uint8_t*, struct anm_rect *,
				 uint8_t*, uint16_t, uint32_t, uint32_t, uint32_t,
				 uint32_t, uint32_t, uint32_t, uint32_t);

/* INITIALIZE the smush system.
 * @dst:  	pointer to the image canvas to draw on
 * @postcb:	callback invoked after rendering a FRME
 * @gamecb:	callback invoked on GAME/GAM2 chunks
 * @hidetbl:	anm_hide_table: 256x8 bits
 * @flebufsize:	file streamer cache size.
 */
int16_t anm_init(uint8_t *dst, anm_frme_postcb_t postcb, anm_frme_gamecb_t gamecb,
		 uint8_t *hidetbl, uint32_t flebufsize);

void anm_terminate(void);

int16_t anm_animator_render(int16_t xoff,int16_t yoff,uint16_t anm_flags);
int16_t anm_process_frme(int16_t xoff,int16_t yoff,uint16_t anm_flags);
void anm_anim_reset(void);
void anm_frame_render(uint8_t *dst, uint8_t *statusbar, uint8_t *ovlbuf,
		      uint8_t *frmedata, uint32_t frmesize, uint8_t *c4tilestore,
		      int16_t xoff, int16_t yoff, struct anm_rect *viewport,
		      anm_frme_gamecb_t gamecb, uint8_t *anm_hide_table,
		      uint16_t anm_flags);						/* 00297c79 */
uint8_t anm_anim_check(void);
void anm_cmd_quit(void);
int16_t anm_cmd_new(char *anmfile,uint16_t anm_flags,int16_t splice1,int16_t splice2);
void anm_cmd_restart(uint16_t anm_flags,uint16_t splice1,uint16_t splice2);
void anm_cmd_splice(uint16_t frme1,uint16_t frme2);
void anm_cmd_clearscreen(void);
uint16_t anm_cmd_pause(uint16_t pcmd);
void anm_cmd_palette_changed(void);
void anm_video_reset(void);
void anm_frame_display(uint8_t *buffer);
int16_t anm_resource_load(struct anm_res *res, char *filename, uint16_t numobjs);
int16_t anm_resource_free(struct anm_res *res);
int16_t anm_resource_render(uint8_t *dst, struct anm_res *anmres, int16_t residx,
			    int16_t xoff, int16_t yoff, struct anm_rect *viewport,
			    uint16_t anm_flags);
uint16_t anm_find_flobject_blocks(uint8_t* data, ptrdiff_t *ofs, uint16_t maxcnt);

int16_t anm_overlay_alloc(void **ppOut);						/* 002a1438 */
void anm_overlay_set(uint8_t *ovlbuf, uint8_t *src, uint8_t *c4tilestore,
		     struct anm_rect *viewport);					/* 002a1445 */
void anm_overlay_show(uint8_t *dst, uint8_t *ovlbuf, uint8_t *c4tilestore,
		      int16_t xoff, int16_t yoff, struct anm_rect *viewport,
		      uint16_t anm_flags);						/* 002a14f6 */
void anm_process_SEGA(uint32_t s1, uint32_t s2, uint32_t s3, uint32_t s4);		/* 002a1555 */
void anm_process_fade(uint8_t *dst, uint8_t *src, uint16_t size);			/* 002a155a */

#endif
