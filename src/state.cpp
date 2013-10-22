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
    mig_vms = new list<Info>;
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
    for(int i=0; i<num_vms; i++)
        for(list<Info>::iterator it=pm_to_vm_map[i]->begin(); it!=pm_to_vm_map[i]->end(); ++it)
            state->accommodateVm(it->index, sdata->getWorkload(state->phase_num, it->index), i);
}

void State::getSortedViolatedVM(Heap *vm_list)
{
    for(int i=0; i<num_vms; i++)
        if(total_util[i] > UTIL_THRESHOLD)
            for(list<Info>::iterator it=pm_to_vm_map[i]->begin(); it!=pm_to_vm_map[i]->end(); ++it)
                vm_list->push(*it);
}

void State::getSortedLTViolatedVM(Heap *vm_list)
{
    for(int i=0; i<num_vms; i++)
        if(total_util[i] < LOWERUTIL_THRESHOLD)
            for(list<Info>::iterator it=pm_to_vm_map[i]->begin(); it!=pm_to_vm_map[i]->end(); ++it)
                vm_list->push(*it);
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
    mig_vms->push_back(Info(vm_info.index, set_index));
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

float State::getSUV(SimData* sdata)
{
    float suv = 0;
    float * reward = new float [num_vms];
    float * penalty = new float [num_vms];
    
    for(int i=0; i<num_vms; i++)
    {
        reward[i] = 0;
        penalty[i] = 0;
    }

    for(int j=0; j<num_vms; j++)
    {
        reward[vm_to_pm_map[j]] += sdata->getVmRevenue(j);
        penalty[vm_to_pm_map[j]] += sdata->getVmPenalty(j);
    }

    for(int i=0; i<num_vms; i++)
    {
        if(total_util[i] > 0)
            suv -= (STATICPOWERCONSTANT + DYNAMICPOWERCONSTANT * (total_util[i]>1?1:total_util[i])) * MAXPOWER * COSTPERKWH;

        if(total_util[i] <= UTIL_THRESHOLD)
            suv += reward[i];
        else
            suv -= penalty[i];
    }

    delete [] reward;
    delete [] penalty;
    return suv;
}

float State::getISUV(State *next_state, SimData* sdata)
{
    list<Info>* mvms = next_state->mig_vms;
    float isuv = 0;
    float* util = new float[num_vms];
    float* reward = new float[num_vms];
    float* penalty = new float[num_vms];
    
    for(int i=0; i<num_vms; i++)
    {
        util[i] = 0;
        reward[i] = 0;
        penalty[i] = 0;
    }

    for(int j=0; j<num_vms; j++)
    {
        reward[vm_to_pm_map[j]] += sdata->getVmRevenue(j);
        penalty[vm_to_pm_map[j]] += sdata->getVmPenalty(j);
        util[vm_to_pm_map[j]] += sdata->getWorkload(phase_num, j);
    }

    for(list<Info>::iterator i = mvms->begin(); i != mvms->end(); ++i)
    {
        util[vm_to_pm_map[i->index]] += MOHCPUINTENSIVE*sdata->getWorkload(phase_num, i->index);
        util[(int)(i->val)] += MOHCPUINTENSIVE*sdata->getWorkload(phase_num, i->index);
    }

    for(int i=0; i<num_vms; i++)
    {
        if(util[i] > 0)
            isuv -= (STATICPOWERCONSTANT + DYNAMICPOWERCONSTANT * (util[i]>1?1:util[i])) * MAXPOWER * COSTPERKWH;

        if(util[i] <= UTIL_THRESHOLD)
            isuv += reward[i];
        else
            isuv -= penalty[i];
    }
    
    delete [] util;
    delete [] reward;
    delete [] penalty;
    return isuv;
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
        for(list<Info>::iterator i = mig_vms->begin(); i != mig_vms->end(); ++i)
            cout<<(i->index)<<",";
    }
}

State::~State()
{
    for(int i=0; i<num_vms; i++) { delete pm_to_vm_map[i];}
    delete [] pm_to_vm_map;
    delete [] total_util;
    delete [] vm_to_pm_map;
    delete mig_vms;
}
