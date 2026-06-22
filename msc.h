/*
 * SMUSHv1 RE
 *
 * msc - misc.
 */

#ifndef _SMUSH1_MSC_H_
#define _SMUSH1_MSC_H_

struct msc_list;

struct msc_list {
	struct msc_list *next;
	struct msc_list *prev;
};


void msc_draw_rect(uint8_t *dst, int16_t xoff, int16_t yoff, uint16_t width,
		   uint16_t height, uint8_t color, uint16_t stride, uint16_t maxy);
void msc_fill_rect(uint8_t *dst, int16_t xoff, int16_t yoff, uint16_t width,
		   uint16_t height, uint8_t color, uint16_t stride);

uint16_t msc_random(uint16_t);
void msc_find_contrast_alt(uint16_t *brightest_index,uint16_t *darkest_index,uint8_t *pal);
int32_t msc_color_distance(int16_t color1,int16_t color2,uint8_t *pal);

void msc_memcpy(void *dst, void *src, uint32_t cnt);
void msc_memset(void *dst, uint8_t val, uint32_t cnt);

int16_t mem_blk_alloc(void **mem_out, uint32_t size);
void mem_blk_free(void **mem);


#endif
