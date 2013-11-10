#ifndef MDP_H
#define MDP_H

#include <iostream>
#include <cmath>
#include <fstream>
#include "algo.h"
#include "simdata.h"
#include "config.h"
#include "stateIterator.h"
#include "policy.h"
using namespace std;

class Mdp : public Policy
{
	long int num_states;
	vector< vector<int> > optimal_policy;
	vector< list<int> > mig_list;

	float getISUV(int phase, int* vm_to_pm_map1, int* vm_to_pm_map2, int* perm_map);
	float getSUV(int phase, int* vm_to_pm_map);
	float compareState(int phase, int* vm_to_pm_map1, int* vm_to_pm_map2, int *best_perm_map);

  public:
	Mdp(Simdata *sdata);
	void run();
	const int* getMapping(int phase_number);
	const int* getMigrationList(int phase_number);
	void dumpStats();
};

#endif
