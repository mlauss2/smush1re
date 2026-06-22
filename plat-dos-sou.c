/*
 * SMUSHv1 RE
 *
 * dos sound drivers
 *
 * - SB
 * - SBpro
 * - SB16
 * - ARIA synth
 */

#include <i86.h>
#include <hwio.h>
#include <stdint.h>
#include <stdlib.h>

uint32_t sou_sb_dmaddr;
uint16_t sou_sb_ioport;
uint16_t sou_sb_dmachan;
uint16_t sou_sb_volume;

uint16_t sou_sbpro_mastervol;
uint16_t sou_sbpro_voicevol;

uint8_t  sou_sb16_vol_saved[4];

uint16_t sou_aria_dspoffset;
uint32_t sou_aria_dataptr;
uint8_t  sou_aria_bufidx;

uint16_t sou_gus_dmasize;
uint8_t  sou_gus_internal_buffer_select;

#define _SBW(ofs, dat) outp(sou_sb_ioport + ofs, dat)
#define _SBR(ofs) inp(sou_sb_ioport + ofs)



/* per dma-channel ioports for the sound card dma mode config */
static const uint8_t sou_x86_dma_ioport_tbl[24] = {
	0x87, 0, 1,  0x83, 2, 3,  0, 0, 0,  0x82, 6, 7,  0, 0, 0,
	0x8b, 0xc4, 0xc6, 0x89, 0xc8, 0xca,  0x8a, 0xcc, 0xce };

static const sou_gus_irqflags_table1[16] = { 0, 0, 1, 3, 0, 2, 0, 4, 0, 0, 0, 5, 6, 0, 0, 7 };
static const sou_gus_irqflags_table2[8] = { 0, 1, 0, 2, 0, 3, 4, 5 };

void sou_sb_dsp_writecmd(uint8_t cmd)
{
	while (0 != _SBR(12)) {
		_asm { nop }
	}
	_SBW(12, cmd);
}

uint8_t sou_sb_dsp_readbyte(void)
{
	while (0 == (_SBR(14) & 0x80)) {
		_asm { nop }
	}
	return _SBR(10);
}

int16_t sou_sb_dsp_reset(void)
{
	int i, j;

	_SBW(6, 1);
	for (int i = 0; i < 24; i++)
		_SBR(14)
	_SBW(6, 0);
	for (i = 0; i < 100; i++) {
		j = sou_sb_dsp_readbyte();
		if (j == 0xAA)
			break;
	}
	return (99 < i);
}

void sou_sb_x86_dma_config(uint32_t dmaddr, uint16_t bytecount, int16_t dmachan,
			   int8_t autoinit, int8_t singlemode)
{
	uint8_t cfg, d;

	d = (dmachan & 3) | 4;

	cfg = 0x08 | (dmachan & 3);
	if (autoinit)
		cfg |= 0x10;
	if (singlemode)
		cfg |= 0x40;

	if (dmachan < 4) {
		outp(0x0a, d);
		outp(0x0c, 0);
		d = 0x0b;
	} else {
		dmaddr = dmaddr & 0xffff0000 | (int)dmaddr >> 1 & 0xffffU;
		bytecount >>= 1;
		outp(0xd4, d);
		outp(0xd8, 0);
		d = 0xd6;
	}
	outp(d, cfg);
	outp(sou_x86_dma_ioport_tbl[dmachan * 3 + 0],dmaddr >> 0x10 & 0xff);
	outp(sou_x86_dma_ioport_tbl[dmachan * 3 + 1],dmaddr & 0xff);
	outp(sou_x86_dma_ioport_tbl[dmachan * 3 + 1],dmaddr >> 8 & 0xff);
	outp(sou_x86_dma_ioport_tbl[dmachan * 3 + 2],(bytecount - 1) & 0xff));
	outp(sou_x86_dma_ioport_tbl[dmachan * 3 + 2],((bytecount - 1) >> 8) & 0xff);

	d = (dmachan < 4) ? 0x0a : 0xd4;
	outp(d, dmachan & 3);
}

void sou_sb1_setup_dma(uint32_t dmaaddr, uint16_t dmasize)
{
	sou_sb_x86_dma_config(dmaaddr, dmasize, sou_sb_dmachan, 0, 1);
	sou_sb_dsp_writecmd(0x40);
	sou_sb_dsp_writecmd(0xa5);
	sou_sb_dsp_writecmd(0x14);
	sou_sb_dsp_writecmd((dmasize - 1) & 0xff);
	sou_sb_dsp_writecmd(((dmasize - 1) >> 8) & 0xff);
}

int16_t sou_sb1_cb1(uint32_t dmaaddr, uint16_t dmasize, uint16_t ioport,
		    uint16_t dma, uint16_t irq, void *lut1, void *lut2)
{
	uint8_t *l1 = (uint8_t *)lut1;
	uint8_t *l2 = (uint8_t *)lut2;
	int j;

	sou_sb_dmaaddr = dmaaddr;
	sou_sb_ioport = ioport;
	sou_sb_dmachan = dma;

	/* 8->8 volume LUTs according to SB1 code */
	for (int i = 0; i < 17; i++) {
		for (int j = 0; j < 256; j++) {
			int v1 = (i * (j - 128)) / 64;
			*l1++ = v1 + 128;
			*l2++ = v1;
		}
	}
	_SBW(12, 0xd0);
	j = sou_sb_dsp_reset();
	if (j == 0) {
		sou_sb_dsp_writecmd(0xd1);
		sou_sb1_setup_dma(dmaaddr, dmacount);
		return 0;
	}
	return -1;
}

void sou_sb1_cb2(void)
{
	sou_sb_dsp_writecmd(0xd3);
}

void sou_sb_play_snd_dma(uint32_t dmaddr, uint32_t dmacnt, uint8_t sbioport)
{

	_SBR(0x0e);
	_SBW(0x0c, 0xd0);
	outp(0xa, 5);
	outp(0xc, 0);
	outp(0xb, 0x49);
	outp(2, dmaddr);
	outp(2, dmaddr >> 8);
	outp(0x83,(char)(dmaddr >> 0x10));
	outp(3, (dmacnt - 1) & 0xff);
	outp(3, ((dmacnt - 1) >> 8) & 0xff);
	outp(10, 1);
	while (0 != (_SBR(0xc) & 0x80)) {
		_asm { nop }
	}
	_SBW(0xc, 0x40);
	while (0 != (_SBR(0xc) & 0x80)) {
		_asm { nop }
	}
	_SBW(0xc, 0xa5);
	while (0 != (_SBR(0xc) & 0x80)) {
		_asm { nop }
	}
	_SBW(0xc, 0x14);
	while (0 != (_SBR(0xc) & 0x80)) {
		_asm { nop }
	}
	_SBW(0xc, (dmacnt - 1) 0xff);
	while (0 != (_SBR(0xc) & 0x80)) {
		_asm { nop }
	}
	_SBW(0xc, (dmacnt - 1) >> 8);
}

int8_t sou_sb1_cb3(uint32_t smaddr, uint16_t dmasize)
{
	if (0 == (inp(0x08) & 2)) {
		return 0;
	} else {
		sou_sb_play_snd_dma(dmaddr, dmasize, sou_sb_ioport);
	}
	return -1;
}

void sou_sb1_conv_s8_to_s8_w_volume(uint8_t *dst8, uint8_t *src8 ,int32_t bytecnt,
				    uint8_t *LUT, uint32_t volume)
{
	volume &= 0x7f;		/* CLAMP!! */
	do {
		*dst8 = LUT[*src8 + volume * 0x100];
		bytecnt--;
		src8++;
		dst8++;
	} while (bytecnt != 0);
}

void sou_sb1_cb4(void *dst, uint8_t *src, uint16_t srccnt, int volume)
{
	sou_sb1_conv_s8_to_s8_w_volume((uint8_t *)dst, src, srccnt,
				       (uint8_t *)AG(sou_soudrv_LUT1), volume);
}

void sou_sb1_accum_u8_to_u8_w_volume(uint8_t *dst, uint8_t *src, uint32_t bytecnt,
				     uint8_t *LUT2, uint32_t volume)

{
	volume &= 0x7f;
	do {
		*dst += LUT2[*src + volume * 0x100];
		bytecnt--;
		src++;
		dst++;
	} while (bytecnt != 0);
}

void sou_sb1_cb5(void *dst,uint8_t *src,uint16_t srccnt,uint16_t volume)
{
	sou_sb1_accum_u8_to_u8_w_volume((uint8_t *)dst, src, srccnt,
					AG(sou_soudrv_LUT2), volume);
}

void sou_sb1_cb6(uint32_t dmaaddr, uint16_t bytecnt)
{
	memset((void *)dmaaddr, AG(sou_soudrv_LUT1)[0], bytecnt);
}

uint16_t sou_sb1_set_callbacks(void **initfn, void **termfn, void **ackfn,
			       void **copyfn, void **accumfn, void **silencefn,
			       void **setvolfn, void **getvolfn)

{
	*initfn = sou_sb1_cb1;
	*termfn = sou_sb1_cb2;
	*ackfn = sou_sb1_cb3;
	*copyfn = sou_sb1_cb4;
	*accumfn = sou_sb1_cb5;
	*silencefn = sou_sb1_cb6;
	*setvolfn = NULL;
	*getvolfn = NULL;
	return 0;		/* 8 bit destination format */
}



/************ SBPRO *******************/

void sou_sbpro_setup_dma(uint32_t dmaaddr, uint16_t dmasize)
{
	sou_sb_x86_dma_config(dmaaddr, dmasize * 2, sou_sb_dmachan, 1, 1);
	sou_sb_dsp_writecmd(0x40);
	sou_sb_dsp_writecmd(0xa5);
	sou_sb_dsp_writecmd(0x48);
	sou_sb_dsp_writecmd(dmasize - 1);
	sou_sb_dsp_writecmd((dmasize - 1) >> 8);
	sou_sb_dsp_writecmd(0x1c);
}

int16_t sou_sbpro_cb1(uint32_t dmaaddr, uint16_t dmasize, uint16_t ioport,
		      uint16_t dma, uint16_t irq, void *lut1, void *lut2)
{
	uint8_t *l1 = (uint8_t *)lut1;
	uint8_t *l2 = (uint8_t *)lut2;
	int j;

	sou_sb_dmaaddr = dmaaddr;
	sou_sb_ioport = ioport;
	sou_sb_dmachan = dma;
	sou_sb_volume = 127;

	/* 8->8 volume LUTs according to SB1 code */
	for (int i = 0; i < 17; i++) {
		for (int j = 0; j < 256; j++) {
			int v1 = (i * (j - 128)) / 64;
			*l1++ = v1 + 128;
			*l2++ = v1;
		}
	}
	_SBW(0x0c, 0xd0);
	j = sou_sb_dsp_reset();
	if (j)
		return -1;
	sou_sb_dsp_writecmd(0xd1);
	_SBW(0x04, 0x22);
	sou_sbpro_mastervol = _SBR(0x05);
	_SBW(0x04, 0x04);
	sou_sbpro_voicevol = _SBR(0x05);
	_SBW(0x04, 0x03);
	_SBW(0x05, 0xff);
	_SBW(0x04, 0x22);
	_SBW(0x05, 0xff);
	sou_sbpro_setup_dma(dmaddr, dmasize);
	return 0;
}

void sou_sbpro_cb2(void)
{
	sou_sb_dsp_writecmd(0xda);
	sou_sb1_setup_dma(sou_sb_dmaaddr, 2);
	_SBW(4, 0x22);
	_SBW(5, sou_sbpro_mastervol);
	_SBW(4, 4);
	_SBW(5, sou_sbpro_mixervol);
	sou_sb_dsp_writecmd(0xd3);
}

int8_t sou_sbpro_cb3(uint32_t dmaaddr, uint16_t dmasize)
{
	_SBR(0x0e);
	return -1;
}

void sou_sbpro_cb7(uint16_t newvol)
{
	int v;

	_SBW(4, 0x22);
	v = (newvol * 15) / 127;
	_SBW(5, v | (v << 4)); /* L/R in lower/upper nibble */
	sou_sb_volume = newvol;
}

uint16_t sou_sbpro_cb8(void)
{
	return sou_sb_volume;
}

uint16_t sou_sbpro_set_callbacks(void **initfn, void **termfn, void **ackfn,
				 void **copyfn, void **accumfn, void **silencefn,
				 void **setvolfn, void **getvolfn)

{
	*initfn = sou_sbpro_cb1;
	*termfn = sou_sbpro_cb2;
	*ackfn = sou_sbpro_cb3;
	*copyfn = sou_sb1_cb4;
	*accumfn = sou_sb1_cb5;
	*silencefn = sou_sb1_cb6;
	*setvolfn = sou_sbpro_cb7;
	*getvolfn = sou_sbpro_cb8;
	return 0;		/* 8 bit destination format */
}


/********************** SB16 **************************/

void sou_sb16_setup_dma(uint32_t dmaaddr, uint16_t dmasize)
{
	int rv = (dmasize >> 1) - 1;

	sou_sb_x86_dma_config(dmaaddr, dmasize * 2, sou_sb_dmachan, 1, 1);
	sou_sb_dsp_writecmd(0x41);
	sou_sb_dsp_writecmd(0x2b);
	sou_sb_dsp_writecmd(0x11);
	sou_sb_dsp_writecmd(0xb6);
	sou_sb_dsp_writecmd(0x10);
	sou_sb_dsp_writecmd(rv & 0xff);
	sou_sb_dsp_writecmd((rv >> 8) & 0xff);
}

int16_t sou_sb16_cb1(uint32_t dmaaddr, uint16_t dmasize, uint16_t ioport,
		     uint16_t dma, uint16_t irq, void *lut1, void *lut2)
{
	int16_t *l1 = (int16_t *)lut1;
	int16_t *l2 = (int16_t *)lut2;
	int j;

	sou_sb_dmaaddr = dmaaddr;
	sou_sb_ioport = ioport;
	sou_sb_dmachan = dma;
	sou_sb_volume = 127;

	for (int x = 0; x < 17; x++) {
		for (int y = 0; y < 256; y++) {
			int z = (y - 128) * x * 4;
			*l1++ = z;
			*l2++ = z;
		}
	}

	_SBW(0x0c, 0xd0);
	j = sou_sb_dsp_reset();
	if (j)
		return -1;
	sou_sb_dsp_writecmd(0xd1);
	_SBW(0x04, 0x30);
	sou_sb16_vol_saved[3] = _SBR(0x05);
	_SBW(0x04, 0x31);
	sou_sb16_vol_saved[1] = _SBR(0x05);
	_SBW(0x04, 0x32);
	sou_sb16_vol_saved[2] = _SBR(0x05);
	_SBW(0x04, 0x33);
	sou_sb16_vol_saved[0] = _SBR(0x05);
	_SBW(0x04, 0x32);
	_SBW(0x05, 0xf8);
	_SBW(0x04, 0x33);
	_SBW(0x05, 0xf8);
	_SBW(0x04, 0x30);
	_SBW(0x05, 0xf8);
	_SBW(0x04, 0x31);
	_SBW(0x05, 0xf8);
	sou_sb16_setup_dma(dmaaddr, dmasize);
	return 0;
}

void sou_sb16_cb2_shutdown(void)
{
	ushort val;

	sou_sb_dsp_writecmd(0xd9);
	sou_sb1_setup_dma(sou_sb_dmaaddr,2);
	_SBW(4, 0x30);
	_SBW(5, sou_sb16_vol_saved[3]);
	_SBW(4, 0x31);
	_SBW(5, sou_sb16_vol_saved[1]);
	_SBW(4, 0x32);
	_SBW(5, sou_sb16_vol_saved[2]);
	_SBW(4, 0x33);
	_SBW(5, sou_sb16_vol_saved[0]);
	sou_sb_dsp_writecmd(0xd3);
}

int8_t sou_sb16_cb3(uint32_t dmaaddr, uint16_t dmasize)
{
	_SBW(4, 0x82);
	if (0 == (_SBR(5) & 2))
		return 0;

	_SBR(0x0f);
	return -1;
}

void sou_sb16_conv_s8_to_s16_w_volume(int16_t *dst, uint8_t *src,
				      uint32_t bytecnt, int16_t *LUT,
				      uint32_t volume)
{
	volume &= 0x7f;
	do {
		*dst = LUT[volume * 0x100 + *src];
		bytecnt--;
		src++;
		dst++;
	} while (bytecnt != 0);
}

void sou_sb16_cb4(void *dst, uint8_t *src, uint16_t bytecnt, uint16_t volume)
{
	sou_sb16_conv_s8_to_s16_w_volume((int16_t *)dst, src, bytecnt,
					 AG(sou_soudrv_LUT1), volume);
}

void sou_sb16_accum_s8_to_s16_w_volume(int16_t *dst, uint8_t *src,
				       uint32_t bytecnt, int16_t *LUT,
				       uint32_t volume)
{
	volume &= 0x7f;
	do {
		*dst = LUT[volume * 0x100 + *src] + *dst;
		bytecnt++
		src++;
		dst++;
	} while (bytecnt != 0);
}

void sou_sb16_cb5(void *dst, uint8_t *src, uint16_t bytecnt, uint16_t volume)

{
	sou_sb16_accum_s8_to_s16_w_volume((int16_t *)dst, src, bytecnt,
					  AG(sou_soudrv_LUT2), volume);
}

void sou_sb16_cb6(void *dst, uint16_t bytecnt)
{
	memset(dst, AG(sou_soudrv_LUT1)[0], bytecnt * 2);
}

void sou_sb16_cb7(uint16_t vol)
{
	int v = (vol * 248) / 127;

	sou_sb_volume = vol;
	_SBW(4, 0x30);
	_SBW(5, v);
	_SBW(4, 0x31);
	_SBW(5, v);
}

uint16_t sou_sb16_set_callbacks(void **initfn, void **termfn, void **ackfn,
				void **copyfn, void **accumfn, void **silencefn,
				void **setvolfn, void **getvolfn)

{
	*initfn = sou_sb16_cb1;
	*termfn = sou_sb16_cb2;
	*ackfn = sou_sb16_cb3;
	*copyfn = sou_sb16_cb4;
	*accumfn = sou_sb16_cb5;
	*silencefn = sou_sb16_cb6;
	*setvolfn = sou_sb16_cb7;
	*getvolfn = sou_sbpro_cb8;
	return 1;		/* 16 bit destination format */
}


/***************** ARIA SYNTH ********************/
/* XXX this is one of the rarest ISA sound cards ever. 16bit IO access too! */

void sou_aria_dspcmd(uint16_t val)
{
	while (0 != (inpw(sou_sb_ioport + 2) & 0x8000)) {
		_asm { nop }
	}
	outpw(sou_sb_ioport + 0, val);
}

int16_t sou_aria_cb1(uint32_t dmaaddr, uint16_t dmasize, uint16_t ioport,
		     uint16_t dma, uint16_t irq, void *lut1, void *lut2)
{
	int16_t *l1 = (int16_t *)lut1;
	int16_t *l2 = (int16_t *)lut2;
	int j;

	/* XXX: the exe uses sou_aria_* named variables */
	sou_sb_dmaaddr = dmaaddr;
	sou_sb_ioport = ioport;
	sou_sb_dmachan = dma;
	sou_sb_volume = 127;

	for (int x = 0; x < 17; x++) {
		for (int y = 0; y < 256; y++) {
			int z = (y - 128) * x * 4;
			*l1++ = z;
			*l2++ = z;
		}
	}
	outw(sou_sb_ioport + 2, 0xca);
	sou_aria_dspcmd(0);
	sou_aria_dspcmd(0);
	sou_aria_dspcmd(0);
	sou_aria_dspcmd(0);
	sou_aria_dspcmd(0xffff);
	sou_aria_dspcmd(4);
	sou_aria_dspcmd(0x7fff);
	sou_aria_dspcmd(0x7fff);
	sou_aria_dspcmd(0xffff);
	sou_aria_dspcmd(3);
	sou_aria_dspcmd(2);
	sou_aria_dspcmd(0xffff);
	sou_aria_dspcmd(5);
	sou_aria_dspcmd(0x200);
	sou_aria_dspcmd(0xffff);
	sou_aria_dspoffset = 0x7c00;
	sou_aria_dspcmd(0x11);
	sou_aria_dspcmd(0);
	sou_aria_dspcmd(0xffff);
}

void sou_aria_cb2(void)
{
	sou_aria_dspcmd(0x12);
	sou_aria_dspcmd(0);
	sou_aria_dspcmd(0xffff);
	sou_aria_dspcmd(0);
	sou_aria_dspcmd(0);
	sou_aria_dspcmd(1);
	sou_aria_dspcmd(0);
	sou_aria_dspcmd(0xffff);
	iopw(sou_sb_ioport + 2, 0xca);
}

int8_t sou_aria_cb3(uint32_t dmaaddr, uint16_t dmasize)
{
	int v;
	int16_t *s16;

	if (0 == (inpw(sou_sb_ioport + 0) & 1))
		return 0;

	v = sou_aria_bufidx;
	sou_aria_bufidx ^= 1;
	if (!v)
		sou_aria_dataptr = dmaaddr;
	outpw(sou_sb_ioport + 4, sou_aria_dspoffset);
	s16 = (int16_t *)sou_aria_dataptr;
	for (i = 0; i < 0x200; i++) {
		iopw(sou_sb_dataport + 6, *s16++);
	}
	sou_aria_dspcmd(0x10);
	sou_aria_dspcmd(0xffff);
	sou_aria_dspoffset = 64000 - sou_aria_dspoffset;
	return sou_aria_bufidx:
}

void sou_aria_cb7(uint16_t vol)
{
	uint16_t v = (vol * 0x7fff) / 127;

	sou_ab_volume = vol;
	sou_aria_dspcmd(4);
	sou_aria_dspcmd(v);
	sou_aria_dspcmd(v);
	sou_aria_dspcmd(0xffff);
}

uint16_t sou_aria_cb8(void)
{
	return sou_sb_volume;
}

uint16_t sou_aria_set_callbacks(void **initfn, void **termfn, void **ackfn,
				void **copyfn, void **accumfn, void **silencefn,
				void **setvolfn, void **getvolfn)

{
	*initfn = sou_aria_cb1;
	*termfn = sou_aria_cb2;
	*ackfn = sou_aria_cb3;
	*copyfn = sou_sb16_cb4;
	*accumfn = sou_sb16_cb5;
	*silencefn = sou_sb16_cb6;
	*setvolfn = sou_aria_cb7;
	*getvolfn = sou_aria_cb8;
	return 1;		/* 16 bit destination format */
}



/*********************  GUS  *****************************/

#define _GRR8(ofs)  	inp(sou_sb_ioport + ofs)
#define _GRW8(ofs, val)	outp(sou_sb_ioport + ofs, val)
#define _GRW16(ofs, val)	outpw(sou_sb_ioport + ofs, val)

/* gus set io bit */
static inline void gsb(int ofs, int bitmas)
{
	uint8_t v = _GRR8(ofs);
	_GRW8(ofs, v | bitmask);
}

/* gus clear io bit */
static inline void gcb(int ofs, int bitmas)
{
	uint8_t v = _GRR8(ofs);
	_GRW8(ofs, v & ~bitmask);
}

uint32_t sou_gus_calculate_dmasize_mask(uint32_t dmasize)
{
	return dmasize >> 1 & 0x1ffff | dmasize & 0xc0000;
}

static inline gus_delay(int cnt)
{
	while (cnt--)
		inp(0x21);
}

void sou_gus_hw_init(void)
{
	_GRW8(0x303, 0x4c);
	_GRW8(0x305, 0);
	gus_delay(100);
	_GRW8(0x303, 0x4c);
	_GRW8(0x305, 1);
	gus_delay(100);
	_GRW8(0x303, 0x41);
	_GRW8(0x305, 0);
	_GRW8(0x303, 0x45);
	_GRW8(0x305, 0);
	_GRW8(0x303, 0x49);
	_GRW8(0x305, 0);
	_GRW8(0x303, 0x0e);
	_GRW8(0x305, 0xcd);
	_GRR8(0x206);
	_GRW8(0x303, 0x41);
	_GRR8(0x305);
	_GRW8(0x303, 0x8f);
	_GRR8(0x305);
	for (int i = 0; i < 14; ++) {
		_GRW8(0x302, i);
		_GRW8(0x303, 0);
		_GRW8(0x305, 7);
		_GRW8(0x303, 0xd);
		_GRW8(0x305, 3);
		gus_delay(10);
		_GRW8(0x303, 1);
		_GRW16(0x304, 0x400);
		_GRW8(0x303, 2);
		_GRW16(0x304, 0);
		_GRW8(0x303, 3);
		_GRW16(0x304, 0);
		_GRW8(0x303, 4);
		_GRW16(0x304, 0);
		_GRW8(0x303, 5);
		_GRW16(0x304, 0);
		_GRW8(0x303, 6);
		_GRW8(0x305, 1);
		_GRW8(0x303, 7);
		_GRW8(0x305, 0x10);
		_GRW8(0x303, 8);
		_GRW8(0x305, 0xe0);
		_GRW8(0x303, 9);
		_GRW16(0x304, 0);
		_GRW8(0x303, 10);
		_GRW16(0x304, 0);
		_GRW8(0x303, 0xb);
		_GRW16(0x304, 0);
		_GRW8(0x303, 0xc);
		_GRW8(0x305, 7);
	}
	_GRR8(0x206);
	_GRW8(0x303, 0x41);
	_GRR8(0x305);
	_GRW8(0x303, 0x49);
	_GRR8(0x305);
	_GRW8(0x303, 0x8f);
	_GRR8(0x305);
	_GRW8(0x303, 0x4c);
	_GRW8(0x305, 7);
}

void sou_gus_setup_irq_routing(uint16_t play_irq, uint16_t rec_irq,
			       uint16_t play_dma, uint16_t rec_dma)
{
	uint8_t b1, b2;

	if ((play_dma == rec_dma) && (play_dma != 0)) {
		b1 = sou_gus_irqflags_table1[play_dma] | 0x40;
	} else {
		b1 = sou_gus_irqflags_table1[play_dma] | sou_gus_irqflags_table1[rec_dma] << 3;
	}
	if ((play_irq == rec_irq) && (play_irq != 0)) {
		b2 = sou_gus_irqflags_table2[play_irq] | 0x40;
	} else {
		b2 = sou_gus_irqflags_table2[play_irq] | sou_gus_irqflags_table2[rec_irq] << 3;
	}
	_GRW8(0x20f, 5);
	_GRW8(0x200, 8);
	_GRW8(0x20b, 0);
	_GRW8(0x20f, 0);
	_GRW8(0x200, 8);
	_GRW8(0x20b, b2 | 0x80);
	_GRW8(0x200, 0x48);
	_GRW8(0x20b, b1);
	_GRW8(0x200, 8);
	_GRW8(0x20b, b2);
	_GRW8(0x200, 0x48);
	_GRW8(0x20b, b1);
	_GRW8(0x302, 0);
	_GRW8(0x200, 9);
	_GRW8(0x302, 0);
}

void sou_gus_set_samplerate(uint16_t srate)
{
	uint32_t r;

	r = (((srate << 9) + 22050) / 44100) * 2;

	_GRW8(0x303, 1);
	_GRW16(0x304, r);
}

int16_t sou_gus_cb1(uint32_t dmaaddr, uint16_t dmasize, uint8_t ioport,
		    uint16_t dma, uint16_t irq, void *LUT1, void *LUT2)
{
	int16_t *l1 = (int16_t *)lut1;
	int16_t *l2 = (int16_t *)lut2;
	uint32_t dm;

	sou_sb_volume = 0x7f;
	sou_sb_ioport = ioport;
	sou_gus_dmasize = dmasize;
	dm = sou_gus_calculate_dmasize_mask(dmasize - 1);

	for (int x = 0; x < 17; x++) {
		for (int y = 0; y < 256; y++) {
			int z = (y - 128) * x * 4;
			*l1++ = z;
			*l2++ = z;
		}
	}

	gsb(0, 1);
	gsb(0, 4);
	gsb(0, 1);
	sou_gus_hw_init();
	sou_gus_setup_irq_routing(0, 1, irq, 0);
	sou_gus_set_samplerate(11025);
	_GRW8(0x302, 0);
	_GRW8(0x303, 10);
	_GRW16(0x304, 0);
	_GRW8(0x303, 11);
	_GRW16(0x304, 0);
	_GRW8(0x303, 2);
	_GRW16(0x304, 0);
	_GRW8(0x303, 3);
	_GRW16(0x304, 0);
	_GRW8(0x303, 4);
	_GRW16(0x304, (dm >> 7) & 0x1fff);
	_GRW8(0x303, 5);
	_GRW16(0x304, (dm & 0x7f) << 9));
	gcb(0, 2);
	sou_gus_internal_buffer_select = 1;
	_GRW8(0x303, 9);
	_GRW16(0x304, 0xf3c0);
	_GRW8(0x303, 12);
	_GRW16(0x305, 7);
	_GRW8(0x303, 13);
	_GRW8(0x305, 0x24);
	gus_delay(10);
	_GRW8(0x303, 13);
	_GRW8(0x305, 7);
	_GRW8(0x303, 0);
	_GRW8(0x305, 0x24);

	return 0;
}

void sou_gus_cb2(void)
{
	gsb(0,1);
	gcb(0, 4);
	gsb(0,2);
	_GRW8(0x303, 0x4c);
	_GRW8(0x305, 0);
	gus_delay(100);
	_GRW8(0x303, 0x4c);
	_GRW8(0x305, 1);
	iVar1 = 0;
	gus_delay(100);
	_GRW8(0x303, 0x41);
	_GRW8(0x305, 0);
	_GRW8(0x303, 0x45);
	_GRW8(0x305, 0);
	_GRW8(0x303, 0x49);
	_GRW8(0x305, 0);
	_GRW8(0x303, 0xe);
	_GRW8(0x305, 0xcd);
	_GRR8(0x206);
	_GRW8(0x303, 0x41);
	_GRR8(0x305);
	_GRW8(0x303, 0x49);
	_GRR8(0x305);
	_GRW8(0x303, 0x8f);
	_GRR8(0x305);
	for (int i = 0; i < 14; i++) {
		_GRW8(0x302,iVar1);
		_GRW8(0x303, 0);
		_GRW8(0x305, 7);
		_GRW8(0x303, 0xd);
		_GRW8(0x305, 3);
		gus_delay(10);
		_GRW8(0x303, 1);
		_GRW16(0x304, 0x400);
		_GRW8(0x303, 2);
		_GRW16(0x304, 0);
		_GRW8(0x303, 3);
		_GRW16(0x304, 0);
		_GRW8(0x303, 4);
		_GRW16(0x304, 0);
		_GRW8(0x303, 5);
		_GRW16(0x304, 0);
		_GRW8(0x303, 6);
		_GRW8(0x305, 1);
		_GRW8(0x303, 7);
		_GRW8(0x305, 0x10);
		_GRW8(0x303, 8);
		_GRW8(0x305, 0xe0);
		_GRW8(0x303, 9);
		_GRW16(0x304, 0);
		_GRW8(0x303, 10);
		_GRW16(0x304, 0);
		_GRW8(0x303, 0xb);
		_GRW16(0x304, 0);
		_GRW8(0x303, 0xc);
		_GRW8(0x305, 7);
	}
	_GRR8(0x206);
	_GRW8(0x303, 0x41);
	_GRR8(0x305);
	_GRW8(0x303, 0x49);
	_GRR8(0x305);
	_GRW8(0x303, 0x8f);
	_GRR8(0x305);
	_GRW8(0x303, 0x4c);
	_GRW8(0x305, 7);
}

void sou_gus_copy_to_internal_ram(uint32_t gf1_start_addr, uint8_t *data, uint32_t datacnt)

{
	while (datacnt--) {
		_GRW8(0x303, 0x43);
		_GRW16(0x304, gf1_start_addr & 0xffff);
		_GRW8(0x303, 0x44);
		_GRW8(0x305, 0);
		_GRW8(0x307, *data++);
		gf1_start_addr = gf1_start_addr + 1;
	}
}

void sou_gus_stream_fill_doublebuffer(uint32_t dmaaddr)
{
	uint8_t r1;
	uint32_t dm = sou_gus_calculate_dmasize_mask(sou_gus_dmasize - 1);
	uint32_t dm2 = sou_gus_calculate_dmasize_mask(sou_gus_dmasize * 2 - 1);

	_GRR8(0x306);
	_GRW8(0x303,0x8f);
	r1 = _GRR8(0x305);
	if ((r1 & 0x80) == 0) {
		if (sou_gus_internal_buffer_select == 0) {
			_GRW8(0x303,4);
			_GRW16(0x304, dm >> 7 & 0x1fff);
			_GRW8(0x303,5);
			_GRW16(0x304, (dm & 0x7f) << 9);
			_GRW8(0x303,0);
			_GRW8(0x305,0x24);
			sou_gus_copy_to_internal_ram(sou_gus_dmasize, snddata, sou_gus_dmasize);
			sou_gus_internal_buffer_select = 0xffff;
		} else {
			_GRW8(0x303,4);
			_GRW16(0x304, dm2 >> 7 & 0x1fff);
			_GRW8(0x303,5);
			_GRW16(0x304, (dm2 & 0x7f) << 9);
			_GRW8(0x303,0);
			_GRW8(0x305,0x2c);
			sou_gus_copy_to_internal_ram(0, snddata, sou_gus_dmasize);
			sou_gus_internal_buffer_select = 0;
		}
	}
}

int8_t sou_gus_cb3(uint32_t dmaaddr, uint16_t dmasize)
{
	sou_gus_stream_fill_doublebuffer(dmaaddr);
	return 1;
}

void sou_gus_cb7_(uint16_t vol)
{
	sou_sb_volume = vol;
	_GRW8(0x303,9);
	_GRW16(0x304, (((sou_gus_volume * 0xf000) / 0x7f) & 0xf000 | 0xff0));
}

uint16_t sou_gus_cb8(void)
{
	return sou_sb_volume;
}

uint16_t sou_gus_set_callbacks(void **initfn, void **termfn, void **ackfn,
				void **copyfn, void **accumfn, void **silencefn,
				void **setvolfn, void **getvolfn)

{
	*initfn = sou_gus_cb1;
	*termfn = sou_gus_cb2;
	*ackfn = sou_gus_cb3;
	*copyfn = sou_sb16_cb4;
	*accumfn = sou_sb16_cb5;
	*silencefn = sou_sb16_cb6;
	*setvolfn = sou_gus_cb7;
	*getvolfn = sou_gus_cb8;
	return 1;		/* 16 bit destination format */
}
