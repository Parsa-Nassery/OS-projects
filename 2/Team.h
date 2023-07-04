#include "libs.h"

using namespace std;

class Team{
private:
    int team_number;
    string name;
    string floder_address;
    vector<vector<string>> data;
    vector<string> wanted_positions;
    vector<vector<int>> wp_counters;

public:
    Team(int num, string nm, vector<string> wp, string fldr);
    void Read_Data_From_CSV();
    void run();
    void analyze_players();
    void send_data();
};