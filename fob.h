/*
 * SMUSHv1 RE
 *
 * fob - Frame OBject related decoders.
 */

#ifndef _SMUSH1_FOB_H_
#define _SMUSH1_FOB_H_


void fob_camera_apply_projection_curve(int16_t *xoff, int16_t *yoff);
void fob_camera_apply_projection_nocurve(int16_t *xoff, int16_t *yoff);
void fob_camera_unapply_projection_curve(int16_t *xoff, int16_t *yoff);

void fob_camera_projection_enable_randomness(void);
void fob_camera_projection_disable_randomness(void);
void fob_camera_projection_set_params(int16_t x, int16_t y, int unused);

void fob_camera_projection_curve_init(int16_t cval1, int16_t extent);

void fob_codec4_tilestore_reset(void);
void fob_codec4_tilestore_prepare(uint8_t *tilestore, uint16_t param);
void fob_codec4_tilestore_generate(uint8_t *dst,int16_t param1);

void fob_decode_render(uint8_t *dst, uint8_t *src, uint8_t *c4tilestore, uint32_t fobjsize,
		       uint8_t *fobjdata, int16_t xoff,int16_t yoff,
		       struct anm_rect *viewport, uint16_t anm_flags);


void fob_codec1_normal_work(uint8_t *dst, uint8_t *src, int16_t xoff, int16_t yoff,
			    uint16_t fobw, uint16_t fobh, uint16_t winx1, uint16_t winy1,
			    uint16_t winx2, uint16_t winy2);
void fob_codec1_flipx_work(uint8_t *dst, uint8_t *src, int16_t xoff, int16_t yoff,
			   uint16_t fobw, uint16_t fobh, uint16_t winx1, uint16_t winy1,
			   uint16_t winx2, uint16_t winy2);
void fob_codec1_flipxy_work(uint8_t *dst, uint8_t *src, int16_t xoff, int16_t yoff,
			    uint16_t fobw, uint16_t fobh, uint16_t winx1, uint16_t winy1,
			    uint16_t winx2, uint16_t winy2);
void fob_codec1_flipy_work(uint8_t *dst, uint8_t *src, int16_t xoff, int16_t yoff,
			   uint16_t fobw, uint16_t fobh, uint16_t winx1, uint16_t winy1,
			   uint16_t winx2, uint16_t winy2);
void fob_codec3_work(uint8_t *dst, uint8_t *src, int16_t xoff, int16_t yoff,
		     uint16_t fobw, uint16_t fobh, uint16_t winx1, uint16_t winy1,
		     uint16_t winx2, uint16_t winy2);
void fob_codec23_work(uint8_t *dst, uint8_t *src, int16_t xoff, int16_t yoff,
		      uint16_t fobw, uint16_t fobh, uint16_t winx1, uint16_t winy1,
		      uint16_t winx2, uint16_t winy2, uint8_t colordelta);
void fob_codec21_work(uint8_t *dst, uint8_t *src, int16_t xoff, int16_t yoff,
		      uint16_t fobw, uint16_t fobh, uint16_t winx1, uint16_t winy1,
		      uint16_t winx2, uint16_t winy2);
void fob_codec20_work(uint8_t *dst, uint8_t *src, int16_t xoff, int16_t yoff,
		      uint16_t fobw, uint16_t fobh, uint16_t winx1, uint16_t winy1,
		      uint16_t winx2, uint16_t winy2);
void fob_codec2_work(uint8_t *dst, uint8_t *src, int16_t xoff, int16_t yoff,
		     uint16_t fobw, uint16_t fobh, uint16_t winx1, uint16_t winy1,
		     uint16_t winx2, uint16_t winy2, uint32_t datasize);

void fob_codec4(uint8_t *dst, uint8_t *src, int16_t xoff, int16_t yoff,
		uint16_t fobw, uint16_t fobh, uint16_t winx1, uint16_t winy1,
		uint16_t winx2, uint16_t winy2, uint8_t *tilestore, uint16_t param2);

void fob_codec5(uint8_t *dst, uint8_t *src, int16_t xoff, int16_t yoff,
		uint16_t fobw, uint16_t fobh, uint16_t winx1, uint16_t winy1,
		uint16_t winx2, uint16_t winy2, uint8_t *tilestore, uint16_t param2);

void fob_codec3_flag8_work(uint8_t *dst, uint8_t *src, int16_t xoff, int16_t yoff,
			   uint16_t fobw, uint16_t fobh, uint16_t winx1, uint16_t winy1,
			   uint16_t winx2, uint16_t winy2);

void fob_codec3_flag8(uint8_t *dst, uint8_t *src, int16_t xoff, int16_t yoff,
		      uint16_t fobw, uint16_t fobh, uint16_t winx1, uint16_t winy1,
		      uint16_t winx2, uint16_t winy2);
void fob_codec23(uint8_t *dst, uint8_t *src, int16_t xoff, int16_t yoff,
		 uint8_t *fobjdata,  uint16_t winx1, uint16_t winy1,
		 uint16_t winx2, uint16_t winy2);
void fob_codec21(uint8_t *dst, uint8_t *src, int16_t xoff, int16_t yoff,
		 uint8_t *fobjdata,  uint16_t winx1, uint16_t winy1,
		 uint16_t winx2, uint16_t winy2);
void fob_codec20(uint8_t *dst, uint8_t *src, int16_t xoff, int16_t yoff,
		 uint8_t *fobjdata,  uint16_t winx1, uint16_t winy1,
		 uint16_t winx2, uint16_t winy2);
void fob_codec1_flipx(uint8_t *dst, uint8_t *src, int16_t xoff, int16_t yoff,
		      uint16_t fobw, uint16_t fobh, uint16_t winx1, uint16_t winy1,
		      uint16_t winx2, uint16_t winy2);
void fob_codec1_flipxy(uint8_t *dst, uint8_t *src, int16_t xoff, int16_t yoff,
		       uint16_t fobw, uint16_t fobh, uint16_t winx1, uint16_t winy1,
		       uint16_t winx2, uint16_t winy2);
void fob_codec1_flipy(uint8_t *dst, uint8_t *src, int16_t xoff, int16_t yoff,
		      uint16_t fobw, uint16_t fobh, uint16_t winx1, uint16_t winy1,
		      uint16_t winx2, uint16_t winy2);
void fob_codec1(uint8_t *dst, uint8_t *src, int16_t xoff, int16_t yoff,
		uint16_t fobw, uint16_t fobh, uint16_t winx1, uint16_t winy1,
		uint16_t winx2, uint16_t winy2);
void fob_codec3(uint8_t *dst, uint8_t *src, int16_t xoff, int16_t yoff,
		uint16_t fobw, uint16_t fobh, uint16_t winx1, uint16_t winy1,
		uint16_t winx2, uint16_t winy2);
void fob_codec31_flipx(uint8_t *dst, uint8_t *src, int16_t xoff, int16_t yoff,
		       uint16_t fobw, uint16_t fobh, uint16_t winx1, uint16_t winy1,
		       uint16_t winx2, uint16_t winy2, uint8_t fob_p1);
void fob_codec31(uint8_t *dst, uint8_t *src, int16_t xoff, int16_t yoff,
		 uint16_t fobw, uint16_t fobh, uint16_t winx1, uint16_t winy1,
		 uint16_t winx2, uint16_t winy2, uint8_t fob_p1);
void fob_codec32(uint8_t *dst, uint8_t *src, int16_t xoff, int16_t yoff,
		 uint16_t fobw, uint16_t fobh, uint16_t winx1, uint16_t winy1,
		 uint16_t winx2, uint16_t winy2, uint8_t fob_p1);
void fob_codec2(uint8_t *dst, uint8_t *src, int16_t xoff, int16_t yoff,
		uint16_t fobw, uint16_t fobh, uint16_t winx1, uint16_t winy1,
		uint16_t winx2, uint16_t winy2, uint32_t datasize);

void fob_codec4_lo(uint8_t *dst, uint8_t *csrc, uint8_t *tilestore, uint8_t *fobsrc,
		   uint16_t yblocks, uint16_t *proj, uint16_t xblocks, uint16_t yoff,
		   uint16_t winy1, uint16_t winy2, uint16_t srcbytes_per_column,
		   uint8_t fob_p1, uint16_t fob_p2);
void fob_codec4_lo_p2(uint8_t *dst, uint8_t *csrc, uint8_t *tilestore, uint8_t *fobsrc,
		      uint16_t yblocks, uint16_t *proj, uint16_t xblocks, uint16_t yoff,
		      uint16_t winy1, uint16_t winy2, uint16_t srcbytes_per_column,
		      uint8_t fob_p1, uint16_t fob_p2);
void fob_codec4_hi(uint8_t *dst, uint8_t *csrc, uint8_t *tilestore, uint8_t *fobsrc,
		   uint16_t yblocks, uint16_t *proj, uint16_t xblocks, uint16_t yoff,
		   uint16_t winy1, uint16_t winy2, uint16_t srcbytes_per_column,
		   uint8_t fob_p1, uint16_t fob_p2);
void fob_codec4_hi_p2(uint8_t *dst, uint8_t *csrc, uint8_t *tilestore, uint8_t *fobsrc,
		      uint16_t yblocks, uint16_t *proj, uint16_t xblocks, uint16_t yoff,
		      uint16_t winy1, uint16_t winy2, uint16_t srcbytes_per_column,
		      uint8_t fob_p1, uint16_t fob_p2);
void fob_codec5_lo(uint8_t *dst, uint8_t *csrc, uint8_t *tilestore, uint8_t *fobsrc,
		    uint16_t yblocks, uint16_t *proj, uint16_t xblocks, uint16_t yoff,
		    uint16_t winy1, uint16_t winy2, uint16_t srcbytes_per_column,
		    uint8_t fob_p1, uint16_t fob_p2);
void fob_codec5_lo_p2(uint8_t *dst, uint8_t *csrc, uint8_t *tilestore, uint8_t *fobsrc,
		      uint16_t yblocks, uint16_t *proj, uint16_t xblocks, uint16_t yoff,
		      uint16_t winy1, uint16_t winy2, uint16_t srcbytes_per_column,
		      uint8_t fob_p1, uint16_t fob_p2);
void fob_codec5_hi(uint8_t *dst, uint8_t *csrc, uint8_t *tilestore, uint8_t *fobsrc,
		   uint16_t yblocks, uint16_t *proj, uint16_t xblocks, uint16_t yoff,
		   uint16_t winy1, uint16_t winy2, uint16_t srcbytes_per_column,
		   uint8_t fob_p1, uint16_t fob_p2);
void fob_codec5_hi_p2(uint8_t *dst, uint8_t *csrc, uint8_t *tilestore, uint8_t *fobsrc,
		      uint16_t yblocks, uint16_t *proj, uint16_t xblocks, uint16_t yoff,
		      uint16_t winy1, uint16_t winy2, uint16_t srcbytes_per_column,
		      uint8_t fob_p1, uint16_t fob_p2);


#endif
