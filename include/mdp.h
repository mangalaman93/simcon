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
	vector< vector<int> > mig_list;

	float getISUV(int phase, int* vm_to_pm_map1, int* vm_to_pm_map2, int* perm_map);
	float getSUV(int phase, int* vm_to_pm_map);
	float compareState(int phase, int* vm_to_pm_map1, int* vm_to_pm_map2, int *best_perm_map);

  public:
	Mdp(SimData *s_data);
	void run(int phases);
	vector<int> getMapping(int phase_number);
	vector<int> getMigrationList(int phase_number);
};

#endif
