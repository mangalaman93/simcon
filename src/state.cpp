#include "state.h"

State::State(int phase_num, SimData *sdata)
{
    this->num_vms = sdata->getNumVM();
    this->phase_num = phase_num;
    pm_to_vm_map = new list<Info>*[num_vms];
    for(int i=0; i<num_vms; i++) { pm_to_vm_map[i] = new list<Info>;}
    vm_to_pm_map = new int[sdata->getNumVM()];
    total_util = new double[num_vms];
    for(int i=0; i<num_vms; i++) { total_util[i]=0;}
    mig_vms = new list<int>;
}

void State::accommodateVm(int vm, double util, int set_index)
{
    pm_to_vm_map[set_index]->push_back(Info(vm, util));
    vm_to_pm_map[vm] = set_index;
    total_util[set_index] += util;
}

bool State::ifVmAllowedOnPm(int set_index, double vm_util)
{
    return (total_util[set_index] + vm_util <= UTIL_THRESHOLD);
}

void State::getNextState(State *state, SimData *sdata)
{
    state->phase_num = phase_num + 1;
    state->num_vms = num_vms;
    for(int i=0; i<num_vms; i++)
    {
        for(list<Info>::iterator it=pm_to_vm_map[i]->begin(); it!=pm_to_vm_map[i]->end(); ++it)
        {
            int vm = it->index;
            double util = sdata->getWorkload(state->phase_num, vm);
            state->pm_to_vm_map[i]->push_back(Info(vm, util));
            state->vm_to_pm_map[vm] = i;
            state->total_util[i] += util;
        }
    }
}

void State::print()
{
    cout<<"[";
    for(int i=0; i<num_vms; i++)
    {
        if(pm_to_vm_map[i]->size() != 0)
        {
            cout<<"{";
            for(list<Info>::iterator it=pm_to_vm_map[i]->begin(); it!=pm_to_vm_map[i]->end(); ++it)
            {
                if(it == pm_to_vm_map[i]->begin()) cout<<it->index;
                else cout<<","<<it->index;
            }
            cout<<"}";
        }
    }
    cout<<"]";
}

void State::printMigrations()
{
    if(mig_vms->empty())
        cout<<"no migrations,";
    else
    {
        cout<<"migrate vms ";
        for(list<int>::iterator i = mig_vms->begin(); i != mig_vms->end(); ++i)
            cout<<*i<<",";
    }
}

void State::getSortedViolatedVM(Heap *vm_list)
{
    for(int i=0; i<num_vms; i++)
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
    for(int i=0; i<num_vms; i++)
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
    for(int i=0; i<num_vms; i++)
        pm_list->push(Info(i, UTIL_THRESHOLD-total_util[i]));
}

void State::migrate(int set_index, Info vm_info)
{
    pm_to_vm_map[vm_to_pm_map[vm_info.index]]->remove(vm_info);
    total_util[vm_to_pm_map[vm_info.index]] -= vm_info.val;
    accommodateVm(vm_info.index, vm_info.val, set_index);
    mig_vms->push_back(vm_info.index);
}

double calMean(double *data, int size, int k)
{
    double sum = 0;
    for(int i=0; i<size; i++) { sum += pow(data[i], k);}
    return sum/size;
}

bool State::isIncrVar(int set_index, Info vm_info)
{
    if(set_index == vm_to_pm_map[vm_info.index]) { return false;}
    double old_var = calMean(total_util, num_vms, 2) - pow(calMean(total_util, num_vms, 1), 2);

    // new utilizations
    total_util[set_index] += vm_info.val;
    total_util[vm_to_pm_map[vm_info.index]] -= vm_info.val;

    double new_var = calMean(total_util, num_vms, 2) - pow(calMean(total_util, num_vms, 1), 2);

    //reverting back to original utilizations
    total_util[set_index] -= vm_info.val;
    total_util[vm_to_pm_map[vm_info.index]] += vm_info.val;

    return (new_var > old_var);
}

State::~State()
{
    for(int i=0; i<num_vms; i++) { delete pm_to_vm_map[i];}
    delete [] pm_to_vm_map;
    delete [] total_util;
    delete [] vm_to_pm_map;
    delete mig_vms;
}
