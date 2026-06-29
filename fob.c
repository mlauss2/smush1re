/*
 * SMUSHv1 RE
 *
 * fob - Frame OBject related decoders.
 *
 * codec1,2,3,4,5,20,21,23,(28,29,30),31,32,33,34 are implemented here based
 *  on the original assembly.
 */

#include "smush.h"


void fob_codec1_normal_work(uint8_t *dst, uint8_t *src, int16_t xoff, int16_t yoff,
			    uint16_t fobw, uint16_t fobh, uint16_t winx1, uint16_t winy1,
			    uint16_t winx2, uint16_t winy2)
{
	if ((yoff + fobh) < 0 || yoff >= 200 || (xoff + fobw) < 0 || xoff >= 320)
		return;

	int16_t skip_top = winy1 - yoff;
	if (skip_top > 0) {
		// Skip data in src for rows that are above the viewport
		for (int16_t i = 0; i < skip_top && i < fobh; i++) {
			uint16_t payload_len = __le16ua(src);
			src += 2 + payload_len;
		}
		yoff += skip_top;
		fobh -= skip_top;
	}

	int16_t skip_bottom = (yoff + fobh) - winy2;
	if (skip_bottom > 0) {
		fobh -= skip_bottom;
	}

	int16_t skip_left = winx1 - xoff;
	if (skip_left > 0) {
		xoff += skip_left;
		fobw -= skip_left;
	}

	int16_t skip_right = (xoff + fobw) - winx2;
	if (skip_right > 0) {
		fobw -= skip_right;
	}

	if (fobh <= 0 || fobw <= 0 || yoff >= 200 || xoff >= 320)
		return;

	for (uint16_t y = 0; y < fobh; y++) {
		int16_t current_y = yoff + y;
		if (current_y < 0 || current_y >= 200)
			continue;

		if (AG(sys_abort_flag) != 0) {
			sys_timer_continue();
		}

		uint16_t payload_len = __le16ua(src);
		src += 2;
		uint8_t *next_line_src = src + payload_len;
		uint8_t *line_src = src;
		uint8_t *line_dst = dst + (current_y * 320) + xoff;
		int16_t remaining_width = fobw;
		int16_t x_cursor = xoff;

		while (remaining_width > 0 && line_src < next_line_src) {
			uint8_t token = *line_src++;
			uint16_t count = (token >> 1) + 1;
			bool is_solid = (token & 1) != 0;

			if (is_solid) {
				uint8_t color = *line_src++;
				uint16_t draw_count = count;
				if (x_cursor + draw_count > 320) {
					draw_count = 320 - x_cursor;
				}
				if (draw_count > remaining_width) {
					draw_count = remaining_width;
				}

				if (draw_count > 0 && x_cursor >= 0) {
					if (color != 0x00) {
						memset(line_dst, color, draw_count);
					}
					line_dst += draw_count;
					x_cursor += draw_count;
				}
				remaining_width -= count;
			} else {
				uint16_t draw_count = count;
				if (draw_count > remaining_width) {
					draw_count = remaining_width;
				}

				for (uint16_t i = 0; i < draw_count; i++) {
					uint8_t pixel = *line_src++;
					if (x_cursor >= 0 && x_cursor < 320) {
						if (pixel != 0x00) {
							*line_dst = pixel;
						}
						line_dst++;
						x_cursor++;
					}
				}
				if (count > draw_count) {
					line_src += (count - draw_count);
				}
				remaining_width -= count;
			}
		}
		src = next_line_src;
	}
}

void fob_codec1_flipx_work(uint8_t *dst, uint8_t *src, int16_t xoff, int16_t yoff,
			    uint16_t fobw, uint16_t fobh, uint16_t winx1, uint16_t winy1,
			    uint16_t winx2, uint16_t winy2)
{
	winx1--;
	winx2--;
	xoff = xoff + fobw - 1;
	int16_t skip_top = winy1 - yoff;
	if (skip_top > 0) {
		yoff += skip_top;
		fobh -= skip_top;
		if (fobh <= 0)
			return;
		for (int16_t i = 0; i < skip_top; i++) {
			uint16_t payload_len = __le16ua(src);
			src += 2 + payload_len;
		}
	}

	int16_t skip_bottom = (yoff + fobh) - winy2;
	if (skip_bottom > 0) {
		fobh -= skip_bottom;
		if (fobh <= 0)
			return;
	}

	int16_t skip_stream_start = 0;
	int16_t right_overflow = xoff - winx2;
	if (right_overflow > 0) {
		xoff -= right_overflow;
		fobw -= right_overflow;
		if (fobw <= 0)
			return;
		skip_stream_start = right_overflow;
	}

	int16_t left_edge = xoff - fobw;
	int16_t left_overflow = winx1 - left_edge;
	if (left_overflow > 0) {
		fobw -= left_overflow;
		if (fobw <= 0)
			return;
	}

	uint8_t *dst_ptr = dst + (yoff * 320) + xoff;
	for (uint16_t y = 0; y < fobh; y++) {
		if (AG(sys_abort_flag) != 0) {
			sys_timer_continue();
		}
		uint16_t payload_len = __le16ua(src);
		src += 2;
		uint8_t *next_line_src = src + payload_len;
		uint8_t *line_src = src;
		uint8_t *line_dst = dst_ptr;
		int16_t remaining_stream_skip = skip_stream_start;
		int16_t remaining_width = fobw;
		while (remaining_width > 0 && line_src < next_line_src) {
			uint8_t token = *line_src++;
			uint16_t count = (token >> 1) + 1;
			bool is_solid = (token & 1) != 0;
			uint8_t color = 0;
			if (is_solid) {
				color = *line_src++;
			}
			if (remaining_stream_skip > 0) {
				if (count <= remaining_stream_skip) {
					remaining_stream_skip -= count;
					if (!is_solid)
						line_src += count;
					continue;
				} else {
					uint16_t visible_count = count - remaining_stream_skip;
					if (!is_solid)
						line_src += remaining_stream_skip;
					count = visible_count;
					remaining_stream_skip = 0;
				}
			}

			uint16_t draw_count = count;
			if (draw_count >= remaining_width) {
				draw_count = remaining_width;
			}
			if (is_solid) {
				if (color == 0x00) {
					line_dst -= draw_count;
				} else {
					for (uint16_t i = 0; i < draw_count; i++) {
						*line_dst-- = color;
					}
				}
			} else {
				for (uint16_t i = 0; i < draw_count; i++) {
					uint8_t pixel = *line_src++;
					if (pixel != 0x00) {
						*line_dst = pixel;
					}
					line_dst--;
				}
				if (count > draw_count) {
					line_src += (count - draw_count);
				}
			}
			remaining_width -= draw_count;
		}
		src = next_line_src;
		dst_ptr += 320;
	}
}

void fob_codec1_flipxy_work(uint8_t *dst, uint8_t *src, int16_t xoff, int16_t yoff,
			   uint16_t fobw, uint16_t fobh, uint16_t winx1, uint16_t winy1,
			   uint16_t winx2, uint16_t winy2)
{
	winx1--;
	winy1--;
	winx2--;
	winy2--;
	xoff = xoff + fobw - 1;
	yoff = yoff + fobh - 1;

	int16_t skip_stream_start_y = yoff - winy2;
	if (skip_stream_start_y > 0) {
		yoff -= skip_stream_start_y;
		fobh -= skip_stream_start_y;
		if (fobh <= 0)
			return;
		for (int16_t i = 0; i < skip_stream_start_y; i++) {
			uint16_t payload_len = __le16ua(src);
			src += 2 + payload_len;
		}
	}

	int16_t top_edge = yoff - fobh;
	int16_t top_overflow = winy1 - top_edge;
	if (top_overflow > 0) {
		fobh -= top_overflow;
		if (fobh <= 0)
			return;
	}

	int16_t skip_stream_start_x = 0;
	int16_t right_overflow = xoff - winx2;
	if (right_overflow > 0) {
		xoff -= right_overflow;
		fobw -= right_overflow;
		if (fobw <= 0)
			return;
		skip_stream_start_x = right_overflow;
	}

	int16_t left_edge = xoff - fobw;
	int16_t left_overflow = winx1 - left_edge;
	if (left_overflow > 0) {
		fobw -= left_overflow;
		if (fobw <= 0)
			return;
	}

	uint8_t *dst_ptr = dst + (yoff * 320) + xoff;
	for (uint16_t y = 0; y < fobh; y++) {
		if (AG(sys_abort_flag) != 0) {
			sys_timer_continue();
		}

		uint16_t payload_len = __le16ua(src);
		src += 2;
		uint8_t *next_line_src = src + payload_len;
		uint8_t *line_src = src;
		uint8_t *line_dst = dst_ptr;
		int16_t remaining_stream_skip = skip_stream_start_x;
		int16_t remaining_width = fobw;
		while (remaining_width > 0 && line_src < next_line_src) {
			uint8_t token = *line_src++;
			uint16_t count = (token >> 1) + 1;
			bool is_solid = (token & 1) != 0;
			uint8_t color = 0;
			if (is_solid) {
				color = *line_src++;
			}
			if (remaining_stream_skip > 0) {
				if (count <= remaining_stream_skip) {
					remaining_stream_skip -= count;
					if (!is_solid)
						line_src += count;
					continue;
				} else {
					uint16_t visible_count = count - remaining_stream_skip;
					if (!is_solid)
						line_src += remaining_stream_skip;
					count = visible_count;
					remaining_stream_skip = 0;
				}
			}
			uint16_t draw_count = count;
			if (draw_count >= remaining_width) {
				draw_count = remaining_width;
			}
			if (is_solid) {
				if (color == 0x00) {
					line_dst -= draw_count;
				} else {
					for (uint16_t i = 0; i < draw_count; i++) {
						*line_dst-- = color;
					}
				}
			} else {
				for (uint16_t i = 0; i < draw_count; i++) {
					uint8_t pixel = *line_src++;
					if (pixel != 0x00) {
						*line_dst = pixel;
					}
					line_dst--;
				}
				if (count > draw_count) {
					line_src += (count - draw_count);
				}
			}
			remaining_width -= draw_count;
		}
		src = next_line_src;
		dst_ptr -= 320;
	}
}

void fob_codec1_flipy_work(uint8_t *dst, uint8_t *src, int16_t xoff, int16_t yoff,
		     uint16_t fobw, uint16_t fobh, uint16_t winx1, uint16_t winy1,
		     uint16_t winx2, uint16_t winy2)
{
	winx1--;
	winy1--;
	winx2--;
	winy2--;
	xoff = xoff + fobw - 1;
	yoff = yoff + fobh - 1;
	int16_t skip_stream_start_y = yoff - winy2;
	if (skip_stream_start_y > 0) {
		yoff -= skip_stream_start_y;
		fobh -= skip_stream_start_y;
		if (fobh <= 0)
			return;
		for (int16_t i = 0; i < skip_stream_start_y; i++) {
			uint16_t payload_len = __le16ua(src);
			src += 2 + payload_len;
		}
	}

	int16_t top_edge = yoff - fobh;
	int16_t top_overflow = winy1 - top_edge;
	if (top_overflow > 0) {
		fobh -= top_overflow;
		if (fobh <= 0)
			return;
	}

	int16_t skip_stream_start_x = 0;
	int16_t right_overflow = xoff - winx2;
	if (right_overflow > 0) {
		xoff -= right_overflow;
		fobw -= right_overflow;
		if (fobw <= 0)
			return;
		skip_stream_start_x = right_overflow;
	}

	int16_t left_edge = xoff - fobw;
	int16_t left_overflow = winx1 - left_edge;
	if (left_overflow > 0) {
		fobw -= left_overflow;
		if (fobw <= 0)
			return;
	}

	uint8_t *dst_ptr = dst + (yoff * 320) + xoff;
	for (uint16_t y = 0; y < fobh; y++) {
		if (AG(sys_abort_flag) != 0) {
			sys_timer_continue();
		}
		uint16_t payload_len = __le16ua(src);
		src += 2;
		uint8_t *next_line_src = src + payload_len;
		uint8_t *line_src = src;
		uint8_t *line_dst = dst_ptr;
		int16_t remaining_stream_skip = skip_stream_start_x;
		int16_t remaining_width = fobw;
		while (remaining_width > 0 && line_src < next_line_src) {
			uint8_t token = *line_src++;
			uint16_t count = (token >> 1) + 1;
			bool is_solid = (token & 1) != 0;
			uint8_t color = 0;
			if (is_solid) {
				color = *line_src++;
			}
			if (remaining_stream_skip > 0) {
				if (count <= remaining_stream_skip) {
					remaining_stream_skip -= count;
					if (!is_solid)
						line_src += count;
					continue;
				} else {
					uint16_t visible_count = count - remaining_stream_skip;
					if (!is_solid)
						line_src += remaining_stream_skip;
					count = visible_count;
					remaining_stream_skip = 0;
				}
			}
			uint16_t draw_count = count;
			if (draw_count >= remaining_width) {
				draw_count = remaining_width;
			}
			if (is_solid) {
				if (color == 0x00) {
					line_dst -= draw_count;
				} else {
					for (uint16_t i = 0; i < draw_count; i++) {
						*line_dst-- = color;
					}
				}
			} else {
				for (uint16_t i = 0; i < draw_count; i++) {
					uint8_t pixel = *line_src++;
					if (pixel != 0x00) {
						*line_dst = pixel;
					}
					line_dst--;
				}
				if (count > draw_count) {
					line_src += (count - draw_count);
				}
			}
			remaining_width -= draw_count;
		}
		src = next_line_src;
		dst_ptr -= 320;
	}
}

void fob_codec3_work(uint8_t *dst, uint8_t *src, int16_t xoff, int16_t yoff,
		     uint16_t fobw, uint16_t fobh, uint16_t winx1, uint16_t winy1,
		     uint16_t winx2, uint16_t winy2)
{
	int16_t skip_top = winy1 - yoff;
	if (skip_top > 0) {
		yoff += skip_top;
		fobh -= skip_top;

		if (fobh <= 0)
			return;
		for (int16_t i = 0; i < skip_top; i++) {
			uint16_t payload_len = __le16ua(src);
			src += 2 + payload_len;
		}
	}

	int16_t skip_bottom = (yoff + fobh) - winy2;
	if (skip_bottom > 0) {
		fobh -= skip_bottom;
		if (fobh <= 0)
			return;
	}

	int16_t skip_left = winx1 - xoff;
	if (skip_left > 0) {
		xoff += skip_left;
		fobw -= skip_left;
		if (fobw <= 0)
			return;
	} else {
		skip_left = 0;
	}

	int16_t skip_right = (xoff + fobw) - winx2;
	if (skip_right > 0) {
		fobw -= skip_right;
		if (fobw <= 0)
			return;
	}

	uint8_t *dst_ptr = dst + (yoff * 320) + xoff;
	for (uint16_t y = 0; y < fobh; y++) {
		if (AG(sys_abort_flag) != 0) {
			sys_timer_continue();
		}

		uint16_t payload_len = __le16ua(src);
		src += 2;
		uint8_t *next_line_src = src + payload_len;
		uint8_t *line_src = src;
		uint8_t *line_dst = dst_ptr;
		int16_t remaining_left_clip = skip_left;
		int16_t remaining_width = fobw;
		while (remaining_width > 0 && line_src < next_line_src) {
			uint8_t token = *line_src++;
			uint16_t count = (token >> 1) + 1;
			bool is_solid = (token & 1) != 0;
			uint8_t color = 0;
			if (is_solid) {
				color = *line_src++;
			}
			if (remaining_left_clip > 0) {
				if (count <= remaining_left_clip) {
					remaining_left_clip -= count;
					if (!is_solid) {
						line_src += count;
					}
					continue;
				} else {
					uint16_t visible_count = count - remaining_left_clip;
					if (!is_solid) {
						line_src += remaining_left_clip;
					}
					count = visible_count;
					remaining_left_clip = 0;
				}
			}

			uint16_t draw_count = count;
			if (draw_count >= remaining_width) {
				draw_count = remaining_width;
			}
			if (is_solid) {
				memset(line_dst, color, draw_count);
			} else {
				memcpy(line_dst, line_src, draw_count);
				line_src += count;
			}
			line_dst += draw_count;
			remaining_width -= draw_count;
		}
		src = next_line_src;
		dst_ptr += 320;
	}
}

void fob_codec23_work(uint8_t *dst, uint8_t *src, int16_t xoff, int16_t yoff,
		      uint16_t fobw, uint16_t fobh, uint16_t winx1, uint16_t winy1,
		      uint16_t winx2, uint16_t winy2, uint8_t fob_p1)
{
	int16_t skip_top = winy1 - yoff;
	if (skip_top > 0) {
		yoff += skip_top;
		fobh -= skip_top;

		if (fobh <= 0)
			return;
		for (int16_t i = 0; i < skip_top; i++) {
			uint16_t payload_len = __le16ua(src);
			src += 2 + payload_len;
		}
	}

	int16_t skip_bottom = (yoff + fobh) - winy2;
	if (skip_bottom > 0) {
		fobh -= skip_bottom;
		if (fobh <= 0)
			return;
	}

	int16_t skip_left = winx1 - xoff;
	if (skip_left > 0) {
		xoff += skip_left;
		fobw -= skip_left;
		if (fobw <= 0)
			return;
	} else {
		skip_left = 0;
	}

	int16_t skip_right = (xoff + fobw) - winx2;
	if (skip_right > 0) {
		fobw -= skip_right;
		if (fobw <= 0)
			return;
	}

	uint8_t *dst_ptr = dst + (yoff * 320) + xoff;
	for (uint16_t y = 0; y < fobh; y++) {
		if (AG(sys_abort_flag) != 0) {
			sys_timer_continue();
		}
		uint16_t payload_len = __le16ua(src);
		src += 2;
		uint8_t *next_line_src = src + payload_len;
		uint8_t *line_src = src;
		uint8_t *line_dst = dst_ptr;
		int16_t remaining_left_clip = skip_left;
		int16_t remaining_width = fobw;
		while (remaining_width > 0 && line_src < next_line_src) {
			uint16_t skip_count = *line_src++;
			if (remaining_left_clip > 0) {
				if (skip_count <= remaining_left_clip) {
					remaining_left_clip -= skip_count;
					skip_count = 0;
				} else {
					skip_count -= remaining_left_clip;
					remaining_left_clip = 0;
				}
			}

			if (skip_count > 0) {
				uint16_t visible_skip = skip_count;
				if (visible_skip >= remaining_width) {
					visible_skip = remaining_width;
				}
				line_dst += visible_skip;
				remaining_width -= visible_skip;
			}

			if (remaining_width <= 0 || line_src >= next_line_src)
				break;

			uint16_t run_count = *line_src++;
			if (remaining_left_clip > 0) {
				if (run_count <= remaining_left_clip) {
					remaining_left_clip -= run_count;
					continue;
				} else {
					uint16_t visible_run = run_count - remaining_left_clip;
					run_count = visible_run;
					remaining_left_clip = 0;
				}
			}
			if (run_count > 0) {
				uint16_t draw_count = run_count;
				if (draw_count >= remaining_width) {
					draw_count = remaining_width;
				}
				for (uint16_t i = 0; i < draw_count; i++) {
					*line_dst = *line_dst + fob_p1;
					line_dst++;
				}
				remaining_width -= draw_count;
			}
		}
		src = next_line_src;
		dst_ptr += 320;
	}
}

void fob_codec21_work(uint8_t *dst, uint8_t *src, int16_t xoff, int16_t yoff,
		      uint16_t fobw, uint16_t fobh, uint16_t winx1, uint16_t winy1,
		      uint16_t winx2, uint16_t winy2)
{
	int16_t skip_top = winy1 - yoff;
	if (skip_top > 0) {
		yoff += skip_top;
		fobh -= skip_top;
		if (fobh <= 0)
			return;

		for (int16_t i = 0; i < skip_top; i++) {
			uint16_t payload_len = __le16ua(src);
			src += 2 + payload_len;
		}
	}

	int16_t skip_bottom = (yoff + fobh) - winy2;
	if (skip_bottom > 0) {
		fobh -= skip_bottom;
		if (fobh <= 0)
			return;
	}

	int16_t skip_left = winx1 - xoff;
	if (skip_left > 0) {
		xoff += skip_left;
		fobw -= skip_left;
		if (fobw <= 0)
			return;
	} else {
		skip_left = 0;
	}

	int16_t skip_right = (xoff + fobw) - winx2;
	if (skip_right > 0) {
		fobw -= skip_right;
		if (fobw <= 0)
			return;
	}

	uint8_t *dst_ptr = dst + (yoff * 320) + xoff;
	for (uint16_t y = 0; y < fobh; y++) {
		if (AG(sys_abort_flag) != 0) {
			sys_timer_continue();
		}

		uint16_t payload_len = __le16ua(src);
		src += 2;
		uint8_t *next_line_src = src + payload_len;
		uint8_t *line_src = src;
		uint8_t *line_dst = dst_ptr;
		int16_t remaining_left_clip = skip_left;
		int16_t remaining_width = fobw;
		while (remaining_width > 0 && line_src < next_line_src) {
			uint16_t skip_count = __le16ua(line_src);
			line_src += 2;

			if (remaining_left_clip > 0) {
				if (skip_count <= remaining_left_clip) {
					remaining_left_clip -= skip_count;
					skip_count = 0;
				} else {
					skip_count -= remaining_left_clip;
					remaining_left_clip = 0;
				}
			}

			if (skip_count > 0) {
				uint16_t visible_skip = skip_count;
				if (visible_skip >= remaining_width) {
					visible_skip = remaining_width;
				}
				line_dst += visible_skip;
				remaining_width -= visible_skip;
			}

			if (remaining_width <= 0)
				break;

			uint16_t run_count = __le16ua(line_src) + 1;
			line_src += 2;
			if (remaining_left_clip > 0) {
				if (run_count <= remaining_left_clip) {
					// Entire run is hidden off-screen left
					remaining_left_clip -= run_count;
					line_src += run_count;
					continue;
				} else {
					uint16_t visible_run = run_count - remaining_left_clip;
					line_src += remaining_left_clip;
					run_count = visible_run;
					remaining_left_clip = 0;
				}
			}
			if (run_count > 0) {
				uint16_t draw_count = run_count;
				if (draw_count >= remaining_width) {
					draw_count = remaining_width;
				}
				memcpy(line_dst, line_src, draw_count);
				line_src += run_count;
				line_dst += draw_count;
				remaining_width -= draw_count;
			}
		}
		src = next_line_src;
		dst_ptr += 320;
	}
}

void fob_codec20_work(uint8_t *dst, uint8_t *src, int16_t xoff, int16_t yoff,
		      uint16_t fobw, uint16_t fobh, uint16_t winx1, uint16_t winy1,
		      uint16_t winx2, uint16_t winy2)
{
	int32_t original_fobw = fobw;

	if (yoff < winy1) {
		int16_t diff = winy1 - yoff;
		yoff += diff;
		fobh -= diff;
		if (fobh <= 0)
			return;
		src += (diff * fobw);
	}

	dst += (yoff * 64) + (yoff * 256);
	if ((yoff + fobh) > winy2) {
		fobh = (winy2 - yoff) + 1;
		if (fobh <= 0)
			return;
	}

	int16_t src_x_skip = 0;
	if (xoff < winx1) {
		int16_t diff = winx1 - xoff;
		xoff += diff;
		fobw -= diff;
		if (fobw <= 0)
			return;
		src_x_skip = diff;
	}
	dst += xoff;
	if ((xoff + fobw) > winx2) {
		fobw = (winx2 - xoff) + 1;
		if (fobw <= 0)
			return;
	}

	int32_t src_stride = src_x_skip + (original_fobw - fobw);
	int32_t dst_stride = 320 - fobw;
	while (fobh > 0) {
		if (AG(sys_abort_flag) != 0) {
			sys_timer_continue();
		}
		int count = fobw;
		for (int i = 0; i < count; i++) {
			*dst++ = *src++;
		}
		src += src_stride;
		dst += dst_stride;

		fobh--;
	}
}

void fob_codec2_work(uint8_t *dst, uint8_t *src, int16_t xoff, int16_t yoff,
		     uint16_t fobw, uint16_t fobh, uint16_t winx1, uint16_t winy1,
		     uint16_t winx2, uint16_t winy2, uint32_t datasize)
{
	int xpos, ypos, xr, yr;

	xpos = xoff - winx1;
	dst += (yoff * 320) + xoff;
	ypos = yoff - winy1;
	xr = winx2 - winx1;
	yr = winy2 - winy1;
	while (datasize > 3) {
		xpos += (int16_t)__le16ua(src);
		ypos += src[2];
		if ((-1 < xpos) && (-1 < ypos) && (xpos < xr) && (ypos < yr))
			*dst = src[3];
		src += 4;
		datasize -= 4;
	}
}

/* in the original assembly, this is an optimized version of standard codec3,
 * using a few x86 assembly tricks for a speed boost with known dimensions.
 */
void fob_codec3_flag8_work(uint8_t *dst, uint8_t *src, int16_t xoff, int16_t yoff,
		      uint16_t fobw, uint16_t fobh, uint16_t winx1, uint16_t winy1,
		      uint16_t winx2, uint16_t winy2)
{
	/* 1. Viewport Y-Clipping (Top) */
	int16_t skip_top = winy1 - yoff;
	if (skip_top > 0) {
		yoff += skip_top;
		fobh -= skip_top;

		if (fobh <= 0)
			return;
		for (int16_t i = 0; i < skip_top; i++) {
			uint16_t payload_len = __le16ua(src);
			src += 2 + payload_len;
		}
	}

	int16_t skip_bottom = (yoff + fobh) - winy2;
	if (skip_bottom > 0) {
		fobh -= skip_bottom;
		if (fobh <= 0)
			return;
	}

	int16_t skip_left = winx1 - xoff;
	if (skip_left > 0) {
		xoff += skip_left;
		fobw -= skip_left;
		if (fobw <= 0)
			return;
	} else {
		skip_left = 0;
	}

	int16_t skip_right = (xoff + fobw) - winx2;
	if (skip_right > 0) {
		fobw -= skip_right;
		if (fobw <= 0)
			return;
	}

	uint8_t *dst_ptr = dst;
	uint16_t visible_width = fobw;
	for (uint16_t y = 0; y < fobh; y++) {
		if (AG(sys_abort_flag) != 0) {
			sys_timer_continue();
		}

		uint16_t payload_len = __le16ua(src);
		src += 2;
		uint8_t *next_line_src = src + payload_len;
		uint8_t *line_src = src;
		uint8_t *line_dst = dst_ptr;
		int16_t remaining_left_clip = skip_left;
		int16_t remaining_width = visible_width;
		while (remaining_width > 0 && line_src < next_line_src) {
			uint8_t token = *line_src++;
			uint16_t count = (token >> 1) + 1;
			bool is_solid = (token & 1) != 0;
			uint8_t color = 0;
			if (is_solid) {
				color = *line_src++;
			}
			if (remaining_left_clip > 0) {
				if (count <= remaining_left_clip) {
					remaining_left_clip -= count;
					if (!is_solid) {
						line_src += count;
					}
					continue;
				} else {
					uint16_t visible_count = count - remaining_left_clip;
					if (!is_solid) {
						line_src += remaining_left_clip;
					}
					count = visible_count;
					remaining_left_clip = 0;
				}
			}
			uint16_t draw_count = count;
			if (draw_count >= remaining_width) {
				draw_count = remaining_width;
			}
			if (is_solid) {
				memset(line_dst, color, draw_count);
			} else {
				memcpy(line_dst, line_src, draw_count);
				line_src += count;
			}
			line_dst += draw_count;
			remaining_width -= draw_count;
		}
		src = next_line_src;
		dst_ptr += visible_width;
	}
}

void fob_camera_apply_projection_curve(int16_t *xoff, int16_t *yoff)
{
	int idx;

	*xoff -= AG(fob_cam_proj_x);
	idx = 0x4f - (*xoff / 4);
	if (idx < 0)
		idx = 0;
	else if (idx > 0x4f)
		idx = 0x4f;
	*yoff = *yoff - ((AG(fob_cam_proj_y) + (AG(fob_cam_proj_extent) / 2)) - AG(fob_cam_proj_curve)[idx]);
}

void fob_camera_apply_projection_nocurve(int16_t *xoff, int16_t *yoff)
{
	*xoff -= AG(fob_cam_proj_x);
	*yoff -= AG(fob_cam_proj_y) + (AG(fob_cam_proj_extent) / 2);
}

void fob_camera_unapply_projection_curve(int16_t *xoff, int16_t *yoff)
{
	int idx;

	idx = 0x4f - (*xoff / 4);
	if (idx < 0)
		idx = 0;
	else if (idx > 0x4f)
		idx = 0x4f;
	*yoff = *yoff + ((AG(fob_cam_proj_y) + (AG(fob_cam_proj_extent) / 2)) - AG(fob_cam_proj_curve)[idx]);
	*xoff += AG(fob_cam_proj_x);
}

void fob_camera_projection_enable_randomness(void)
{
	AG(fob_cam_rand_en) = 0xff;
}

void fob_camera_projection_disable_randomness(void)
{
	AG(fob_cam_rand_en) = 0;
}

void fob_camera_projection_set_params(int16_t x, int16_t y, int unused)
{
	AG(fob_cam_proj_x) = x;
	AG(fob_cam_proj_y) = y;
	if (AG(fob_cam_rand_en) != 0) {
		AG(fob_cam_proj_x) += msc_random(5) - 2;
		AG(fob_cam_proj_y) += msc_random(5) - 2;
	}
}

void fob_camera_projection_curve_init(int16_t cval1,int16_t extent)
{
	int16_t v1, v3, v4, v5;
	int i;

	AG(fob_cam_proj_extent) = extent;

	v1 = cval1 / 2;
	v3 = 0;
	v5 = 0;
	if (v1 < 0) {
		v4 = v1;
		for (i = 0; i < 0x28; i++) {
			v5 -= v1;
			if (v3 < v5) {
				v3 += 0x28;
				v4 += 1;
			}
			AG(fob_cam_proj_curve)[i] = v4;
			AG(fob_cam_proj_curve)[0x4f - i] = -v4;
		}
	} else {
		v4 = v1;
		for (i = 0; i < 0x28; i++) {
			v5 += v1;
			if (v3 < v5) {
				v3 += 0x28;
				v4 -= 1;
			}
			AG(fob_cam_proj_curve)[i] = v4;
			AG(fob_cam_proj_curve)[0x4f - i] = -v4;

		}
	}
}

void fob_codec4(uint8_t *dst, uint8_t *src, int16_t xoff, int16_t yoff,
		uint16_t fobw, uint16_t fobh, uint16_t winx1, uint16_t winy1,
		uint16_t winx2, uint16_t winy2, uint8_t *tilestore, uint16_t fob_p2)
{
	int x1, y1, xb, yb, srcbytes_per_column, sv4;
	uint8_t *dst2, *src2;
	uint16_t *projp;

	if (AG(fobdec_c4_tilestore_idx) != 0) {
		tilestore += 0x2000;
	}

	x1 = xoff - AG(fob_cam_proj_x);
	y1 = yoff - (AG(fob_cam_proj_y) + (AG(fob_cam_proj_extent) / 2));
	src2 = src + (fob_p2 * 8);
	dst2 = dst + x1 + (y1 * 320);
	yb = (fobh + 3) / 4;	/* y blocks */
	if (fob_p2 == 0) {
		srcbytes_per_column = yb;
	} else {
		int c1 = yb / 8;
		int c2 = yb & 7;	/* remainder */
		srcbytes_per_column = (c1 * 9) + c2 + (c2 ? 1 : 0);
	}

	if (x1 < winx1) {
		sv4 = (winx1 + 3) - (x1 / 4);
		dst2 += (sv4 * 4);
		x1 += sv4 * 4;
		fobw -= sv4 * 4;
		src2 += sv4 * srcbytes_per_column;
	}
	if (winx2 < (fobw + x1 + 3)) {
		fobw = (winx2 - x1) - 3;
	}
	if (fobw < 1)
		return;
	xb = (fobw + 3) / 4;	/* x blocks */
	if (xb < 1)
		return;

	projp = &(AG(fob_cam_proj_curve)[79 - (x1 / 4)]);
	yb -= 1;
	srcbytes_per_column &= 0xff;
	if (AG(game_param_gfxquality) < 2) {
		if (fob_p2 == 0) {
			fob_codec4_lo(dst2, src2, tilestore, src, yb, projp, xb, yoff, winy1, winy2, srcbytes_per_column,
					AG(fobdec_c4_tilestore_param)[AG(fobdec_c4_tilestore_idx)], 0);
		} else {
			fob_codec4_lo_p2(dst2, src2, tilestore, src, yb, projp, xb, yoff, winy1, winy2, srcbytes_per_column,
					 AG(fobdec_c4_tilestore_param)[AG(fobdec_c4_tilestore_idx)], fob_p2);
		}
	} else {
		if (fob_p2 == 0) {
			fob_codec4_hi(dst2, src2, tilestore, src, yb, projp, xb, yoff, winy1, winy2, srcbytes_per_column,
				      AG(fobdec_c4_tilestore_param)[AG(fobdec_c4_tilestore_idx)], 0);
		} else {
			fob_codec4_hi_p2(dst2, src2, tilestore, src, yb, projp, xb, yoff, winy1, winy2, srcbytes_per_column,
					 AG(fobdec_c4_tilestore_param)[AG(fobdec_c4_tilestore_idx)], fob_p2);
		}
		
	}
}

void fob_codec5(uint8_t *dst, uint8_t *src, int16_t xoff, int16_t yoff,
		uint16_t fobw, uint16_t fobh, uint16_t winx1, uint16_t winy1,
		uint16_t winx2, uint16_t winy2, uint8_t *tilestore, uint16_t fob_p2)
{
	int x1, y1, xb, yb, srcbytes_per_column, sv4;
	uint8_t *dst2, *src2;
	uint16_t *projp;
	
	if (AG(fobdec_c4_tilestore_idx) != 0) {
		tilestore += 0x2000;
	}
	
	x1 = xoff - AG(fob_cam_proj_x);
	y1 = yoff - (AG(fob_cam_proj_y) + (AG(fob_cam_proj_extent) / 2));
	src2 = src + (fob_p2 * 8);
	dst2 = dst + x1 + (y1 * 320);
	yb = (fobh + 3) / 4;	/* y blocks */
	if (fob_p2 == 0) {
		srcbytes_per_column = yb;
	} else {
		int c1 = yb / 8;
		int c2 = yb & 7;	/* remainder */
		srcbytes_per_column = (c1 * 9) + c2 + (c2 ? 1 : 0);
	}

	if (x1 < winx1) {
		sv4 = (winx1 + 3) - (x1 / 4);
		dst2 += (sv4 * 4);
		x1 += sv4 * 4;
		fobw -= sv4 * 4;
		src2 += sv4 * srcbytes_per_column;
	}
	if (winx2 < (fobw + x1 + 3)) {
		fobw = (winx2 - x1) - 3;
	}
	if (fobw < 1)
		return;
	xb = (fobw + 3) / 4;	/* x blocks */
	if (xb < 1)
		return;
	
	projp = &(AG(fob_cam_proj_curve)[79 - (x1 / 4)]);
	yb -= 1;
	srcbytes_per_column &= 0xff;
	if (AG(game_param_gfxquality) < 2) {
		if (fob_p2 == 0) {
			fob_codec5_lo(dst2, src2, tilestore, src, yb, projp, xb, yoff, winy1, winy2, srcbytes_per_column,
				      AG(fobdec_c4_tilestore_param)[AG(fobdec_c4_tilestore_idx)], 0);
		} else {
			fob_codec5_lo_p2(dst2, src2, tilestore, src, yb, projp, xb, yoff,winy1, winy2, srcbytes_per_column,
					 AG(fobdec_c4_tilestore_param)[AG(fobdec_c4_tilestore_idx)], fob_p2);
		}
	} else {
		if (fob_p2 == 0) {
			fob_codec5_hi(dst2, src2, tilestore, src, yb, projp, xb, yoff, winy1, winy2, srcbytes_per_column,
				      AG(fobdec_c4_tilestore_param)[AG(fobdec_c4_tilestore_idx)], 0);
		} else {
			fob_codec5_hi_p2(dst2, src2, tilestore, src, yb, projp, xb, yoff,winy1, winy2, srcbytes_per_column,
					 AG(fobdec_c4_tilestore_param)[AG(fobdec_c4_tilestore_idx)], fob_p2);
		}
		
	}
}

void fob_codec4_lo(uint8_t *dst, uint8_t *csrc, uint8_t *tilestore, uint8_t *fobsrc,
		   uint16_t yblocks, uint16_t *proj, uint16_t xblocks, uint16_t yoff,
		   uint16_t winy1, uint16_t winy2, uint16_t srcbytes_per_column,
		   uint8_t fob_p1, uint16_t fob_p2)
{
	uint8_t *col_dst = dst;
	uint8_t *col_csrc = csrc;

	while (xblocks > 0) {
		if (AG(sys_abort_flag) == 0) {
			sys_timer_continue();
		}

		uint16_t proj_val = *proj;
		proj--;

		uint32_t fb_y_offset = (proj_val << 6) + (proj_val << 8);
		uint8_t *current_dst = col_dst + fb_y_offset;
		int16_t skip_top_blocks = 0;
		int16_t skip_bottom_blocks = 0;
		int16_t block_start_y = yoff + proj_val;

		if (block_start_y < winy1) {
			skip_top_blocks = (winy1 - block_start_y + 3) >> 2;
		}

		int16_t block_end_y = block_start_y + (yblocks << 2);
		if (block_end_y > winy2) {
			skip_bottom_blocks = (block_end_y - winy2 + 3) >> 2;
		}

		int16_t blocks_to_draw = yblocks - skip_bottom_blocks - skip_top_blocks;
		if (blocks_to_draw > 0) {
			uint8_t *draw_src = col_csrc + skip_top_blocks;
			uint8_t *draw_dst = current_dst + (skip_top_blocks * 1280);
			while (blocks_to_draw > 0) {
				uint8_t tile_idx = *draw_src++;
				if (tile_idx == 0x80) {
					draw_dst += 1280;
				} else {
					uint32_t *t32 = (uint32_t *)(tilestore + (tile_idx << 4));
					uint32_t *d32 = (uint32_t *)draw_dst;

					*d32 = t32[0]; draw_dst += 320; d32 = (uint32_t *)draw_dst;
					*d32 = t32[1]; draw_dst += 320; d32 = (uint32_t *)draw_dst;
					*d32 = t32[2]; draw_dst += 320; d32 = (uint32_t *)draw_dst;
					*d32 = t32[3]; draw_dst += 320;
				}

				blocks_to_draw--;
			}
		}
		col_dst += 4;
		col_csrc += srcbytes_per_column;
		xblocks--;
	}
}

void fob_codec4_lo_p2(uint8_t *dst, uint8_t *csrc, uint8_t *tilestore, uint8_t *fobsrc,
		      uint16_t yblocks, uint16_t *proj, uint16_t xblocks, uint16_t yoff,
		      uint16_t winy1, uint16_t winy2, uint16_t srcbytes_per_column,
		      uint8_t fob_p1, uint16_t fob_p2)
{
	if (fob_p2 != 0) {
		uint8_t *custom_dst = tilestore + 0x1000;
		uint8_t *unpack_src = fobsrc;
		uint16_t unpack_loops = fob_p2 << 2;

		for (uint16_t i = 0; i < unpack_loops; i++) {
			uint8_t byte1 = *unpack_src++;
			*custom_dst++ = (byte1 >> 4)   + fob_p1;
			*custom_dst++ = (byte1 & 0x0F) + fob_p1;

			byte1 = *unpack_src++;
			*custom_dst++ = (byte1 >> 4)   + fob_p1;
			*custom_dst++ = (byte1 & 0x0F) + fob_p1;
		}
	}

	uint8_t *col_dst = dst;
	uint8_t *col_csrc = csrc;

	while (xblocks > 0) {
		if (AG(sys_abort_flag) == 0) {
			sys_timer_continue();
		}

		uint16_t proj_val = *proj;
		proj--;

		uint32_t fb_y_offset = (proj_val << 6) + (proj_val << 8);
		uint8_t *current_dst = col_dst + fb_y_offset;

		int16_t skip_top_blocks = 0;
		int16_t skip_bottom_blocks = 0;
		int16_t block_start_y = yoff + proj_val;

		if (block_start_y < winy1) {
			skip_top_blocks = (winy1 - block_start_y + 3) >> 2;
		}

		int16_t block_end_y = block_start_y + (yblocks << 2);
		if (block_end_y > winy2) {
			skip_bottom_blocks = (block_end_y - winy2 + 3) >> 2;
		}

		int16_t blocks_to_draw = yblocks - skip_bottom_blocks - skip_top_blocks;

		if (blocks_to_draw <= 0) {
			return;
		}

		int16_t skip_chunks = skip_top_blocks >> 3;
		int16_t skip_rem    = skip_top_blocks & 0x07;

		uint8_t *draw_src = col_csrc + (skip_chunks * 9);
		uint16_t control_word = (*draw_src) | 0x0100;

		if (skip_rem != 0) {
			draw_src += skip_rem;
			control_word <<= skip_rem;
		}
		draw_src++;

		uint8_t *draw_dst = current_dst + (skip_top_blocks * 1280);

		while (blocks_to_draw > 0) {
			uint8_t tile_idx = *draw_src++;
			int use_custom = (control_word & 0x0080) != 0;

			if (use_custom) {
				uint32_t *t32 = (uint32_t *)(tilestore + 0x1000 + (tile_idx << 4));
				uint32_t *d32 = (uint32_t *)draw_dst;

				*d32 = t32[0]; draw_dst += 320; d32 = (uint32_t *)draw_dst;
				*d32 = t32[1]; draw_dst += 320; d32 = (uint32_t *)draw_dst;
				*d32 = t32[2]; draw_dst += 320; d32 = (uint32_t *)draw_dst;
				*d32 = t32[3]; draw_dst += 320;
			} else {
				if (tile_idx == 0x80) {
					draw_dst += 1280;
				} else {
					uint32_t *t32 = (uint32_t *)(tilestore + (tile_idx << 4));
					uint32_t *d32 = (uint32_t *)draw_dst;

					*d32 = t32[0]; draw_dst += 320; d32 = (uint32_t *)draw_dst;
					*d32 = t32[1]; draw_dst += 320; d32 = (uint32_t *)draw_dst;
					*d32 = t32[2]; draw_dst += 320; d32 = (uint32_t *)draw_dst;
					*d32 = t32[3]; draw_dst += 320;
				}
			}

			int carry = (control_word & 0x8000) != 0;
			control_word <<= 1;

			if (carry) {
				control_word = (*draw_src) | 0x0100;
				draw_src++;
			}

			blocks_to_draw--;
		}

		col_dst += 4;
		col_csrc += srcbytes_per_column;
		xblocks--;
	}
}

void fob_codec4_hi(uint8_t *dst, uint8_t *csrc, uint8_t *tilestore, uint8_t *fobsrc,
		   uint16_t yblocks, uint16_t *proj, uint16_t xblocks, uint16_t yoff,
		   uint16_t winy1, uint16_t winy2, uint16_t srcbytes_per_column,
		   uint8_t fob_p1, uint16_t fob_p2)
{
	uint8_t *col_dst = dst;
	uint8_t *col_csrc = csrc;

	while (xblocks > 0) {
		if (AG(sys_abort_flag) == 0) {
			sys_timer_continue();
		}

		uint16_t proj_val = *proj;
		proj--;

		uint32_t fb_y_offset = (proj_val << 6) + (proj_val << 8);
		uint8_t *current_dst = col_dst + fb_y_offset;

		int16_t skip_top_blocks = 0;
		int16_t skip_bottom_blocks = 0;
		int16_t block_start_y = yoff + proj_val;

		if (block_start_y < winy1) {
			skip_top_blocks = (winy1 - block_start_y + 3) >> 2;
		}

		int16_t block_end_y = block_start_y + (yblocks << 2);
		if (block_end_y > winy2) {
			skip_bottom_blocks = (block_end_y - winy2 + 3) >> 2;
		}

		int16_t blocks_to_draw = yblocks - skip_bottom_blocks - skip_top_blocks;
		if (blocks_to_draw <= 0) {
			return;
		}

		uint8_t *draw_src = col_csrc + skip_top_blocks;
		uint8_t *draw_dst = current_dst + (skip_top_blocks * 1280);

		if ((fob_p1 & 0x80) != 0) {
			while (blocks_to_draw > 0) {
				uint8_t tile_idx = *draw_src++;

				if (tile_idx == 0x80) {
					draw_dst += 1280;
				} else {
					uint32_t *t32 = (uint32_t *)(tilestore + (tile_idx << 4));
					uint32_t *d32 = (uint32_t *)draw_dst;

					*d32 = t32[0]; draw_dst += 320; d32 = (uint32_t *)draw_dst;
					*d32 = t32[1]; draw_dst += 320; d32 = (uint32_t *)draw_dst;
					*d32 = t32[2]; draw_dst += 320; d32 = (uint32_t *)draw_dst;
					*d32 = t32[3]; draw_dst += 320;

					draw_dst[-960] = ((draw_dst[-961] + draw_dst[-960]) >> 1) | 0x80;
					draw_dst[-640] = ((draw_dst[-641] + draw_dst[-640]) >> 1) | 0x80;
					draw_dst[-320] = ((draw_dst[-321] + draw_dst[-320]) >> 1) | 0x80;

					uint32_t top_row = *(uint32_t *)(draw_dst - 1600);
					uint32_t cur_row = *(uint32_t *)(draw_dst - 1280);
					*(uint32_t *)(draw_dst - 1280) = ((top_row + cur_row) >> 1) | 0x80808080;
				}

				blocks_to_draw--;
			}

		} else {
			while (blocks_to_draw > 0) {
				uint8_t tile_idx = *draw_src++;

				uint32_t *t32 = (uint32_t *)(tilestore + (tile_idx << 4));
				uint32_t *d32 = (uint32_t *)draw_dst;

				*d32 = t32[0]; draw_dst += 320; d32 = (uint32_t *)draw_dst;
				*d32 = t32[1]; draw_dst += 320; d32 = (uint32_t *)draw_dst;
				*d32 = t32[2]; draw_dst += 320; d32 = (uint32_t *)draw_dst;
				*d32 = t32[3]; draw_dst += 320;

				draw_dst[-960] = ((draw_dst[-961] + draw_dst[-960]) >> 1) & 0x7F;
				draw_dst[-640] = ((draw_dst[-641] + draw_dst[-640]) >> 1) & 0x7F;
				draw_dst[-320] = ((draw_dst[-321] + draw_dst[-320]) >> 1) & 0x7F;

				uint32_t top_row = *(uint32_t *)(draw_dst - 1600);
				uint32_t cur_row = *(uint32_t *)(draw_dst - 1280);
				*(uint32_t *)(draw_dst - 1280) = ((top_row + cur_row) >> 1) & 0x7F7F7F7F;

				blocks_to_draw--;
			}
		}
		col_dst += 4;
		col_csrc += srcbytes_per_column;
		xblocks--;
	}
}

void fob_codec4_hi_p2(uint8_t *dst, uint8_t *csrc, uint8_t *tilestore, uint8_t *fobsrc,
		      uint16_t yblocks, uint16_t *proj, uint16_t xblocks, uint16_t yoff,
		      uint16_t winy1, uint16_t winy2, uint16_t srcbytes_per_column,
		      uint8_t fob_p1, uint16_t fob_p2)
{
	if (fob_p2 != 0) {
		uint8_t *custom_dst = tilestore + 0x1000;
		uint8_t *unpack_src = fobsrc;
		uint16_t unpack_loops = fob_p2 << 2;

		for (uint16_t i = 0; i < unpack_loops; i++) {
			uint8_t byte1 = *unpack_src++;
			*custom_dst++ = (byte1 >> 4)   + fob_p1;
			*custom_dst++ = (byte1 & 0x0F) + fob_p1;

			byte1 = *unpack_src++;
			*custom_dst++ = (byte1 >> 4)   + fob_p1;
			*custom_dst++ = (byte1 & 0x0F) + fob_p1;
		}
	}

	uint8_t *col_dst = dst;
	uint8_t *col_csrc = csrc;

	while (xblocks > 0) {
		if (AG(sys_abort_flag) == 0) {
			sys_timer_continue();
		}

		uint16_t proj_val = *proj;
		proj--;

		uint32_t fb_y_offset = (proj_val << 6) + (proj_val << 8);
		uint8_t *current_dst = col_dst + fb_y_offset;

		int16_t skip_top_blocks = 0;
		int16_t skip_bottom_blocks = 0;
		int16_t block_start_y = yoff + proj_val;

		if (block_start_y < winy1) {
			skip_top_blocks = (winy1 - block_start_y + 3) >> 2;
		}

		int16_t block_end_y = block_start_y + (yblocks << 2);
		if (block_end_y > winy2) {
			skip_bottom_blocks = (block_end_y - winy2 + 3) >> 2;
		}

		int16_t blocks_to_draw = yblocks - skip_bottom_blocks - skip_top_blocks;

		if (blocks_to_draw <= 0) {
			return;
		}

		int16_t skip_chunks = skip_top_blocks >> 3;
		int16_t skip_rem    = skip_top_blocks & 0x07;

		uint8_t *draw_src = col_csrc + (skip_chunks * 9);
		uint16_t control_word = (*draw_src) | 0x0100;

		if (skip_rem != 0) {
			draw_src += skip_rem;
			control_word <<= skip_rem;
		}
		draw_src++;

		uint8_t *draw_dst = current_dst + (skip_top_blocks * 1280);

		if ((fob_p1 & 0x80) != 0) {
			while (blocks_to_draw > 0) {
				uint8_t tile_idx = *draw_src++;
				int use_custom = (control_word & 0x0080) != 0;

				if (use_custom) {
					uint32_t *t32 = (uint32_t *)(tilestore + 0x1000 + (tile_idx << 4));
					uint32_t *d32 = (uint32_t *)draw_dst;

					*d32 = t32[0]; draw_dst += 320; d32 = (uint32_t *)draw_dst;
					*d32 = t32[1]; draw_dst += 320; d32 = (uint32_t *)draw_dst;
					*d32 = t32[2]; draw_dst += 320; d32 = (uint32_t *)draw_dst;
					*d32 = t32[3]; draw_dst += 320;

					draw_dst[-960] = ((draw_dst[-961] + draw_dst[-960]) >> 1) | 0x80;
					draw_dst[-640] = ((draw_dst[-641] + draw_dst[-640]) >> 1) | 0x80;
					draw_dst[-320] = ((draw_dst[-321] + draw_dst[-320]) >> 1) | 0x80;

					uint32_t top_row = *(uint32_t *)(draw_dst - 1600);
					uint32_t cur_row = *(uint32_t *)(draw_dst - 1280);
					*(uint32_t *)(draw_dst - 1280) = ((top_row + cur_row) >> 1) | 0x80808080;
				} else {
					if (tile_idx == 0x80) {
						draw_dst += 1280;
					} else {
						uint32_t *t32 = (uint32_t *)(tilestore + (tile_idx << 4));
						uint32_t *d32 = (uint32_t *)draw_dst;

						*d32 = t32[0]; draw_dst += 320; d32 = (uint32_t *)draw_dst;
						*d32 = t32[1]; draw_dst += 320; d32 = (uint32_t *)draw_dst;
						*d32 = t32[2]; draw_dst += 320; d32 = (uint32_t *)draw_dst;
						*d32 = t32[3]; draw_dst += 320;

						draw_dst[-960] = ((draw_dst[-961] + draw_dst[-960]) >> 1) | 0x80;
						draw_dst[-640] = ((draw_dst[-641] + draw_dst[-640]) >> 1) | 0x80;
						draw_dst[-320] = ((draw_dst[-321] + draw_dst[-320]) >> 1) | 0x80;

						uint32_t top_row = *(uint32_t *)(draw_dst - 1600);
						uint32_t cur_row = *(uint32_t *)(draw_dst - 1280);
						*(uint32_t *)(draw_dst - 1280) = ((top_row + cur_row) >> 1) | 0x80808080;
					}
				}

				int carry = (control_word & 0x8000) != 0;
				control_word <<= 1;
				if (carry) {
					control_word = (*draw_src) | 0x0100;
					draw_src++;
				}
				blocks_to_draw--;
			}
		} else {
			while (blocks_to_draw > 0) {
				uint8_t tile_idx = *draw_src++;
				int use_custom = (control_word & 0x0080) != 0;

				if (use_custom) {
					uint32_t *t32 = (uint32_t *)(tilestore + 0x1000 + (tile_idx << 4));
					uint32_t *d32 = (uint32_t *)draw_dst;

					*d32 = t32[0]; draw_dst += 320; d32 = (uint32_t *)draw_dst;
					*d32 = t32[1]; draw_dst += 320; d32 = (uint32_t *)draw_dst;
					*d32 = t32[2]; draw_dst += 320; d32 = (uint32_t *)draw_dst;
					*d32 = t32[3]; draw_dst += 320;

					draw_dst[-960] = ((draw_dst[-961] + draw_dst[-960]) >> 1) & 0x7F;
					draw_dst[-640] = ((draw_dst[-641] + draw_dst[-640]) >> 1) & 0x7F;
					draw_dst[-320] = ((draw_dst[-321] + draw_dst[-320]) >> 1) & 0x7F;

					uint32_t top_row = *(uint32_t *)(draw_dst - 1600);
					uint32_t cur_row = *(uint32_t *)(draw_dst - 1280);
					*(uint32_t *)(draw_dst - 1280) = ((top_row + cur_row) >> 1) & 0x7F7F7F7F;
				} else {
					if (tile_idx == 0x80) {
						draw_dst += 1280;
					} else {
						uint32_t *t32 = (uint32_t *)(tilestore + (tile_idx << 4));
						uint32_t *d32 = (uint32_t *)draw_dst;

						*d32 = t32[0]; draw_dst += 320; d32 = (uint32_t *)draw_dst;
						*d32 = t32[1]; draw_dst += 320; d32 = (uint32_t *)draw_dst;
						*d32 = t32[2]; draw_dst += 320; d32 = (uint32_t *)draw_dst;
						*d32 = t32[3]; draw_dst += 320;

						draw_dst[-960] = ((draw_dst[-961] + draw_dst[-960]) >> 1) & 0x7F;
						draw_dst[-640] = ((draw_dst[-641] + draw_dst[-640]) >> 1) & 0x7F;
						draw_dst[-320] = ((draw_dst[-321] + draw_dst[-320]) >> 1) & 0x7F;

						uint32_t top_row = *(uint32_t *)(draw_dst - 1600);
						uint32_t cur_row = *(uint32_t *)(draw_dst - 1280);
						*(uint32_t *)(draw_dst - 1280) = ((top_row + cur_row) >> 1) & 0x7F7F7F7F;
					}
				}

				int carry = (control_word & 0x8000) != 0;
				control_word <<= 1;
				if (carry) {
					control_word = (*draw_src) | 0x0100;
					draw_src++;
				}

				blocks_to_draw--;
			}
		}
		col_dst += 4;
		col_csrc += srcbytes_per_column;
		xblocks--;
	}
}

void fob_codec5_lo(uint8_t *dst, uint8_t *csrc, uint8_t *tilestore, uint8_t *fobsrc,
		   uint16_t yblocks, uint16_t *proj, uint16_t xblocks, uint16_t yoff,
		   uint16_t winy1, uint16_t winy2, uint16_t srcbytes_per_column,
		   uint8_t fob_p1, uint16_t fob_p2)
{
	uint8_t *col_dst = dst;
	uint8_t *col_csrc = csrc;

	while (xblocks > 0) {
		if (AG(sys_abort_flag) == 0) {
			sys_timer_continue();
		}
		uint16_t proj_val = *proj;
		proj--;

		uint32_t fb_y_offset = proj_val * 320;
		uint8_t *current_dst = col_dst + fb_y_offset;
		int16_t skip_top_blocks = 0;
		int16_t skip_bottom_blocks = 0;
		int16_t block_start_y = yoff + proj_val;

		if (block_start_y < winy1) {
			skip_top_blocks = (winy1 - block_start_y + 3) >> 2;
		}

		int16_t block_end_y = block_start_y + (yblocks << 2);
		if (block_end_y > winy2) {
			skip_bottom_blocks = (block_end_y - winy2 + 3) >> 2;
		}

		int16_t blocks_to_draw = yblocks - skip_bottom_blocks - skip_top_blocks;

		if (blocks_to_draw > 0) {
			uint8_t *draw_src = col_csrc + skip_top_blocks;
			uint8_t *draw_dst = current_dst + (skip_top_blocks * 1280);

			while (blocks_to_draw > 0) {
				uint8_t tile_idx = *draw_src++;
				uint32_t *tile_data = (uint32_t *)(tilestore + (tile_idx << 4));
				uint32_t *d32 = (uint32_t *)draw_dst;

				*d32 = tile_data[0];
				draw_dst += 320;
				d32 = (uint32_t *)draw_dst;

				*d32 = tile_data[1];
				draw_dst += 320;
				d32 = (uint32_t *)draw_dst;

				*d32 = tile_data[2];
				draw_dst += 320;
				d32 = (uint32_t *)draw_dst;

				*d32 = tile_data[3];
				draw_dst += 320;

				blocks_to_draw--;
			}
		}

		col_dst += 4;
		col_csrc += srcbytes_per_column;
		xblocks--;
	}
}

void fob_codec5_lo_p2(uint8_t *dst, uint8_t *csrc, uint8_t *tilestore, uint8_t *fobsrc,
		      uint16_t yblocks, uint16_t *proj, uint16_t xblocks, uint16_t yoff,
		      uint16_t winy1, uint16_t winy2, uint16_t srcbytes_per_column,
		      uint8_t fob_p1, uint16_t fob_p2)
{
	if (fob_p2 != 0) {
		uint8_t *custom_dst = tilestore + 0x1000;
		uint8_t *unpack_src = fobsrc;
		uint16_t unpack_loops = fob_p2 << 2;

		for (uint16_t i = 0; i < unpack_loops; i++) {
			uint8_t byte1 = *unpack_src++;
			*custom_dst++ = (byte1 >> 4)   + fob_p1;
			*custom_dst++ = (byte1 & 0x0F) + fob_p1;
			byte1 = *unpack_src++;
			*custom_dst++ = (byte1 >> 4)   + fob_p1;
			*custom_dst++ = (byte1 & 0x0F) + fob_p1;
		}
	}

	uint8_t *col_dst = dst;
	uint8_t *col_csrc = csrc;

	while (xblocks > 0) {
		if (AG(sys_abort_flag) == 0) {
			sys_timer_continue();
		}

		uint16_t proj_val = *proj;
		proj--;

		uint32_t fb_y_offset = (proj_val << 6) + (proj_val << 8);
		uint8_t *current_dst = col_dst + fb_y_offset;
		int16_t skip_top_blocks = 0;
		int16_t skip_bottom_blocks = 0;
		int16_t block_start_y = yoff + proj_val;

		if (block_start_y < winy1) {
			skip_top_blocks = (winy1 - block_start_y + 3) >> 2;
		}

		int16_t block_end_y = block_start_y + (yblocks << 2);
		if (block_end_y > winy2) {
			skip_bottom_blocks = (block_end_y - winy2 + 3) >> 2;
		}

		int16_t blocks_to_draw = yblocks - skip_bottom_blocks - skip_top_blocks;

		if (blocks_to_draw <= 0) {
			return;
		}

		int16_t skip_chunks = skip_top_blocks >> 3;
		int16_t skip_rem    = skip_top_blocks & 0x07;
		uint8_t *draw_src = col_csrc + (skip_chunks * 9);
		uint16_t control_word = (*draw_src) | 0x0100;
		if (skip_rem != 0) {
			draw_src += skip_rem;
			control_word <<= skip_rem;
		}
		draw_src++;

		uint8_t *draw_dst = current_dst + (skip_top_blocks * 1280);
		while (blocks_to_draw > 0) {
			uint8_t tile_idx = *draw_src++;
			int use_custom = (control_word & 0x0080) != 0;

			uint32_t *t32;
			if (use_custom) {
				t32 = (uint32_t *)(tilestore + 0x1000 + (tile_idx << 4));
			} else {
				t32 = (uint32_t *)(tilestore + (tile_idx << 4));
			}
			uint32_t *d32 = (uint32_t *)draw_dst;

			*d32 = t32[0]; draw_dst += 320; d32 = (uint32_t *)draw_dst;
			*d32 = t32[1]; draw_dst += 320; d32 = (uint32_t *)draw_dst;
			*d32 = t32[2]; draw_dst += 320; d32 = (uint32_t *)draw_dst;
			*d32 = t32[3]; draw_dst += 320;

			int carry = (control_word & 0x8000) != 0;
			control_word <<= 1;

			if (carry) {
				control_word = (*draw_src) | 0x0100; // Load next control byte & sentinel
				draw_src++;
			}

			blocks_to_draw--;
		}

		col_dst += 4;
		col_csrc += srcbytes_per_column;
		xblocks--;
	}
}

void fob_codec5_hi(uint8_t *dst, uint8_t *csrc, uint8_t *tilestore, uint8_t *fobsrc,
		   uint16_t yblocks, uint16_t *proj, uint16_t xblocks, uint16_t yoff,
		   uint16_t winy1, uint16_t winy2, uint16_t srcbytes_per_column,
		   uint8_t fob_p1, uint16_t fob_p2)
{
	uint8_t *col_dst = dst;
	uint8_t *col_csrc = csrc;

	while (xblocks > 0) {
		if (AG(sys_abort_flag) == 0) {
			sys_timer_continue();
		}

		uint16_t proj_val = *proj;
		proj--;

		uint32_t fb_y_offset = (proj_val << 6) + (proj_val << 8);
		uint8_t *current_dst = col_dst + fb_y_offset;
		int16_t skip_top_blocks = 0;
		int16_t skip_bottom_blocks = 0;
		int16_t block_start_y = yoff + proj_val;

		if (block_start_y < winy1) {
			skip_top_blocks = (winy1 - block_start_y + 3) >> 2;
		}

		int16_t block_end_y = block_start_y + (yblocks << 2);
		if (block_end_y > winy2) {
			skip_bottom_blocks = (block_end_y - winy2 + 3) >> 2;
		}

		int16_t blocks_to_draw = yblocks - skip_bottom_blocks - skip_top_blocks;
		if (blocks_to_draw <= 0) {
			return;
		}

		uint8_t *draw_src = col_csrc + skip_top_blocks;
		uint8_t *draw_dst = current_dst + (skip_top_blocks * 1280);
		if ((fob_p1 & 0x80) != 0) {
			while (blocks_to_draw > 0) {
				uint8_t tile_idx = *draw_src++;
				uint32_t *t32 = (uint32_t *)(tilestore + (tile_idx << 4));
				uint32_t *d32 = (uint32_t *)draw_dst;

				*d32 = t32[0]; draw_dst += 320; d32 = (uint32_t *)draw_dst;
				*d32 = t32[1]; draw_dst += 320; d32 = (uint32_t *)draw_dst;
				*d32 = t32[2]; draw_dst += 320; d32 = (uint32_t *)draw_dst;
				*d32 = t32[3]; draw_dst += 320;

				draw_dst[-960] = ((draw_dst[-961] + draw_dst[-960]) >> 1) | 0x80;
				draw_dst[-640] = ((draw_dst[-641] + draw_dst[-640]) >> 1) | 0x80;
				draw_dst[-320] = ((draw_dst[-321] + draw_dst[-320]) >> 1) | 0x80;

				uint32_t top_row = *(uint32_t *)(draw_dst - 1600);
				uint32_t cur_row = *(uint32_t *)(draw_dst - 1280);
				*(uint32_t *)(draw_dst - 1280) = ((top_row + cur_row) >> 1) | 0x80808080;

				blocks_to_draw--;
			}
		} else {
			while (blocks_to_draw > 0) {
				uint8_t tile_idx = *draw_src++;
				uint32_t *t32 = (uint32_t *)(tilestore + (tile_idx << 4));
				uint32_t *d32 = (uint32_t *)draw_dst;

				*d32 = t32[0]; draw_dst += 320; d32 = (uint32_t *)draw_dst;
				*d32 = t32[1]; draw_dst += 320; d32 = (uint32_t *)draw_dst;
				*d32 = t32[2]; draw_dst += 320; d32 = (uint32_t *)draw_dst;
				*d32 = t32[3]; draw_dst += 320;

				draw_dst[-960] = ((draw_dst[-961] + draw_dst[-960]) >> 1) & 0x7F;
				draw_dst[-640] = ((draw_dst[-641] + draw_dst[-640]) >> 1) & 0x7F;
				draw_dst[-320] = ((draw_dst[-321] + draw_dst[-320]) >> 1) & 0x7F;

				uint32_t top_row = *(uint32_t *)(draw_dst - 1600);
				uint32_t cur_row = *(uint32_t *)(draw_dst - 1280);
				*(uint32_t *)(draw_dst - 1280) = ((top_row + cur_row) >> 1) & 0x7F7F7F7F;

				blocks_to_draw--;
			}
		}
		col_dst += 4;
		col_csrc += srcbytes_per_column;
		xblocks--;
	}
}

void fob_codec5_hi_p2(uint8_t *dst, uint8_t *csrc, uint8_t *tilestore, uint8_t *fobsrc,
		      uint16_t yblocks, uint16_t *proj, uint16_t xblocks, uint16_t yoff,
		      uint16_t winy1, uint16_t winy2, uint16_t srcbytes_per_column,
		      uint8_t fob_p1, uint16_t fob_p2)
{
	if (fob_p2 != 0) {
		uint8_t *custom_dst = tilestore + 0x1000;
		uint8_t *unpack_src = fobsrc;
		uint16_t unpack_loops = fob_p2 << 2;

		for (uint16_t i = 0; i < unpack_loops; i++) {
			uint8_t byte1 = *unpack_src++;
			*custom_dst++ = (byte1 >> 4)   + fob_p1;
			*custom_dst++ = (byte1 & 0x0F) + fob_p1;
			byte1 = *unpack_src++;
			*custom_dst++ = (byte1 >> 4)   + fob_p1;
			*custom_dst++ = (byte1 & 0x0F) + fob_p1;
		}
	}

	uint8_t *col_dst = dst;
	uint8_t *col_csrc = csrc;

	while (xblocks > 0) {
		if (AG(sys_abort_flag) == 0) {
			sys_timer_continue();
		}

		uint16_t proj_val = *proj;
		proj--;

		uint32_t fb_y_offset = (proj_val << 6) + (proj_val << 8);
		uint8_t *current_dst = col_dst + fb_y_offset;
		int16_t skip_top_blocks = 0;
		int16_t skip_bottom_blocks = 0;
		int16_t block_start_y = yoff + proj_val;

		if (block_start_y < winy1) {
			skip_top_blocks = (winy1 - block_start_y + 3) >> 2;
		}

		int16_t block_end_y = block_start_y + (yblocks << 2);
		if (block_end_y > winy2) {
			skip_bottom_blocks = (block_end_y - winy2 + 3) >> 2;
		}

		int16_t blocks_to_draw = yblocks - skip_bottom_blocks - skip_top_blocks;
		if (blocks_to_draw <= 0) {
			return;
		}

		int16_t skip_chunks = skip_top_blocks >> 3;
		int16_t skip_rem    = skip_top_blocks & 0x07;
		uint8_t *draw_src = col_csrc + (skip_chunks * 9);
		uint16_t control_word = (*draw_src) | 0x0100;

		if (skip_rem != 0) {
			draw_src += skip_rem;
			control_word <<= skip_rem;
		}
		draw_src++;

		uint8_t *draw_dst = current_dst + (skip_top_blocks * 1280);

		if ((fob_p1 & 0x80) != 0) {
			while (blocks_to_draw > 0) {
				uint8_t tile_idx = *draw_src++;
				int use_custom = (control_word & 0x0080) != 0;

				uint32_t *t32;
				if (use_custom) {
					t32 = (uint32_t *)(tilestore + 0x1000 + (tile_idx << 4));
				} else {
					t32 = (uint32_t *)(tilestore + (tile_idx << 4));
				}
				uint32_t *d32 = (uint32_t *)draw_dst;

				*d32 = t32[0]; draw_dst += 320; d32 = (uint32_t *)draw_dst;
				*d32 = t32[1]; draw_dst += 320; d32 = (uint32_t *)draw_dst;
				*d32 = t32[2]; draw_dst += 320; d32 = (uint32_t *)draw_dst;
				*d32 = t32[3]; draw_dst += 320;

				draw_dst[-960] = ((draw_dst[-961] + draw_dst[-960]) >> 1) | 0x80;
				draw_dst[-640] = ((draw_dst[-641] + draw_dst[-640]) >> 1) | 0x80;
				draw_dst[-320] = ((draw_dst[-321] + draw_dst[-320]) >> 1) | 0x80;

				uint32_t top_row = *(uint32_t *)(draw_dst - 1600);
				uint32_t cur_row = *(uint32_t *)(draw_dst - 1280);
				*(uint32_t *)(draw_dst - 1280) = ((top_row + cur_row) >> 1) | 0x80808080;

				int carry = (control_word & 0x8000) != 0;
				control_word <<= 1;
				if (carry) {
					control_word = (*draw_src) | 0x0100;
					draw_src++;
				}

				blocks_to_draw--;
			}
		} else {
			while (blocks_to_draw > 0) {
				uint8_t tile_idx = *draw_src++;
				int use_custom = (control_word & 0x0080) != 0;

				uint32_t *t32;
				if (use_custom)
					t32 = (uint32_t *)(tilestore + 0x1000 + (tile_idx << 4));
				else
					t32 = (uint32_t *)(tilestore + (tile_idx << 4));

				uint32_t *d32 = (uint32_t *)draw_dst;

				*d32 = t32[0]; draw_dst += 320; d32 = (uint32_t *)draw_dst;
				*d32 = t32[1]; draw_dst += 320; d32 = (uint32_t *)draw_dst;
				*d32 = t32[2]; draw_dst += 320; d32 = (uint32_t *)draw_dst;
				*d32 = t32[3]; draw_dst += 320;

				draw_dst[-960] = ((draw_dst[-961] + draw_dst[-960]) >> 1) & 0x7F;
				draw_dst[-640] = ((draw_dst[-641] + draw_dst[-640]) >> 1) & 0x7F;
				draw_dst[-320] = ((draw_dst[-321] + draw_dst[-320]) >> 1) & 0x7F;

				uint32_t top_row = *(uint32_t *)(draw_dst - 1600);
				uint32_t cur_row = *(uint32_t *)(draw_dst - 1280);
				*(uint32_t *)(draw_dst - 1280) = ((top_row + cur_row) >> 1) & 0x7F7F7F7F;

				int carry = (control_word & 0x8000) != 0;
				control_word <<= 1;
				if (carry) {
					control_word = (*draw_src) | 0x0100;
					draw_src++;
				}

				blocks_to_draw--;
			}
		}
		col_dst += 4;
		col_csrc += srcbytes_per_column;
		xblocks--;
	}
}

void fob_codec4_tilestore_reset(void)
{
	AG(fobdec_c4_tilestore_param)[0] = 0xffff;
	AG(fobdec_c4_tilestore_param)[1] = 0xffff;
}

void fob_codec4_tilestore_prepare(uint8_t *tilestore, uint16_t param)
{
	if (tilestore != AG(fobdec_c4_last_tilestore)) {
		fob_codec4_tilestore_reset();
	}
	AG(fobdec_c4_last_tilestore) = tilestore;
	if (param == AG(fobdec_c4_tilestore_param)[0]) {
		AG(fobdec_c4_tilestore_idx) = 0;
	} else if (param == AG(fobdec_c4_tilestore_param)[1]) {
		AG(fobdec_c4_tilestore_idx) = 1;
	} else {
		AG(fobdec_c4_tilestore_idx) ^= 1;
		AG(fobdec_c4_tilestore_param)[AG(fobdec_c4_tilestore_idx)] = param;
		if (AG(fobdec_c4_tilestore_idx) != 0)
			tilestore += 0x2000;
		fob_codec4_tilestore_generate(tilestore, param);
	}
}

void fob_codec4_tilestore_generate(uint8_t *dst,int16_t param1)
{
	int i, j, k, l, m, n, o, p;

	if (param1 < 0x100) {
		for (i = 1; i < 16; i += 2) {			// i = l24
			for (k = 0; k < 16; k++) {		// k = bx
				j = i + param1;			// j = l34
				l = k + param1;			// k = l30
				m = (j + l) / 2;		// esi, dx
				n = (j + m) / 2;		// n = l28, l20 (16bit)
				o = (l + m) / 2;		// o = l28, l1c (16bit)
				if (j == m || l == m) {
					*dst++ = l; *dst++ = j; *dst++ = l; *dst++ = j;
					*dst++ = j; *dst++ = l; *dst++ = j; *dst++ = j;
					*dst++ = l; *dst++ = j; *dst++ = l; *dst++ = j;
					*dst++ = l; *dst++ = l; *dst++ = j; *dst++ = l;
				} else {
					*dst++ = m; *dst++ = m; *dst++ = n; *dst++ = j;
					*dst++ = m; *dst++ = m; *dst++ = n; *dst++ = j;
					*dst++ = o; *dst++ = o; *dst++ = m; *dst++ = n;
					*dst++ = l; *dst++ = l; *dst++ = o; *dst++ = m;
				}
			}
		}

		for (i = 0; i < 16; i += 2) {			// i = l24
			for (k = 0; k < 16; k++) {		// k = bx
				j = i + param1;			// j = l34
				l = k + param1;			// l = l2c
				m = (j + l) / 2;		// m = si, dx
				n = (j + m) / 2;		// n = l28, l20 (16bit)
				o = (l + m) / 2;		// o = l28, l1c (16bit)
				if (m == j || m == l) {
					*dst++ = j; *dst++ = j; *dst++ = l; *dst++ = j;
					*dst++ = j; *dst++ = j; *dst++ = j; *dst++ = l;
					*dst++ = l; *dst++ = j; *dst++ = l; *dst++ = l;
					*dst++ = j; *dst++ = l; *dst++ = j; *dst++ = l;
				} else {
					*dst++ = j; *dst++ = j; *dst++ = n; *dst++ = m;
					*dst++ = j; *dst++ = j; *dst++ = n; *dst++ = m;
					*dst++ = n; *dst++ = n; *dst++ = m; *dst++ = o;
					*dst++ = m; *dst++ = m; *dst++ = o; *dst++ = l;
				}
			}
		}
	} else {
		param1 -= 0x100;
		for (i = 0; i < 8; i++) {
			for (k = 0; k < 8; k++) {
				j = i + param1;
				l = k + param1;
				p = (j + l) >> 1;
				n = (j + p) >> 1;
				m = (p + l) >> 1;
				*dst++ = p; *dst++ = p; *dst++ = n; *dst++ = j;
				*dst++ = p; *dst++ = p; *dst++ = n; *dst++ = j;
				*dst++ = m; *dst++ = m; *dst++ = p; *dst++ = j;
				*dst++ = l; *dst++ = l; *dst++ = m; *dst++ = p;
			}
		}
		for (i = 0; i < 8; i++) {
			for (k = 0; k < 8; k++) {
				j = i + param1;
				l = k + param1;
				n = (j + l) >> 1;
				m = (l + n) >> 1;
				*dst++ = j; *dst++ = j; *dst++ = j; *dst++ = j;
				*dst++ = n; *dst++ = n; *dst++ = n; *dst++ = n;
				*dst++ = m; *dst++ = m; *dst++ = m; *dst++ = m;
				*dst++ = l; *dst++ = l; *dst++ = l; *dst++ = l;
			}
		}
		for (i = 0; i < 8; i++)	{
			for (k = 0; k < 8; k++) {
				j = i + param1;
				l = k + param1;
				m = (j + l) >> 1;
				n = (j + m) >> 1;
				o = (l + m) >> 1;
				*dst++ = j; *dst++ = j; *dst++ = n; *dst++ = m;
				*dst++ = j; *dst++ = j; *dst++ = n; *dst++ = m;
				*dst++ = n; *dst++ = n; *dst++ = m; *dst++ = o;
				*dst++ = m; *dst++ = m; *dst++ = o; *dst++ = l;
			}
		}
		for (i = 0; i < 8; i++) {
			for (k = 0; k < 8; k++) {
				j = i + param1;
				l = k + param1;
				m = (j + l) >> 1;
				n = (l + m) >> 1;
				*dst++ = j; *dst++ = m; *dst++ = n; *dst++ = l;
				*dst++ = j; *dst++ = m; *dst++ = n; *dst++ = l;
				*dst++ = j; *dst++ = m; *dst++ = n; *dst++ = l;
				*dst++ = j; *dst++ = m; *dst++ = n; *dst++ = l;
			}
		}
	}
}

void fob_decode_render(uint8_t *dst,uint8_t *src,uint8_t *c4tilestore,uint32_t fobjsize,
		       uint8_t *fobjdata,int16_t xoff,int16_t yoff,struct anm_rect *viewport,
		       uint16_t anm_flags)
{
	int16_t wx1, wx2, wy1, wy2, fobw, fobh, fobwh, fobhh, c4param;
	uint8_t codec = fobjdata[0];

	wx1 = viewport->x;
	wy1 = viewport->y;
	wx2 = wx1 + viewport->w - 1;
	wy2 = wy1 + viewport->h - 1;

	if ((anm_flags & ANM_FLAG_FOB_IGN_OFS) == 0) {
		xoff += (int16_t)__le16ua(src + 2);
		yoff += (int16_t)__le16ua(src + 4);
	}
	fobw = (int16_t)__le16ua(src + 6);
	fobh = (int16_t)__le16ua(src + 8);
	fobwh = fobw >> 1;
	fobhh = fobh >> 1;

	if ((anm_flags & ANM_FLAG_FTCH_RESTORE) == 0) {
		if (((anm_flags & 0x0080) == 0) && (codec == 23)) {
			fob_camera_apply_projection_nocurve(&xoff, &yoff);
		} else if (((anm_flags & ANM_FLAG_0x0080) == 0) && (codec != 5) && (codec != 4) && (codec != 33) && (codec != 34)) {
			xoff += fobw >> 1;
			yoff += fobh >> 1;
			fob_camera_apply_projection_curve(&xoff, &yoff);
			xoff -= fobw >> 1;
			yoff -= fobh >> 1;
		}
	} else {
		int16_t x = xoff + fobwh;
		int16_t y = yoff + fobhh;
		fob_camera_apply_projection_curve(&x, &y);
		x -= fobwh;
		y -= fobhh;
		xoff -= (x - (xoff / 4));
		yoff -= (y - (yoff / 4));
	}

	if ((anm_flags & ANM_FLAG_FOB_ORG_CENTER) != 0) {
		xoff -= fobwh;
		yoff -= fobhh;
	}

	if ((codec == 3) || ((codec == 1) && ((anm_flags & 0x0100) != 0))) {
		fob_codec3(dst, src, xoff, yoff, fobw, fobh, wx1, wy1, wx2 + 1, wy2 + 1);
	} else if (codec == 1) {
		if ((((anm_flags & 0x2000) == 0) || (anm_flags & 0x4000) == 0)) {
			if ((anm_flags & 0x2000) == 0) {
				if ((anm_flags & 0x4000) == 0) {
					fob_codec1(dst, src, xoff, yoff, fobw, fobh, wx1, wy1, wx2 + 1, wy2 + 1);
				} else {
					fob_codec1_flipy(dst, src, xoff, yoff, fobw, fobh, wx1, wy1, wx2 + 1, wy2 + 1);
				}
			} else {
				fob_codec1_flipx(dst, src, xoff, yoff, fobw, fobh, wx1, wy1, wx2 + 1, wy2 + 1);
			}
		} else {
			fob_codec1_flipxy(dst, src, xoff, yoff, fobw, fobh, wx1, wy1, wx2 + 1, wy2 + 1);
		}
	} else if (codec == 2) {
		fob_codec2(dst, src, xoff, yoff, fobw, fobh, wx1, wy1, wx2 + 1, wy2 + 1, fobjsize);
	} else if (codec == 20) {
		fob_codec20(dst, src, xoff, yoff, fobjdata, wx1, wy1, wx2, wy2);
	} else if (codec == 21) {
		fob_codec21(dst, src, xoff, yoff, fobjdata, wx1, wy1, wx2, wy2);
	} else if (codec == 23) {
		fob_codec23(dst, src, xoff, yoff, fobjdata, wx1, wy1, wx2, wy2);
	} else 	if ((codec == 4) || (codec == 5) || (codec == 33) || (codec == 34)) {
		if (3 < wx1)
			wx1 -= 4;
		if (wx2 < 316)
			wx2 += 4;
		if (3 < wy1)
			wy1 -= 4;
		if (wy2 < 196)
			wy2 += 4;

		c4param = fobjdata[1];
		if ((codec == 33) || (codec == 34))
			c4param += 0x100;
		fob_codec4_tilestore_prepare(c4tilestore, c4param);
		if (((anm_flags & 0x0100) == 0) && (codec != 5) && (codec != 34)) {
			fob_codec4(dst, src, xoff, yoff, fobw, fobh, wx1, wy1, wx2 + 1, wy2 + 1, c4tilestore, __le16ua(fobjdata + 0xc));
		} else {
			fob_codec5(dst, src, xoff, yoff, fobw, fobh, wx1, wy1, wx2 + 1, wy2 + 1, c4tilestore, __le16ua(fobjdata + 0xc));
		}
	} else if ((codec == 31) || (codec == 32)) {
		if ((codec == 32) || ((anm_flags & 0x0100) != 0)) {
			fob_codec32(dst, src, xoff, yoff, fobw, fobh, wx1, wy1, wx2 + 1, wy2 + 1, fobjdata[1]);
		} else if (((anm_flags & 0x6000) != 0)) {
			fob_codec31_flipx(dst, src, xoff, yoff, fobw, fobh, wx1, wy1, wx2 + 1, wy2 + 1, fobjdata[1]);
		} else {
			fob_codec31(dst, src, xoff, yoff, fobw, fobh, wx1, wy1, wx2 + 1, wy2 + 1, fobjdata[1]);
		}
	}
}

void fob_codec3_flag8(uint8_t *dst, uint8_t *src, int16_t xoff, int16_t yoff,
		      uint16_t fobw, uint16_t fobh, uint16_t winx1, uint16_t winy1,
		      uint16_t winx2, uint16_t winy2)
{
	fob_codec3_flag8_work(dst, src, xoff, yoff, fobw, fobh, winx1, winy1,
			      winx2, winy2);
}

void fob_codec23(uint8_t *dst, uint8_t *src, int16_t xoff, int16_t yoff,
		 uint8_t *fobjdata,  uint16_t winx1, uint16_t winy1,
		 uint16_t winx2, uint16_t winy2)
{
	fob_codec23_work(dst, src, xoff, yoff, __le16ua(fobjdata + 6),
			 __le16ua(fobjdata + 8), winx1, winy1, winx2, winy2,
			 (uint8_t)(fobjdata[1] - 0x30));
}

void fob_codec21(uint8_t *dst, uint8_t *src, int16_t xoff, int16_t yoff,
		 uint8_t *fobjdata,  uint16_t winx1, uint16_t winy1,
		 uint16_t winx2, uint16_t winy2)
{
	fob_codec21_work(dst, src, xoff, yoff, __le16ua(fobjdata + 6),
			 __le16ua(fobjdata + 8), winx1, winy1, winx2, winy2);
}

void fob_codec20(uint8_t *dst, uint8_t *src, int16_t xoff, int16_t yoff,
		 uint8_t *fobjdata,  uint16_t winx1, uint16_t winy1,
		 uint16_t winx2, uint16_t winy2)
{
	fob_codec20_work(dst, src, xoff, yoff, __le16ua(fobjdata + 6),
			 __le16ua(fobjdata + 8), winx1, winy1, winx2, winy2);
}

void fob_codec1_flipx(uint8_t *dst, uint8_t *src, int16_t xoff, int16_t yoff,
		      uint16_t fobw, uint16_t fobh, uint16_t winx1, uint16_t winy1,
		      uint16_t winx2, uint16_t winy2)
{
	fob_codec1_flipx_work(dst, src, xoff, yoff, fobw, fobh, winx1, winy1,
			      winx2, winy2);
}

void fob_codec1_flipxy(uint8_t *dst, uint8_t *src, int16_t xoff, int16_t yoff,
		       uint16_t fobw, uint16_t fobh, uint16_t winx1, uint16_t winy1,
		       uint16_t winx2, uint16_t winy2)
{
	fob_codec1_flipxy_work(dst, src, xoff, yoff, fobw, fobh, winx1, winy1,
			       winx2, winy2);
}

void fob_codec1_flipy(uint8_t *dst, uint8_t *src, int16_t xoff, int16_t yoff,
		      uint16_t fobw, uint16_t fobh, uint16_t winx1, uint16_t winy1,
		      uint16_t winx2, uint16_t winy2)
{
	fob_codec1_flipy_work(dst, src, xoff, yoff, fobw, fobh, winx1, winy1,
			      winx2, winy2);
}

void fob_codec1(uint8_t *dst, uint8_t *src, int16_t xoff, int16_t yoff,
		uint16_t fobw, uint16_t fobh, uint16_t winx1, uint16_t winy1,
		uint16_t winx2, uint16_t winy2)
{
	fob_codec1_normal_work(dst, src, xoff, yoff, fobw, fobh, winx1, winy1,
			       winx2, winy2);
}

void fob_codec3(uint8_t *dst, uint8_t *src, int16_t xoff, int16_t yoff,
		uint16_t fobw, uint16_t fobh, uint16_t winx1, uint16_t winy1,
		uint16_t winx2, uint16_t winy2)
{
	fob_codec3_work(dst, src, xoff, yoff, fobw, fobh, winx1, winy1,
			winx2, winy2);
}

void fob_codec31_flipx(uint8_t *dst, uint8_t *src, int16_t xoff, int16_t yoff,
		       uint16_t fobw, uint16_t fobh, uint16_t winx1, uint16_t winy1,
		       uint16_t winx2, uint16_t winy2, uint8_t fob_p1)
{
	fob_codec31(dst, src, xoff, yoff, fobw, fobh, winx1, winy1,
		    winx2, winy2, fob_p1);
}

void fob_codec31(uint8_t *dst, uint8_t *src, int16_t xoff, int16_t yoff,
		 uint16_t fobw, uint16_t fobh, uint16_t winx1, uint16_t winy1,
		 uint16_t winx2, uint16_t winy2, uint8_t fob_p1)
{
	int16_t skip_top = winy1 - yoff;
	if (skip_top > 0) {
		yoff += skip_top;
		fobh -= skip_top;
		if (fobh <= 0)
			return;
		for (int16_t i = 0; i < skip_top; i++) {
			uint16_t payload_len = __le16ua(src);
			src += 2 + payload_len;
		}
	}

	int16_t skip_bottom = (yoff + fobh) - winy2;
	if (skip_bottom > 0) {
		fobh -= skip_bottom;
		if (fobh <= 0)
			return;
	}

	int16_t skip_left = winx1 - xoff;
	if (skip_left > 0) {
		xoff += skip_left;
		fobw -= skip_left;
		if (fobw <= 0)
			return;
	} else {
		skip_left = 0;
	}

	int16_t skip_right = (xoff + fobw) - winx2;
	if (skip_right > 0) {
		fobw -= skip_right;
		if (fobw <= 0)
			return;
	}

	uint8_t *dst_ptr = dst + (yoff * 320) + xoff;
	for (uint16_t y = 0; y < fobh; y++) {
		if (AG(sys_abort_flag) != 0) {
			sys_timer_continue();
		}
		uint16_t payload_len = __le16ua(src);
		src += 2;
		uint8_t *next_line_src = src + payload_len;
		uint8_t *line_src = src;
		uint8_t *line_dst = dst_ptr;
		int16_t remaining_left_clip = skip_left;
		int16_t remaining_width = fobw;
		while (remaining_width > 0 && line_src < next_line_src) {
			uint8_t token = *line_src++;
			bool is_solid = (token & 1) != 0;
			uint16_t pixel_count = is_solid ? (token + 1) : (token + 2);
			uint8_t solid_color_byte = 0;
			if (is_solid) {
				solid_color_byte = *line_src++;
			}
			for (uint16_t i = 0; i < pixel_count; i++) {
				uint8_t pixel;
				if (is_solid) {
					pixel = (i % 2 == 0) ? (solid_color_byte & 0x0F) : (solid_color_byte >> 4);
				} else {
					uint8_t raw_byte = line_src[i / 2];
					pixel = (i % 2 == 0) ? (raw_byte & 0x0F) : (raw_byte >> 4);
				}
				if (remaining_left_clip > 0) {
					remaining_left_clip--;
					continue;
				}
				if (remaining_width > 0) {
					if (pixel != 0x00) {
						*line_dst = pixel + fob_p1;
					}
					line_dst++;
					remaining_width--;
				}
			}
			if (!is_solid) {
				line_src += pixel_count / 2;
			}
		}
		src = next_line_src;
		dst_ptr += 320;
	}
}

void fob_codec32(uint8_t *dst, uint8_t *src, int16_t xoff, int16_t yoff,
		 uint16_t fobw, uint16_t fobh, uint16_t winx1, uint16_t winy1,
		 uint16_t winx2, uint16_t winy2, uint8_t fob_p1)
{
	int16_t skip_top = winy1 - yoff;
	if (skip_top > 0) {
		yoff += skip_top;
		fobh -= skip_top;
		if (fobh <= 0)
			return;
		for (int16_t i = 0; i < skip_top; i++) {
			uint16_t payload_len = __le16ua(src);
			src += 2 + payload_len;
		}
	}

	int16_t skip_bottom = (yoff + fobh) - winy2;
	if (skip_bottom > 0) {
		fobh -= skip_bottom;
		if (fobh <= 0)
			return;
	}

	int16_t skip_left = winx1 - xoff;
	if (skip_left > 0) {
		xoff += skip_left;
		fobw -= skip_left;
		if (fobw <= 0)
			return;
	} else {
		skip_left = 0;
	}

	int16_t skip_right = (xoff + fobw) - winx2;
	if (skip_right > 0) {
		fobw -= skip_right;
		if (fobw <= 0)
			return;
	}

	uint8_t *dst_ptr = dst + (yoff * 320) + xoff;
	for (uint16_t y = 0; y < fobh; y++) {
		if (AG(sys_abort_flag) != 0) {
			sys_timer_continue();
		}

		uint16_t payload_len = __le16ua(src);
		src += 2;
		uint8_t *next_line_src = src + payload_len;
		uint8_t *line_src = src;
		uint8_t *line_dst = dst_ptr;
		int16_t remaining_left_clip = skip_left;
		int16_t remaining_width = fobw;
		while (remaining_width > 0 && line_src < next_line_src) {
			uint8_t token = *line_src++;
			bool is_solid = (token & 1) != 0;
			uint16_t pixel_count = is_solid ? (token + 1) : (token + 2);
			uint8_t solid_color_byte = 0;
			if (is_solid) {
				solid_color_byte = *line_src++;
			}
			for (uint16_t i = 0; i < pixel_count; i++) {
				uint8_t pixel;
				if (is_solid) {
					pixel = (i % 2 == 0) ? (solid_color_byte & 0x0F) : (solid_color_byte >> 4);
				} else {
					uint8_t raw_byte = line_src[i / 2];
					pixel = (i % 2 == 0) ? (raw_byte & 0x0F) : (raw_byte >> 4);
				}
				if (remaining_left_clip > 0) {
					remaining_left_clip--;
					continue;
				}
				if (remaining_width > 0) {
					*line_dst = pixel + fob_p1;
					line_dst++;
					remaining_width--;
				}
			}
			if (!is_solid) {
				line_src += pixel_count / 2;
			}
		}
		src = next_line_src;
		dst_ptr += 320;
	}
}

void fob_codec2(uint8_t *dst, uint8_t *src, int16_t xoff, int16_t yoff,
		uint16_t fobw, uint16_t fobh, uint16_t winx1, uint16_t winy1,
		uint16_t winx2, uint16_t winy2, uint32_t datasize)
{
	fob_codec2_work(dst, src, xoff, yoff, fobw, fobh, winx1, winy1,
			winx2, winy2, datasize);
}
