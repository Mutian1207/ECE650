#include<cstdio>
#include<cstdlib>
#include<iostream>
#include<string.h>
#include<vector>
class Potato{
public:
  int hops_left;
  int cnt;
  int path[500];
  
  Potato() = default;
  Potato(int hops): hops_left(hops),cnt(0){memset(path,0,sizeof(path));};
  /*  Potato( Potato &&rhs) noexcept:hops_left(0),cnt(0){
    std::swap(hops_left,rhs.hops_left);
    std::swap(cnt,rhs.cnt);
    std::swap(path,rhs.path);
  } 
  Potato & operator=(Potato &&rhs) noexcept {
    if(this!=&rhs){
      std::swap(hops_left,rhs.hops_left);
      std::swap(cnt,rhs.cnt);
      std::swap(path,rhs.path);
    }
    return *this;

  }
  Potato(const Potato &rhs):hops_left(rhs.hops_left),cnt(rhs.cnt){
    for(size_t i = 0;i<sizeof(rhs.path);i++)
      path.push_back(rhs.path[i]);
  }
  Potato &operator =(const Potato &rhs){
    
    if(this != &rhs){
      path.clear();
      for(size_t i =0;i<512;i++){
        path.push_back(rhs.path[i]);
      }
      
      hops_left = rhs.hops_left;
      cnt = rhs.cnt;
    }
    return *this;
  }
  //  ~Potato(){};*/
};
