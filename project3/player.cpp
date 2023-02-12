#include"helper.hpp"
#include<assert.h>

int main(int argc,char*argv[]){
  if(argc!=3){
    std::cout<<"argument error"<<std::endl;
    std::cout<<"player <machine_name> <port_num>"<<std::endl;
  }
  const char * hostname = argv[1];
  const char * port_num = argv[2];
  //get master fd connection
  int to_master_fd = build_client(hostname,port_num);
  // receive game info
  auto player_no=0;
  auto num_players=0;
  auto num_hops = 0;
  //  std::cout<<"to master fd:"<<to_master_fd<<std::endl;
  //std::cout<<"to master fd port:"<<get_port(to_master_fd)<<std::endl;
  // std::cout<<"connection built"<<std::endl;
  recv(to_master_fd,&player_no,sizeof(player_no),0);
  recv(to_master_fd,&num_players,sizeof(num_players),0);
  recv(to_master_fd,&num_hops,sizeof(num_hops),0);

  // std::cout<<"build player server..."<<std::endl;
  //player plays as a server -> for connection to neighbor
  char Myport[50];
  assert(sprintf(Myport,"%d",5477+player_no)>=0);
  //std::string str = std::to_string(local_port);
  //const char* port = str.c_str(); 
  auto player_fd = init_server(Myport);
  std::string player_ip = get_ip(player_fd);
  std::string to_master_ip = get_ip(player_fd);
  int player_port = get_port(player_fd);
  std::cout<<"player server ip:"<<player_ip<<std::endl;
  std::cout<<"to master ip:"<<to_master_ip<<std::endl;
  
  //from client send player ready to play info to ringmaster
  //  std::cout<<"to master fd:"<<to_master_fd<<std::endl;
  // std::cout<<"to master fd port:"<<get_port(to_master_fd)<<std::endl;

  // std::cout<<"player side player fd:"<<player_fd<<std::endl;
  //std::cout<<"player port numb:"<< player_port<<std::endl;
  send(to_master_fd,&player_port,sizeof(player_no),0);
  

  
  close(to_master_fd);
  return 0;

}
