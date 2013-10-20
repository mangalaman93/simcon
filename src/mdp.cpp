#include <iostream>
#include <cmath>
#include "algo.h"
#include "simdata.h"
#include "config.h"
#include "stateIterator.h"
using namespace std;

#define DEBUG false

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
float getSUV(int phase, int* vm_to_pm_map, int num_vms, SimData* sdata)
{
    float suv = 0;
    float * util = new float [num_vms];
    float * reward = new float [num_vms];
    float * penalty = new float [num_vms];
    
    for(int i=0; i<num_vms; i++)
    {
        util[i] = 0;
        reward[i] = 0;
        penalty[i] = 0;
    }

    for(int j=0; j<num_vms; j++)
    {
        util[vm_to_pm_map[j]] += sdata->getWorkload(phase, j);
        reward[vm_to_pm_map[j]] += sdata->getVmRevenue(j);
        penalty[vm_to_pm_map[j]] += sdata->getVmPenalty(j);
    }

    for(int i=0; i<num_vms; i++)
    {
        if(util[i] > 0)
            suv -= (STATICPOWERCONSTANT + DYNAMICPOWERCONSTANT * (util[i]>1?1:util[i])) * MAXPOWER * COSTPERKWH;

        if(util[i] <= UTIL_THRESHOLD)
            suv += reward[i];
        else
            suv -= penalty[i];
    }
    
    delete [] util;
    delete [] reward;
    delete [] penalty;
    return suv;
}

// ISUV = intermediate state utility value
float getISUV(int phase, int* vm_to_pm_map1, int* vm_to_pm_map2, int* perm_map, int num_vms, SimData* sdata)
{
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
        reward[vm_to_pm_map1[j]] += sdata->getVmRevenue(j);
        penalty[vm_to_pm_map1[j]] += sdata->getVmPenalty(j);
        if(vm_to_pm_map1[j] == perm_map[vm_to_pm_map2[j]])
            util[vm_to_pm_map1[j]] += sdata->getWorkload(phase, j);
        else
        {
            util[vm_to_pm_map1[j]] += (1 + MOHCPUINTENSIVE)*sdata->getWorkload(phase, j);
            util[perm_map[vm_to_pm_map2[j]]] += MOHCPUINTENSIVE*sdata->getWorkload(phase, j);
        }
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

// function for the step of local optimization
float compareState(int phase, int* vm_to_pm_map1, int* vm_to_pm_map2, int num_vms, SimData* sdata, int *best_perm_map)
{
    // calculate State Utility Value
    float SUV = getSUV(phase, vm_to_pm_map1, num_vms, sdata);

    // permutation map stores mapping of PMs to the permuted PMs
    int* perm_map = new int[num_vms];
    for(int i=0; i<num_vms; i++) { perm_map[i]=i;}

    float ISUV = getISUV(phase, vm_to_pm_map1, vm_to_pm_map2, perm_map, num_vms, sdata);
    for(int i=0; i<num_vms; i++)
        best_perm_map[i] = perm_map[i];

    for(int i=2; i<fact(num_vms); i++)
    {
        getNext(perm_map, num_vms);
        float temp = getISUV(phase, vm_to_pm_map1, vm_to_pm_map2, perm_map, num_vms, sdata);
        if(temp > ISUV)
        {
            ISUV = temp;
            for(int j=0; j<num_vms; j++)
                best_perm_map[j] = perm_map[j];
        }
    }

    delete [] perm_map;
    return (SUV * (1 - MIGRATIONDURATION) + ISUV * MIGRATIONDURATION);
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
    cout<<"total number of states : "<<num_states<<endl;

    // constructing transition matrix
    cout<<"constructing transition matrix ..."<<endl;
    Matrix<float> trans_table(num_phases, num_states, num_states);
    Matrix<int*> mig_table(num_phases, num_states, num_states);
    StateIterator sitr(num_vms);
    StateIterator sitr2(num_vms);

    cout<<"total phases: "<<num_phases<<endl<<"phases complete: "; cout.flush();
    for(int p=0; p<num_phases; p++)
    {
        for(sitr.begin(); sitr.end(); ++sitr)
        {
            for(sitr2.begin(); sitr2.end(); ++sitr2)
            {
                mig_table(p, (int)sitr, (int)sitr2) = new int[num_vms];
                trans_table(p, (int)sitr, (int)sitr2) = compareState(p, *sitr, *sitr2, num_vms, s_data, mig_table(p,(int)sitr,(int)sitr2));
            }
        }
        cout<<p<<","; cout.flush();
    }
    cout<<endl<<"transition matrix prepared ..."<<endl;

    // printing the transition matrix
    if(DEBUG)
    {
        for(int p=0; p<num_phases; p++)
        {
            for(sitr.begin(); sitr.end(); ++sitr)
            {
                for(sitr2.begin(); sitr2.end(); ++sitr2)
                {
                    cout<<p<<"\t"<<(int)sitr<<"\t"<<(int)sitr2<<"\t"<<trans_table(p, (int)sitr, (int)sitr2)<<"\t";
                    sitr.print();
                    cout<<"\t";
                    sitr2.print();
                    cout<<"\t";
                    for(int j=0; j<num_vms; j++)
                    {
                        if((*sitr)[j] != (mig_table(p, (int)sitr, (int)sitr2))[(*sitr2)[j]])
                            cout<<j<<", ";
                    }
                    cout<<endl;
                }
            }
        }
    }

    // finding the most optimum cycle
    StateIterator sitr3(num_vms);
    Matrix<float> temp_trans(num_states, num_states);
    Matrix<list<int> > policy(num_states, num_states);
    Matrix<list<int> > new_policy(num_states, num_states);
    for(int p=0; p<num_phases-2; p++)
    {
        for(sitr.begin(); sitr.end(); ++sitr)
        {
            for(sitr3.begin(); sitr3.end(); ++sitr3)
            {
                float max = trans_table(p, (int)sitr, 0) + trans_table(p+1, 0, (int)sitr3);
                new_policy((int)sitr, (int)sitr3) = policy((int)sitr, 0);
                new_policy((int)sitr, (int)sitr3).push_back(0);
                for(sitr2.begin(); sitr2.end(); ++sitr2)
                {
                    float temp = trans_table(p, (int)sitr, (int)sitr2) + trans_table(p+1, (int)sitr2, (int)sitr3);
                    if(max < temp)
                    {
                        max = temp;
                        new_policy((int)sitr, (int)sitr3) = policy((int)sitr, (int)sitr2);
                        new_policy((int)sitr, (int)sitr3).push_back((int)sitr2);
                    }
                }
                temp_trans((int)sitr, (int)sitr3) = max;
            }
        }

        // copying the data
        for(int i=0; i<num_states; i++)
        {
            for(int j=0; j<num_states; j++)
            {
                trans_table(p+1, i, j) = temp_trans(i, j);
                policy(i, j) = new_policy(i, j);
            }
        }
    }

    float max_profit = trans_table(num_phases-2, 0, 0) + trans_table(num_phases-1, 0, 0);
    list<int> final_policy = policy(0, 0);
    final_policy.push_back(0);
    final_policy.push_back(0);
    final_policy.push_front(0);
    for(sitr.begin(); sitr.end(); ++sitr)
    {
        for(sitr2.begin(); sitr2.end(); ++sitr2)
        {
            float temp = trans_table(num_phases-2, (int)sitr, (int)sitr2) + trans_table(num_phases-1, (int)sitr2, (int)sitr);
            if(temp > max_profit)
            {
                max_profit = temp;
                final_policy = policy((int)sitr, (int)sitr2);
                final_policy.push_back((int)sitr2);
                final_policy.push_back((int)sitr);
                final_policy.push_front((int)sitr);
            }
        }
    }

    // printing the policy
    int i = 0;
    list<int>::iterator it=final_policy.begin();
    int last_policy = *it;
    ++it;
    for(; it!=final_policy.end(); ++it)
    {
        StateIterator sitr1(num_vms);
        for(sitr1.begin(); sitr1.end(); ++sitr1)
            if((int)sitr1 == last_policy) { break;}

        sitr1.print();
        cout<<"\t\t->\tmigrate vms: ";

        StateIterator sitr2(num_vms);
        for(sitr2.begin(); sitr2.end(); ++sitr2)
            if((int)sitr2 == *it) { break;}
     
        for(int j=0; j<num_vms; j++)
        {
            if((*sitr1)[j] != (mig_table(i, last_policy, *it))[(*sitr2)[j]])
                cout<<j<<", ";
        }
        cout<<endl;

        i++;
        last_policy = *it;
    }
    cout<<"overall profit: "<<max_profit<<endl;

    for(int p=0; p<num_phases; p++)
        for(int i=0; i<num_states; i++)
            for(int j=0; j<num_states; j++)
                delete [] mig_table(p, i, j);
    delete s_data;
}
