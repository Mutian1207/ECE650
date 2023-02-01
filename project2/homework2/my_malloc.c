#include "my_malloc.h"
#include<assert.h>
#include<unistd.h>


static meta *fhead=NULL;
static meta *ftail=NULL;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t sbrklock = PTHREAD_MUTEX_INITIALIZER;
__thread meta *fh = NULL;
__thread meta *ft = NULL;
//locking version
void *ts_malloc_lock(size_t size){
  pthread_mutex_lock(&lock);
  char * pos;
  pos = _malloc(size,1);
  pthread_mutex_unlock(&lock);
  return pos;
}

void ts_free_lock(void *ptr){
  pthread_mutex_lock(&lock);
  
  meta *free_block = (meta *)((char *) ptr-sizeof(meta));
  //printf("free function - before free block\n");
  // printfl();
  free_block->isfree =1;
  add_freeblk(free_block,1);
  merge(free_block,1);
  pthread_mutex_unlock(&lock);
  // printf("after add free block to free list\n");
  // printfl();

}
// non-locking version
void *ts_malloc_nolock(size_t size){
  return _malloc(size,0);
}



void ts_free_nolock(void *ptr){
  meta *free_block = (meta *)((char*)ptr - sizeof(meta));
  free_block->isfree =1;
  add_freeblk(free_block,0);
  merge(free_block,0);
}

//init a new meta
void set_meta(meta * node, size_t size, meta*prev,meta*next,int isfree){
  node->size = size;
  node->prev = prev;
  node->next = next;
  node->isfree = isfree;  
}

//if no block in free list is suitable
//extend heap -> additional space for memo
void * allocate_newmemo(size_t size){
 
  pthread_mutex_lock(&sbrklock);
  meta * cur = sbrk(0);
  char *pos = sbrk(size+sizeof(meta));
  pthread_mutex_unlock(&sbrklock);
  if(pos != (void *)-1){
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
meta * find_freeblock(size_t size,const int withlock){
  meta *free_block = NULL;
  meta *cur = NULL;
  if(withlock){
    if(fhead==NULL && ftail == NULL) return NULL;
    cur = fhead;
  }else{
    if(fh==NULL && ft == NULL) return NULL;
    cur = fh;
  }
  //first appropriate free block

 
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
  
  return free_block;
}
//split block to allocate target size
void * split(meta * block,size_t size, const int withlock){
  assert(block!=NULL);
  // post part left (free)
  meta * new_meta =(meta *)((char *)block + size+ sizeof(meta));
  size_t left_size = block->size-size;
  remove_freeblock(block,withlock);
  // if left space size > meta size means we can split and store another space to free list

  if(left_size > sizeof(meta)) {
    set_meta(new_meta,left_size-sizeof(meta),NULL,NULL,1);
    add_freeblk(new_meta,withlock);
    //set block change the size
    set_meta(block,size,NULL,NULL,0);
  }
  //else we cannot split so dont change the block just waste some space
  return (char *)block + sizeof(meta);
}
// remove a meta from the free list
void remove_freeblock(meta *freeblock,const int withlock){
 
  if(withlock){
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
  }else{
    if(fh==ft && fh==freeblock){
      fh = NULL;
      ft = NULL;
    }else if(fh==freeblock){
      fh = freeblock->next;
      fh->prev = NULL;
      freeblock->next = NULL;
    }else if(ft==freeblock){
      ft = freeblock->prev;
      ft->next = NULL;
      freeblock->prev =NULL;
    }else{
      freeblock->prev->next=freeblock->next;
      freeblock->next->prev = freeblock->prev;
    }
    
  }

}
//add a meta to the free list
void add_freeblk(meta *blk,const int withlock){

  if(withlock) {
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
  }else{
    if(fh==NULL && ft==NULL){
      fh = blk;
      ft = blk;
      blk->prev = NULL;
      blk->next = NULL;
    }else if(blk<fh){
      blk->next = fh;
      fh->prev = blk;
      blk->prev = NULL;
      fh = blk;
    }else if(blk>fh && ft >blk){
      meta*cur =fh;
      while(cur != NULL){
        if(cur<blk) cur = cur->next;
        else break;
      }
      cur->prev->next = blk;
      blk->prev = cur->prev;
      blk->next = cur;
      cur->prev = blk; 
    }else{
      ft->next = blk;
      blk->prev = ft;
      blk->next =NULL;
      ft = blk;
    }
  }
}
// merge two free blocks
void merge(meta*blk,const int withlock){
  //  printf("before merge\n");
  // printfl();
  if(blk->next!=NULL){
    if((char*)blk + sizeof(meta) + blk->size == (char*) blk->next){
      blk->size += sizeof(meta);
      blk->size += blk->next->size;
      remove_freeblock(blk->next,withlock);
    }
  }
  if(blk->prev!=NULL){
    if((char*)blk->prev+sizeof(meta) +blk->prev->size == (char*) blk){
      blk->prev->size += sizeof(meta);
      blk->prev->size += blk->size;
      remove_freeblock(blk,withlock);
    }

  }
  //printf("after merge\n");
  //printfl();
}



void * _malloc(size_t size,const int withlock){
 
  //find an appropriate free block
  meta * free_block = find_freeblock(size,withlock);
  //if found then use that block to split -> prev part for contain / post part still free
  //  printf("malloc function\n");
  //printfl();
  if(free_block !=NULL){
    // printf("target size to malloc:%zu,free_block found:%p, with size:%zu\n",size,free_block,free_block->size);
    return split(free_block,size, withlock);
  }
  //else allocate new memory
  else{
    //printf("new space allocated, now break pos:%p\n",sbrk(0));
    return allocate_newmemo(size);
  }

}


