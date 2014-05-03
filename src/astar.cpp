#include "astar.h"

Astar::Astar(SimData *s_data) : Policy(s_data)
{
    num_states = Bell::get(num_vms);
}

// factorial calculation
long factStar(int n)
{
    if(n == 0)
        return 1;
    else
        return (n*factStar(n-1));
}

// get the next PM permutation given the older one
void getNextStar(int *value, int N)
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
float Astar::getSUV(int phase, int* vm_to_pm_map)
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
            suv -= (STATICPOWERCONSTANT + DYNAMICPOWERCONSTANT * (util[i]>1?1:util[i])) * MAXPOWER * COSTPERKWH /1000/60;

        if(util[i] <= UTIL_THRESHOLD)
            suv += reward[i];
        else
            suv -= penalty[i];
    }

    delete [] util;
    delete [] reward;
    delete [] penalty;
    return suv * PHASE_LENGTH;
}

// ISUV = intermediate state utility value
float Astar::getISUV(int phase, int* vm_to_pm_map1, int* vm_to_pm_map2, int* perm_map)
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
            isuv -= (STATICPOWERCONSTANT + DYNAMICPOWERCONSTANT * (util[i]>1?1:util[i])) * MAXPOWER * COSTPERKWH /60/1000;

        if(util[i] <= UTIL_THRESHOLD)
            isuv += reward[i];
        else
            isuv -= penalty[i];
    }

    delete [] util;
    delete [] reward;
    delete [] penalty;
    return isuv * PHASE_LENGTH;
}

// function for the step of local optimization
float Astar::compareState(int phase, int* vm_to_pm_map1, int* vm_to_pm_map2, int *best_perm_map)
{
    // calculate State Utility Value
    float SUV = getSUV(phase, vm_to_pm_map1);

    // permutation map stores mapping of PMs to the permuted PMs
    int* perm_map = new int[num_vms];
    for(int i=0; i<num_vms; i++) { perm_map[i]=i;}

    float ISUV = getISUV(phase, vm_to_pm_map1, vm_to_pm_map2, perm_map);
    for(int i=0; i<num_vms; i++)
        best_perm_map[i] = perm_map[i];

    for(int i=1; i<factStar(num_vms); i++)
    {
        getNextStar(perm_map, num_vms);
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

void* updateTransMatStar(void *threadarg)
{
    struct ThreadDataStar *my_data = (struct ThreadDataStar*) threadarg;
    int p = my_data->p;

    StateIterator sitr(my_data->num_vms);
    StateIterator sitr2(my_data->num_vms);
    for(sitr.begin(); sitr.end(); ++sitr)
    {
        for(sitr2.begin(); sitr2.end(); ++sitr2)
        {
            (*(my_data->mig_table))(p,(int)sitr,(int)sitr2) = new int[my_data->num_vms];
            (*(my_data->trans_table))(p,(int)sitr,(int)sitr2) = my_data->astar->compareState(p, *sitr, *sitr2, (*(my_data->mig_table))(p,(int)sitr, (int)sitr2));
        }
        cout<<p<<"_"<<(int)sitr<<", "; cout.flush();
    }

    pthread_exit(NULL);
}

void Astar::run(int phases)
{
    if(phases < num_phases)
    {
        cout<<"error occured, minimum 1 cycle is required to run MDP!"<<endl;
        exit(1);
    }
    run_for_phases = phases;

    // constructing transition matrix
    cout<<"constructing transition matrix ..."<<endl;
    Matrix<float> trans_table(num_phases, num_states, num_states);
    Matrix<int*> mig_table(num_phases, num_states, num_states);
    StateIterator sitr(num_vms);
    StateIterator sitr2(num_vms);
    StateIterator sitr3(num_vms);

    cout<<"runnnig MDP! total phases: "<<num_phases<<", total states: "<<num_states<<endl;
    cout<<"creating threads: "; cout.flush();
    pthread_t threads[num_phases];
    struct ThreadDataStar* td[num_phases];
    pthread_attr_t attr;
    void *status;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    for(int p=0; p<num_phases; p++)
    {
        td[p] = new struct ThreadDataStar(num_vms, p, &mig_table, &trans_table, this);
        int rc = pthread_create(&threads[p], NULL, updateTransMatStar, (void *)td[p]);
        if(rc)
        {
            cout << "Error:unable to create thread," << rc << endl;
            exit(-1);
        }
        cout<<p<<","; cout.flush();
    }
    cout<<endl<<"created "<<num_phases<<" number of thread!"<<endl;

    // wait for other threads to complete
    pthread_attr_destroy(&attr);
    cout<<"thread complete: ";
    for(int p=0; p<num_phases; p++)
    {
        int rc = pthread_join(threads[p], &status);
        if (rc)
        {
            cout << "Error:unable to join," << rc << endl;
            exit(-1);
        }
        delete td[p];
        cout<<p<<","<<endl;
    }
    cout<<endl<<"transition matrix prepared ..."<<endl;

    // converting to cost from profit & finding out min edge
    float min_edge_cost = -trans_table(0, 0, 0);
    for(int p=0; p<num_phases; p++)
    {
        for(sitr.begin(); sitr.end(); ++sitr)
        {
            for(sitr2.begin(); sitr2.end(); ++sitr2)
            {
                trans_table(p, (int)sitr, (int)sitr2) *= -1;
                if(min_edge_cost > trans_table(p, (int)sitr, (int)sitr2))
                    min_edge_cost = trans_table(p, (int)sitr, (int)sitr2);
            }
        }
    }

    // decreasing min_edge cost for safety purposes (so that no entry/cost is 0)
    min_edge_cost--;

    // shifting in A*
    for(int p=0; p<num_phases; p++)
        for(sitr.begin(); sitr.end(); ++sitr)
            for(sitr2.begin(); sitr2.end(); ++sitr2)
                trans_table(p, (int)sitr, (int)sitr2) -= min_edge_cost;

    float min_cost = numeric_limits<float>::max();
    int best_start_state = -1;
    Matrix<int> *best_path = new Matrix<int>(num_phases+1, num_states);
    Matrix<int> *temp_path = new Matrix<int>(num_phases+1, num_states);
    for(int start_state=0; start_state<num_states; start_state++)
    {
        // A* search
        AstarNodeHeap open_list;
        Matrix<float> g_value(num_phases+1, num_states);
        Matrix<bool> closed(num_phases+1, num_states);

        // initialising
        for(int i=0; i<num_phases+1; i++)
        {
            for(int j=0; j<num_states; j++)
            {
                closed(i, j) = false;
                g_value(i, j) = numeric_limits<float>::max();
            }
        }

        // adding phase 0 node
        g_value(0, start_state) = 0;
        AstarNode to_relax(0, start_state, 0);
        open_list.push(to_relax);

        while(to_relax.phase_number != num_phases)
        {
            if(DEBUG)
            {
                priority_queue<AstarNode, vector<AstarNode>, CompareAstarNode> temp_list(open_list);
                while(!temp_list.empty()) {
                    cout<<temp_list.top().phase_number<<","<<temp_list.top().state_index<<"->"<<temp_list.top().g_plus_h<<endl;
                    temp_list.pop();
                }
                cout<<endl;

                for(int i=0; i<num_phases+1; i++)
                {
                    for(int j=0; j<num_states; j++)
                        cout<<closed(i, j)<<",";
                    cout<<endl;
                }
                cout<<endl;
            }

            open_list.pop();
            if((to_relax.g_plus_h != g_value(to_relax.phase_number, to_relax.state_index))
             || closed(to_relax.phase_number, to_relax.state_index))
            {
                to_relax = open_list.top();
                continue;
            }

            int new_phase_number = to_relax.phase_number + 1;
            if(new_phase_number == num_phases)
            {
                float new_g_value = g_value(to_relax.phase_number, to_relax.state_index) +
                                    trans_table(to_relax.phase_number, to_relax.state_index, start_state);

                if(new_g_value < g_value(num_phases, start_state))
                {
                    g_value(num_phases, start_state) = g_value(to_relax.phase_number, to_relax.state_index) +
                                                       trans_table(to_relax.phase_number, to_relax.state_index, start_state);
                    (*temp_path)(num_phases, start_state) = to_relax.state_index;
                    open_list.push(AstarNode(num_phases, start_state, new_g_value));

                    closed(num_phases, start_state) = false;
                }
            } else
            {
                // relax node (first add all the adjacent nodes to open list, excluding which are already closed & having higher g value)
                for(sitr.begin(); sitr.end(); ++sitr)
                {
                    float new_g_value = g_value(to_relax.phase_number, to_relax.state_index) +
                                        trans_table(to_relax.phase_number, to_relax.state_index, (int)sitr);
                    if(new_g_value < g_value(new_phase_number, (int)sitr))
                    {
                        g_value(new_phase_number, (int)sitr) = new_g_value;
                        (*temp_path)(new_phase_number, (int)sitr) = to_relax.state_index;
                        open_list.push(AstarNode(new_phase_number, (int)sitr, new_g_value));
                        closed(new_phase_number, (int)sitr) = false;
                    }
                }
            }

            closed(to_relax.phase_number, to_relax.state_index) = true;
            to_relax = open_list.top();
        }

        if(min_cost > g_value(num_phases, start_state))
        {
            min_cost = g_value(num_phases, start_state);
            Matrix<int>* temp = best_path;
            best_path = temp_path;
            temp_path = temp;
            best_start_state = start_state;
        }
    }
    max_profit = min_cost + num_phases*min_edge_cost;
    max_profit *= -1;

    for(int p=0; p<num_phases; p++)
        for(sitr.begin(); sitr.end(); ++sitr)
            for(sitr2.begin(); sitr2.end(); ++sitr2)
                trans_table(p, (int)sitr, (int)sitr2) += min_edge_cost;

    for(int p=0; p<num_phases; p++)
        for(sitr.begin(); sitr.end(); ++sitr)
            for(sitr2.begin(); sitr2.end(); ++sitr2)
                trans_table(p, (int)sitr, (int)sitr2) *= -1;

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

    list<int> final_policy;
    final_policy.push_back(best_start_state);
    int back_pointer = best_start_state;
    for(int i=num_phases; i>0; i--)
    {
        back_pointer = (*best_path)(i, back_pointer);
        final_policy.push_front(back_pointer);
    }
    cout<<endl<<"max profit cycle found ..."<<endl;

    ofstream profilt_file("results/astar_cum_profits.txt");
    ofstream util_file("results/astar_util.txt");
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
            util[(*sitr1)[j]] += sdata->getWorkload(mod_i, j);
        for(int j=0; j<num_vms; j++)
            iutil[j] = util[j];

        // storing the policy
        vector<int> v;
        for(int j=0; j<num_vms; j++)
            v.push_back((*sitr1)[j]);
        optimal_policy.push_back(v);
        mig_list.push_back(vector<int>());

        for(int j=0; j<num_vms; j++)
        {
            mig_list[i].push_back(-1);
            if((*sitr1)[j] != (mig_table(mod_i, last_policy, *it))[(*sitr2)[j]])
            {
                iutil[(*sitr1)[j]] += MOHCPUINTENSIVE*sdata->getWorkload(mod_i, j);
                int to_pm = (mig_table(mod_i, last_policy, *it))[(*sitr2)[j]];
                iutil[to_pm] += MOHCPUINTENSIVE*sdata->getWorkload(mod_i, j);

                // storing the migration list
                mig_list[i][j] = to_pm;
            }
        }

        // dumping utilizations
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
    for(int p=0; p<num_phases; p++)
        for(int i=0; i<num_states; i++)
            for(int j=0; j<num_states; j++)
                delete [] mig_table(p, i, j);

    delete best_path;
    delete temp_path;
}

void Astar::getMapping(int phase_number, vector<int>* mapping)
{
    if(phase_number >= run_for_phases)
    {
        cout<<"error occured, mapping doesn't exists!"<<endl;
        exit(1);
    }

    for(unsigned int i=0; i<optimal_policy[phase_number].size(); i++)
        (*mapping)[i] = optimal_policy[phase_number][i];
}

void Astar::getMigrationList(int phase_number, vector<int>* mapping)
{
    if(phase_number >= run_for_phases)
    {
        cout<<"error occured, migration list doesn't exists!"<<endl;
        exit(1);
    }

    for(unsigned int i=0; i<mig_list[phase_number].size(); i++)
        (*mapping)[i] = mig_list[phase_number][i];
}

void Astar::dumpPolicy()
{
    dumpPolicyHelper("astar");
}
