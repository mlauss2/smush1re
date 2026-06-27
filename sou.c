/*
 * SMUSHv1 RE
 *
 * sou - sound.
 */

#include "smush.h"

/* audio tags */
#define CREA 0x43726561
#define PSAD 0x50534144
#define PSD2 0x50534432
#define PVOC 0x50564f43
#define RAW_ 0x52415720
#define SAUD 0x53415544


int16_t sou_init(void)
{
	int16_t ret;

	/* 8 bit dest format; sou_init_platform() can override */
	AG(sou_soudrv_sizeshift) = 0;
	ret = sou_init_platform();
	if (ret)
		return ret;

	if (AG(sou_soudrv_pref_bufsize) < 150000)
		return -2;

	for (int i = 0; i < 255; i++)
		AG(sou_hooks)[i] = 0;

	AG(sou_global_volume) = 16;
	int s = 0x1100 < AG(sou_soudrv_sizeshift);
	ret = mem_blk_alloc((void **)&(AG(sou_soudrv_LUT1)), s);
	if (ret)
		goto out;

	ret = mem_blk_alloc((void **)&(AG(sou_soudrv_LUT2)), s);
	if (ret)
		goto out;

	ret = mem_blk_alloc((void **)&((AG(sou_channel)[1]).buffer), 45000);
	if (ret)
		goto out;
	ret = mem_blk_alloc((void **)&((AG(sou_channel)[2]).buffer), 45000);
	if (ret)
		goto out;
	ret = mem_blk_alloc((void **)&((AG(sou_channel)[3]).buffer), 45000);
	if (ret)
		goto out;
	ret = mem_blk_alloc((void **)&(AG(sou_engine_fill_buf5120)), 5120);
	if (ret)
		goto out;
	do {
		ret = mem_blk_alloc((void **)&((AG(sou_channel)[0]).buffer), AG(sou_soudrv_pref_bufsize));
		if (ret) {
			AG(sou_soudrv_pref_bufsize) -= 10000;
			if (AG(sou_soudrv_pref_bufsize) < 20000)
				goto out;
		}
	} while (ret != 0);

	snprintf(AG(sou_soudrv_cfgstr1), 100, "SOUND CONFIG:  BASE %X, IRQ %d, CHAN %d",
		 AG(sou_soudrv_iobase), AG(sou_soudrv_irq), AG(sou_soudrv_dma));
	snprintf(AG(sou_soudrv_cfgstr2), 100, "SOUND BUFFER:  %d BYTES", AG(sou_soudrv_pref_bufsize));
	msc_memset((AG(sou_channel)[0]).buffer, 127, AG(sou_soudrv_pref_bufsize));
	msc_memset((AG(sou_channel)[1]).buffer, 127, 45000);
	msc_memset((AG(sou_channel)[2]).buffer, 127, 45000);
	msc_memset((AG(sou_channel)[3]).buffer, 127, 45000);

	AG(sou_channel)[0].status = 0;
	AG(sou_channel)[1].status = 0;
	AG(sou_channel)[2].status = 0;
	AG(sou_channel)[3].status = 0;

	ret = AG(sou_soudrv_cb1_init_fn)(AG(sou_dmabuf_arr), 0x400 << AG(sou_soudrv_sizeshift),
					 AG(sou_soudrv_iobase), AG(sou_soudrv_dma),
					 AG(sou_soudrv_irq), AG(sou_soudrv_LUT1),
					 AG(sou_soudrv_LUT2));
	AG(sou_soudrv_initstatus) = (ret == 0);
	return 0;

out:
	sou_terminate_platform();
	return -2;
}

void sou_terminate(void)
{
	if (AG(sou_soudrv_type) != 0) {
		if (AG(sou_soudrv_initstatus) != 0) {
			/* AG(sou_soudrv_cb2_shutdown_fn)(); */
		}
		sou_terminate_platform();
		mem_blk_free((void **)&((AG(sou_channel)[0]).buffer));
		mem_blk_free((void **)&((AG(sou_channel)[1]).buffer));
		mem_blk_free((void **)&((AG(sou_channel)[2]).buffer));
		mem_blk_free((void **)&((AG(sou_channel)[3]).buffer));
		mem_blk_free((void **)&(AG(sou_engine_fill_buf5120)));
		mem_blk_free((void **)&(AG(sou_soudrv_LUT1)));
		mem_blk_free((void **)&(AG(sou_soudrv_LUT2)));
	}
}

void sou_engine_start(uint8_t *data, uint16_t streamid)
{
	uint32_t cid, tid, idx, maxidx, size, is_voice;

	if (AG(sou_soudrv_type) == 0)
		return;

	cid = be32_to_cpu(*(uint32_t *)data + 0);
	if (cid == CREA) {
		uint16_t s1 = le16_to_cpu(*(uint16_t *)(data + 0x14));
		uint8_t e1 = data[s1 + 1];
		uint32_t s2 = (e1 << 8) | e1 | (e1 << 16);
		data = (uint8_t *)(data + s1 + 6);
		sou_engine_start_stream_raw(data, s2, streamid);
	} else if (cid == RAW_) {
		sou_engine_start_stream_raw(data + 8, le16_to_cpu(*(uint16_t *)(data + 4)), streamid);
	} else if (cid != PSAD && cid != PSD2 && cid != PVOC && cid != SAUD) {
		return;
	}

	is_voice = (cid == PVOC);
	if (is_voice && (AG(sou_voice_enabled) == 0))
		return;

	if (cid == SAUD) {
		tid = 0;
		idx = 0;
		maxidx = 1;
		size = be32_to_cpu(*(uint32_t *)(data + 4)) + 8;
	} else {
		size = be32_to_cpu(*(uint32_t *)(data + 4)) - 12;
		tid = be32_to_cpu(*(uint32_t *)(data + 8));
		idx = be32_to_cpu(*(uint32_t *)(data + 12));
		maxidx = be32_to_cpu(*(uint32_t *)(data + 16));
		data += 20;
	}
	sou_engine_start_stream(data, size, tid, idx, maxidx, streamid, is_voice);
}

void sou_engine_kill_dead_channels(void)
{
	if (AG(sou_paused))
		return;

	for (int i = 3; -1 < i; i--) {
		struct _sou_channel *s = &(AG(sou_channel)[i]);
		if ((s->status == 3) && (s->status_prev == 3) && (s->updated == 0)) {
			s->status = 0;
		}
		s->updated = 0;
		s->status_prev = s->status;
	}
	if (AG(smush_debug_enabled)) {
		struct _sou_channel *s = &(AG(sou_channel)[0]);
		if (s->playpos < s->bytesread) {
			msc_memset(AG(anm_default_dst) + 0x280a, 0xe3,
				   (s->bytesread - s->playpos) >> 11);
			(AG(anm_default_dst))[AG(sou_soudrv_pref_bufsize) + 0x28a0] = 0xe3;
		}
	}
}

void sou_engine_start_stream_raw(uint8_t *data, uint32_t size, uint16_t streamid)
{
	struct _sou_channel *s;
	int chnum;

	if (AG(sou_paused) != 0)
		return;

	if (size < 90001) {
		if (44987 < size) {
			size = 44987;
		}
		for (chnum = 3; 0 < chnum; chnum--) {
			s = &(AG(sou_channel)[chnum]);
			if ((s->status == 0) && (s->buffer != NULL))
				break;
		}
		if (chnum == 0)
			return;
	} else {
		if ((AG(sou_soudrv_pref_bufsize) - 13) < size) {
			size = AG(sou_soudrv_pref_bufsize) - 13;
		}
		chnum = 0;
	}

	s = &(AG(sou_channel)[chnum]);
	if (s->buffer == NULL)
		return;
	s->streamid = streamid;
	s->buffer[0] = 1;
	s->buffer[1] = 8;
	s->buffer[2] = 0;
	s->buffer[3] = 0;
	s->buffer[4] = 0;
	s->buffer[5] = 0;
	s->buffer[6] = (size >> 24);
	s->buffer[7] = (size >> 16);
	s->buffer[8] = (size >>  8);
	s->buffer[9] = (size & 0xff);
	s->buffer[10] = 0;
	s->buffer[11] = 0;
	msc_memcpy(s->buffer + 12, data, size);
	s->strkptr = s->buffer;
	s->sdatptr = s->buffer + 12;
	s->maxpcmsize = (chnum == 0) ? (AG(sou_soudrv_pref_bufsize) - 12) : 44987;
	s->bytesread = size;
	s->updated = 1;
	s->status = 2;
}

void sou_engine_start_stream(uint8_t *data, uint32_t datasize, int32_t tid,
			     int32_t idx, int32_t maxidx, uint16_t streamid,
			     int8_t is_voice)
{
	struct _sou_channel *s;
	uint32_t maxcopysize, copysize;
	int chidx;

	if (AG(sou_paused) != 0)
		return;

	if (idx == 0) {
		chidx = 0;
		if (be32_to_cpu(*(uint32_t *)(data + 4)) < 45000) {
			for (chidx = 3; 0 < chidx; chidx--) {
				s = &(AG(sou_channel)[chidx]);
				if (s->buffer && (s->status == 0))
					break;
			}
			if (chidx == 0) {
				for (chidx = 3; 0 < chidx; chidx--) {
					s = &(AG(sou_channel)[chidx]);
					if (s->is_voice == 0)
						break;
				}

				if (chidx == 0) {
					if (((AG(sou_channel)[1]).age_cntr < (AG(sou_channel)[2]).age_cntr)
					    && ((AG(sou_channel)[3]).age_cntr < (AG(sou_channel)[2]).age_cntr)) {
						chidx = 2;
					} if ((AG(sou_channel)[1]).age_cntr < (AG(sou_channel)[3]).age_cntr) {
						chidx = 3;
					} else {
						chidx = 1;
					}
				}
			}
			(AG(sou_channel)[3]).age_cntr++;
			(AG(sou_channel)[2]).age_cntr++;
			(AG(sou_channel)[1]).age_cntr++;
			s = &(AG(sou_channel)[chidx]);
			s->status = 0;
			s->age_cntr = 0;
		}
		s = &(AG(sou_channel)[chidx]);
		s->streamid = streamid;
		s->trkid = tid;
		s->curridx = idx;
		s->maxidx = maxidx;
		maxcopysize = (chidx == 0) ? AG(sou_soudrv_pref_bufsize) : 45000;
		copysize = (datasize < maxcopysize) ? datasize : 45000;
		msc_memcpy(s->buffer, data, copysize);
		s->strkptr = s->buffer + 16;
		s->sdatptr = s->buffer + 24 + be32_to_cpu(*(uint32_t *)(s->buffer + 12));
		s->maxpcmsize = copysize + (uintptr_t)s->buffer - (uintptr_t)s->sdatptr;
		s->bytesread = datasize;
		s->sdatsize = be32_to_cpu(*(uint32_t *)(s->sdatptr - 4));
		s->updated = 1;
		s->is_voice = is_voice;
		s->status = 2;
	} else {
		for (chidx = 3; -1 < chidx; chidx--) {
			s = &(AG(sou_channel)[chidx]);
			if (s->buffer && (s->status != 0) && (s->trkid == tid)
			    && (s->maxidx == maxidx) && (s->curridx == idx)) {
				uint32_t b1 = s->bytesread % s->maxpcmsize, copysize;
				uint8_t *dst;
				s->curridx += 1;
				if (s->maxpcmsize < (b1 + datasize)) {
					msc_memcpy(s->sdatptr + b1, data, s->maxpcmsize - b1);
					data += (s->maxpcmsize - b1);
					dst = s->sdatptr;
					copysize = (datasize + b1) - s->maxpcmsize;
				} else {
					dst = s->sdatptr + b1;
					copysize = datasize;
				}
				msc_memcpy(dst, data, copysize);
				//x86_interrupts_disable();
				s->bytesread += datasize;
				//x86_interrupts_enable();
				s->updated = 1;
			}
		}
	}
}

/* this is the core of the sou_engine_fill() function, without the DOS/x86-specific
 * parts.
 */
void sou_engine_fill_core(void)
{
	int still_need_data, ret;
	ret = AG(sou_soudrv_cb3_ackint_fn)(AG(sou_dmabuf_arr)[1 - AG(sou_dmabuf_idx)], 0x400 << (AG(sou_soudrv_sizeshift & 0x1f)));
	if (!ret)
		return;

	still_need_data = 1;
	if (AG(sou_paused) == 0) {

	}
	if (still_need_data) {
		AG(sou_soudrv_cb6_silence_fn)(AG(sou_dmabuf_arr)[AG(sou_dmabuf_idx)], 0x400);
	}
	AG(sou_dmabuf_idx) ^= 1;

}

void sou_pause(void)
{
	AG(sou_pause_vol) = sou_engine_get_volume();
	sou_engine_set_volume(0);
	AG(sou_paused) = 1;
}

void sou_resume(void)
{
	sou_engine_set_volume(AG(sou_pause_vol));
	AG(sou_paused) = 0;
}

void sou_engine_stop_all(void)
{
	(AG(sou_channel)[0]).status = 0;
	(AG(sou_channel)[1]).status = 0;
	(AG(sou_channel)[2]).status = 0;
	(AG(sou_channel)[3]).status = 0;
}

uint16_t sou_engine_get_volume(void)
{
	if (AG(sou_global_volume) >= 15)
		return 127;
	return AG(sou_global_volume) << 3;
}

void sou_engine_set_volume(uint16_t vol)
{
	if (vol < 127)
		vol = vol >> 3;
	else
		vol = 16;

	/* yes that's in the assembly, it does not set sou_global_volume directly,
	 * instead relying on sou_hooks[255] being sou_global_volume...
	 */
	sou_engine_sethook(0xff, vol);
}

void sou_engine_stop(uint16_t streamid)
{
	for (int i = 0; i < 4; i++) {
		struct _sou_channel *s = &(AG(sou_channel)[i]);
		if (s->streamid == streamid) {
			s->status = 0;
			s->streamid = 0;
		}
	}
}

int16_t sou_engine_query(uint16_t streamid)
{
	if (streamid == 0)
		return ((AG(sou_channel)[0]).status != 0);
	for (int i = 0; i < 4; i++) {
		struct _sou_channel *s = &(AG(sou_channel)[i]);
		if (s->streamid == streamid)
			return (s->status != 0);
	}
	return 0;
}

uint8_t sou_engine_gethook(uint8_t index)
{
	/* index 0xff -> sou_global_volume! */
	return AG(sou_hooks)[index];
}

void sou_engine_sethook(uint8_t index,uint8_t val)
{
	/* index 0xff -> sou_global_volume! */
	AG(sou_hooks)[index] = val;
}

void sou_drv_set_volume(uint16_t vol)
{
	if (AG(sou_soudrv_cb7_setvol_fn))
		AG(sou_soudrv_cb7_setvol_fn)(vol);
}

uint16_t sou_drv_get_volume(void)
{
	if (AG(sou_soudrv_cb8_getvol_fn)) {
		return AG(sou_soudrv_cb8_getvol_fn)();
	}
	return 127;
}
