#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>

typedef struct node{
  size_t size;
  struct node *prev;
  struct node *next;
  int isfree;
} meta;

// locking version
void *ts_malloc_lock(size_t size);
void ts_free_lock(void *ptr);

//non-locking version
void *ts_malloc_nolock(size_t size);
void ts_free_nolock(void *ptr);



//best fit malloc/ free

void bf_free(void *ptr);
//helper function
void printfl();
void set_meta(meta *node,size_t size,meta*prev,meta*next,int isfree);
void *get_curpos();
void *allocate_newmemo(size_t size);
meta *find_freeblock(size_t size, const int withlock);
void *split(meta *block,size_t size,const int withlock);
void remove_freeblock(meta *freeblock,const int withlock);
void add_freeblk(meta *blk,const int withlock);
void merge(meta *blk,const int withlock);
void * _malloc(size_t size,const int withlock);
//performance

unsigned long get_data_segment_size();

unsigned long get_data_segment_free_space_size();


