#include<stdio.h>
#include<stdlib.h>

typedef struct node{
  size_t size;
  struct node *prev;
  struct node *next;
  int isfree;
} meta;

//first fit malloc / free
void *ff_malloc(size_t size);
void ff_free(void *ptr);



//best fit malloc/ free
void *bf_malloc(size_t size);
void bf_free(void *ptr);
//helper function
void printfl();
void set_meta(meta *node,size_t size,meta*prev,meta*next,int isfree);
void *get_curpos();
void *allocate_newmemo(size_t size);
meta *find_freeblock(int first_fit, size_t size);
void *split(meta *block,size_t size);
void remove_freeblock(meta *freeblock);
void add_freeblk(meta *blk);
void merge(meta *blk);
void * _malloc(int first_fit,size_t size);
//performance

unsigned long get_data_segment_size();

unsigned long get_data_segment_free_space_size();


