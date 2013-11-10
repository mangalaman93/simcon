/*
This class handles the new state representation. It provides
various method for performing algorithm specific tasks.
*/

#ifndef STATE_H
#define STATE_H

#include <iostream>
#include <list>
#include <queue>
#include <cmath>
#include <algorithm>
#include "simdata.h"
#include "config.h"
using namespace std;

struct Info
{
    int index;
    float val;
    Info(int i, float u) : index(i), val(u) {}
    bool operator==(const Info& info) { return info.index == index;}
};

class CompareVM
{
    bool reverse;
public:
    CompareVM(const bool& rev=false) { reverse=rev;}
    bool operator()(Info& vm1, Info& vm2)
    {
        return reverse?(vm1.val > vm2.val):(vm1.val < vm2.val);
    }
};

typedef priority_queue<Info, vector<Info>, CompareVM> Heap;

class State
{
    int phase_num;
    int num_vms;
    list<Info> **pm_to_vm_map;
    int *vm_to_pm_map;
    float *total_util;
    list<Info> *mig_vms;

    void sortPMs();

public:
    State(int phase_num, SimData *sdata);
    ~State();
    void accommodateVm(int vm, float util, int set_index);
    bool ifVmAllowedOnPm(int set_index, float vm_util);
    void getNextState(State *state, SimData *sdata);
    void getSortedViolatedVM(Heap *vm_list);
    void getSortedLTViolatedVM(Heap *vm_list);
    void getSortedPM(Heap *pm_list);
    void migrate(int set_index, Info vm_info);
    bool isIncrVar(int set_index, Info vm_info);
    float getSUV(SimData* sdata);
    float getISUV(State *next_state, SimData* sdata);
    float get_total_util(int pm);
    void set_intermediate_util(State *next_state, float* iutil, SimData* sdata);
    void print();
    void printMigrations();
};

#endif
