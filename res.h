/*
 * SMUSHv1 RE
 *
 * res - Resource.
 */

#ifndef _SMUSH_RES_H_
#define _SMUSH_RES_H_

#include "smush.h"

int16_t res_resource_load(char *filename, void **buf_out);
void res_resource_free(void **res);

#endif
