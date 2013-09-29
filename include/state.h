#ifndef STATE_H
#define STATE_H

#include <iostream>
#include <list>
#include <queue>
#include <cmath>
#include "simdata.h"
#include "config.h"
using namespace std;

struct Info
{
    int index;
    double val;
    Info(int i, double u) : index(i), val(u) {}
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
    int num_pms;
    list<Info> **pm_to_vm_map;
    int *vm_to_pm_map;
    double *total_util;
    double power_cost;;
    double revenue;

public:
    State(int phase_num, SimData *sdata);
    ~State();
    void accommodateVm(int vm, double util, int set_index);
    bool ifVmAllowedOnPm(int set_index, double vm_util);
    void getNextState(State *state, SimData *sdata);
    void getSortedViolatedVM(Heap *vm_list);
    void getSortedLTViolatedVM(Heap *vm_list);
    void getSortedPM(Heap *pm_list);
    void setPowerCostAndRevenue(SimData *sdata);
    void migrate(int set_index, Info vm_info);
    bool ifProfit(int set_index, Info vm_info, SimData *sdata);
    bool isIncrVar(int set_index, Info vm_info);
    void print();
    double getPowerBenefit(Info vm_info, int set_index, SimData *sdata);
    double getRevenue();
};

#endif
