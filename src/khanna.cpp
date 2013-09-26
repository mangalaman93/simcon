#include <iostream>
#include <algorithm>
using namespace std;


State* getInitialState(float* sorted_vm_array) 
{
	State* ini_state = new State();

	return ini_state;
}

bool isSlaViolated(State* curr_s, list<int> s_violated_vm)
{

}


int main()
{
	// reading the data from standard input
	SimData *s_data = new SimData();
	s_data->readInputFile();

	// sorting the VMs in their order of decreasing utilization
	float sorted_utilization_vm_array[NUM_VM];
	sort(sorted_utilization_vm_array, s_data->workload[0]);

	// final policy, contains num_phases number of states
	State *policy = new State[s_data->NUM_PHASES];

	//getting the initial placement using FFD algorithm
	policy[0] = getInitialState(sorted_utilization_vm_array);

	// getting the VM-PM placement for rest of the phases
	for(int i=0; i<s_data->NUM_PHASES; i++)
	{
		// getting the next state assuming no migration
		State *curr_state = policy[i]->getNextState();

		//  migration based on SLA violation
		list<int> sorted_violated_vm_list = new list<int>();
		if(isSlaViolated(curr_state, sorted_violated_vm_list))
		{
			while(isSlaViolated(curr_state)) 
			{
				migrate(curr_state, sorted_violated_vm_list)
			}
		}

		// migration based on underutlized PMs
		if(isLowerThreholdViolated(curr_state, sorted_underutilized_vm_list)) 
		{
			while(isLowerThreholdViolated(curr_state))
			{
				migrate(curr_state, sorted_underutilized_vm_list)
			}
		}			
		policy[i+1] = curr_state;
	}
	return 0;
}
