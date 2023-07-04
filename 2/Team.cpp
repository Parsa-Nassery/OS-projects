#include "Team.h"



Team :: Team(int num, string nm, vector<string> wp, string fldr){
    team_number = num;
    name = nm;
    wanted_positions = wp;
    floder_address = fldr;
}


void Team :: Read_Data_From_CSV(){
    ifstream file;
    file.open(floder_address + "/" + name);
    string line, word;
    int i = 0;
    while (getline(file, line)){
        vector<string> temp;
        stringstream ss(line);
        while (getline(ss, word, ',')){
            temp.push_back(word);
        }
        i ++;
        data.push_back(temp);
    }
    file.close();
    return;
}


void Team :: analyze_players(){
    for (int awp = 0; awp < wanted_positions.size(); awp ++){
        vector<int> temp;
        for (int pl = 0; pl < data.size(); pl ++){
            if (data[pl][1].compare(wanted_positions[awp]) == 0){
                temp.push_back(stoi(data[pl][2]));
            }
        }
        wp_counters.push_back(temp);
    }
    return;
}


void Team :: send_data(){
    for (int i = 0; i < wanted_positions.size(); i ++){
        string fifo = "./fifo" + to_string(i) + "_" + to_string(team_number);
        int fd;
        mkfifo(fifo.c_str(), 0666);
        fd = open(fifo.c_str(), O_WRONLY);
        string message;
        for (int j = 0; j < wp_counters[i].size(); j ++){
            if (j == 0){
                message = message + to_string(wp_counters[i][j]);
            }
            else{
                message = message + "_" + to_string(wp_counters[i][j]);
            }
        }
        string final_msg = to_string(message.size());
        final_msg = final_msg + message;
        write(fd, final_msg.c_str(), MAXLEN);
        close(fd);
    }
    return;
}






void Team :: run(){
    Read_Data_From_CSV();
    analyze_players();
    send_data();
    return;
}






int main(int argc, char** argv){
    // 1 -> num
    // 2 -> folder
    // 3 -> wp
    // 4 -> pipe

    int num = stoi(argv[1]);
    string folder = argv[2];
    string wpstr = argv[3];
    int read_pipe = stoi(argv[4]);

    vector<string> wpvec;
    for (int i = 0; (wpstr[i] != '\0'); i ++){
        string tenew;
        for (i ; (wpstr[i] != '_') & (wpstr[i] != '\0'); i ++){
            tenew.push_back(wpstr[i]);
        }
        wpvec.push_back(tenew);
        if (wpstr[i] == '\0'){
            break;
        }
    }

    char buffer[MAXLEN];
    
    read(read_pipe, buffer, MAXLEN);
    string name = string(buffer);
    close(read_pipe);
    Team tm(num, name, wpvec, folder);
    tm.run();
    return 0;
}