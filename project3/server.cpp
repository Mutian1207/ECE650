#include"helper.hpp"


int main(int argc, char *argv[]){

  if(argc!=4){
    std::cout<<"argument error"<<std::endl;
    std::cout<<"ringmaster <port_num> <num_players> <num_hops>"<<std::endl;
    exit(EXIT_FAILURE);
  }
  const char *port_num = argv[1];
  int num_players = atoi(argv[2]);
  int num_hops = atoi(argv[3]);

  std::cout<<"Potato Ringmaster"<<std::endl;
  std::cout<<"Players = "<<num_players<<std::endl;
  std::cout<<"Hops = "<< num_hops<<std::endl;
  // init ringmaster server
  int master_fd;
  master_fd = init_server(port_num);
  
  int client_fd;
  std::string *client_ip;
  //build client
  std::cout<<"ring master socket fd:"<<master_fd<<std::endl;
  for(auto i =0 ;i<num_players;i++){
    client_fd = server_accept(master_fd,client_ip);
    std::cout<<"client fd:"<<client_fd<<std::endl;
  }


  close(master_fd);
  close(client_fd);
  return 0;
}
