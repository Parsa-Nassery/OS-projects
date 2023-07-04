#include "libs.h"

using namespace std;


class ClubAgeStats{
private:
    string folder_addres;
    string str_wp;
    vector<string> positions;
    vector<string> country;
    vector<string> wanted_positions;
    vector<vector<string>> teams;
    vector<string> ordered_teams;
    vector<string> paths;
    vector<int> club_cc;
    int country_c = 0;
    int clubs_c = 0;
    int positions_c = 0;
    int wp_c = 0;
public:
    ClubAgeStats(char* folder);
    void run();
    void Add_Positions();
    void Country_Counter_func();
    vector<string> purification_teams(vector<string> in);
};