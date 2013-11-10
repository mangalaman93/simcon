#include <iostream>
#include <queue>
#include <fstream>
#include "simdata.h"
#include "state.h"
using namespace std;

int main()
{
	int num_vms, num_phases;
	cin>>num_vms;
	cin>>num_phases;

	SimData *s_data = new SimData(num_vms, num_phases);
	s_data->readInput();

	State **policy = new State*[num_phases];
	for(int i=0; i<num_phases; i++) { policy[i] = new State(i, s_data);}

	/* PHASE 0 BEGINS */
	Heap *sorted_vms = new Heap();
	for(int i=0; i<num_vms; i++) { sorted_vms->push(Info(i, s_data->getWorkload(0, i)));}
	for(int i=0; i<num_vms; i++)
	{
		// Assuming all the states can be accommodated on the given PMs
		int vm_index = sorted_vms->top().index;
		float vm_util =  sorted_vms->top().val;
		for(int j=0; j<num_vms; j++)
		{
			if(policy[0]->ifVmAllowedOnPm(j, vm_util))
			{
				policy[0]->accommodateVm(vm_index, vm_util, j);
				break;
			}
		}
		sorted_vms->pop();
	}

	/* PHASE 0 ENDS # REST PHASES BEGINS */
	for(int i=0; i<num_phases-1; i++)
	{
		// getting the next state assuming no migration
		State *next_state = policy[i+1];
		policy[i]->getNextState(next_state, s_data);

		// migration based on SLA violation
		while(true)
		{
			Heap *sorted_violated_vm = new Heap(CompareVM(true));
			next_state->getSortedViolatedVM(sorted_violated_vm);
			if(sorted_violated_vm->empty())
			{
				delete sorted_violated_vm;
				break;
			}

			Heap *sorted_pm = new Heap(CompareVM(true));
			next_state->getSortedPM(sorted_pm);

			Info vm_to_migrate = sorted_violated_vm->top();
			while(true)
			{
				if(sorted_pm->top().val >= vm_to_migrate.val)
				{
					next_state->migrate(sorted_pm->top().index, vm_to_migrate);
					break;
				}
				sorted_pm->pop();

				if(sorted_pm->empty())
				{
					cout<<"Error occurred: no migration possible even on SLA violation!"<<endl;
					delete sorted_violated_vm;
					delete sorted_pm;
					exit(1);
				}
			}

			delete sorted_violated_vm;
			delete sorted_pm;
		}

		// migration based on underutilized PMs
		Heap *sorted_violated_vm = new Heap(CompareVM(true));
		next_state->getSortedLTViolatedVM(sorted_violated_vm);
		while(!sorted_violated_vm->empty())
		{
			Heap *sorted_pm = new Heap(CompareVM(true));
			next_state->getSortedPM(sorted_pm);

			Info vm_to_migrate = sorted_violated_vm->top();
			bool flag = true;

			while(!sorted_pm->empty())
			{
				if(sorted_pm->top().val >= vm_to_migrate.val
					&& next_state->isIncrVar(sorted_pm->top().index, vm_to_migrate))
				{
					next_state->migrate(sorted_pm->top().index, vm_to_migrate);
					flag = false;
					break;
				}
				sorted_pm->pop();
			}

			delete sorted_pm;
			if(flag && vm_to_migrate.val != sorted_violated_vm->top().val) break;
			else sorted_violated_vm->pop();
		}
		delete sorted_violated_vm;
	}

	for(int i=0; i<num_phases; i++)
	{
		policy[i]->print();
		if(i+1 != num_phases)
		{
			cout<<"\t==>\t";
			policy[i+1]->printMigrations();
		}
        cout<<endl;
	}

	ofstream profilt_file("results/khanna_cum_profits.txt");
	ofstream util_file("results/khanna_util.txt");
	if(!(profilt_file.is_open() && util_file.is_open()))
    {
        cout<<"cannot open files, exiting!"<<endl;
        exit(1);
    }

	float* iutil = new float[num_vms];
	float overall_profit = 0;
	for(int p=0; p<num_phases; p++)
	{
		// finding utilization of each pm
		for(int j=0; j<num_vms; j++)
		    iutil[j] = policy[p]->get_total_util(j);

		if(p+1 != num_phases)
			policy[p]->set_intermediate_util(policy[p+1], iutil, s_data);

		util_file << p << "\t";
		for(int j=0; j<num_vms; j++)
		    util_file << policy[p]->get_total_util(j) << "\t";
		util_file << endl << (p+1-MIGRATIONDURATION) << "\t";
		for(int j=0; j<num_vms; j++)
		    util_file << iutil[j] << "\t";
		util_file << endl;

		float profit = policy[p]->getSUV(s_data);
		if(p+1 != num_phases)
			profit = (profit*(1 - MIGRATIONDURATION) + MIGRATIONDURATION*policy[p]->getISUV(policy[p+1], s_data));
        overall_profit += profit;
        profilt_file << p << "\t" << overall_profit << endl;
	}
	cout<<"overall profit: "<<overall_profit<<endl;
	delete [] iutil;
	profilt_file.close();
	util_file.close();

	for(int i=0; i<num_phases; i++) { delete policy[i];}
	delete [] policy;
	delete s_data;
	return 0;
}
