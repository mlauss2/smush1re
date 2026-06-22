/*
 * SMUSHv1 RE
 *
 * res - Resource.
 */

#include "smush.h"

int16_t res_resource_load(char *filename, void **buf_out)
{
	int h, r, size;

	res_resource_free(buf_out);
	h = fle_open(filename, "rb");
	if (!h)
		return -4;
	fle_seek(h, 0, 2);
	size = fle_tell(h);
	r = mem_blk_alloc(buf_out, size);
	if (r == 0) {
		fle_seek(h, 0, 0);
		fle_cd_read(h, *buf_out, size);
	}
	fle_close(h);
	return 0;
}

void res_resource_free(void **res)
{
	mem_blk_free(res);
}
