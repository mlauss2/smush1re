/*
 * SMUSHv1 RE
 *
 * fle - file related
 */

#ifndef _SMUSH_FLE_H_
#define _SMUSH_FLE_H_

#include "smush.h"

enum fle_seek_whence {
	FLE_SEEK_SET = 0,
	FLE_SEEK_CUR = 1,
	FLE_SEEK_END = 2,
};

uint32_t fle_open(char *filename, char *openmode);
void fle_close(uint32_t handle);
int16_t fle_cd_checksum(uint32_t *data,uint32_t blkindex,uint16_t numblks, uint32_t *cksumbuf);
int32_t fle_cd_read(uint32_t flehandle,void *dstbuf,uint32_t size);
uint32_t fle_seek(uint32_t flehandle,uint32_t offset,int16_t whence);
uint32_t fle_tell(uint32_t flehandle);
int16_t fle_streamer_init(uint32_t size);
void fle_streamer_terminate(void);
void fle_streamer_seek(uint32_t flehandle,int32_t seekofs);
int16_t fle_streamer_switch(void);
int32_t fle_streamer_get_available_data(void);
int8_t fle_streamer_check(uint32_t size);
uint8_t * fle_streamer_dispense(uint32_t size);
void fle_streamer_acquire(void);

#endif
