#include<iostream>
#include<cstring>
#include<sys/socket.h>
#include<netdb.h>
#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include <arpa/inet.h>
#include<vector>
int init_server(const char * port);
int build_client(const char * hostname, const char * port);
std::string get_ip(int sock_fd);
int server_accept(int socket_fd);
int get_port(int socket_fd);

