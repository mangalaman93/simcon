/*
Note that algorithm assumes the availability of as many Physical Machines
as the total virtual machines are there, though the PMs used would be optimal.
[num_pms is, therefore, replaced with num_vms everywhere in the code]
*/

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

    // deleting the old policy if the program has been executed before
	if(run_for_phases > 0)
	{
		for(int i=0; i<=run_for_phases; i++) { delete policy[i];}
		delete [] policy;
	}
	run_for_phases = phases;

	// creating array to store new policy, note that we run khanna for one extra phase to
	// find out migrations of the last phase
	policy = new State*[run_for_phases+1];
	for(int i=0; i<=run_for_phases; i++) { policy[i] = new State(i, sdata);}

	/* PHASE 0 BEGINS */
	Heap *sorted_vms = new Heap();

	// constructing list (heap) of all VMs in non decreasing order of their requirements
	for(int i=0; i<num_vms; i++) { sorted_vms->push(Info(i, sdata->getWorkload(0, i)));}

	// Best Fit Bin Packing Algorithm
	for(int i=0; i<num_vms; i++)
	{
		// Assuming all the VMs can be accommodated on the given PMs
		int vm_index = sorted_vms->top().index;
		float vm_util = sorted_vms->top().val;
		bool assigned = false;
		for(int j=0; j<num_vms; j++)
		{
			if(policy[0]->ifVmAllowedOnPm(j, vm_util))
			{
				policy[0]->accommodateVm(vm_index, vm_util, j);
				assigned = true;
				break;
			}
		}
		if(assigned)
			sorted_vms->pop();
		else
		{
			cerr<<"cannot assign VM "<<vm_index<<endl;
			exit(-1);
		}
	}
	delete sorted_vms;

	/* PHASE 0 ENDS # REST PHASES BEGINS */
	for(int i=0; i<run_for_phases; i++)
	{
		// getting the next state assuming no migration
		State *next_state = policy[i+1];
		policy[i]->getNextState(next_state, sdata);

		// migration based on SLA violation
		while(true)
		{
			// constructing list of VMs whose SLA is violated in non decreasing order of their requirements
			Heap *sorted_violated_vm = new Heap(CompareVM(true));
			next_state->getSortedViolatedVM(sorted_violated_vm);

			// if no PM is violating SLA
			if(sorted_violated_vm->empty())
			{
				delete sorted_violated_vm;
				break;
			}

			// constructing list (min heap) of PMs in non decreasing order of their residual capacity
			Heap *sorted_pm = new Heap(CompareVM(true));
			next_state->getSortedPM(sorted_pm);

			// searching for best fit PM for given VM
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

		// getting list of all underutilized VMs in non decreasing order of their requirements
		Heap *sorted_violated_vm = new Heap(CompareVM(true));
		next_state->getSortedLTViolatedVM(sorted_violated_vm);

		// migration based on underutilized PMs
		while(!sorted_violated_vm->empty())
		{
			// getting list of PMs in non decreasing order of their residual capacities
			Heap *sorted_pm = new Heap(CompareVM(true));
			next_state->getSortedPM(sorted_pm);

			// finding the best fit for given VM with the condition that variance increases
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

			// end of loop only if no migration happened
			// also it is possible that equally utilizaed VM on different PM may be able to migrate
			if(flag && vm_to_migrate.val != sorted_violated_vm->top().val) break;
			else sorted_violated_vm->pop();
		}

		delete sorted_violated_vm;
	}

	// dumping data to files
	ofstream profilt_file("results/khanna_cum_profits.txt");
	ofstream util_file("results/khanna_util.txt");
	if(!(profilt_file.is_open() && util_file.is_open()))
    {
        cout<<"cannot open files, exiting!"<<endl;
        exit(1);
    }

    // iutil stores utilizations when VMs are migrating
	float* iutil = new float[num_vms];
	max_profit = 0;
	for(int p=0; p<run_for_phases; p++)
	{
		// finding utilization of each pm
		for(int j=0; j<num_vms; j++)
		    iutil[j] = policy[p+1]->get_total_util(j);
		policy[p+1]->setIntermediateUtil(iutil, sdata);

		// dumping utilizations
		util_file << (p+MIGRATIONDURATION) << "\t";
		for(int j=0; j<num_vms; j++)
		    util_file << policy[p]->get_total_util(j) << "\t";
		util_file << endl << p+1 << "\t";
		for(int j=0; j<num_vms; j++)
		    util_file << iutil[j] << "\t";
		util_file << endl;

		// dumping profit, note that in Khanna runs for t=MigrationDuration to t=RunForPhase+MigrationDuration
        max_profit += (policy[p]->getSUV(sdata)*(1 - MIGRATIONDURATION) + MIGRATIONDURATION*policy[p+1]->getISUV(sdata));
        profilt_file << p+1 << "\t" << max_profit << endl;
	}
	delete [] iutil;
	profilt_file.close();
	util_file.close();
}

void Khanna::getMapping(int phase_number, vector<int>* mapping)
{
	if(phase_number >= run_for_phases)
    {
        cout<<"error occured, mapping doesn't exists!"<<endl;
        exit(1);
    }

    int *vm_to_pm_map = policy[phase_number]->getVmPmMaping();
    for(int j=0; j<num_vms; j++)
        (*mapping)[j] = (vm_to_pm_map[j]);
}

void Khanna::getMigrationList(int phase_number, vector<int>* mapping)
{
	if(phase_number >= run_for_phases)
    {
        cout<<"error occured, migration list doesn't exists!"<<endl;
        exit(1);
    }

    policy[phase_number+1]->getMigList(mapping);
}

void Khanna::dumpPolicy()
{
    dumpPolicyHelper(string("khanna"));
}

Khanna::~Khanna()
{
	for(int i=0; i<=run_for_phases; i++) { delete policy[i];}
	delete [] policy;
}
