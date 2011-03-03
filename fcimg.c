/*
 * Copyright (C) 2011 <fwhacking|gmail:com>
 *
 * This is free software, licensed under the GNU General Public License v2.
 * See /LICENSE for more information.
 *
 */
 
/*
 * Code taken from busybox unzip applet
 */
 
/*
 * http://www.pkware.com/company/standards/appnote/
 * http://www.info-zip.org/pub/infozip/doc/appnote-iz-latest.zip
 */

#include <byteswap.h>
#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "fcimg.h"

static void print_usage(const char *name)
{
	printf("Usage: %s firmware\n", name);
}

int main(int argc, char **argv)
{
	FILE *fd;
	FC_HEADER *hdr;
	ZIP_HEADER *zhdr;
	ROMFS_HEADER *rhdr;
	unsigned char *fp;
	unsigned int romfs_start, romfs_length;
	unsigned int filename_len, dostime;
	uint32_t magic;
	char *filename;
	long size;

	if (argc != 2)
	{
		print_usage(argv[0]);
		exit(1);
	}

	fd = fopen(argv[1], "rb");
	if (fd == NULL)
	{
		printf("Could not read the firmware file '%s'\n", argv[1]);
		print_usage(argv[0]);
		exit(1);
	}

	size = fseek(fd, 0L, SEEK_END);
	size = ftell(fd);
	//~ printf("Firmware file size : %ld bytes\n\n", size);
	fseek(fd, 0L, SEEK_SET);

	fp = malloc(size);
	fread(fp, size, 1, fd);
	fclose(fd);

	hdr = (FC_HEADER *) fp;
	
	if (strncmp(hdr->magic, FOSCAM_MAGIC, 4) != 0)
	{
		printf("Magic '%s' not found\n", FOSCAM_MAGIC);
		free(fp);
		exit(1);
	}
	
	romfs_start = hdr->romfs_start;
	romfs_length = hdr->romfs_length;

	printf("[0x%08X] FIRMWARE HEADER\n", 0);
	printf("----------------------------------------\n");
	//~ printf("[0x%02lX] magic\t\t\t%s\n", offsetof(FC_HEADER, magic), FOSCAM_MAGIC);
	printf("[+0x%02lX] romfs start\t0x%08x\n", offsetof(FC_HEADER, romfs_start), romfs_start);
	printf("[+0x%02lX] romfs length\t%d\n", offsetof(FC_HEADER, romfs_length), romfs_length);
	printf("----------------------------------------\n\n");
	
	zhdr = (ZIP_HEADER *) (fp + sizeof(FC_HEADER));

	memcpy(&magic, &zhdr->magic, 4);
	magic = SWAP_BE32(magic);
	
	if (magic != ZIP_MAGIC)
	{
		printf("Magic '0x504b0304' not found, got '%x'\n", magic);
		free(fp);
		exit(1);
	}
	
	/* modification time */
	dostime = zhdr->modtime | (zhdr->moddate << 16);
	
	/* filename */
	filename_len = SWAP_LE16(zhdr->filename_len);
	filename = malloc(filename_len + 1);
	memcpy(filename, (char *) zhdr + ZIP_HEADER_LEN, filename_len);
	filename[filename_len] = '\0';

	printf("[0x%08lX] ZIP HEADER\n", sizeof(FC_HEADER));
	printf("----------------------------------------\n");
	//~ printf("[+0x%02lX] magic\t\t0x%08x\n", offsetof(ZIP_HEADER, magic), ZIP_MAGIC);
	//~ printf("[+0x%02lX] version\t\t%d\n", offsetof(ZIP_HEADER, version), SWAP_LE16(zhdr->version));
	//~ printf("[+0x%02lX] flags\t\t0x%04x\n", offsetof(ZIP_HEADER, flags), SWAP_LE16(zhdr->flags));
	//~ printf("[+0x%02lX] method\t\t0x%04x\n", offsetof(ZIP_HEADER, method), SWAP_LE16(zhdr->method));
	printf("[+0x%02lX] lastmod\t\t%02u-%02u-%02u %02u:%02u\n",
								offsetof(ZIP_HEADER, moddate),
								(dostime & 0x01e00000) >> 21,
								(dostime & 0x001f0000) >> 16,
								(((dostime & 0xfe000000) >> 25) + 1980) % 100,
								(dostime & 0x0000f800) >> 11,
								(dostime & 0x000007e0) >> 5);
	printf("[+0x%02lX] crc32\t\t0x%08x\n", offsetof(ZIP_HEADER, crc32), SWAP_LE32(zhdr->crc32));
	printf("[+0x%02lX] compsize\t%d\n", offsetof(ZIP_HEADER, cmpsize), SWAP_LE32(zhdr->cmpsize));
	printf("[+0x%02lX] ucompsize\t%d\n", offsetof(ZIP_HEADER, ucmpsize), SWAP_LE32(zhdr->ucmpsize));
	printf("[+0x%02X] filename\t%s\n", ZIP_HEADER_LEN, filename);
	printf("----------------------------------------\n\n");
	
	rhdr = (ROMFS_HEADER *) (fp + sizeof(FC_HEADER) + romfs_start);

	if (strncmp(rhdr->magic, ROMFS_MAGIC, 8) != 0)
	{
		printf("Magic '%s' not found\n", ROMFS_MAGIC);
		free(fp);
		exit(1);
	}

	printf("[0x%08lX] ROMFS HEADER\n", sizeof(FC_HEADER) + romfs_start);
	printf("----------------------------------------\n");
	//~ printf("[0x%02lX] magic\t\t\t%s\n", offsetof(ROMFS_HEADER, magic), ROMFS_MAGIC);
	printf("[+0x%02lX] size\t\t%d\n", offsetof(ROMFS_HEADER, size), SWAP_BE32(rhdr->size));
	printf("[+0x%02lX] checksum\t0x%8x\n", offsetof(ROMFS_HEADER, checksum), SWAP_BE32(rhdr->checksum));
	printf("[+0x%02lX] name\t\t%s\n", offsetof(ROMFS_HEADER, name), rhdr->name);
	printf("----------------------------------------\n\n");

	free(fp);

	exit(0);
}
