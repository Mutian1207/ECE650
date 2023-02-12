#include"helper.hpp"


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
  std::cout<<"connection built"<<std::endl;
  recv(to_master_fd,&player_no,sizeof(player_no),0);
  recv(to_master_fd,&num_players,sizeof(num_players),0);
  recv(to_master_fd,&num_hops,sizeof(num_hops),0);
  std::cout<<"build player server..."<<std::endl;
  //player plays as a server -> for connection to neighbor
  auto player_fd = init_server("");
  //from client send player ready to play info to ringmaster
  std::cout<<"player side player fd:"<<player_fd<<std::endl;
  send(to_master_fd,&player_fd,sizeof(player_no),0);
  

  
  close(to_master_fd);
  return 0;

}
