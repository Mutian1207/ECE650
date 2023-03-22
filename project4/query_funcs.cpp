#include "query_funcs.h"
#include"helper.h"
#include<iomanip>
void add_player(connection *C, int team_id, int jersey_num, string first_name, string last_name,
                int mpg, int ppg, int rpg, int apg, double spg, double bpg)
{
    std::stringstream query;
    work W(*C);
    query<<"INSERT INTO PLAYER (TEAM_ID, UNIFORM_NUM, FIRST_NAME, LAST_NAME, MPG, PPG, RPG, APG, SPG, BPG) VALUES ("
    <<team_id<<", "<<jersey_num<<", "<< W.quote(first_name)<< ", "<<W.quote(last_name)<<", "<<mpg
    <<", "<< ppg<<","<< rpg<< ", "<< apg<<", "<<spg<<", "<<bpg<<");";
    W.exec(query.str());
    W.commit();
}


void add_team(connection *C, string name, int state_id, int color_id, int wins, int losses)
{
    std::stringstream query;
    work W(*C);
    query<<"INSERT INTO TEAM (NAME, STATE_ID, COLOR_ID, WINS, LOSSES) VALUES ("<<W.quote(name)<<", "
    <<state_id<<", "<<color_id<<", "<< wins<<", "<<losses<<");";
    W.exec(query.str());
    W.commit();
}


void add_state(connection *C, string name)
{ 
    work W(*C);
    std::stringstream query;
    query<<"INSERT INTO STATE (NAME) VALUES ("<<W.quote(name)<<");";
    W.exec(query.str());
    W.commit();
}


void add_color(connection *C, string name)
{
    work W(*C);
    stringstream query;
    query<<"INSERT INTO COLOR (NAME) VALUES ("<<W.quote(name)<<");";
    //SqlExec(query.str(),C);
    W.exec(query.str());
    W.commit();
}

/*
 * All use_ params are used as flags for corresponding attributes
 * a 1 for a use_ param means this attribute is enabled (i.e. a WHERE clause is needed)
 * a 0 for a use_ param means this attribute is disabled
 */
void query1(connection *C,
	    int use_mpg, int min_mpg, int max_mpg,
            int use_ppg, int min_ppg, int max_ppg,
            int use_rpg, int min_rpg, int max_rpg,
            int use_apg, int min_apg, int max_apg,
            int use_spg, double min_spg, double max_spg,
            int use_bpg, double min_bpg, double max_bpg
            )
{
    std::stringstream query;
    query<<"SELECT * FROM PLAYER";
    std::vector<std::string> names{"MPG","PPG","RPG","APG","SPG","BPG"};
    std::vector<int> flag{use_mpg,use_ppg,use_rpg,use_apg,use_spg,use_bpg};
    std::vector<int> minv{min_mpg,min_ppg,min_rpg,min_apg};
    std::vector<int> maxv{max_mpg,max_ppg,max_rpg,max_apg};
    
    std::vector<double> mindv{min_spg,min_bpg};
    std::vector<double> maxdv{max_spg,max_bpg};
    int fflag = 1;
    for(auto i=0;i<flag.size();i++){
        if(flag[i]){
            if(fflag){
                query<<" WHERE ";
                fflag=false;
                if(i<4){
                query<<"(" << names[i] <<" between "<<minv[i]<<" AND "<<maxv[i]<<")";
                }else{
                query<<"(" << names[i] <<" between "<<mindv[i-4]<<" AND "<<maxdv[i-4]<<")";
                }
            }else{
                query<<" AND ";
                if(i<4){
                query<<"(" << names[i] <<" between "<<minv[i]<<" AND "<<maxv[i]<<")";
                }else{
                query<<"(" << names[i] <<" between "<<mindv[i-4]<<" AND "<<maxdv[i-4]<<")";
                }
            }
        }
    }
    query<<";";
    nontransaction N(*C);
    result res(N.exec(query.str()));
    std::cout<<"PLAYER_ID TEAM_ID UNIFORM_NUM FIRST_NAME LAST_NAME MPG PPG RPG APG SPG BPG"<<std::endl;
    for(result::iterator r=res.begin();r!=res.end();r++){
        cout<<r[0].as<int>()<<" "<<r[1].as<int>()<<" "<<r[2].as<int>()<<" "
        <<r[3].as<std::string>()<<" "<<r[4].as<std::string>()<<" "<<r[5].as<int>()<<" "
        <<r[6].as<int>()<<" "<<r[7].as<int>()<<" "<<r[8].as<int>()<<" "
        <<std::fixed<<std::setprecision(1)<<r[9].as<double>()<<" "<<r[10].as<double>()<<endl;
    }
}


void query2(connection *C, string team_color)
{
    std::stringstream query;
    query<<"SELECT TEAM.NAME FROM TEAM, COLOR WHERE TEAM.COLOR_ID = COLOR.COLOR_ID AND COLOR.NAME = "
    << "\'"<<team_color<<"\'"<<";";
    nontransaction N(*C);
    result res(N.exec(query.str()));
    std::cout<<"NAME"<<std::endl;
    for(result::iterator it = res.begin();it!=res.end();++it){
        cout<<it[0].as<std::string>()<<std::endl;
    }

}


void query3(connection *C, string team_name)
{
    stringstream query;
    query<<"SELECT PLAYER.FIRST_NAME, PLAYER.LAST_NAME FROM PLAYER, TEAM "
    <<"WHERE PLAYER.TEAM_ID = TEAM.TEAM_ID AND TEAM.NAME = "<< "\'"<<team_name<< "\'"
    <<" ORDER BY PPG DESC;";
    
    nontransaction N(*C);
    result res(N.exec(query.str()));
    std::cout<<"FIRST_NAME LAST_NAME"<<std::endl;
    for(result::iterator it = res.begin();it!=res.end();++it){
        cout<<it[0].as<std::string>()<<" "<<it[1].as<std::string>()<<std::endl;
    }

}


void query4(connection *C, string team_state, string team_color)
{   
    stringstream query;
    query<<"SELECT PLAYER.UNIFORM_NUM, PLAYER.FIRST_NAME, PLAYER.LAST_NAME FROM PLAYER, TEAM, STATE, COLOR "
    <<"WHERE PLAYER.TEAM_ID = TEAM.TEAM_ID AND TEAM.STATE_ID = STATE.STATE_ID AND TEAM.COLOR_ID = COLOR.COLOR_ID"
    <<" AND STATE.NAME = " << "\'"<<team_state<< "\'"<<" AND COLOR.NAME = "<<  "\'"<<team_color<< "\'"<<";";
    nontransaction N(*C);
    result res(N.exec(query.str()));
    std::cout<<"UNIFORM_NUM FIRST_NAME LAST_NAME"<<std::endl;
    for(result::iterator it = res.begin();it!=res.end();++it){
        cout<<it[0].as<int>()<<" "<<it[1].as<std::string>()<<" "<<it[2].as<std::string>()<<std::endl;
    }
    
}


void query5(connection *C, int num_wins)
{
    stringstream query;
    query<<"SELECT PLAYER.FIRST_NAME, PLAYER.LAST_NAME, TEAM.NAME, TEAM.WINS FROM PLAYER, TEAM"
    << " WHERE PLAYER.TEAM_ID = TEAM.TEAM_ID AND TEAM.WINS > "<< "\'"<<num_wins<< "\'"<<";";
   
    nontransaction N(*C);
    result res(N.exec(query.str()));
    std::cout<<"FIRST_NAME LAST_NAME NAME WINS"<<std::endl;
    for(result::iterator it = res.begin();it!=res.end();++it){
        cout<<it[0].as<std::string>()<<" "<<it[1].as<std::string>()<<" "<<it[2].as<std::string>()<<" "
        <<it[3].as<int>()<<std::endl;
    }
    
}
