#include "state.h"

State::State()
{
}

State::~State()
{}


void State::getNextState()
{
	State* new_state = new State();
	new_state->phase_num = phase_num+1;
	for(int=0; i<NUM_PM; i++)
	{
		new_state->vm_on_pm_tup[i] = this->vm_on_pm_tup[i];
	}
	return new_state;
}

bool State::ifVmAllowedOnPm(float vm_util, int pmi)
{
	return (vm_on_pm_tup[pmi-1].total_util + pmi < UTIL_THRESHOLD);
}
