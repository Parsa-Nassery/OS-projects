#include "libs.h"

using namespace std;

class Position{
private:
    string name;
    vector<int> ages;
    int clubs_c;
    int id;
public:
    Position(string nm, int cc, int idd);
    void run();
    void get_from_team(int i);
    void data_analyzer(int read_fd);
    void describe();
};