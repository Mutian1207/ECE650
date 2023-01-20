#include "my_malloc.h"
#include<assert.h>
#include<unistd.h>

static meta *fhead=NULL;
static meta *ftail=NULL;
static unsigned long total_size = 0;
static unsigned long free_size = 0;

void printfl(){                
  meta *cur = fhead;
  printf("print free list once\n");
  while(cur!=NULL){
    printf("cur : %p, cur->size: %zu\n",cur,cur->size);
    cur = cur->next;
  }
  }


//init a new meta
void set_meta(meta * node, size_t size, meta*prev,meta*next,int isfree){
  node->size = size;
  node->prev = prev;
  node->next = next;
  node->isfree = isfree;  
}

//get current break position
void * get_curpos(){
  return sbrk(0);
}
//if no block in free list is suitable
//extend heap -> additional space for memo
void * allocate_newmemo(size_t size){
  meta * cur = get_curpos();
  if(sbrk(size+sizeof(meta)) != (void *)-1){
    // create a meta struct
    total_size+=size+sizeof(meta);
    cur->size =size;
    cur->prev = NULL;
    cur->next = NULL;
    cur->isfree = 0;
    return (char *)cur + sizeof(meta);
  }else{
    return NULL;
  }
}

//find the approriate free block accommodate for  
meta * find_freeblock(int first_fit,size_t size){
 
  if(fhead==NULL && ftail == NULL) return NULL;
  meta * free_block = NULL;
  meta * cur = fhead;
  //first appropriate free block

  if(first_fit){
    while(cur!=NULL){
      if(cur->size < size){
	cur = cur->next;
      }else{
	break;
      }
    }
    free_block = cur;
  }else{
    // to find the best block
    while(cur!=NULL){
      //if found one block  > size
      if(cur->size>size){
	if(free_block==NULL){
	  free_block = cur;
	}else{
	  // compare current free block size with free_block size
	  if(free_block->size>cur->size) free_block = cur;
	}
	cur = cur->next;
	
      }else if(cur->size==size){
	free_block = cur;
	break;	
      }else{
	cur = cur->next;
      }
    }  
  }
  return free_block;
}
//split block to allocate target size
void * split(meta * block,size_t size){
  assert(block!=NULL);
  // post part left (free)
  meta * new_meta =(meta *)((char *)block + size+ sizeof(meta));
  size_t left_size = block->size-size;
  remove_freeblock(block);
  // if left space size > meta size means we can split and store another space to free list

  if(left_size > sizeof(meta)) {
    set_meta(new_meta,left_size-sizeof(meta),NULL,NULL,1);
    add_freeblk(new_meta);
    //set block change the size
      set_meta(block,size,NULL,NULL,0);
  }
  //else we cannot split so dont change the block just waste some space
  return (char *)block + sizeof(meta);
}
// remove a meta from the free list
void remove_freeblock(meta *freeblock){
  
  if(fhead==ftail && fhead==freeblock){
    fhead = NULL;
    ftail = NULL;
  }else if(fhead==freeblock){
    fhead = freeblock->next;
    fhead->prev = NULL;
    freeblock->next = NULL;
  }else if(ftail==freeblock){
    ftail = freeblock->prev;
    ftail->next = NULL;
    freeblock->prev =NULL;
  }else{
    freeblock->prev->next=freeblock->next;
    freeblock->next->prev = freeblock->prev;
  }
}
//add a meta to the free list
void add_freeblk(meta *blk){
  if(fhead==NULL && ftail==NULL){
    fhead = blk;
    ftail = blk;
    blk->prev = NULL;
    blk->next = NULL;
  }else if(blk<fhead){
    blk->next = fhead;
    fhead->prev = blk;
    blk->prev = NULL;
    fhead = blk;
  }else if(blk>fhead && ftail >blk){
    meta*cur =fhead;
    while(cur != NULL){
      if(cur<blk) cur = cur->next;
      else break;
    }
     cur->prev->next = blk;
     blk->prev = cur->prev;
     blk->next = cur;
     cur->prev = blk; 
  }else{
    ftail->next = blk;
    blk->prev = ftail;
    blk->next =NULL;
    ftail = blk;
  }  
}
// merge two free blocks
void merge(meta*blk){
  if(blk->next!=NULL){
    if((char*)blk + sizeof(meta) + blk->size == (char*) blk->next){
      blk->size += sizeof(meta);
      blk->size += blk->next->size;
      remove_freeblock(blk->next);
    }
  }
  if(blk->prev!=NULL){
    if((char*)blk->prev+sizeof(meta) +blk->prev->size == (char*) blk){
      blk->prev->size += sizeof(meta);
      blk->prev->size += blk->size;
      remove_freeblock(blk);
    }

  }
  
}



void * _malloc(int first_fit,size_t size){
 
  //find an appropriate free block
  meta * free_block = find_freeblock(first_fit,size);
  //if found then use that block to split -> prev part for contain / post part still free
  //  printf("malloc function\n");
  //printfl();
  if(free_block !=NULL){
    // printf("target size to malloc:%zu,free_block found:%p, with size:%zu\n",size,free_block,free_block->size);
    return split(free_block,size);
  }
  //else allocate new memory
  else{
    //printf("new space allocated, now break pos:%p\n",sbrk(0));
    return allocate_newmemo(size);
  }

}
//first fit malloc
void *ff_malloc(size_t size){
  return _malloc(1,size);
}

// first fit free
void ff_free(void * ptr){
  meta *free_block = (meta *)((char *) ptr-sizeof(meta));
  //printf("free function - before free block\n");
  // printfl();
  free_block->isfree =1;
  add_freeblk(free_block);
  merge(free_block);
  // printf("after add free block to free list\n");
  // printfl();
}

//best fit malloc
void *bf_malloc(size_t size){
  return _malloc(0,size);
}

//best fit free
void bf_free(void *ptr){
  ff_free(ptr);
}

unsigned long get_data_segment_size(){
  return total_size;
}

unsigned long get_data_segment_free_space_size(){
  meta * cur = fhead;
  while(cur!=NULL){
    free_size += cur->size+sizeof(meta);
    cur = cur->next;
  }
  return free_size;
}
