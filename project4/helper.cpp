#include"helper.h"
#include"query_funcs.h"
// EXCECUTE SQL COMMAND
void SqlExec(string query,connection *C){
    work W(*C);
    W.exec(query);
    W.commit();
}
//DROP TABLE
void DropTable(connection *C){
    std::vector<std::string> tablename{"PLAYER","TEAM","STATE","COLOR"};
    for(auto name:tablename){
        std::string query = "DROP TABLE IF EXISTS "+name+" CASCADE;";
        SqlExec(query,C);
    }
}

//CREATE TABLE

void CreateTable(connection *C){
    std::ifstream f("createTable.txt");
    if(!f.is_open()){
        std::cerr<<"error opening file"<<std::endl;
        exit(EXIT_FAILURE);
    }
    std::string query;
    std::string line;
    while(std::getline(f,line)){
        query+=line;
    }
    SqlExec(query,C);
    f.close();
}


//ADD Player from txt

void addPlayer(connection*C,std::string file){
    std::ifstream f(file);
    std::stringstream parser;
    if(!f.is_open()){
        std::cerr<<"error opening file"<<std::endl;
        exit(EXIT_FAILURE);
    }
    std::string line;
    int number,team_id,jersey_num;
    string first_name,last_name;
	int mpg, ppg, rpg, apg;
    double spg ,bpg;
    while(std::getline(f,line)){
        parser.clear();
        parser<<line;
        parser>>number>>team_id>>jersey_num>>first_name>>last_name>>mpg>>ppg>>rpg>>apg>>spg>>bpg;
        add_player(C,team_id,jersey_num,first_name,last_name,mpg,ppg,rpg,apg,spg,bpg);
    }
    f.close();

}

//add Team from txt;
void addTeam(connection*C,std::string file){
    std::ifstream f(file);
    std::stringstream parser;
    
    if(!f.is_open()){
        std::cerr<<"error opening file"<<std::endl;
        exit(EXIT_FAILURE);
    }
    std::string line;
   
    string name;
    int number,state_id,color_id, wins, losses;
    while(std::getline(f,line)){
        parser.clear();
        parser<<line;
        parser>>number>>name>>state_id>>color_id>>wins>>losses;
        add_team(C,name,state_id,color_id,wins,losses);
    }
    f.close();

}
//add state from txt
void addState(connection*C,std::string file){
    std::ifstream f(file);
    std::stringstream parser;
    if(!f.is_open()){
        std::cerr<<"error opening file"<<std::endl;
        exit(EXIT_FAILURE);
    }
    std::string line;
   
    string name;
    int number;
    while(std::getline(f,line)){
        parser.clear();
        parser<<line;
        parser>>number>>name;
        add_state(C,name);
    }
    f.close();

}
//add color from txt
void addColor(connection*C,std::string file){
    std::ifstream f(file);
    std::stringstream parser;
    if(!f.is_open()){
        std::cerr<<"error opening file"<<std::endl;
        exit(EXIT_FAILURE);
    }
    std::string line;
   
    string name;
    int number;
    while(std::getline(f,line)){
        parser.clear();
        parser<<line;
        parser>>number>>name;
        add_color(C,name);
    }
    f.close();
}