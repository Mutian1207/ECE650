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
  int player_no=0;
  int num_players=0;
  int num_hops = 0;
  //  std::cout<<"to master fd:"<<to_master_fd<<std::endl;
  //std::cout<<"to master fd port:"<<get_port(to_master_fd)<<std::endl;
  // std::cout<<"connection built"<<std::endl;
  recv(to_master_fd,&player_no,sizeof(player_no),0);
  recv(to_master_fd,&num_players,sizeof(num_players),0);
  recv(to_master_fd,&num_hops,sizeof(num_hops),0);
  
  // std::cout<<"build player server..."<<std::endl;
  //player plays as a server -> for connection to neighbor
  char Myport[100];
  assert(sprintf(Myport,"%d",1999+player_no)>=0);
  //std::string str = std::to_string(local_port);
  //const char* port = str.c_str(); 
  int player_server_fd = init_server(Myport);
  std::string master_fd_ip = get_ip(to_master_fd);
  // std::string player_ip = get_ip(player_server_fd);
  //char to_master_ip[100];
  // memset(to_master_ip,0,sizeof(to_master_ip));
  //strcpy(to_master_ip,master_fd_ip.c_str());
  
  /*for(size_t i = 0;i<get_ip(to_master_fd).length();i++){
    std::cout<<"char:"<<get_ip(to_master_fd)[i];
    to_master_ip[i]= master_fd_ip[i];
    }*/
    //memmove(to_master_ip,master_fd_ip.c_str(),sizeof(to_master_ip));
 
     
  
  int player_port = get_port(player_server_fd);
  //  std::cout<<"player server ip:"<<player_ip<<std::endl;
  //  std::cout<<"to master ip :"<<to_master_ip<<std::endl;
  
  //from client send player ready to play info to ringmaster
  //  std::cout<<"to master fd:"<<to_master_fd<<std::endl;

  // std::cout<<"to master fd port:"<<get_port(to_master_fd)<<std::endl;

  // std::cout<<"player side player fd:"<<player_server_fd<<std::endl;
  //std::cout<<"player port numb:"<< player_port<<std::endl;
  send(to_master_fd,&player_port,sizeof(player_no),0);
  //send(to_master_fd,&to_master_ip,sizeof(to_master_ip),0);
  std::cout<<"Connected as player "<<player_no << " out of "<<num_players<< " total players"<<std::endl;
  
  /*
    build another client connecting to right neighbor server
    connecting to right neighbor
    receive right neighbor ip(hostname) + port from ringmaster
 */
  char right_neighbor_server_ip[100];
  int right_neighbor_server_port;
  recv(to_master_fd,&right_neighbor_server_port,sizeof(right_neighbor_server_port),0);
  recv(to_master_fd,&right_neighbor_server_ip,sizeof(right_neighbor_server_ip),0);
  
  char right_port[100];
  sprintf(right_port,"%d",right_neighbor_server_port);
  std::cout<<"right neighbor_server_ip:"<<right_neighbor_server_ip<<std::endl;
  std::cout<<"right neighbor port:"<<right_port<<std::endl;
  int right_neighbor_fd = build_client(right_neighbor_server_ip,right_port);
  std::string left_neighbor_ip;
  int left_neighbor_fd = server_accept(player_server_fd,&left_neighbor_ip);
  std::cout<<"left neighbor ip:"<<left_neighbor_ip<<std::endl;
  //test right neighbor and left neighbor
  srand((unsigned int)time(NULL)+player_no);
    int signal = rand()%277;
  std::cout<<"sending to left neighbor:"<<signal<<std::endl;
  send(left_neighbor_fd,&signal,sizeof(signal),0);
  int rec;
  recv(right_neighbor_fd,&rec,sizeof(rec),0);
  std::cout<<"receive:"<<rec<<std::endl;
  
  
  // receive the potato to start the game
  Potato potato;
  fd_set rfds;
  struct timeval tv;
  int retval;
  tv.tv_sec = 10;
  tv.tv_usec=0;
  int maxfd = std::max(to_master_fd,right_neighbor_fd);
  maxfd = std::max(maxfd,left_neighbor_fd);
  
  while(1){
    FD_ZERO(&rfds);
  //set all 3 fd to rfds 
    FD_SET(to_master_fd,&rfds);
    FD_SET(right_neighbor_fd,&rfds);
    FD_SET(left_neighbor_fd,&rfds);

    /*
      int select(int nfds, fd_set *restrict readfds,
                  fd_set *restrict writefds, fd_set *restrict exceptfds,
                  struct timeval *restrict timeout);
       void FD_CLR(int fd, fd_set *set);
       int  FD_ISSET(int fd, fd_set *set);
       void FD_SET(int fd, fd_set *set);
       void FD_ZERO(fd_set *set);

      select函数共有5个参数，其中参数和返回值：
      maxfd：This argument should be set to the highest-numbered file
              descriptor in any of the three sets, plus 1.  The
              indicated file descriptors in each set are checked, up to
              this limit (but see BUGS).
      readset：将所有关注“是否存在待读取数据”的文件描述符注册到fd_set变量，并传递其地址值。
      writeset： 将所有关注“是否可传输无阻塞数据”的文件描述符注册到fd_set变量，并传递其地址值。
      exceptset：将所有关注“是否发生异常”的文件描述符注册到fd_set变量，并传递其地址值。
      timeout：调用select后，为防止陷入无限阻塞状态，传递超时信息。
      返回值：错误返回-1，超时返回0。当关注的事件返回时，返回大于0的值，该值是发生事件的文件描述符数。
     */
    /*
      第一，文件描述符的监视范围与第一个参数有关。实际上，select函数要求通过第一个参数传递监视对象文件描述符的数量。因此，需要得到注       册在fd_set变量中的文件描述符数。但每次新建文件描述符时，其值都会增1，故只需将最大的文件描述符值加1再传递到select函数即可。（加1      是因为文件描述符的值从0开始）
     */
    
    retval = select(maxfd+1,&rfds,NULL,NULL,&tv);
    if(retval == 0){
      std::cerr<<"time out"<<std::endl;
    }else if(retval == -1){
      std::cerr<<"select() error"<<std::endl;
    }else{
      int tmp_fd = -1;
      if(FD_ISSET(to_master_fd,&rfds)){
        std::cout<<"receive from ringmaster"<<std::endl;
        tmp_fd = to_master_fd;
      }else if(FD_ISSET(right_neighbor_fd,&rfds)){
        std::cout<<"receive from right neighbor"<<std::endl;
        tmp_fd = right_neighbor_fd;
      }else if(FD_ISSET(left_neighbor_fd,&rfds)){
        std::cout<<"receive from left neighbor"<<std::endl;
        tmp_fd = left_neighbor_fd;
      }
      assert(tmp_fd!=-1);
      recv(tmp_fd,&potato,sizeof(potato),0);
      std::cout<<"receive! potato hopsleft:"<<potato.hops_left<<std::endl;
    }
    if(potato.hops_left==1){
      potato.hops_left--;
      
      potato.path[potato.cnt++] = player_no;
      send(to_master_fd,&potato,sizeof(potato),0);
      std::cout<<"I'm it"<<std::endl;
    }else if(potato.hops_left>1){
      assert(potato.hops_left>1);
      potato.hops_left--;
      
      potato.path[potato.cnt++]=player_no;
      int random = rand()%2;
      if(random==0){
        send(left_neighbor_fd,&potato,sizeof(potato),0);
        std::cout<<"Sending potato to "<< (player_no-1+num_players)%num_players<<std::endl;
      }else{
        send(right_neighbor_fd,&potato,sizeof(potato),0);
        std::cout<<"Sending potato to "<<(player_no+1+num_players)%num_players<<std::endl;
      }
    }//else gonna shut down the game
    else if(potato.hops_left==0){
      break;
    }
    FD_CLR(to_master_fd,&rfds);
    FD_CLR(right_neighbor_fd,&rfds);
    FD_CLR(left_neighbor_fd,&rfds);

  }

  
  close(to_master_fd);
  close(right_neighbor_fd);
  close(left_neighbor_fd);
  return 0;

}
