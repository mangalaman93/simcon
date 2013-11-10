#include "mdp.h"

Mdp::Mdp(Simdata *sdata) : Policy(sdata)
{
    num_states = Bell::get(num_vms);
}

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
float Mdp::getSUV(int phase, int* vm_to_pm_map)
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
float Mdp::getISUV(int phase, int* vm_to_pm_map1, int* vm_to_pm_map2, int* perm_map)
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
float Mdp::compareState(int phase, int* vm_to_pm_map1, int* vm_to_pm_map2, int *best_perm_map)
{
    // calculate State Utility Value
    float SUV = getSUV(phase, vm_to_pm_map1);

    // permutation map stores mapping of PMs to the permuted PMs
    int* perm_map = new int[num_vms];
    for(int i=0; i<num_vms; i++) { perm_map[i]=i;}

    float ISUV = getISUV(phase, vm_to_pm_map1, vm_to_pm_map2, perm_map);
    for(int i=0; i<num_vms; i++)
        best_perm_map[i] = perm_map[i];

    for(int i=1; i<fact(num_vms); i++)
    {
        getNext(perm_map, num_vms);
        float temp = getISUV(phase, vm_to_pm_map1, vm_to_pm_map2, perm_map);
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

int run(int phases)
{
    if(phases < sdata->getNumPhases())
    {
        cout<<"error occured, minimum 1 cycle is required to run MDP!"<<endl;
        exit(1);
    }
    run_for_phases = phases;

    // constructing transition matrix
    cout<<"constructing transition matrix ..."<<endl;
    StateIterator sitr(num_vms);
    StateIterator sitr2(num_vms);
    StateIterator sitr3(num_vms);

    cout<<"runnnig MDP! total phases: "<<num_phases<<endl<<"phases complete: "; cout.flush();
    for(int p=0; p<num_phases; p++)
    {
        for(sitr.begin(); sitr.end(); ++sitr)
            for(sitr2.begin(); sitr2.end(); ++sitr2)
                trans_table(p, (int)sitr, (int)sitr2) = compareState(p, *sitr, *sitr2, mig_table(p,(int)sitr,(int)sitr2));
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
    Matrix<float> temp_trans(num_states, num_states);
    Matrix<float> mid_trans(num_states, num_states);
    Matrix<list<int> > policy(num_states, num_states);
    Matrix<list<int> > new_policy(num_states, num_states);

    // copying the data
    for(int i=0; i<num_states; i++)
        for(int j=0; j<num_states; j++)
            mid_trans(i, j) = trans_table(0, i, j);

    cout<<"finding the max profit cycle, total phases: "<<run_for_phases<<endl<<"phases complete: "; cout.flush();
    for(int p=0; p<run_for_phases-2; p++)
    {
        for(sitr.begin(); sitr.end(); ++sitr)
        {
            for(sitr3.begin(); sitr3.end(); ++sitr3)
            {
                float max = mid_trans((int)sitr, 0) + trans_table((p+1)%num_phases, 0, (int)sitr3);
                int index = 0;
                for(sitr2.begin(); sitr2.end(); ++sitr2)
                {
                    float temp = mid_trans((int)sitr, (int)sitr2) + trans_table((p+1)%num_phases, (int)sitr2, (int)sitr3);
                    if(max < temp)
                    {
                        max = temp;
                        index = (int)sitr2;
                    }
                }
                new_policy((int)sitr, (int)sitr3) = policy((int)sitr, index);
                new_policy((int)sitr, (int)sitr3).push_back(index);
                temp_trans((int)sitr, (int)sitr3) = max;
            }
        }

        // copying the data
        for(int i=0; i<num_states; i++)
        {
            for(int j=0; j<num_states; j++)
            {
                mid_trans(i, j) = temp_trans(i, j);
                policy(i, j) = new_policy(i, j);
            }
        }
        cout<<p<<","; cout.flush();
    }

    cout<<(run_for_phases-1); cout.flush();
    max_profit = mid_trans(0, 0) + trans_table((run_for_phases-1)%num_phases, 0, 0);
    int index1 = 0;
    int index2 = 0;
    for(sitr.begin(); sitr.end(); ++sitr)
    {
        for(sitr2.begin(); sitr2.end(); ++sitr2)
        {
            float temp = mid_trans((int)sitr, (int)sitr2) + trans_table((run_for_phases-1)%num_phases, (int)sitr2, (int)sitr);
            if(temp > max_profit)
            {
                max_profit = temp;
                index1 = (int)sitr;
                index2 = (int)sitr2;
            }
        }
    }
    list<int> final_policy = policy(index1, index2);
    final_policy.push_back(index2);
    final_policy.push_back(index1);
    final_policy.push_front(index1);
    cout<<endl<<"max profit cycle found ..."<<endl;
}

void Mdp::dumpStats()
{
    ofstream profilt_file("results/mdp_cum_profits.txt");
    ofstream util_file("results/mdp_util.txt");
    if(!(profilt_file.is_open() && util_file.is_open()))
    {
        cout<<"cannot open files, exiting!"<<endl;
        exit(1);
    }

    int i = 0;
    float cum_profit = 0;
    float* util = new float[num_vms];
    float* iutil = new float[num_vms];
    list<int>::iterator it = final_policy.begin();
    int last_policy = *it;
    ++it;

    for(; it!=final_policy.end(); ++it)
    {
        int mod_i = i%num_phases;

        // storing cumulative profit
        cum_profit += trans_table(mod_i, last_policy, *it);
        profilt_file << i << "\t" << cum_profit << endl;

        // finding the state vm to pm mapping
        StateIterator sitr1(num_vms);
        for(sitr1.begin(); sitr1.end(); ++sitr1)
            if((int)sitr1 == last_policy) { break;}
        StateIterator sitr2(num_vms);
        for(sitr2.begin(); sitr2.end(); ++sitr2)
            if((int)sitr2 == *it) { break;}

        // finding utilization of each pm
        for(int j=0; j<num_vms; j++)
            util[j] = 0;
        for(int j=0; j<num_vms; j++)
            util[(*sitr1)[j]] += s_data->getWorkload(mod_i, j);
        for(int j=0; j<num_vms; j++)
            iutil[j] = util[j];

        for(int j=0; j<num_vms; j++)
        {
            if((*sitr1)[j] != (mig_table(mod_i, last_policy, *it))[(*sitr2)[j]])
            {
                iutil[(*sitr1)[j]] += MOHCPUINTENSIVE*s_data->getWorkload(mod_i, j);
                iutil[(mig_table(mod_i, last_policy, *it))[(*sitr2)[j]]] += MOHCPUINTENSIVE*s_data->getWorkload(mod_i, j);
            }
        }

        // storing the utilization
        util_file << i << "\t";
        for(int j=0; j<num_vms; j++)
            util_file << util[j] << "\t";
        util_file << endl << (i+(1-MIGRATIONDURATION)) << "\t";
        for(int j=0; j<num_vms; j++)
            util_file << iutil[j] << "\t";
        util_file << endl;

        i++;
        last_policy = *it;
    }
    profilt_file.close();
    util_file.close();

    delete [] util;
    delete [] iutil;
}

const int* Mdp::getMapping(int phase_number)
{
    if(phase_number > run_for_phases)
    {
        cout<<"error occured, mapping doesn't exists!"<<endl;
        exit(1);
    }
}

const int* Mdp::getMigrationList(int phase_number)
{
    if(phase_number > run_for_phases)
    {
        cout<<"error occured, mapping doesn't exists!"<<endl;
        exit(1);
    }
}

Mdp::~Mdp()
{
    for(int p=0; p<num_phases; p++)
        for(int i=0; i<num_states; i++)
            for(int j=0; j<num_states; j++)
                delete [] mig_table(p, i, j);
}
