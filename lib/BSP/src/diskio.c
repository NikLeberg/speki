/**
 *****************************************************************************
 * @addtogroup 	FatFs FatFs
 * @{
 * @defgroup	FatFs_DiskIO Disk IO
 * @brief		Low level disk I/O module skeleton for FatFs.
 * @{
 *
 * @file		diskio.c
 * @version		R0.10
 * @date		2013-10-02
 * @author		ChaN
 *
 * @brief		Low level disk I/O module skeleton for FatFs.\n
 * @note		If a working storage control module is available, it should
 *				be attached to the FatFs via a glue function rather than
 *				modifying it.\n
 *				This is an example of glue functions to attach various
 *				exsisting storage control module to the FatFs module with a
 *				defined API.
 *
 *****************************************************************************
 * @copyright
 * @{
 *
 * Copyright &copy; 2013, ChaN, all right reserved.
 *
 * FatFs module is a generic FAT file system module for small embedded
 * systems.
 * This is a free software that opened for education, research and commercial
 * developments under license policy of following terms.
 *
 * * The FatFs module is a free software and there is NO WARRANTY.
 * * No restriction on use. You can use, modify and redistribute it for
 *   personal, non-profit or commercial products UNDER YOUR RESPONSIBILITY.
 * * Redistributions of source code must retain the above copyright notice.
 *
 * @}
 *****************************************************************************
 */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*----- Header-Files -------------------------------------------------------*/
#include <string.h>
#include "diskio.h"					/* FatFs lower layer API				*/
#include "stm32f4xx.h"
#include "stm32f4_sdio_sd.h"

/*----- Macros -------------------------------------------------------------*/
#define BLOCK_SIZE			512		/**< Block Size in Bytes				*/

/*----- Data types ---------------------------------------------------------*/

/*----- Function prototypes ------------------------------------------------*/

/*----- Data ---------------------------------------------------------------*/

/*----- Implementation -----------------------------------------------------*/
/**
 *****************************************************************************
 * \brief		Initialize the SD Card\n
 *				Returns the disk status.
 *
 * \param[in]	drv		Physical drive number (0..)
 * \return		DSTATUS
 *****************************************************************************
 */
DSTATUS disk_initialize(BYTE drv) {

	DSTATUS stat = 0;

#ifdef DBGIO
	printf("disk_initialize %d\n", drv);
#endif

	/* Supports only single drive */
	if (drv) {
		stat |= STA_NOINIT;
	}

	/* SD Init */
	if (SD_Init() != SD_OK) {
#ifdef DBGIO
		puts("Initialization Fail");
#endif
		stat |= STA_NOINIT;
	}

	return (stat);
}

/**
 *****************************************************************************
 * \brief		Return disk status
 *
 * \param[in]	drv		Physical drive number (0..)
 * \return		DSTATUS
 *****************************************************************************
 */
DSTATUS disk_status(BYTE drv) {

	DSTATUS stat = 0;

	if (SD_Detect() != SD_PRESENT )
		stat |= STA_NODISK;

	/* STA_NOTINIT - Subsystem not initialized								*/
	/* STA_PROTECTED - Write protected, MMC/SD switch if available			*/

	return (stat);
}

/**
 *****************************************************************************
 * \brief		Read from the SD Card
 *
 * \param[in]	drv		Physical drive number (0..)
 * \param[out]	buff	Data buffer to store read data
 * \param[in]	sector	Sector address (LBA)
 * \param[in]	count	Number of sectors to read (1..128)
 * \return		DRESULT
 *****************************************************************************
 */
DRESULT disk_read(BYTE drv, BYTE *buff, DWORD sector, UINT count) {

	SD_Error Status;

#ifdef DBGIO
	printf("disk_read %d %p %10d %d\n",drv,buff,sector,count);
#endif

	if (SD_Detect() != SD_PRESENT )
		return (RES_NOTRDY);

	/* DMA Alignment failure, do single up to aligned buffer */
	if ((DWORD) buff & 3) {
		DRESULT res = RES_OK;
		DWORD scratch[BLOCK_SIZE / 4]; /* Alignment assured, you'll need a sufficiently big stack */

		while (count--) {
			res = disk_read(drv, (void *) scratch, sector++, 1);

			if (res != RES_OK)
				break;

			memcpy(buff, scratch, BLOCK_SIZE);

			buff += BLOCK_SIZE;
		}

		return (res);
	}

	/* 4GB Compliant */
	Status = SD_ReadMultiBlocksFIXED(buff, sector, BLOCK_SIZE, count);

	if (Status == SD_OK) {
		SDTransferState State;

		/* Check if the Transfer is finished */
		Status = SD_WaitReadOperation();

		/* BUSY, OK (DONE), ERROR (FAIL) */
		while ((State = SD_GetStatus()) == SD_TRANSFER_BUSY)
			;

		if ((State == SD_TRANSFER_ERROR) || (Status != SD_OK))
			return (RES_ERROR);
		else
			return (RES_OK);
	}
	else
		return (RES_ERROR);
}

#if _USE_WRITE
/**
 *****************************************************************************
 * \brief		Write to the SD Card
 *
 * \param[in]	drv		Physical drive number (0..)
 * \param[in]	buff	Data to be written
 * \param[in]	sector	Sector address (LBA)
 * \param[in]	count	Number of sectors to write (1..255)
 * \return		DRESULT
 *****************************************************************************
 */
DRESULT disk_write(BYTE drv, const BYTE *buff, DWORD sector, BYTE count) {

	SD_Error Status;

#ifdef DBGIO
	printf("disk_write %d %p %10d %d\n",drv,buff,sector,count);
#endif

	if (SD_Detect() != SD_PRESENT )
		return (RES_NOTRDY);

	/* DMA Alignment failure, do single up to aligned buffer */
	if ((DWORD) buff & 3) {
		DRESULT res = RES_OK;
		DWORD scratch[BLOCK_SIZE / 4]; /* Alignment assured, you'll need a sufficiently big stack */

		while (count--) {
			memcpy(scratch, buff, BLOCK_SIZE);

			res = disk_write(drv, (void *) scratch, sector++, 1);

			if (res != RES_OK)
				break;

			buff += BLOCK_SIZE;
		}

		return (res);
	}

	/* 4GB Compliant */
	Status = SD_WriteMultiBlocksFIXED((uint8_t *) buff, sector, BLOCK_SIZE,
	                                  count);

	if (Status == SD_OK) {
		SDTransferState State;

		/* Check if the Transfer is finished */
		Status = SD_WaitWriteOperation();

		/* BUSY, OK (DONE), ERROR (FAIL) */
		while ((State = SD_GetStatus()) == SD_TRANSFER_BUSY)
			;

		if ((State == SD_TRANSFER_ERROR) || (Status != SD_OK))
			return (RES_ERROR);
		else
			return (RES_OK);
	}
	else
		return (RES_ERROR);
}
#endif /* _USE_WRITE */

#if _USE_IOCTL
/**
 *****************************************************************************
 * \brief		IO Ctl, this is just a RES_OK
 *
 * \param[in]	pdrv	Physical drive number (0..)
 * \param[in]	cmd		Control code
 * \param[in]	buff	Buffer to send/receive control data
 * \return		DRESULT
 *****************************************************************************
 */
DRESULT disk_ioctl(BYTE pdrv, BYTE cmd, void *buff) {
	return RES_OK;
}
#endif /* _USE_IOCTL */

#ifdef __cplusplus
}
#endif /* __cplusplus */

/**
 * @}
 * @}
 */
