/*-----------------------------------------------------------------------*/
/* Low level disk I/O module SKELETON for FatFs     (C)ChaN, 2019        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/
#include <system.h>
#include "ff.h"			/* Obtains integer types */
#include "diskio.h"		/* Declarations of disk functions */

/* Definitions of physical drive number for each drive */
#define DEV_IDE		0
#define DEV_RAM		1	/* Example: Map Ramdisk to physical drive 0 */


/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
		BYTE pdrv		/* Physical drive nmuber to identify the drive */
		)
{
	DSTATUS stat;
	int result;

	switch (pdrv) {

		case DEV_IDE:
			result=0;
			return RES_OK;
			break;

		case DEV_RAM :
			result=0;
			return RES_OK;
			break;

	}
	return STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
		BYTE pdrv				/* Physical drive nmuber to identify the drive */
		)
{
	DSTATUS stat;
	int result;

	switch (pdrv) {
		case DEV_IDE:
			result=0;
			return RES_OK;
			break;

		case DEV_RAM :
			result=0;
			return RES_OK;
			break;
	}
	return STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/
DRESULT disk_read (
		BYTE pdrv,		/* Physical drive nmuber to identify the drive */
		BYTE *buff,		/* Data buffer to store read data */
		LBA_t sector,	/* Start sector in LBA */
		UINT count		/* Number of sectors to read */
		)
{
	DRESULT res;
	int result;

	switch (pdrv) {

		case DEV_IDE:
			result=0;
			hdc_read_sectors(sector,count,buff);
			return RES_OK;
			break;

		case DEV_RAM :
			result=0;
			rdc_read_sectors(sector,count,buff);
			return RES_OK;
			break;
	}

	return RES_PARERR;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if FF_FS_READONLY == 0
DRESULT disk_write (
		BYTE pdrv,			/* Physical drive nmuber to identify the drive */
		const BYTE *buff,	/* Data to be written */
		LBA_t sector,		/* Start sector in LBA */
		UINT count			/* Number of sectors to write */
		)
{
	DRESULT res;
	int result;

	switch (pdrv) {

		case DEV_IDE:
			result=0;
			hdc_write_sectors(sector,count,(void*)buff);
			return RES_OK;
			break;
		case DEV_RAM :
			result=0;
			rdc_write_sectors(sector,count,(void*)buff);
			return RES_OK;
			break;
	}

	return RES_PARERR;
}

#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

uint32_t hdc_get_primary_sector_count();
DRESULT disk_ioctl (
		BYTE pdrv,		/* Physical drive nmuber (0..) */
		BYTE cmd,		/* Control code */
		void *buff		/* Buffer to send/receive control data */
		)
{
	DRESULT res;
	int result;

	switch (pdrv) {
		case DEV_IDE:
			{
				if(cmd==GET_SECTOR_COUNT)
				{
					uint32_t sc=hdc_get_primary_sector_count();
					//printf("ioctl GET_SECTOR_COUNT %u\n",sc);
					*((LBA_t*)buff)=sc;
				}
				result=0;
				return RES_OK;
			}break;

		case DEV_RAM :
			result=0;
			return RES_OK;
			break;
	}

	return RES_PARERR;
}

