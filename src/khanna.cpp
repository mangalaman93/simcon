#include <iostream>
#include <queue>
#include "simdata.h"
#include "state.h"
using namespace std;

int main()
{
	int num_vms, num_pms, num_phases;
	cin>>num_pms;
	cin>>num_vms;
	cin>>num_phases;

	SimData *s_data = new SimData(num_pms, num_vms, num_phases);
	s_data->readInput();
	
	State *policy[num_phases];
	for(int i=0; i<num_phases; i++) { policy[i] = new State(i, s_data);}

	/* PHASE 0 BEGINS */
	Heap *sorted_vms = new Heap();
	for(int i=0; i<num_vms; i++) { sorted_vms->push(Info(i, s_data->getWorkload(0, i)));}
	for(int i=0; i<num_vms; i++)
	{
		// Assuming all the states can be accommodated on the given PMs
		int vm_index = sorted_vms->top().index;
		double vm_util =  sorted_vms->top().val;
		for(int j=0; j<num_pms; j++)
		{
			if(policy[0]->ifVmAllowedOnPm(j, vm_util))
			{
				policy[0]->accommodateVm(vm_index, vm_util, j);
				break;
			}

			if(j == num_pms-1)
			{
				cout<<"Error occurred: available PMs are not enough!"<<endl;
				exit(1);
			}
		}
		sorted_vms->pop();
	}

	/* PHASE 0 ENDS # REST PHASES BEGINS */
	for(int i=0; i<s_data->getNumPhases()-1; i++)
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
        cout<<"\t===>\t";
        
        int next_state = i+1;
        if(i == num_phases - 1) { next_state = 0;}

        list<int> *migrated_vms = policy[i]->compareState(policy[next_state]);
		if(migrated_vms->empty())
			cout<<"no migrations";
		else
		{
			cout<<"migrate vm "<<*(migrated_vms->begin());
			for(list<int>::iterator it=++(migrated_vms->begin()); it!=migrated_vms->end(); ++it)
				cout<<", "<<*it;
		}
        cout<<endl;

        delete migrated_vms;
	}

	for(int i=0; i<num_phases; i++) { delete policy[i];}
	delete s_data;
	return 0;
}
