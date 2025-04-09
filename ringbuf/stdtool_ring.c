/*------------------------------------------------------------------------------
--brief: ringbuf component module
--build: LiQiang 2013/12/15
--note : 1- only suitable for uint8_t type buffering 
         2- objetive programming, also is re-enterable, can be multi-instanted [***] 
         3- serve with element add, preview, delete. fucntions are strong.
         4- stretage is 'ring header with hi prio', that is if old data in ring 
            is not pop out in time, they may be flushed by the new data.  
         
         LiQiang 2020/5/12
         add ini() fucntion, and uni-code 
         
 [how to use with the init?]
    STDTOOL_RING_STRU Ring;//new a instance

    void myRingInit(void){ //init the instance
        #define MY_BUF_SIZE 512
        static uint8_t mybuf[MY_BUF_SIZE]; //the buf's real content, size is 512B 

        Ring.Buf      = mybuf;//ptr to the ring storage pool,
        Ring.BufLoads = 0; //init there is ZERO byte in ring,
        Ring.BUF_SIZE = MY_BUF_SIZE;
        Ring.Wi       = 0;
    }

 [how to add a element?]
    uint8_t t[128];
    n = read_port(t); //n is the size actually has been read 
    stdtool_ring_add(&Ring, t, n);
    
  [how to preview and delete a element or block?]
    uint8_t tbuf[128];
    
    if(stdtool_ring_preview_subbuf(&Ring, tbuf, 4) == 1){ //return 1 menas previewing ok. 
        .... deal with the 4 elements with your code, buf[0..3]...
        stdtool_ring_delete(&Ring,4);//delete the 4 elements after use. 
    }
------------------------------------------------------------------------------*/


#include "stdtool_ring.h"

/*------------------------------------------------------------------------------
--brief: ring struct init
--in   : r  pointer to the ring to be init
         valid_loads_num, how many data are valid in the pool. 
         buf_len, the buf pool's total size 
         write_index, where the index is when adding a element. 
--out  : none
--note : none,     
------------------------------------------------------------------------------*/
void stdtool_ring_init(STDTOOL_RING_STRU *r, uint8_t pool[], uint32_t valid_loads_num, uint32_t buf_len, uint32_t write_index){

    r->Buf      = pool;//ptr to the ring storage pool,
    r->BufLoads = valid_loads_num; //init there is ZERO byte in ring,
    r->BUF_SIZE = buf_len;
    r->Wi       = write_index;
}


/*------------------------------------------------------------------------------
--brief: most commonly used init method, start from ZERO, and clear the pool,
--in   : r, the ring structure
         buf_len, ring storage pool size,
--out  : 
--note : none,     
------------------------------------------------------------------------------*/
void stdtool_ring_init_fast(STDTOOL_RING_STRU *r, uint8_t pool[], uint32_t buf_len){
    int i;
    for(i=0; i<buf_len; i++){
        r->Buf[i] = 0;
    }
    stdtool_ring_init(r, pool, 0, buf_len, 0);
    
}

/*------------------------------------------------------------------------------
--brief: write valid data to the ringbuf, where there are BufLoads items.
--in   : a= the data to add, n= cnt of data.
--out  : the valid cnt of this time
--note : 1- stretage is 'ring header with hi prio', that is if old data in ring 
            is not pop out in time, they may be flushed by the new data.  
------------------------------------------------------------------------------*/
int32_t stdtool_ring_add(STDTOOL_RING_STRU *r, uint8_t a[], uint32_t n){
    uint32_t i;
    if (n > 0) {
        for (i = 0; i < n; i++) {                           // data to the ring, new data may flush data not read out.
            r->Buf[r->Wi] = a[i];
            r->Wi++;
            if (r->Wi >= r->BUF_SIZE) r->Wi = 0;
        }
        r->BufLoads += n;
        if (r->BufLoads > r->BUF_SIZE) r->BufLoads = r->BUF_SIZE; //if new flush un-read data, ringload will be more than SIZE, here we make a limitation. 
    }
    return r->BufLoads;
}


/*------------------------------------------------------------------------------
--brief: preview ring queue elements, from where last delete from. will not
         delete anyone; if has never deleted a element, will preview form start.

--in   : view= where to store the data to preview.
         n= cnt to preview 
--out  : if ring has no enough data, (<n), will return false.
--note : none.     
------------------------------------------------------------------------------*/
int32_t stdtool_ring_preview_subbuf(STDTOOL_RING_STRU *r, uint8_t view[] , int n)
{
    if (r->BufLoads >= n) {                                 //only continue when the ring has enough data.
        uint32_t i;
        uint32_t ri = (r->Wi < r->BufLoads) ? (r->Wi + r->BUF_SIZE- r->BufLoads) : (r->Wi - r->BufLoads);//calc where to read from wi and ringload.
        for (i = 0; i < n; i++) {
            view[i] = r->Buf[ri++];
            if (ri >= r->BUF_SIZE) ri = 0;
        }
        return 1;
    } else {
        return 0;
    }
}


/*------------------------------------------------------------------------------
--brief: delete element(s) from the ring.

--in   : n, cnt of data to be delete 
--out  : 1= delete ok, 
--note : only move the pointer index, the real deleting (covering) is the flushing.
------------------------------------------------------------------------------*/
int32_t stdtool_ring_delete(STDTOOL_RING_STRU *r, int n)
{
    if(r->BufLoads >= n) {
        r->BufLoads -= n;
        return 1;
    }else {
        return 0;
    }
}


