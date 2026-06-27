/*
 * SMUSHv1 RE
 *
 * sou - sound.
 */

#ifndef _SMUSH_SOU_H_
#define _SMUSH_SOU_H_

#include "smush.h"

/* sound buffer refill function, needs to be implemented in the platform.
 * in DOS, this function is directly hooked to the hardware interrupt of the
 * PIC which serves the sound card IRQ (i.e. NOT hooked to the sound card IRQ
 * itself), and it checks the PICs INTSTAT register whether the sound card IRQ
 * has triggered.
 * It's basically called at a regular interval by the sound card when the DMA
 *  count has elapsed.
 */
void sou_engine_fill(void);

/* per-platform sound init function: select sound card, setup interrupts/DMA,
 * setup callbacks, setup DMA buffers, ...
 * sou_init() will call this, then set up channels and finally call callback 1.
 * return 0 on success, every other as failure.
 */
int16_t sou_init_platform(void);

/* per-platform sound teardown: disable the card, unhook interrupts, free resources
 */
void sou_terminate_platform(void);

/* init sound card: DMA buffer + buffer size, IO/DMA/IRQ numbers, and
 * space for 2 lookup tables to aid volume application and format conversion in
 * cb4/5
 */
typedef int16_t(*sou_soudrv_cb1_fn_t)(void *dmaaddr, uint32_t dmasize, uint16_t x86io,
				      uint16_t x86dma, uint16_t x86irq, void *lut1,
				      void *lut2);

/* shut down sound driver */
typedef void(*sou_soudrv_cb2_fn_t)(void);

/* ack interrupt for this particular dma address */
typedef int8_t(*sou_soudrv_cb3_fn_t)(void *dmaaddr, uint32_t dmasize);

/* convert 8-bit unsigned source data to destination hardware format and apply
 * stream volume to it.
 */
typedef void(*sou_soudrv_cb4_fn_t)(void *dst, uint8_t *src, uint16_t srccnt, uint16_t volume);

/* same as above, but also accumulate to the dst buffer. */
typedef void(*sou_soudrv_cb5_fn_t)(void *dst, uint8_t *src, uint16_t srccnt, uint16_t volume);

/* generate silence */
typedef void(*sou_soudrv_cb6_fn_t)(void *dmaaddr, uint32_t dmasize);

/* set hardware mixer volume */
typedef void(*sou_soudrv_cb7_fn_t)(uint16_t vol);

/* get hardware mixer volume */
typedef uint16_t(*sou_soudrv_cb8_fn_t)(void);


/* sound channel. There are 4 of them in RA1.  In the original EXE, these members
 * are all individual arrays of 4, while in RA2+, they are organized in a
 * structure like the one below.
 */
struct _sou_channel {
	uint8_t *	buffer;			/* 002b23e1 */
	uint16_t	streamid;		/* 002b9f7c */
	uint8_t		updated;		/* 002b9f84 */
	uint8_t		is_voice;		/* 002b9f88 */
	uint8_t		status;			/* 002b9fac */
	uint8_t *	sdatptr;		/* 002b9fc0 */
	uint32_t	maxpcmsize;		/* 002ba0d0 */
	uint32_t	bytesread;		/* 002ba0e0 */
	uint32_t	playpos;		/* 002ba0f0 */
	uint32_t	sdatsize;		/* 002ba100 */
	uint8_t *	strkptr;		/* 002ba110 */
	uint8_t		status_prev;		/* 002ba120 */
	uint32_t	age_cntr;		/* 002ba138 */
	uint32_t	maxidx;			/* 002ba148 */
	uint32_t	curridx;		/* 002ba158 */
	uint32_t	trkid;			/* 002ba168 */

};

/* fill control data, per channel, same notes as above. */
struct _sou_engine_fill {
	uint16_t	waitvol;		/* 002b240a */
	uint16_t	volume;			/* 002ba178 */
	uint8_t *	srcptr;			/* 002ba180 */
	uint8_t		status;			/* 002ba184 */
	uint32_t	playlen;		/* 002ba18c */
	uint32_t	playpos;		/* 002ba19c */
	uint8_t *	comm_ptr;		/* 002ba1ac */
	uint32_t	srcmaxsize;		/* 002ba1bc */
	uint8_t *	sdatptr;		/* 002ba1cc */
};

int16_t sou_init(void);
void sou_terminate(void);

void sou_engine_start(uint8_t *data, uint16_t streamid);
void sou_pause(void);
void sou_resume(void);
void sou_engine_stop_all(void);
uint16_t sou_engine_get_volume(void);
void sou_engine_set_volume(uint16_t vol);
void sou_engine_stop(uint16_t streamid);
int16_t sou_engine_query(uint16_t streamid);
uint8_t sou_engine_gethook(uint8_t index);
void sou_engine_sethook(uint8_t index,uint8_t val);
void sou_engine_kill_dead_channels(void);

void sou_engine_start_stream_raw(uint8_t *data, uint32_t size, uint16_t streamid);
void sou_engine_start_stream(uint8_t *data, uint32_t datasize, int32_t tid,
			     int32_t idx, int32_t maxidx, uint16_t streamid,
			     int8_t is_voice);

void sou_engine_fill_core(void);

void sou_drv_set_volume(uint16_t vol);
uint16_t sou_drv_get_volume(void);

#endif
