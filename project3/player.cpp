#include"helper.hpp"


int main(int argc,char*argv[]){
  if(argc!=3){
    std::cout<<"argument error"<<std::endl;
    std::cout<<"player <machine_name> <port_num>"<<std::endl;
  }
  const char * hostname = argv[1];
  const char * port_num = argv[2];
  int player_fd = build_client(hostname,port_num);
  std::cout<<"player build client fd:"<<player_fd<<std::endl;
  
  std::cout<<"connection!"<<std::endl;
  close(player_fd);
  return 0;

}
