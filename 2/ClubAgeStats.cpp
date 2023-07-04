#include "ClubAgeStats.h"

ClubAgeStats :: ClubAgeStats(char* folder){
    folder_addres = folder;
}


vector<string> ClubAgeStats :: purification_teams(vector<string> entery){
    int double_dot = 500, alone_dot = 400;
    for (int i = 0; i < entery.size(); i ++){
        if (entery[i].compare(".") == 0){
            alone_dot = i;
        }
        if (entery[i].compare("..") == 0){
            double_dot = i;
        }
    }
    if (double_dot > alone_dot){
        entery.erase(entery.begin() + double_dot);
        entery.erase(entery.begin() + alone_dot);
    }
    else{
        entery.erase(entery.begin() + alone_dot);
        entery.erase(entery.begin() + double_dot);
    }
    return entery;
}

void ClubAgeStats :: Add_Positions(){
    vector<string> temp;
    string line, word;
    fstream f (folder_addres + "/positions" + ".csv", ios :: in);
    if (f.is_open()){
        while(getline(f, line)){
            temp.clear();
            stringstream str(line);
            while(getline(str, word, ',')){
                positions.push_back(word);
                positions_c ++;
            }
        }
    }
    else{
        cout << "can't open positions.csv\n" << endl;
    }
    return;
}


void ClubAgeStats :: Country_Counter_func(){
    vector<string> temp;
    DIR *dr;
    struct dirent *en;
    dr = opendir("./clubs");
    if (dr) {
        while ((en = readdir(dr)) != NULL) {
            country_c ++;
            temp.push_back(en->d_name);
        }
        closedir(dr);
    }
    
    for (int i = 2; i < country_c; i ++){
        if("positions.csv" != temp[i]){
            country.push_back(temp[i]);
        }
    }
    country_c = country_c - 3;

    for (int i = 0; i < country_c; i ++){
        vector<string> nowteam;
        int now = 0;
        DIR *dr;
        struct dirent *en;
        string first = "./clubs/";
        first.append(country[i]);
        char dirpath [MAXLEN];
        strcpy(dirpath, first.c_str());
        dr = opendir(dirpath);
        if (dr){
            while((en = readdir(dr)) != NULL){
                now ++;
                clubs_c ++;
                nowteam.push_back(en->d_name);
            }
            closedir(dr);
        }
        clubs_c --;
        clubs_c --;
        now --;
        now --;
        club_cc.push_back(now);
        nowteam = purification_teams(nowteam);
        teams.push_back(nowteam);
        for (int k = 0; k < nowteam.size(); k ++){
            ordered_teams.push_back(nowteam[k]);
            paths.push_back(first);
        }
    }
    return;
}


void ClubAgeStats :: run(){
    Add_Positions();
    Country_Counter_func();
    cout << "positions:" << endl;
    for (int i = 0; i < positions_c; i ++){
        cout << positions[i] << " ";
    }
    cout << endl << "Enter positions to get stats:" << endl;
    char pos[MAXLEN];
    cin.getline(pos, MAXLEN);
    for (int i = 0; pos[i] != '\0'; i ++){
        string one;
        for (i; (pos[i] != '\0') & (pos[i] != ' '); i ++){
            one.push_back(pos[i]);
        }
        wp_c ++;
        wanted_positions.push_back(one);
    }

    str_wp = wanted_positions[0];
    for (int i = 1; i < wanted_positions.size(); i ++){
        str_wp = str_wp + "_";
        str_wp = str_wp + wanted_positions[i];
    }


    int pipes[wp_c][2];
    int pipes_for_Team_names[clubs_c][2];
    for (int i = 0; i < wp_c; i ++){
        pipe(pipes[i]);
    }
    for (int i = 0; i < clubs_c; i ++){
        pipe(pipes_for_Team_names[i]);
    }


    for (int i = 0; i < clubs_c; i ++){
        int pipe = fork();
        if (pipe == 0){
            string team_exc = "./Team.out";
            string team_id = to_string(i);
            string folder_for_team = paths[i];
            string wp_for_team = str_wp;
            string pip_read_team = to_string(pipes_for_Team_names[i][0]);
            char* arguments[6] = {&team_exc[0], &team_id[0], &folder_for_team[0], &wp_for_team[0], &pip_read_team[0], NULL};
            execv(arguments[0], arguments);
        }
        else if (pipe > 0){
            write(pipes_for_Team_names[i][1], ordered_teams[i].c_str(), ordered_teams[i].size());
            close(pipes_for_Team_names[i][1]);
        }
    }
    for (int i = 0; i < wp_c; i ++){
        int pipe = fork();
        if (pipe == 0){
            string pos_exc = "./Position.out";
            string pipe_read_pos = to_string(pipes[i][0]);
            string cc_for_pos = to_string(clubs_c);
            string id_for_pos = to_string(i);
            char* arguments[5] = {&pos_exc[0], &pipe_read_pos[0], &cc_for_pos[0], &id_for_pos[0], NULL};
            execv(arguments[0], arguments);
        }
        else if (pipe > 0){
            write(pipes[i][1], wanted_positions[i].c_str(), wanted_positions[i].size());
            close(pipes[i][1]);
        }
    }

    while(wait(NULL) > 0);
    return;
}


int main(int argc, char** argv){
    ClubAgeStats cas(argv[1]);
    cas.run();
}