#include "my_malloc.h"
#include<assert.h>
#include<unistd.h>

static meta *fhead=NULL;
static meta *ftail=NULL;
//static unsigned long totalsize = 0;
//static unsigned long allocated = 0;

void printfl(){
  meta *cur = fhead;
  while(cur!=NULL){
    printf("cur : %p, cur->size: %lu\n",cur,cur->size);
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
  if(fhead==NULL && ftail == fhead) return NULL;
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
  meta * new_meta =(meta *) (char *)block + size+ sizeof(meta);
  set_meta(new_meta,block->size-size-sizeof(meta),block->prev,block->next,1);
  // insert new meta to freelist
  add_freeblk(new_meta);
  remove_freeblock(block);
  set_meta(block,size,NULL,NULL,0);
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
    fhead= blk;
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
  if(blk->prev!=NULL){
    if((char*) blk == (char*) blk->prev + blk->prev->size+sizeof(meta)){
      blk->prev->size +=blk->size+sizeof(meta);
      blk->next->prev = blk->prev;
      blk->prev->next = blk->next;
      blk = blk->prev;
    } 
  }
  if(blk->next!=NULL){
    if((char*) blk->next == (char*)blk+blk->size+sizeof(meta)){
      blk->size+=blk->next->size;
      blk->next = blk->next->next;
      blk->next->prev = blk;
   }
  }
  
}



void * _malloc(int first_fit,size_t size){
 
  //find an appropriate free block
  meta * free_block = find_freeblock(first_fit,size);
  
  //if found then use that block to split -> prev part for contain / post part still free
  if(free_block !=NULL){
    return split(free_block,size);
  }
  //else allocate new memory
  else{
    return allocate_newmemo(size);
  }

}
//first fit malloc
void *ff_malloc(size_t size){
  return _malloc(1,size);
}

// first fit free
void ff_free(void * ptr){
  meta *free_block = (meta *)(char *) ptr-sizeof(meta);
  free_block->isfree =1;
  add_freeblk(free_block);

  merge(free_block);
  
}

//best fit malloc
void *bf_malloc(size_t size){
  return _malloc(0,size);
}

//best fit free
void bf_free(void *ptr){
  ff_free(ptr);
}
