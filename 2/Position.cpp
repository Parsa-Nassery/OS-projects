#include "Position.h"

Position :: Position(string nm, int cc, int idd){
    name = nm;
    clubs_c = cc;
    id = idd;
}


void Position :: data_analyzer(int fd){
    char message[MAXLEN];
    char temp[MAXLEN];
    read(fd, temp, 1);
    int buf_size = temp[0] - '0';
    read(fd, message, buf_size);
    message[buf_size] = '\0';
    for (int i = 0; message[i] != '\0'; i ++){
        string sub;
        for (i; (message[i] != '_') & (message[i] != '\0'); i ++){
            sub.push_back(message[i]);
        }
        sub.push_back('\0');
        ages.push_back(stoi(sub));
        if (message[i] == '\0'){
            break;
        }
    }
    return;
}


void Position :: describe(){
    int min = 1000;
    int max = 0;
    int sum = 0;
    for (int i = 0; i < ages.size(); i ++){
        sum = sum + ages[i];
        if (min > ages[i]){
            min = ages[i];
        }
        if (max < ages[i]){
            max = ages[i];
        }
    }
    cout << name << " max: " << max << endl;
    cout << name << " min: " << min << endl;
    cout << name << " ave: " << sum / ages.size() << endl;
    cout << name << " count: " << ages.size() << endl;
    return;
}


void Position :: get_from_team(int i){
    string fifo = "./fifo" + to_string(id) + "_" + to_string(i);
    mkfifo(fifo.c_str(), 0666);
    int read = open(fifo.c_str(), O_RDONLY);
    data_analyzer(read);
    close(read);
}


void Position :: run(){
    for (int i = 0; i < clubs_c; i ++){
        get_from_team(i);
    }
    describe();
}









int main(int argc, char** argv){
    //1 -> pipe
    //2 -> cc
    //3 -> posid

    char buffer[MAXLEN];
    int read_pipe = stoi(argv[1]);
    int cc = stoi(argv[2]);
    int pos_num = stoi(argv[3]);

    read(read_pipe, buffer, MAXLEN);
    string nm = string(buffer);
    close(read_pipe);
    
    Position pos(nm, cc, pos_num);
    pos.run();
    return 0;
}