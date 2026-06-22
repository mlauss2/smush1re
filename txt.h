/*
 * SMUSHv1 RE
 *
 * txt - SMUSHv1 fontstore handling and text rendering.
 */

#ifndef _SMUSHV1_TXT_H_
#define _SMUSHV1_TXT_H_

/* SMUSHv1 font store entry:
 *  membase - pointer to font data in memory
 *  rescnt - number of glyphs stored.
 *  ofsarr - offset of glyph data from membase, for 256 glyphs.
 *
 * NOTE: this is a struct anm_res with 256 offsets.
 */
struct txt_font_v1 {
	void *membase;
	uint16_t rescnt;
	uint32_t ofsarr[256];
};

void txt_load_fonts(char **filelist,uint16_t *maxglyphs,uint16_t fontcnt);
void txt_free_fonts(void);

void txt_font_print(uint8_t *dst, struct anm_rect *viewport, int16_t xoff, int16_t yoff,
		    uint16_t anm_flags, uint16_t maxprintlen, char *text);
int16_t txt_font_print_len(uint8_t *dst, struct anm_rect *viewport, int16_t xoff,
			   int16_t yoff, uint16_t anm_flags, uint16_t maxprintlen,
			   uint16_t textlen, char *text);
void txt_font_printf(uint8_t *dst, struct anm_rect *viewport, int16_t xoff, int16_t yoff,
		     uint16_t anm_flags, uint16_t maxprintlen, char *fmtstr, ...);

#endif
