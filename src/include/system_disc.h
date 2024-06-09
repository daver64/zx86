#ifndef _SYSTEM_DISC_H_
#define _SYSTEM_DISC_H_

#include <stdint.h>
#ifdef __cplusplus
extern "C"
{
#endif
/* DISC.C */
uint32_t hdc_read_sectors(uint64_t LBA, uint8_t sectorcount, void *t);
uint32_t hdc_write_sectors(uint64_t LBA, uint8_t sectorcount, void *t);
uint32_t rdc_read_sectors(uint64_t LBA, uint8_t sectorcount, void *t);
uint32_t rdc_write_sectors(uint64_t LBA, uint8_t sectorcount, void *t);
/* DFS.C */
unsigned int dfs_mount();
unsigned int dfs_unmount();

#ifdef __cplusplus
}
#endif
#endif
