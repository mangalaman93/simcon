#include <iostream>
#include <queue>
#include "simdata.h"
#include "state.h"
using namespace std;

struct VmInfo
{
    int index;
    double util;
	VmInfo(int i, double u) : index(i), util(u) {}
};

class CompareVM {
public:
	bool operator()(VmInfo& vm1, VmInfo& vm2) { vm1.util<vm2.util;}
};

typedef priority_queue<VmInfo, vector<VmInfo>, CompareVM> pq_vm;

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
	pq_vm *sorted_vms = new pq_vm();
	for(int i=0; i<num_vms; i++)
	{
		VmInfo vi(i, s_data->getWorkload(0, i));
		sorted_vms->push(vi);
	}

	for(int i=0; i<num_vms; i++)
	{
		// Assuming all the states can be accommodated on the given PMs
		int vm_index = sorted_vms->top().index;
		for(int j=0; j<num_pms; j++)
		{
			double vm_util = s_data->getWorkload(0, vm_index);
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
		policy[i]->getNextState(policy[i+1]);

		// // migration based on SLA violation
		// pq_vm sorted_violated_vm = new pq_vm();
		// if(isSlaViolated(curr_state, sorted_violated_vm_list))
		// {
		// 	while(isSlaViolated(curr_state)) 
		// 	{
		// 		migrate(curr_state, sorted_violated_vm_list)
		// 	}
		// }

		// // migration based on underutilized PMs
		// if(isLowerThreholdViolated(curr_state, sorted_underutilized_vm_list)) 
		// {
		// 	while(isLowerThreholdViolated(curr_state))
		// 	{
		// 		migrate(curr_state, sorted_underutilized_vm_list)
		// 	}
		// }			
		// policy[i+1] = curr_state;
	}

	for(int i=0; i<num_phases; i++) { delete policy[i];}
	delete s_data;
	return 0;
}
