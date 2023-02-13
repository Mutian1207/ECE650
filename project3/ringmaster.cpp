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
  // build client info
  
  int client_fd;
  std::string client_ip;
  int client_port;
 
  std::vector< std::pair<std::vector<int>,std::string> > player_all;
  
  //build client
  //  std::cout<<"ring master socket fd:"<<master_fd<<std::endl;
  //std::cout<<"ring master port :"<<get_port(master_fd)<<std::endl;
  for(auto i =0 ;i<num_players;i++){
    client_fd = server_accept(master_fd);
    //  ssize_t send(int sockfd, const void *buf, size_t len, int flags);
    //send player number and total player number and num_hops to client
    send(client_fd,&i,sizeof(i),0);
    send(client_fd,&num_players,sizeof(num_players),0);
    send(client_fd,&num_hops,sizeof(num_hops),0);

    
    //receive response from player (ready to go)
    recv(client_fd,&client_port,sizeof(client_port),0);
    std::vector<int> player_fd_port{client_fd,client_port};
    std::string player_ip = get_ip(client_fd);
    std::pair<std::vector<int>,std::string > newp{player_fd_port,player_ip};
    player_all.push_back(newp);
    std::cout<<"Player "<<i<<" is ready to play"<<std::endl;
    std::cout<<"client fd ip:"<<player_ip<<std::endl;
    // std::cout<<"client fd:"<<client_fd<<std::endl;
    //std::cout<<"client port:"<<client_port<<std::endl;
    //std::cout<<"player server fd:"<<player_server_fd<<std::endl;
    //std::cout<<"player server port:"<<get_port(player_server_fd)<<std::endl;
  }
  
  
  //Ready to go
  srand((unsigned int)time(NULL)+num_players);
  int random = rand()%num_players;
  std::cout<<"Ready to start the game, sending potato to player "<<random<<std::endl;
  

  
  

  close(master_fd);
  close(client_fd);
  return 0;
}
