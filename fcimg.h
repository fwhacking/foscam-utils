/*
 * Copyright (C) 2011 <fwhacking|gmail:com>
 *
 * This is free software, licensed under the GNU General Public License v2.
 * See /LICENSE for more information.
 *
 */

/*
 * Some code borrowed from BusyBox platform.h
 * Copyright 2006, Bernhard Fischer
 */

#ifndef _FCIMG_H_
#define _FCIMG_H_

#include <byteswap.h>
#include <endian.h>

#define PACKED __attribute__ ((__packed__))

#ifdef __BIG_ENDIAN__
# define FC_BIG_ENDIAN 1
# define FC_LITTLE_ENDIAN 0
#elif __BYTE_ORDER == __BIG_ENDIAN
# define FC_BIG_ENDIAN 1
# define FC_LITTLE_ENDIAN 0
#else
# define FC_BIG_ENDIAN 0
# define FC_LITTLE_ENDIAN 1
#endif

#if FC_BIG_ENDIAN
#define SWAP_BE16(x) (x)
#define SWAP_BE32(x) (x)
#define SWAP_BE64(x) (x)
#define SWAP_LE16(x) bswap_16(x)
#define SWAP_LE32(x) bswap_32(x)
#define SWAP_LE64(x) bswap_64(x)
#else
#define SWAP_BE16(x) bswap_16(x)
#define SWAP_BE32(x) bswap_32(x)
#define SWAP_BE64(x) bswap_64(x)
#define SWAP_LE16(x) (x)
#define SWAP_LE32(x) (x)
#define SWAP_LE64(x) (x)
#endif

#define FOSCAM_MAGIC	"BNEG"
#define ROMFS_MAGIC		"-rom1fs-"
#define ZIP_MAGIC		0x504b0304

#define ZIP_HEADER_LEN	30

typedef struct _FC_HEADER
{
	char magic[4];
	uint32_t unknown1;
	uint32_t unknown2;
	uint32_t romfs_start;
	uint32_t romfs_length;
} FC_HEADER;

typedef struct _ROMFS_HEADER
{
	char magic[8];
	uint32_t size;
	uint32_t checksum;
	char name[16];
} ROMFS_HEADER;

typedef struct _ZIP_HEADER
{
	uint32_t magic;
	uint16_t version;
	uint16_t flags;
	uint16_t method;
	uint16_t modtime;
	uint16_t moddate;
	uint32_t crc32 PACKED;
	uint32_t cmpsize PACKED;
	uint32_t ucmpsize PACKED;
	uint16_t filename_len;
	uint16_t extra_len;
} ZIP_HEADER;

#endif // _FCIMG_H_
