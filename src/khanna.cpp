#include "khanna.h"

Khanna::Khanna(SimData *s_data) : Policy(s_data)
{
	run_for_phases = -1;
}

void Khanna::run(int phases)
{
	if(phases < 1)
    {
        cout<<"error occured, minimum 1 phase is required to run khanna!"<<endl;
        exit(1);
    }
	if(run_for_phases > 0)
	{
		for(int i=0; i<=run_for_phases; i++) { delete policy[i];}
		delete [] policy;
	}
	run_for_phases = phases;

	policy = new State*[run_for_phases+1];
	for(int i=0; i<=run_for_phases; i++) { policy[i] = new State(i, sdata);}

	/* PHASE 0 BEGINS */
	Heap *sorted_vms = new Heap();
	for(int i=0; i<num_vms; i++) { sorted_vms->push(Info(i, sdata->getWorkload(0, i)));}
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
	for(int i=0; i<run_for_phases; i++)
	{
		// getting the next state assuming no migration
		State *next_state = policy[i+1];
		policy[i]->getNextState(next_state, sdata);

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

	ofstream profilt_file("results/khanna_cum_profits.txt");
	ofstream util_file("results/khanna_util.txt");
	if(!(profilt_file.is_open() && util_file.is_open()))
    {
        cout<<"cannot open files, exiting!"<<endl;
        exit(1);
    }

	float* iutil = new float[num_vms];
	max_profit = 0;
	for(int p=0; p<run_for_phases; p++)
	{
		// finding utilization of each pm
		for(int j=0; j<num_vms; j++)
		    iutil[j] = policy[p]->get_total_util(j);
		policy[p]->set_intermediate_util(policy[p+1], iutil, sdata);

		util_file << p << "\t";
		for(int j=0; j<num_vms; j++)
		    util_file << policy[p]->get_total_util(j) << "\t";
		util_file << endl << (p+1-MIGRATIONDURATION) << "\t";
		for(int j=0; j<num_vms; j++)
		    util_file << iutil[j] << "\t";
		util_file << endl;

		float profit = policy[p]->getSUV(sdata);
		profit = (profit*(1 - MIGRATIONDURATION) + MIGRATIONDURATION*policy[p]->getISUV(policy[p+1], sdata));
        max_profit += profit;
        profilt_file << p << "\t" << max_profit << endl;
	}
	delete [] iutil;
	profilt_file.close();
	util_file.close();
}

vector<int> Khanna::getMapping(int phase_number)
{
	if(phase_number >= run_for_phases)
    {
        cout<<"error occured, mapping doesn't exists!"<<endl;
        exit(1);
    }

    int *vm_to_pm_map = policy[phase_number]->getVmPmMaping();
    return vector<int>(vm_to_pm_map, vm_to_pm_map + num_vms*sizeof(vm_to_pm_map));
}

vector<int> Khanna::getMigrationList(int phase_number)
{
	if(phase_number >= run_for_phases)
    {
        cout<<"error occured, migration list doesn't exists!"<<endl;
        exit(1);
    }

    return policy[phase_number+1]->getMigList();
}

Khanna::~Khanna()
{
	for(int i=0; i<=run_for_phases; i++) { delete policy[i];}
	delete [] policy;
}
