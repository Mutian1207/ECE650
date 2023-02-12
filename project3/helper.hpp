#include<iostream>
#include<cstring>
#include<sys/socket.h>
#include<netdb.h>
#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>


int init_server(const char * port);
int build_client(const char * hostname, const char * port);
int get_ip(struct sockaddr sock_addr);
int server_accept(int socket_fd,std::string *ip);
int get_port(int socket_fd);

