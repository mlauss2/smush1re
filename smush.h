/*
 * SMUSHv1 RE
 *
 * Main header
 *
 */

#ifndef _SMUSH_H_
#define _SMUSH_H_

#include <inttypes.h>
#include <setjmp.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define bswap_16(value) \
	((((value) & 0xff) << 8) | ((value) >> 8))

#define bswap_32(value) \
	(((uint32_t)bswap_16((uint16_t)((value) & 0xffff)) << 16) | \
	  (uint32_t)bswap_16((uint16_t)((value) >> 16)))



#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__

#define le32_to_cpu(x) (x)
#define le16_to_cpu(x) (x)
#define be32_to_cpu(x) bswap_32(x)
#define be16_to_cpu(x) bswap_16(x)
#define cpu_to_le16(x) (x)


static inline uint16_t __le16ua(uint8_t *p)
{
	return p[0] | (p[1] << 8);
}

static inline uint32_t __le32ua(uint8_t *p)
{
	return (uint32_t)p[0] | ((uint32_t)p[1] << 8) |
	       ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}

static inline uint16_t __be16ua(uint8_t *p)
{
	return p[1] | (p[0] << 8);
}

static inline uint32_t __be32ua(uint8_t *p)
{
	return (uint32_t)p[3] | ((uint32_t)p[2] << 8) |
	((uint32_t)p[1] << 16) | ((uint32_t)p[0] << 24);
}

static inline uint16_t __le16(uint8_t *p)
{
	return *(uint16_t *)p;
}

static inline uint32_t __le32(uint8_t *p)
{
	return *(uint32_t *)p;
}

static inline uint16_t __be16(uint8_t *p)
{
	return bswap_16(*(uint16_t *)p);
}

static inline uint32_t __be32(uint8_t *p)
{
	return bswap_32(*(uint32_t *)p);
}


#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__

#define be32_to_cpu(x)  (x)
#define be16_to_cpu(x)  (x)
#define le32_to_cpu(x)  bswap_32(x)
#define le16_to_cpu(x)  bswap_16(x)
#define cpu_to_le16(x)  bswap_16(x)

/* bytewise read an unaligned 16bit value from memory */
static inline uint16_t __le16ua(uint8_t *p)
{
	return p[1] | (p[0] << 8)
}

/* bytewise read an unaligned 32bit value from memory */
static inline uint32_t __le32ua(uint8_t *p)
{
	return (uint32_t)p[3] | ((uint32_t)p[2] << 8) |
	((uint32_t)p[1] << 16) | ((uint32_t)p[0] << 24);
}

/* bytewise read an unaligned 16bit value from memory */
static inline uint16_t __be16ua(uint8_t *p)
{
	return p[0] | (p[1] << 8)
}

/* bytewise read an unaligned 32bit value from memory */
static inline uint32_t __be32ua(uint8_t *p)
{
	return (uint32_t)p[0] | ((uint32_t)p[1] << 8) |
	((uint32_t)p[2] << 16) | ((uint32_t)p[4] << 24);
}

static inline uint16_t __be16(uint8_t *p)
{
	return *(uint16_t *)p;
}

static inline uint32_t __be32(uint8_t *p)
{
	return *(uint32_t *)p;
}

static inline uint16_t __le16(uint8_t *p)
{
	return bswap_16(*(uint16_t *)p);
}

static inline uint32_t __le32(uint8_t *p)
{
	return bswap_32(*(uint32_t *)p);
}

#else

#error "unknown endianness"

#endif

#include "anm.h"
#include "ctl.h"
#include "fle.h"
#include "fob.h"
#include "msc.h"
#include "res.h"
#include "sou.h"
#include "sys.h"
#include "txt.h"
#include "vid.h"
#include "game.h"
#include "anm_globals.h"

#endif
