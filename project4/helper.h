#include "exerciser.h"
#include<vector>
#include<string>
#include<iostream>
#include<fstream>
void SqlExec(string query,connection *C);

void DropTable(connection *C);

void CreateTable(connection *C);
void addPlayer(connection*C,std::string file);
void addTeam(connection*C,std::string file);
void addState(connection*C,std::string file);
void addColor(connection*C,std::string file);