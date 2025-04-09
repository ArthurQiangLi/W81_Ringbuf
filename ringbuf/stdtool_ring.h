/*
    the ring buf, common lib.
    LiQiang  2013/12/15
*/
#ifndef _RING_BUF_H
#define _RING_BUF_H

#include <stdint.h>


typedef struct {
    uint8_t *Buf;                                           //buf pointer, loadde by init(), which is re-enterable.
    uint32_t BufLoads;                                      //buf[]'s length of valid data, init as 0.
    uint32_t BUF_SIZE;                                      //buf[]'s size
    uint32_t Wi;                                            //the pointer to write the ring, mainly the RingAddFromPort 
                                                            //...do the writing, the RingPreview the the reading, init as 0.

} STDTOOL_RING_STRU;

extern void    stdtool_ring_init(STDTOOL_RING_STRU *r, uint8_t pool[], uint32_t valid_loads_num, uint32_t buf_len, uint32_t write_index);
extern void    stdtool_ring_init_fast(STDTOOL_RING_STRU *r, uint8_t pool[], uint32_t buf_len);
extern int32_t stdtool_ring_add(STDTOOL_RING_STRU *r, uint8_t a[], uint32_t n);
extern int32_t stdtool_ring_preview_subbuf(STDTOOL_RING_STRU *r, uint8_t view[] , int n);
extern int32_t stdtool_ring_delete(STDTOOL_RING_STRU *r, int n);


#endif /* _RING_BUF_H */

