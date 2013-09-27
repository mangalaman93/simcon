#include "state.h"

State::State(int phase_num, SimData *sdata)
{
    this->num_pms = sdata->getNumPM();
    this->phase_num = phase_num;
    pm_to_vm_map = new list<Info>*[num_pms];
    for(int i=0; i<num_pms; i++) { pm_to_vm_map[i] = new list<Info>;}
    total_util = new double[num_pms];

    for(int i=0; i<num_pms; i++) { total_util[i]=0;}
}

void State::accommodateVm(int vm, double util, int set_index)
{
    pm_to_vm_map[set_index]->push_back(Info(vm, util));
    total_util[set_index] += util;
}

bool State::ifVmAllowedOnPm(int set_index, double vm_util)
{
    return (total_util[set_index] + vm_util < UTIL_THRESHOLD);
}

void State::getNextState(State *state, SimData *sdata)
{
    state->phase_num = phase_num + 1;
    state->num_pms = num_pms;
    for(int i=0; i<num_pms; i++)
    {
        for(list<Info>::iterator it=pm_to_vm_map[i]->begin(); it!=pm_to_vm_map[i]->end(); ++it)
        {
            int vm = it->index;
            double util = sdata->getWorkload(state->phase_num, vm);
            state->pm_to_vm_map[i]->push_back(Info(vm, util));
            state->total_util[i] += util;
        }
    }
}

void State::print()
{
    for(int i=0; i<num_pms; i++)
    {
        for(list<Info>::iterator it=pm_to_vm_map[i]->begin(); it!=pm_to_vm_map[i]->end(); ++it)
            cout<<it->index<<"\t";
        cout<<endl;
    }
}

void State::getSortedViolatedVM(Heap *vm_list)
{
    for(int i=0; i<num_pms; i++)
    {
        if(total_util[i] > UTIL_THRESHOLD)
        {
            for(list<Info>::iterator it=pm_to_vm_map[i]->begin(); it!=pm_to_vm_map[i]->end(); ++it)
                vm_list->push(*it);
        }
    }
}

void State::getSortedLTViolatedVM(Heap *vm_list)
{
    for(int i=0; i<num_pms; i++)
    {
        if(total_util[i] < LOWERUTIL_THRESHOLD)
        {
            for(list<Info>::iterator it=pm_to_vm_map[i]->begin(); it!=pm_to_vm_map[i]->end(); ++it)
                vm_list->push(*it);
        }
    }
}

void State::getSortedPM(Heap *pm_list)
{
    for(int i=0; i<num_pms; i++)
    {
        pm_list->push(Info(i, UTIL_THRESHOLD-total_util[i]));
    }
}

State::~State()
{
    for(int i=0; i<num_pms; i++) { delete pm_to_vm_map[i];}
    delete pm_to_vm_map;
    delete total_util;
}
