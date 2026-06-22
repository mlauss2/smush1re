/*
 * SMUSHv1 RE
 *
 * txt - SMUSHv1 fontstore handling and text rendering.
 */

#include "smush.h"
#include <stdarg.h>

static const uint32_t pow10_table[10] = { 1, 10, 100, 1000, 10000, 100000,
					  1000000, 10000000, 100000000, 1000000000 };

void txt_load_fonts(char **filelist,uint16_t *maxglyphs,uint16_t fontcnt)
{
	int ret;

	AG(txt_fontcnt) = fontcnt;
	for (int i = 0; i < fontcnt; i++) {
		ret = anm_resource_load((struct anm_res *)&(AG(txt_fontstore)[i].membase), filelist[i], maxglyphs[i]);
		if (ret)
			break;
	}
}

void txt_free_fonts(void)
{
	for (int i = 0; i < anm_globals.txt_fontcnt; i++)
		anm_resource_free((struct anm_res *)&(AG(txt_fontstore)[i]));
}

void txt_font_print(uint8_t *dst, struct anm_rect *viewport, int16_t xoff, int16_t yoff,
		    uint16_t anm_flags, uint16_t maxprintlen, char *text)
{
	int i = strlen(text);
	txt_font_print_len(dst, viewport, xoff, yoff, anm_flags, maxprintlen, i, text);
}

int16_t txt_font_print_len(uint8_t *dst, struct anm_rect *viewport, int16_t xoff,
			   int16_t yoff, uint16_t anm_flags, uint16_t maxprintlen,
			   uint16_t textlen, char *text)
{
	int16_t xst;
	uint16_t uc;
	int selfont, width, tw, w;
	char *t2, t3;
	struct txt_font_v1 *fs;

	if (maxprintlen < 1)
		return 0;

	fs = &(AG(txt_fontstore)[0]);
	if (fs->membase == NULL)
		return 1;

	xst = xoff;
	while (textlen--) {
		xoff = xst;
		if ((anm_flags & 0x0204) != 0) {
			selfont = 0;
			width = 0;
			t2 = text;
			tw = textlen;
			while (tw--) {
				char c = *t2++;
				if (c == 0)
					break;
				if (c == 225)
					c = 127;
				if ((selfont == 2) && (c == 0x20))
					c = 0x21;
				if (c == '<') {
					selfont++;
					fs = &(AG(txt_fontstore)[selfont]);
				} else if (c == '>') {
					selfont--;
					fs = &(AG(txt_fontstore)[selfont]);
				} else {
					if (c < 0x21) {
						w = *(uint16_t *)(fs->membase + fs->ofsarr[0x27] + 0x16);
						w = (w > 8) ? 8 : w;
					} else {
						w = *(uint16_t *)(fs->membase + fs->ofsarr[c] + 0x16);
					}
					width += w;
				}
			}
			if ((anm_flags & 0x200) == 0) {
				xoff = xst - width;
			} else {
				xoff = xst - (width >> 1);
			}
		}

		selfont = 0;
		fs = &anm_globals.txt_fontstore[0];
		while ((t3 = *text) != 0) {
			if (t3 == 225)
				t3 = 127;
			if ((selfont == 2) && (t3 == 0x20))
				t3 = 0x21;
			if (t3 == '<') {
				selfont++;
				fs = &(AG(txt_fontstore)[selfont]);
			} else if (t3 == '>') {
				selfont--;
				fs = &(AG(txt_fontstore)[selfont]);
			} else if (t3 < 0x21) {
				w = *(uint16_t *)(fs->membase + fs->ofsarr[0x27] + 0x16);
				w = (w < 8) ? w : 8;
				xoff += w;
			} else {
				w = *(uint16_t *)(fs->membase + fs->ofsarr[t3 - 0x21] + 0x16);
				xoff += w;
				anm_resource_render(dst, (struct anm_res *)fs,
						    t3 - 0x21, xoff, yoff, viewport, anm_flags);
			}
			textlen--;
			text++;
			maxprintlen--;
			if (!maxprintlen)
				return 0;
		}
		yoff += *(uint16_t *)(fs->membase + fs->ofsarr[0x27] + 0x18) + 4;
		text++;
	}
}

void txt_font_printf(uint8_t *dst, struct anm_rect *viewport, int16_t xoff, int16_t yoff,
		     uint16_t anm_flags, uint16_t maxprintlen, char *fmtstr, ...)
{
	char buffer[80];
	char *out_ptr = buffer;
	va_list args;
	int16_t width = 0;
	uint8_t force_plus = 0;
	uint8_t zero_fill = 0;
	uint8_t is_formatting = 0;

	va_start(args, fmtstr);

	while (*fmtstr) {
		if (!is_formatting) {
			if (*fmtstr == '%') {
				is_formatting = 1;
				force_plus = 0;
				zero_fill = 0;
				width = 0;
				fmtstr++;
			} else {
				*out_ptr++ = *fmtstr++;
			}
		} else {
			char c = *fmtstr++;

			if (c == '+') {
				force_plus = 1;
			} else if (c == '0') {
				if (width == 0)
					zero_fill = 1;
				else
					width = (width * 10) + 0;
			} else if (c >= '1' && c <= '9') {
				width = (width * 10) + (c - '0');
			} else if (c == '%') {
				*out_ptr++ = '%';
				is_formatting = 0;
			} else if (c == 'c') {
				*out_ptr++ = (char)va_arg(args, int);
				is_formatting = 0;
			} else if (c == 's') {
				char *s = va_arg(args, char *);
				if (s) {
					while (*s)
						*out_ptr++ = *s++;
				}
				is_formatting = 0;
			} else if (c == 'd') {
				int32_t val = va_arg(args, int32_t);
				int32_t abs_val;
				int16_t actual_digits = 0;

				if (val < 0) {
					*out_ptr++ = '-';
					abs_val = -val;
				} else {
					if (force_plus)
						*out_ptr++ = '+';
					abs_val = val;
				}

				/* Determine number of digits needed */
				while (actual_digits < 10 && pow10_table[actual_digits] <= abs_val) {
					actual_digits++;
				}

				/* Padding: Field Width */
				while (actual_digits < width) {
					*out_ptr++ = zero_fill ? '0' : ' ';
					actual_digits++;
				}

				/* Digit conversion via subtraction from pow10_table */
				while (actual_digits > 0) {
					actual_digits--;
					uint8_t digit_val = 0;
					int32_t p = pow10_table[actual_digits];

					while (p != 0 && abs_val >= p) {
						abs_val -= p;
						digit_val++;
					}
					*out_ptr++ = digit_val + '0';
				}
				is_formatting = 0;
			} else {
				/* unknown fmt specifier */
				is_formatting = 0;
				out_ptr--;
				fmtstr--;
			}
		}
	}

	*out_ptr = '\0';
	va_end(args);

	txt_font_print(dst, viewport, xoff, yoff, anm_flags, maxprintlen, buffer);
}

