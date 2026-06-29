/*
 * SMUSHv1 RE
 *
 * fle - file related
 *
 * STATUS: Done.
 */

#include "smush.h"


uint32_t fle_open(char *filename, char *openmode)
{
	FILE *f;
	ssize_t r;
	uint32_t u32, cs;
	int i, j, ckscnt;
	char chkfile[80], *s;

	for (i = 1; i < 10; i++)
		if (AG(fle_handles)[i] == 0)
			break;

	if (i > 9)
		return 0;

	f = fopen(filename, openmode);
	AG(fle_handles)[i] = f;
	if (!f)
		return 0;

	AG(fle_offsets)[i] = 0;
	fseek(f, 0, SEEK_END);
	AG(fle_sizes)[i] = ftell(f);
	fseek(f, 0, SEEK_SET);
	strcpy(chkfile, filename);
	s = strchr(chkfile, '.');
	if (s)
		*s = 0;
	strcat(chkfile, ".CHK");

	/* the engine now tries 50 times to verify the .CHK checksum file itself
	 * related to the main .ANM file, but doesn't do anything special if this
	 * verification fails 50 times.
	 */
	ckscnt = 50;
	while (ckscnt--) {
		FILE *chkf = fopen(chkfile, "rb");
		if (chkf) {
			fseek(chkf, 0, SEEK_END);
			cs = ftell(chkf);
			fseek(chkf, 0, SEEK_SET);
			int16_t ret = mem_blk_alloc((void **)&(AG(fle_cksum_file_buffer)[i]), cs);
			if (ret == 0) {
				r = fread(AG(fle_cksum_file_buffer)[i], cs, 1, chkf);
				if (AG(smush_debug_enabled))
					printf("Reading checksum size " PRIu32 "\n", cs);
			}
			fclose(chkf);
		}
		if (AG(fle_cksum_file_buffer)[i] == 0)
			break;
		uint32_t cks = 0;
		uint32_t *cd = (uint32_t *)(((uint8_t *)AG(fle_cksum_file_buffer)[i]) + 8);
		j = (cs - 12) / 4;
		while (j--) {
			cks += *cd++;
		}
		if (cks == *cd) {
			if (AG(smush_debug_enabled))
				printf("+ checksum okay.\n");
			ckscnt = 0;
			break;
		} else {
			mem_blk_free((void **)&(AG(fle_cksum_file_buffer)[i]));
			if (AG(smush_debug_enabled))
				printf("- checksum failed.\n");
		}
	}
	return i;
}

void fle_close(uint32_t handle)
{
	if (handle && handle < 10) {
		fclose(AG(fle_handles)[handle]);
		AG(fle_handles)[handle] = 0;
		mem_blk_free((void *)(AG(fle_cksum_file_buffer))[handle]);
	}
}

int16_t fle_cd_checksum(uint32_t *data,uint32_t blkindex,uint16_t numblks, uint32_t *cksumbuf)
{
	uint32_t *csb, iv, bref;

	csb = (uint32_t *)((uint8_t *)(cksumbuf) + (blkindex * 4) + 8);
	do {
		if (numblks == 0) {
			AG(fle_cksum_2b238c) = 50;
			return 0;
		}
		iv = 0;
		for (int j = 32; j; j--) {
			iv += data[0] + data[1] + data[2] + data[3] +
			      data[4] + data[5] + data[6] + data[7] +
			      data[8] + data[9] + data[10] + data[11] +
			      data[12] + data[13] + data[14] + data[15];
			data += 16;
		}
		--numblks;
		bref = *csb++;
	} while (iv == bref);
	if (AG(smush_debug_enabled))
		printf("Sum_err.." PRIu32 " !=" PRIu32 "\n", iv, bref);
	sprintf(AG(anm_errstr), "CD Checksum error.." PRIu32 " !=" PRIu32 "\n", iv, bref);
	return 1;
}

int32_t fle_cd_read(uint32_t flehandle,void *dstbuf,uint32_t size)
{
	int dataread, retrycnt, retry2;
	uint32_t r, v1, v2, v3, *d;
	ssize_t rs;

	retrycnt = 50;
	retry2 = 5;
	dataread = 0;
	while (1) {
		r = fread(dstbuf, 1, size, AG(fle_handles)[flehandle]);
		if (AG(fle_cksum_file_buffer)[flehandle] == NULL) {
			AG(fle_offsets)[flehandle] += r;
			if (dataread) {
				sys_timer_less_cpuusage();
			}
			return r;
		}
		if (AG(fle_sizes)[flehandle] < (AG(fle_offsets)[flehandle] + size)) {
			size = AG(fle_sizes)[flehandle] - AG(fle_offsets)[flehandle];
		}
		v1 = AG(fle_offsets)[flehandle] & 0xfffff800;
		v2 = AG(fle_offsets)[flehandle] - v1;
		v3 = (size - v2) >> 11;
		if (v3 <= 0)
			break;
		d = (uint32_t *)((uint8_t *)dstbuf + v2);
		v1 = (AG(fle_offsets)[flehandle]) >> 11;
		if (0 == fle_cd_checksum(d, v1, v3, AG(fle_cksum_file_buffer)[flehandle]))
			break;	/* OK */

		if (--retrycnt == 0) {
			if (retry2-- > 0) {
				retrycnt = 50;
				if (!dataread) {
					dataread = 1;
					sys_timer_more_cpuusage();
				}
				/* this apparently forces the laser of the cdrom
				 * drive to refocus, and maybe get a better read result
				 */
				fseek(AG(fle_handles)[flehandle], 0, SEEK_SET);
				rs = fread(dstbuf, 1, size, AG(fle_handles)[flehandle]);
			} else {
				/* give up, drive cannot recover this block */
				if (AG(smush_debug_enabled)) {
					printf("Continuing with bad block!\n");
				}
			}
		}

		/* set to retry position */
		fflush(AG(fle_handles)[flehandle]);
		fseek(AG(fle_handles)[flehandle], AG(fle_offsets)[flehandle], SEEK_SET);
	}

	AG(fle_offsets)[flehandle] += r;
	if (dataread) {
		sys_timer_less_cpuusage();
	}

	return r;
}

uint32_t fle_seek(uint32_t flehandle,uint32_t offset,int16_t whence)
{
	uint32_t r;

	fseek(AG(fle_handles)[flehandle], offset, whence);
	r = ftell(AG(fle_handles)[flehandle]);
	AG(fle_offsets)[flehandle] = r;
	return r;
}

uint32_t fle_tell(uint32_t flehandle)
{
	return ftell(AG(fle_handles)[flehandle]);
}

int16_t fle_streamer_init(uint32_t size)
{
	int16_t ret;
	void *buf;
	ret = mem_blk_alloc((void **)&buf, size + 0xffff);
	//ret = mem_blk_alloc((void **)&(AG(fle_buffer)), size + 0xffff);
	if (ret == 0) {
		AG(fle_buffer) = (uintptr_t)buf;
		AG(fle_bufsize_2kaligned) = ((size >> 8) & 0x00fffff8) << 8;
		AG(fle_buffer_2kaligned_end) = AG(fle_buffer) + AG(fle_bufsize_2kaligned);
		AG(fle_streamer_seeknew_flag) = 0;
		AG(fle_streamer_readbuffer_wrptr) = (uintptr_t)AG(fle_buffer);
		AG(fle_streamer_readbuffer_rdptr) = (uintptr_t)AG(fle_buffer);
		AG(fle_streamer_readbuffer_lastreadsize) = 0;
		AG(fle_streamer_inblk_offset) = 0;
		AG(fle_streamer_fhandle) = 0;
		AG(fle_streamer_dispense_inhibit) = 0;
		msc_memset(buf, 0, AG(fle_bufsize_2kaligned));
	}
	return ret;
}

void fle_streamer_terminate(void)
{
	mem_blk_free((void *)&(AG(fle_buffer)));
}

void fle_streamer_seek(uint32_t flehandle, int32_t seekofs)
{
	if (AG(smush_debug_enabled)) {
		/* original has 2 nested loops to waste cycles to generate delay */
	}
	fle_seek(flehandle, ((seekofs >> 8) &  0x00fffff8) << 8, 0);
	AG(fle_streamer_fhandle) = flehandle;
	AG(fle_streamer_readbuffer_newpos) = AG(fle_streamer_readbuffer_wrptr);
	AG(fle_streamer_inblk_offset) = seekofs & 0x07ff;
	AG(fle_streamer_seeknew_flag) = 0xff;
	/* it doesn't make sense, but that's what the assembly says: INC + DEC */
	AG(fle_streamer_dispense_inhibit) += 1;
	AG(fle_streamer_dispense_inhibit) -= 1;
}

int16_t fle_streamer_switch(void)
{
	if (AG(fle_streamer_seeknew_flag)) {
		AG(fle_streamer_seeknew_flag) = 0;
		AG(fle_streamer_readbuffer_rdptr) = AG(fle_streamer_readbuffer_newpos);
		AG(fle_streamer_readbuffer_lastreadsize) = 0;
		return 0;
	}
	return -1;
}

int32_t fle_streamer_get_available_data(void)
{
	if (AG(fle_streamer_readbuffer_rdptr) <= AG(fle_streamer_readbuffer_wrptr)) {
		return (AG(fle_streamer_readbuffer_rdptr) - AG(fle_streamer_readbuffer_wrptr));
	}
	return AG(fle_streamer_readbuffer_wrptr) - AG(fle_streamer_readbuffer_rdptr) + AG(fle_bufsize_2kaligned);
}

int8_t fle_streamer_check(uint32_t size)
{
	if (AG(fle_streamer_readbuffer_wrptr) < AG(fle_streamer_readbuffer_rdptr)) {
		if ((AG(fle_bufsize_2kaligned) + AG(fle_streamer_readbuffer_wrptr)) < (AG(fle_streamer_readbuffer_rdptr) + size)) {
			return 0;
		}
	} else if (AG(fle_streamer_readbuffer_wrptr) < (AG(fle_streamer_readbuffer_rdptr) + size)) {
		return 0;
	}
	return 0xff;
}

uint8_t *fle_streamer_dispense(uint32_t size)
{
	int c1, c2;
	int32_t s1;

	if ((AG(fle_streamer_fhandle) == 0) || (AG(fle_streamer_dispense_inhibit) != 0))
		return (uint8_t *)(-1);

	if (AG(fle_buffer_ready) == 0) {
		AG(fle_buffer_ready) = 1;
		AG(fle_streamer_readbuffer_rdptr) += AG(fle_streamer_readbuffer_lastreadsize);
		AG(fle_streamer_readbuffer_lastreadsize) = 0;
		if (AG(fle_buffer_2kaligned_end) <= AG(fle_streamer_readbuffer_rdptr)) {
			AG(fle_streamer_readbuffer_rdptr) -= AG(fle_bufsize_2kaligned);
		}
	}

	if (AG(fle_streamer_seeknew_flag) != 0) {
		if (AG(fle_streamer_readbuffer_newpos) < AG(fle_streamer_readbuffer_rdptr)) {
			c1 = ((AG(fle_streamer_readbuffer_rdptr) + size) <  (AG(fle_bufsize_2kaligned) + AG(fle_streamer_readbuffer_newpos)));
			c2 = ((AG(fle_streamer_readbuffer_rdptr) + size) == (AG(fle_bufsize_2kaligned) + AG(fle_streamer_readbuffer_newpos)));
		} else {
			c1 = ((AG(fle_streamer_readbuffer_rdptr) + size) <  AG(fle_streamer_readbuffer_newpos));
			c2 = ((AG(fle_streamer_readbuffer_rdptr) + size) == AG(fle_streamer_readbuffer_newpos));
		}
		if ((c1 == 0) && (c2 == 0)) {
			fle_streamer_switch();
			AG(fle_buffer_ready) = 0;
			return NULL;
		}
	}

	if ((AG(fle_streamer_seeknew_flag) == 0) && (AG(fle_streamer_inblk_offset) != 0)) {
		size += AG(fle_streamer_inblk_offset);
	}
	if (fle_streamer_check(size) != 0) {
		AG(fle_buffer_ready) = 0;
		s1 = AG(fle_streamer_readbuffer_rdptr) + size - AG(fle_buffer_2kaligned_end);
		AG(fle_streamer_readbuffer_lastreadsize) = size;
		if (s1 > 0) {
			msc_memcpy((void *)AG(fle_buffer_2kaligned_end), (void *)AG(fle_buffer), s1);
		}
		if ((AG(fle_streamer_seeknew_flag) == 0) && (AG(fle_streamer_inblk_offset) != 0)) {
			uint8_t *r = (uint8_t *)(AG(fle_streamer_readbuffer_rdptr) + AG(fle_streamer_inblk_offset));
			AG(fle_streamer_inblk_offset) = 0;
			return r;
		}
		return (uint8_t *)AG(fle_streamer_readbuffer_rdptr);
	}
	return (uint8_t *)(-1);
}

void fle_streamer_acquire(void)
{
	uint32_t size;


	if (0 == AG(fle_streamer_fhandle))
		return;

	if ((AG(fle_streamer_readbuffer_wrptr) < AG(fle_streamer_readbuffer_rdptr)) ||
	    (AG(fle_streamer_readbuffer_rdptr) == AG(fle_buffer))) {
		if ((AG(fle_streamer_readbuffer_rdptr) == AG(fle_buffer)) &&
		    (0x800 < (AG(fle_buffer_2kaligned_end) - AG(fle_streamer_readbuffer_wrptr)))) {
			size = (AG(fle_buffer_2kaligned_end) - AG(fle_streamer_readbuffer_wrptr)) - 0x800;
		} else {
			size = AG(fle_streamer_readbuffer_rdptr) - AG(fle_streamer_readbuffer_wrptr);
			size -= 0x800;
			size = (size >> 8) & 0x00fffff8;
			size = size << 8;
			if (size < 0x800)
				return;
		}
	} else {
		size = AG(fle_buffer_2kaligned_end) - AG(fle_streamer_readbuffer_wrptr);
	}
	if (size > 0x10000) {
		size = 0x10000;
	}

	if (size == 0) {
		sys_delay(100);
	} else {
		sys_delay(1345);
		fle_cd_read(AG(fle_streamer_fhandle), (void *)AG(fle_streamer_readbuffer_wrptr), size);
	}
	AG(fle_streamer_readbuffer_wrptr) += size;
	if (AG(fle_buffer_2kaligned_end) <= AG(fle_streamer_readbuffer_wrptr)) {
		AG(fle_streamer_readbuffer_wrptr) -= AG(fle_bufsize_2kaligned);
	}
}
