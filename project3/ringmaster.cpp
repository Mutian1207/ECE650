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
    client_fd = server_accept(master_fd,&client_ip);
    //  ssize_t send(int sockfd, const void *buf, size_t len, int flags);
    //send player number and total player number and num_hops to client
    send(client_fd,&i,sizeof(i),0);
    send(client_fd,&num_players,sizeof(num_players),0);
    send(client_fd,&num_hops,sizeof(num_hops),0);

    //char player_ip[100];
    
    
    //receive player server port and ip from player (ready to go)
    recv(client_fd,&client_port,sizeof(client_port),0);
    //recv(client_fd,&player_ip,sizeof(player_ip),0);
    //    std::string player_ip_store = *client_ip;
    //    std::cout<<"receive player ip: "<<player_ip<<" , receive player port: "<<client_port<<std::endl;
    std::vector<int> player_fd_port{i,client_fd,client_port};    
    std::pair<std::vector<int>,std::string > newp{player_fd_port,client_ip};
    player_all.push_back(newp);
    std::cout<<"Player "<<i<<" is ready to play"<<std::endl;
    //    std::cout<<"client fd ip:"<<player_ip<<std::endl;
    // std::cout<<"client fd:"<<client_fd<<std::endl;
    //std::cout<<"client port:"<<client_port<<std::endl;
    //std::cout<<"player server fd:"<<player_server_fd<<std::endl;
    //std::cout<<"player server port:"<<get_port(player_server_fd)<<std::endl;
  }
  
  //connect neighbors
   for(auto i =0 ;i<num_players;i++){
    int right_neighbor = (i+1)%num_players;
    int right_neighbor_server_port = player_all[right_neighbor].first[2];
    std::string right_neighbor_str_ip = player_all[right_neighbor].second;
    char right_neighbor_server_ip[100];
    memset(right_neighbor_server_ip, 0, sizeof(right_neighbor_server_ip));
    strcpy(right_neighbor_server_ip,right_neighbor_str_ip.c_str());
    //std::cout<<"right_neighbor_server_ip:"<<right_neighbor_server_ip<<std::endl;
    //send through the right client and ringmaster server fd
    send(player_all[i].first[1],&right_neighbor_server_port,sizeof(right_neighbor_server_port),0);
    send(player_all[i].first[1],&right_neighbor_server_ip,sizeof(right_neighbor_server_ip),0);
    
    }
  
  //Ready to go
  srand((unsigned int)time(NULL)+num_players);
  int random = rand()%num_players;

  //init potato

  Potato po(num_hops);
  //pass potato to first player and receive last potato
  if(num_hops!=0){
    send(player_all[random].first[1],&po,sizeof(po),0);
      std::cout<<"Ready to start the game, sending potato to player "<<random<<std::endl;
    fd_set rfds;
    int maxfd = -1;
    FD_ZERO(&rfds);
    for(auto vec:player_all){
      maxfd = std::max(maxfd,vec.first[1]);
      FD_SET(vec.first[1],&rfds);
    }

    int retval = select(maxfd+1,&rfds,NULL,NULL,NULL);
    if(retval ==0){
      std::cerr<<"time out"<<std::endl;
    }else if(retval ==-1){
      std::cerr<<"select() error"<<std::endl;
    }else{
      for(int i = 0;i<num_players;i++){
        if(FD_ISSET(player_all[i].first[1],&rfds)){
          std::cout<<"receive last potato from player "<<i<<std::endl;
          std::cout<<"potato hops left:"<<po.hops_left<<std::endl;
          recv(player_all[i].first[1],&po,sizeof(po),0);
          assert(po.hops_left==0);
          break;
        }
      } 
    }

    //send last potato to all players to shut down
    for(auto vec:player_all){
      send(vec.first[1],&po,sizeof(po),0);
    }
    std::cout<<"Trace of potato:"<<std::endl;
    for(int i =0;i<po.cnt;i++){
      if(i!=po.cnt-1){
        std::cout<<po.path[i]<<",";
      }else{
        std::cout<<po.path[i]<<std::endl;
      }
    }

    
  }
  
  
  
 
    for(size_t i = 0;i<player_all.size();i++){
    close(player_all[i].first[1]);

    }
    close(master_fd);
  return 0;
}
