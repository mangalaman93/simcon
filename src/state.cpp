#include "state.h"

State::State(int phase_num, SimData *sdata)
{
    this->num_pms = sdata->getNumPM();
    this->phase_num = phase_num;
    pm_to_vm_map = new list<Info>*[num_pms];
    for(int i=0; i<num_pms; i++) { pm_to_vm_map[i] = new list<Info>;}
    vm_to_pm_map = new int[sdata->getNumVM()];
    total_util = new double[num_pms];
    for(int i=0; i<num_pms; i++) { total_util[i]=0;}
    this->power_cost = INFINITY;
    this->revenue = -INFINITY;
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
    state->num_pms = num_pms;
    state->power_cost=0;
    state->revenue=0;
    for(int i=0; i<num_pms; i++)
    {
        for(list<Info>::iterator it=pm_to_vm_map[i]->begin(); it!=pm_to_vm_map[i]->end(); ++it)
        {
            int vm = it->index;
            double util = sdata->getWorkload(state->phase_num, vm);
            state->pm_to_vm_map[i]->push_back(Info(vm, util));
            state->vm_to_pm_map[vm] = i;
            state->total_util[i] += util;
        }
        power_cost += (STATICPOWERCONSTANT * MAXPOWER + DYNAMICPOWERCONSTANT * MAXPOWER *
                      (state->total_util[i] > 1.0 ? 1.0:state->total_util[i])) * COSTPERKWH * sdata->getPhaseDuration(phase_num);
        if(total_util[i] <= UTIL_THRESHOLD)
            revenue += state->pm_to_vm_map[i]->size()*IMREWARD*sdata->getPhaseDuration(phase_num);
        else
            revenue -= state->pm_to_vm_map[i]->size()*IMPENALTY*sdata->getPhaseDuration(phase_num);
    }
}

void State::setPowerCostAndRevenue(SimData *sdata)
{
    if(power_cost == INFINITY)
    {
        power_cost = 0;
        revenue = 0;
        for(int i=0; i<num_pms; i++)
        {
            power_cost += (STATICPOWERCONSTANT * MAXPOWER + DYNAMICPOWERCONSTANT * MAXPOWER *
                          (total_util[i] > 1.0 ? 1.0:total_util[i])) * COSTPERKWH * sdata->getPhaseDuration(phase_num);
            if(total_util[i] <= UTIL_THRESHOLD)
                revenue += pm_to_vm_map[i]->size()*IMREWARD * (sdata->getPhaseDuration(phase_num));
            else
                revenue -= pm_to_vm_map[i]->size()*IMPENALTY * (sdata->getPhaseDuration(phase_num));
        }
    }
}

void State::print()
{
    cout<<"[";
    for(int i=0; i<num_pms; i++)
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

void State::getSortedViolatedVM(Heap *vm_list)
{
    for(int i=0; i<num_pms; i++)
    {
        if(total_util[i] > UTIL_THRESHOLD)
        {
            double mig_value = UTIL_THRESHOLD - total_util[i];
            for(list<Info>::iterator it=pm_to_vm_map[i]->begin(); it!=pm_to_vm_map[i]->end(); ++it)
            {
               if(mig_value > 0)
               {
                    vm_list->push(*it);
                    mig_value -= (*it).val;
               }
            }
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
        if(total_util[i] <= UTIL_THRESHOLD)
            pm_list->push(Info(i, UTIL_THRESHOLD-total_util[i]));
    }
}

void State::migrate(int set_index, Info vm_info)
{
    pm_to_vm_map[vm_to_pm_map[vm_info.index]]->remove(vm_info);
    total_util[vm_to_pm_map[vm_info.index]] -= vm_info.val;
    accommodateVm(vm_info.index, vm_info.val, set_index);
}

double calMean(double *data, int size, int k)
{
    double sum = 0;
    for(int i=0; i<size; i++)
    {
        sum += pow(data[i], k);
    }

    return sum/size;
}

bool State::ifProfit(int set_index, Info vm_info, SimData *sdata)
{
    double lrevenue = revenue;
    double nrevenue = 0;
    int new_pm_index = vm_to_pm_map[vm_info.index];
    migrate(set_index,vm_info);

    for(int i=0; i<num_pms; i++)
    {
        if(total_util[i] <= UTIL_THRESHOLD)
            nrevenue += pm_to_vm_map[i]->size()*IMREWARD * (sdata->getPhaseDuration(phase_num));
        else
            nrevenue -= pm_to_vm_map[i]->size()*IMPENALTY * (sdata->getPhaseDuration(phase_num));
    }

    if(nrevenue <= lrevenue)
    {
        //discard migration
        migrate(new_pm_index, vm_info);
        return false;
    }
    else
        revenue = nrevenue;
    return true;
}

bool State::isIncrVar(int set_index, Info vm_info)
{
    if(set_index == vm_to_pm_map[vm_info.index]) { return false;}
    double old_var = calMean(total_util, num_pms, 2) - pow(calMean(total_util, num_pms, 1), 2);

    // new utilizations
    total_util[set_index] += vm_info.val;
    total_util[vm_to_pm_map[vm_info.index]] -= vm_info.val;

    double new_var = calMean(total_util, num_pms, 2) - pow(calMean(total_util, num_pms, 1), 2);

    //reverting back to original utilizations
    total_util[set_index] -= vm_info.val;
    total_util[vm_to_pm_map[vm_info.index]] += vm_info.val;

    return (new_var > old_var);
}

double State::getPowerBenefit(Info vm_info, int set_index, SimData *sdata)
{
    //will always be 0 or less than 0 without a power model and constant phase duration
    return vm_info.val*(sdata->getPhaseDuration(phase_num+1)-sdata->getPhaseDuration(phase_num));
}

double State::getRevenue()
{
    return revenue;
}

State::~State()
{
    for(int i=0; i<num_pms; i++) { delete pm_to_vm_map[i];}
    delete pm_to_vm_map;
    delete total_util;
}
