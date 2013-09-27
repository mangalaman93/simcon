#ifndef STATE_H
#define STATE_H

#include <iostream>
#include <list>
#include "simdata.h"
#include "config.h"
using namespace std;

class State
{
    int phase_num;
    int num_pms;
    list<int> **pm_to_vm_map;
    double *total_util;

public:
    State(int phase_num, SimData *sdata);
    ~State();
    void accommodateVm(int vm, double util, int set_index);
    bool ifVmAllowedOnPm(int set_index, double vm_util);
    void print();
    void getNextState(State *state);
};

#endif
