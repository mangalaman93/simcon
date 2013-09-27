#include "state.h"

State::State(int phase_num, SimData *sdata)
{
    this->num_pms = sdata->getNumPM();
    this->phase_num = phase_num;
    pm_to_vm_map = new list<int>*[num_pms];
    for(int i=0; i<num_pms; i++) { pm_to_vm_map[i] = new list<int>;}
    total_util = new double[num_pms];

    for(int i=0; i<num_pms; i++) { total_util[i]=0;}
}

void State::accommodateVm(int vm, double util, int set_index)
{
    pm_to_vm_map[set_index]->push_back(vm);
    total_util[set_index] += util;
}

bool State::ifVmAllowedOnPm(int set_index, double vm_util)
{
    return (total_util[set_index] + vm_util < UTIL_THRESHOLD);
}

void State::getNextState(State *state)
{
    state->phase_num = phase_num + 1;
    state->num_pms = num_pms;
    for(int i=0; i<num_pms; i++)
    {
        for(list<int>::iterator it=pm_to_vm_map[i]->begin(); it!=pm_to_vm_map[i]->end(); ++it)
            state->pm_to_vm_map[i]->push_back(*it);
        state->total_util[i] = total_util[i];
    }
}

void State::print()
{
    for(int i=0; i<num_pms; i++)
    {
        for(list<int>::iterator it=pm_to_vm_map[i]->begin(); it!=pm_to_vm_map[i]->end(); ++it)
            cout<<*it<<"\t";
        cout<<endl;
    }
}

State::~State()
{
    for(int i=0; i<num_pms; i++) { delete pm_to_vm_map[i];}
    delete pm_to_vm_map;
    delete total_util;
}
