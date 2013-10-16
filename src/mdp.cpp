#include <iostream>
#include <cmath>
#include "algo.h"
#include "simdata.h"
#include "state.h"
#include "stateIterator.h"
using namespace std;

#define DEBUG true

float compareState(int phase, int* vm_to_pm_map1, int* vm_to_pm_map2, int num_vms)
{
    float max_profit = 0;
    int index = 0;
    for(int shift=0; shift<num_vms; shift++)
    {
        // calculate state utility value and intermediate state utility value
        float SUV = 0, ISUV = 0;
        float power_cost;
        ISUV += shift;
    }

    return max_profit;
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

    // constructing transition matrix
    Matrix<float> trans_table(num_phases, num_states, num_states);
    StateIterator sitr(num_vms);
    StateIterator sitr2(num_vms);
    for(int p=0; p<num_phases; p++)
    {
        for(sitr.begin(); sitr.end(); ++sitr)
        {
            for(sitr2.begin(); sitr2.end(); ++sitr2)
            {
                trans_table(p, (int)sitr, (int)sitr2) = compareState(p, *sitr, *sitr2, num_vms);
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
        cout<<policy[i]<<endl;
    }
    cout<<"overall profit: "<<min_profit<<endl;

    delete s_data;
    delete policy;
    delete cycle;
    delete trans_profit;
}
