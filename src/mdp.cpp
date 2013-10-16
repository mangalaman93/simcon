#include <iostream>
#include <cmath>
#include "algo.h"
#include "simdata.h"
#include "state.h"
#include "stateIterator.h"
using namespace std;

#define DEBUG true

// factorial calculation
long fact(int n)
{
    if(n == 0)
        return 1;
    else
        return (n*fact(n-1));
}

// get the next PM permutation given the older one
void getNext(int *value, int N)
{
    int i = N - 1;
    while(value[i-1] >= value[i]) 
        i = i-1;

    int j = N;
    while(value[j-1] <= value[i-1]) 
        j = j-1;

    int temp = value[i-1];
    value[i-1] = value[j-1];
    value[j-1] = temp;

    i++;
    j = N;
    while(i < j)
    {
        temp = value[i-1];
        value[i-1] = value[j-1];
        value[j-1] = temp;
        i++;
        j--;
    }
}

// SUV = state utility value
float getSUV(int phase, int* vm_to_pm_map1, int num_vms, SimData* sdata)
{
    float suv;
    int* util = new int[num_vms];
    int* reward = new int[num_vms];
    int* penalty = new int[num_vms];
    
    for(int i=0; i<num_vms; i++)
    {
        util[i] = 0;
        reward[i] = 0;
        penalty[i] = 0;
    }

    for(int j=0; j<num_vms; j++)
    {
        util[vm_to_pm_map1[j]] += sdata->getWorkload(phase, j);
        reward[vm_to_pm_map1[j]] += sdata->getVmRevenue(j);
        penalty[vm_to_pm_map1[j]] += sdata->getVmPenalty(j);
    }

    for(int i=0; i<num_vms; i++)
    {
        if(util[i] > 0)
        {
            suv -= STATICPOWERCONSTANT * MAXPOWER * COSTPERKWH; 
            suv -= DYNAMICPOWERCONSTANT * MAXPOWER * (util[i] > 1.0 ? 1.0:util[i]) * COSTPERKWH; 
        }

        if(util[i] <= UTIL_THRESHOLD)
            suv += reward[i];
        else
            suv -= penalty[i];
    }
    
    delete util;
    delete reward;
    delete penalty;
    return suv;
}

// ISUV = intermediate state utility value
float getISUV(int phase, int* vm_to_pm_map1, int* vm_to_pm_map2,
              int* perm_map, int num_vms, SimData* sdata)
{
    float isuv;
    int* util = new int[num_vms];
    int* reward = new int[num_vms];
    int* penalty = new int[num_vms];
    
    for(int i=0; i<num_vms; i++)
    {
        util[i] = 0;
        reward[i] = 0;
        penalty[i] = 0;
    }

    for(int j=0; j<num_vms; j++)
    {
        if(vm_to_pm_map1[j] == perm_map[vm_to_pm_map2[j]])
        {
            util[vm_to_pm_map1[j]]+=sdata->getWorkload(phase, j);
            reward[vm_to_pm_map1[j]] += sdata->getVmRevenue(j);
            penalty[vm_to_pm_map1[j]] += sdata->getVmPenalty(j);
        }
        else 
        {
            util[vm_to_pm_map1[j]] += (1 + MOHCPUINTENSIVE)*sdata->getWorkload(phase, j);
            util[perm_map[vm_to_pm_map2[j]]] += MOHCPUINTENSIVE*sdata->getWorkload(phase, j);
        }
    }

    for(int i=0; i<num_vms; i++)
    {
        if(util[i] > 0)
        {
            isuv -= STATICPOWERCONSTANT * MAXPOWER * COSTPERKWH; 
            isuv -= DYNAMICPOWERCONSTANT * MAXPOWER * (util[i] > 1.0 ? 1.0:util[i]) * COSTPERKWH; 
        }

        if(util[i] <= UTIL_THRESHOLD)
            isuv += reward[i];
        else
            isuv -= penalty[i];
    }
    
    delete util;
    delete reward;
    delete penalty;
    return isuv;
}

// function for the step of local optimization
float compareState(int phase, int* vm_to_pm_map1, int* vm_to_pm_map2, int num_vms, SimData* sdata, int *perm_map)
{
    // calculate State Utility Value
    float SUV = getSUV(phase, vm_to_pm_map1, num_vms, sdata);

    for(int i=0; i<num_vms; i++) { perm_map[i] = i;}
    float ISUV = getISUV(phase, vm_to_pm_map1, vm_to_pm_map2, perm_map, num_vms, sdata);
    for(int i=2; i<fact(num_vms); i++)
    {
        // calculate intermediate state utility value
        getNext(perm_map, num_vms);
        float temp = getISUV(phase, vm_to_pm_map1, vm_to_pm_map2, perm_map, num_vms, sdata);
        if(temp > ISUV)
            ISUV = temp;
    }

    return (SUV*(1-MIGRATIONDURATION)+ISUV*MIGRATIONDURATION);
}

int main()
{
    int num_vms, num_phases;
    cin>>num_vms;
    cin>>num_phases;

    SimData *s_data = new SimData(num_vms, num_phases);
    s_data->readInput();

    // calculating the possible number of states (bell number)
    long int num_states = Bell::get(num_vms);
    cout<<"num of states : "<<num_states<<endl;

    // constructing transition matrix
    Matrix<float> trans_table(num_phases, num_states, num_states);
    Matrix<int*> mig_table(num_phases, num_states, num_states);
    StateIterator sitr(num_vms);
    StateIterator sitr2(num_vms);
    for(int p=0; p<num_phases; p++)
    {
        for(sitr.begin(); sitr.end(); ++sitr)
        {
            for(sitr2.begin(); sitr2.end(); ++sitr2)
            {
                mig_table(p, (int)sitr, (int)sitr2) = new int[num_vms];
                trans_table(p, (int)sitr, (int)sitr2) = compareState(p, *sitr, *sitr2, num_vms, s_data,
                                                                     mig_table(p, (int)sitr, (int)sitr2));
            }
        }
    }
    cout<<"transition matrix prepared..."<<endl;

    // iterating over all possible cycles
    int *cycle = new int[num_phases];
    for(int i=0; i<num_phases; i++) { cycle[i]=0;}
    float *trans_profit = new float[num_phases];
    float profit = 0;
    for(int i=0; i<num_phases; i++)
    {
        trans_profit[i] = trans_table(0, 0, 0);
        profit += trans_profit[i];
    }

    // final output
    int *policy = new int[num_phases];
    for(int i=0; i<num_phases; i++) { policy[i]=cycle[i];}
    float min_profit = profit;

    cout<<"Total loops: "<<pow(num_states, num_phases)<<endl;
    long int loop_count = 0;

    for(double count=1; count<pow(num_states, num_phases); count++)
    {
        if(DEBUG)
        {
            int temp = (long int)(count/pow(10, 8));
            if(loop_count != temp)
            {
                cout<<"beginning loop number: "<<count<<endl;
                loop_count = temp;
            }
        }

        cycle[num_phases-1]++;
        int i;
        for(i=num_phases-1; i>=0; i--)
        {
            bool br = false;
            if(cycle[i] == num_states)
            {
                cycle[i] = 0;
                cycle[i-1]++;
            } else
            {
                br = true;
            }

            int i_plus_one;
            if(i+1 == num_phases) { i_plus_one = 0;}
            else { i_plus_one = i+1;}

            profit -= trans_profit[i];
            trans_profit[i] = trans_table(i, cycle[i], cycle[i_plus_one]);
            profit += trans_profit[i];

            if(br) break;
        }
        
        int i_minus_one;
        if(i-1 == -1) { i_minus_one = num_phases-1;}
        else { i_minus_one = i-1;}

        profit -= trans_profit[i_minus_one];
        trans_profit[i_minus_one] = trans_table(i_minus_one, cycle[i_minus_one], cycle[i]);
        profit += trans_profit[i_minus_one];

        // choose the minimum
        if(min_profit > profit)
        {
            min_profit = profit;
            for(int i=0; i<num_phases; i++) { policy[i] = cycle[i];}
        }
    }

    for(int i=0; i<num_phases; i++)
    {
        StateIterator sitr(num_vms);
        for(sitr.begin(); sitr.end(); ++sitr)
            if((int)sitr == policy[i])
                sitr.print();
    }
    cout<<"overall profit: "<<min_profit<<endl;

    for(int p=0; p<num_phases; p++)
    {
        for(int i=0; i<num_states; i++)
        {
            for(int j=0; j<num_states; j++)
            {
                delete mig_table(p, i, j);
            }
        }
    }
    delete s_data;
    delete policy;
    delete cycle;
    delete trans_profit;
}
