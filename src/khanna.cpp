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
		}
		sorted_vms->pop();
	}

	/* PHASE 0 ENDS # REST PHASES BEGINS */
	for(int i=0; i<s_data->getNumPhases()-1; i++)
	{
		// getting the next state assuming no migration
		State *curr_state = policy[i+1];
		policy[i]->getNextState(curr_state, s_data);

		// migration based on SLA violation
		Heap *sorted_violated_vm = new Heap(CompareVM(true));
		curr_state->getSortedViolatedVM(sorted_violated_vm);
		Heap *sorted_pm = new Heap(CompareVM(true));
		curr_state->getSortedPM(sorted_pm);
		while(!sorted_violated_vm->empty())
		{
			cout << sorted_violated_vm->top().index << endl;
			sorted_violated_vm->pop();
		}

		// migration based on underutilized PMs
		curr_state->getSortedLTViolatedVM(sorted_violated_vm);
		while(!sorted_violated_vm->empty())
		{
			cout << sorted_violated_vm->top().index << endl;
			sorted_violated_vm->pop();
		}
	}

	for(int i=0; i<num_phases; i++) { delete policy[i];}
	delete s_data;
	return 0;
}
